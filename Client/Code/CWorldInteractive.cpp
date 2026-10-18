#include "pch.h"
#include "CWorldInteractive.h"
#include "CAnimator.h"
#include "CBoxCollider.h"
#include "CCombatStat.h"
#include "CCreateHelper.h"
#include "CDInputMgr.h"
#include "CDropSystem.h"
#include "CInventory.h"
#include "CPlayer.h"
#include "CRcTex.h"
#include "CRenderer.h"
#include "CSpriteEff.h"
#include "CState.h"
#include "CTexture.h"
#include "CTransform.h"
#include "ItemInstance.h"
#include "CVillageStateMgr.h"

CWorldInteractive::CWorldInteractive(DEVICE graphicDev)
    : CRenderObject(graphicDev),

      m_CombatStatCom(nullptr),
      m_BoxColCom(nullptr),
      m_InventoryCom(nullptr),
      m_SpriteEffCom(nullptr)
{ }

CWorldInteractive::CWorldInteractive(const CWorldInteractive& rhs)
    : CRenderObject(rhs),

      m_CombatStatCom(nullptr),
      m_BoxColCom(nullptr),
      m_InventoryCom(nullptr),
      m_SpriteEffCom(nullptr),

      m_bPlayerOverlap(rhs.m_bPlayerOverlap)
{ }

CWorldInteractive::~CWorldInteractive()
{ }

HRESULT CWorldInteractive::Ready_GameObject()
{
    if (FAILED(CRenderObject::Ready_GameObject()))
        return E_FAIL;

    m_TransformCom->Set_Rotation({ D3DXToRadian(40), 0, 0 });
    m_SpriteEffCom->InitOutLine(D3DCOLOR_ARGB(255, 255, 255, 255), 0.05f);

    return S_OK;
}

void CWorldInteractive::Initialize_GameObject()
{
    CRenderObject::Initialize_GameObject();
}

_int CWorldInteractive::Update_GameObject(const _float& timeDelta)
{
    _int  exit = CRenderObject::Update_GameObject(timeDelta);
    
        if (m_CombatStatCom->Is_Dead() == true ) // 컴포넌트 내부에서 처리함 0 만들고 Dead true
        {
            if (m_bInvenDropped == false)
            {
            // 플레이어 위치
            _vec3 DropDir = m_CombatStatCom->Get_Dir();
            int offset = 40; //
            CDropSystem::GetInstance()->SpawnAllFromInven(m_GraphicDev, m_InventoryCom, DropDir, offset);
            m_bHovering = false;
            m_BoxColCom->Set_CollisionActive(false);

            m_bInvenDropped = true;
           }

            return exit;
        }


    //CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

    // 플레이어가 가까이 있을떄만
    if (m_bPlayerOverlap == true)
    {
        //플레이어 상태 변환

        if (Is_Hovering() == true)
        {
            // 호버링이 되어 있을떄 
            m_bHovering = true; // 렌더 아웃라인 용
            // cout << "hovering" << endl;
        }
        else
            m_bHovering = false;
    }
    else
        m_bHovering = false;

    return exit;
}

void CWorldInteractive::LateUpdate_GameObject(const _float& timeDelta)
{
    CRenderObject::LateUpdate_GameObject(timeDelta);
}

void CWorldInteractive::Render_GameObject()
{
    Render_Setting();

    if (m_bHovering == true)
        m_SpriteEffCom->OutLineEff(m_BufferCom);

    m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());

    if (m_BufferCom)
        m_BufferCom->Render_Buffer();

    Render_Reset();

    // 충돌체 렌더 (디버그용)
    if (m_BoxColCom)
        m_BoxColCom->Render();
}

void CWorldInteractive::OnBeginOverlap(CCollider* self, CCollider* other)
{
  
        CRenderObject::OnBeginOverlap(self, other);

        CPlayer* ply = dynamic_cast<CPlayer*>(other->Get_Owner());
        if (ply)
            m_bPlayerOverlap = true;
    
}

void CWorldInteractive::OnEndOverlap(CCollider* self, CCollider* other)
{
  
        CRenderObject::OnEndOverlap(self, other);


        CPlayer* ply = dynamic_cast<CPlayer*>(other->Get_Owner());

        if (m_bPlayerOverlap == true && ply != nullptr)
            m_bPlayerOverlap = false;
  
}

bool CWorldInteractive::Is_Hovering()
{
    ////////////
    // 여기는 분리 할거 임 1프레임에 1 번만 하게
    POINT pt;
    GetCursorPos(&pt);
    ScreenToClient(CDInputMgr::GetInstance()->Get_HWND(), &pt);

    D3DVIEWPORT9 ViewPort;
    ZeroMemory(&ViewPort, sizeof(D3DVIEWPORT9));

    m_GraphicDev->GetViewport(&ViewPort);

    _vec3 vMousePos;

    //vMousePos.x = pt.x / (ViewPort.Width * 0.5) - 1.f;
    //vMousePos.y = pt.y / (ViewPort.Height * 0.5) + 1.f;
    //vMousePos.z = 1.f;
    vMousePos.x = ((2.0f * pt.x) / ViewPort.Width) - 1.0f;
    vMousePos.y = ((-2.0f * pt.y) / ViewPort.Height) + 1.0f;
    vMousePos.z = 1.0f; // z=1로 해야 정상적인 pick ray가 됨

    D3DXMATRIX matProj;
    m_GraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
    D3DXMatrixInverse(&matProj, nullptr, &matProj);
    //coord
    D3DXVec3TransformCoord(&vMousePos, &vMousePos, &matProj);

    D3DXMATRIX matView;
    m_GraphicDev->GetTransform(D3DTS_VIEW, &matView);
    D3DXMatrixInverse(&matView, nullptr, &matView);
    // 뷰 행렬에서 레이랑 해야함
    _vec3 vRayPos = { matView._41, matView._42, matView._43 };// RayPos 는 카메라 위치
    //_vec3 vRayDir = vMousePos - vRayPos;

    _vec3 vRayDir; //마우스 방향을 월드 방향으로 변환
    D3DXVec3TransformNormal(&vRayDir, &vMousePos, &matView);
    D3DXVec3Normalize(&vRayDir, &vRayDir);

    /////////

    // 이건 할 필요 없음 월드 공간 레이 vs 월드 AABB 테스트 할거라서
    /*D3DXMATRIX matWorld = (this->Get_Transform()->Get_World());
    D3DXMatrixInverse(&matWorld, 0, &matWorld);

    D3DXVec3TransformCoord(&vRayPos, &vRayPos, &matWorld);
    D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matWorld);*/

    //_vec3 half = m_BoxColCom->Get_RawSize() * .5f;
    _vec3 Min = m_BoxColCom->Get_Transform()->Get_Pos() - m_BoxColCom->Get_Size() * .5f;
    _vec3 Max = m_BoxColCom->Get_Transform()->Get_Pos() + m_BoxColCom->Get_Size() * .5f;

    //float minX = m_BoxColCom->Get_Transform()->Get_Pos().x - m_BoxColCom->Get_RawSize().x * .5f;
    //float maxX = m_BoxColCom->Get_Transform()->Get_Pos().x + m_BoxColCom->Get_RawSize().x * .5f;
    //float minY = m_BoxColCom->Get_Transform()->Get_Pos().y - m_BoxColCom->Get_RawSize().y * .5f;
    //float maxY = m_BoxColCom->Get_Transform()->Get_Pos().y + m_BoxColCom->Get_RawSize().y * .5f;

    float timeMin = (Min.x - vRayPos.x) / vRayDir.x;
    float timeMax = (Max.x - vRayPos.x) / vRayDir.x;
    if (timeMin > timeMax) swap(timeMin, timeMax);

    float timeMinY = (Min.y - vRayPos.y) / vRayDir.y;
    float timeMaxY = (Max.y - vRayPos.y) / vRayDir.y;
    if (timeMinY > timeMaxY) swap(timeMinY, timeMaxY);

    if (timeMinY > timeMax || timeMin > timeMaxY) return false;

    if (timeMinY > timeMin) timeMin = timeMinY;
    if (timeMaxY < timeMax) timeMax = timeMaxY;

    float timeMinZ = (Min.z - vRayPos.z) / vRayDir.z;
    float timeMaxZ = (Max.z - vRayPos.z) / vRayDir.z;
    if (timeMinZ > timeMaxZ) swap(timeMinZ, timeMaxZ);

    if (timeMinZ > timeMax || timeMin > timeMaxZ) return false;

    if (timeMinZ > timeMin) timeMin = timeMinZ;
    if (timeMaxZ < timeMax) timeMax = timeMaxZ;

    //

    if (timeMin > 0.f)
        return true;

    return false;
}

HRESULT CWorldInteractive::Add_Component()
{
    if (FAILED(CRenderObject::Add_Component()))
        return E_FAIL;

    //// Animator
    //m_AnimatorCom = CreateProtoComponent<CAnimator>(this, COMPONENTTYPE::ANIMATOR);
    //NULL_CHECK_RETURN(m_AnimatorCom, E_FAIL);
    //m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::ANIMATOR, m_AnimatorCom });

    //// State
    //m_StateCom = CreateProtoComponent<CState>(this, COMPONENTTYPE::STATE);
    //NULL_CHECK_RETURN(m_StateCom, E_FAIL);
    //m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::STATE, m_StateCom });

    // CombatStat
    m_CombatStatCom = CreateProtoComponent<CCombatStat>(this, COMPONENTTYPE::COMBATSTAT);
    NULL_CHECK_RETURN(m_CombatStatCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::COMBATSTAT, m_CombatStatCom });

    m_CombatStatCom->Set_OnDamagedEvent([this]()
    {
        this->OnInteractiveDamaged();
    });

    // BoxCollider
    m_BoxColCom = CreateProtoComponent<CBoxCollider>(this, COMPONENTTYPE::BOX_COLL);
    NULL_CHECK_RETURN(m_BoxColCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::BOX_COLL, m_BoxColCom });

    // Inventory
    m_InventoryCom = CreateProtoComponent<CInventory>(this, COMPONENTTYPE::INVENTORY);
    NULL_CHECK_RETURN(m_InventoryCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::INVENTORY, m_InventoryCom });

    // effect
    m_SpriteEffCom = CreateProtoComponent<CSpriteEff>(this, COMPONENTTYPE::SPRITEEFFECT);
    NULL_CHECK_RETURN(m_SpriteEffCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::SPRITEEFFECT, m_SpriteEffCom });

    return S_OK;
}

void CWorldInteractive::Free()
{
    CRenderObject::Free();
}

void CWorldInteractive::Check_WorldState()
{
    _vec3 myPos = m_TransformCom->Get_Pos();
    if (auto state = CVillageStateMgr::GetInstance()->Get_InteractiveState(myPos))
    {
        if (m_CombatStatCom)
        {
            m_CombatStatCom->Set_Hp(state->hp);
            if (state->isDead)
            {
                m_CombatStatCom->Set_Hp(0.f); 
                m_CombatStatCom->On_Dead();
                m_bInvenDropped = true;
            }
        }
    }
}

void CWorldInteractive::OnInteractiveDamaged()
{
    SavedInteractive data;
    data.pos = m_TransformCom->Get_Pos();
    data.hp = m_CombatStatCom->Get_Hp();
    data.isDead = (data.hp <= 0.f);

    CVillageStateMgr::GetInstance()->Save_Interactive(data);
}
