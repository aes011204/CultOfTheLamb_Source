#include "pch.h"
#include "CTarotNPC.h"
#include <CRcTex.h>
#include "CTexture.h"
#include "CCreateHelper.h"
#include "CCharacterBase.h"
#include "CTransform.h"
#include "CAnimator.h"
#include "CPlayer.h"
#include "CState.h"
#include "CCollider.h"
#include "CDInputMgr.h"
#include "CBoxCollider.h"
#include "CTarotStageController.h"
#include "CRenderer.h"
#include "CCameraController.h"
#include "CManagement.h"
#include "CCamSetupBuilder.h"
#include "CLayer.h"

CTarotNPC::CTarotNPC(DEVICE graphicDev)
    : CCharacterBase(graphicDev),
      m_bPlayerOverlap(false),
      m_bHovering(false),
      m_TarotStageController(nullptr) {}

CTarotNPC::CTarotNPC(const CTarotNPC& rhs)
    : CCharacterBase(rhs),
      m_bPlayerOverlap(false),
      m_bHovering(false),
      m_TarotStageController(nullptr) {}

CTarotNPC::~CTarotNPC() {}

void CTarotNPC::Render_Reset()
{
    CCharacterBase::Render_Reset();
}

HRESULT CTarotNPC::Ready_GameObject()
{
    if (FAILED(CCharacterBase::Ready_GameObject()))
        return E_FAIL;

    m_TransformCom->Set_Pos(181.f, 2.f, 30.5f);
    m_TransformCom->Set_Scale(3.f, 3.f, 3.f);
    m_BoxColCom->Set_RawSize(_vec3(2.f, 2.f,2.f));

    m_BoxColCom->Set_CollisionLayer(COLLISION_LAYERTYPE::ITEM);

    m_TransformCom->Set_RotationAxis(ROT_X, D3DXToRadian(40.f));

    return S_OK;
}

void CTarotNPC::Initialize_GameObject()
{
    CCharacterBase::Initialize_GameObject();
}

_int CTarotNPC::Update_GameObject(const _float& timeDelta)
{
    int exit = CCharacterBase::Update_GameObject(timeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDERID::RENDER_ALPHA, this);

    m_bHovering = Is_Hovering();

    if(m_bPlayerOverlap == true && m_bHovering == true)
    {
        if(CDInputMgr::GetInstance()->KeyDown(DIK_E))
        {
            m_TarotStageController->Set_Active(true);
        }

        //TODO 유림 대사
        //타로 카드 고르는 거 연결

    }

    return exit;
}

void CTarotNPC::LateUpdate_GameObject(const _float& timeDelta)
{
    CCharacterBase::LateUpdate_GameObject(timeDelta);
}

void CTarotNPC::Render_GameObject()
{
    CCharacterBase::Render_GameObject();

    m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());

    Render_Setting();

    if (m_TextureCom && m_AnimatorCom)
    {
        _int           frame = m_AnimatorCom->Get_CurFrame();
        m_TextureCom->Set_Texture(L"TarotNpcIdle", frame);
    }

    m_BufferCom->Render_Buffer();

    Render_Reset();
}

bool CTarotNPC::Is_Hovering()
{
    ///////////
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
     3DXMatrixInverse(&matWorld, 0, &matWorld);
    
     3DXVec3TransformCoord(&vRayPos, &vRayPos, &matWorld);
     3DXVec3TransformNormal(&vRayDir, &vRayDir, &matWorld);*/
    
    //_vec3 half = m_BoxColCom->Get_RawSize() * .5f;
    
    _vec3 Min = m_BoxColCom->Get_Transform()->Get_Pos() - m_BoxColCom->Get_Size() * .5f;
    _vec3 Max = m_BoxColCom->Get_Transform()->Get_Pos() + m_BoxColCom->Get_Size() * .5f;
    
    
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

void CTarotNPC::OnBeginOverlap(CCollider* self, CCollider* other)
{
    CCharacterBase::OnBeginOverlap(self, other);

//   auto* ply = dynamic_cast<CPlayer*>(other ? other->Get_Owner() : nullptr);
//   if (!ply) return;

    m_bPlayerOverlap = true;

   CTransform* playerTransform = dynamic_cast<CTransform*>(CManagement::GetInstance()->Get_Component(ID_DYNAMIC, LAYERTYPE::GAMELOGIC, OBJTYPE::PLAYER, COMPONENTTYPE::TRANSFORM));

    auto* scene = CManagement::GetInstance()->Get_Scene();
    if (!scene) return;

    auto* layer = scene->Get_Layer(LAYERTYPE::GAMELOGIC);
    if (!layer) return;

    auto* camController = dynamic_cast<CCameraController*>(
        layer->Get_Component(ID_DYNAMIC, OBJTYPE::CAMERA, COMPONENTTYPE::CAM_CONTROLLER)
        );
    if (!camController) return;

    CCamSetupBuilder(camController)
        .SetTarget(playerTransform)
        .PivotOffset(_vec3{ 0.f, 0.f, 6.7f })
        .RotX(45.f)
        .Distance(18.5f)
        .LagSpeed(5.f)     
        .ZoomSpeed(10.f)
        .Apply();
}

void CTarotNPC::OnEndOverlap(CCollider* self, CCollider* other)
{
    CCharacterBase::OnEndOverlap(self, other);

    //CPlayer* ply = dynamic_cast<CPlayer*>(other->Get_Owner());

    if (m_bPlayerOverlap == true)
        m_bPlayerOverlap = false;

    CTransform* playerTransform = dynamic_cast<CTransform*>(CManagement::GetInstance()->Get_Component(ID_DYNAMIC, LAYERTYPE::GAMELOGIC, OBJTYPE::PLAYER, COMPONENTTYPE::TRANSFORM));

    auto* scene = CManagement::GetInstance()->Get_Scene();
    if (!scene) return;

    auto* layer = scene->Get_Layer(LAYERTYPE::GAMELOGIC);
    if (!layer) return;

    auto* camController = dynamic_cast<CCameraController*>(
        layer->Get_Component(ID_DYNAMIC, OBJTYPE::CAMERA, COMPONENTTYPE::CAM_CONTROLLER));

    if (!camController) return;

           // 플레이어한테 줌인
           CCamSetupBuilder(camController)
               .SetTarget(playerTransform)
			.PivotOffset(_vec3{0.f,0.f,0.f})
			.Distance(12.f)
			.RotX(45.f)
			.RotY(0.f)
			.LagSpeed(10.f)
			.ZoomSpeed(5.f)
               .Apply();
        
    
}

void CTarotNPC::OnStayOverlap(CCollider* self, CCollider* other)
{
    CCharacterBase::OnStayOverlap(self, other);
}

CTarotNPC* CTarotNPC::Create(DEVICE graphicDev)
{
    auto tarotNPC = new CTarotNPC(graphicDev);

    if(FAILED(tarotNPC->Ready_GameObject()))
    {
        MSG_BOX("TarotNPC Create Failed");
        Safe_Release(tarotNPC);
        return nullptr;
    }
    return tarotNPC;
}

void CTarotNPC::Animation_Setting()
{
    m_AnimatorCom->Create_Animation(L"TarotNpcIdle", 86, 0.02f);
    m_AnimatorCom->Play_Animation(L"TarotNpcIdle", ANIMSTATE::LOOP, true);
  //  m_StateCom->Set_AnimInfo(ACTORSTATE::IDLE, L"TarotNpcIdle", ANIMSTATE::LOOP);

}

HRESULT CTarotNPC::Add_Component()
{
    if(FAILED(CCharacterBase::Add_Component()))
        return E_FAIL;

    //Animator,State,BoxCollider은 CharacterBase에 있음
    // Texture
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_TAROTNPC);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);

    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_TAROTNPC, m_TextureCom });

    m_AnimatorCom->Set_TextureType(COMPONENTTYPE::TEX_TAROTNPC);

    return S_OK;
}

void CTarotNPC::Render_Setting()
{
    CCharacterBase::Render_Setting();
}

void CTarotNPC::Free()
{
    CCharacterBase::Free();
}
