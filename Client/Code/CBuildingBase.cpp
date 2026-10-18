#include "pch.h"
#include "CBuildingBase.h"

#include "CBoxCollider.h"
#include "CCreateHelper.h"
#include "CDInputMgr.h"
#include "CPlayer.h"
#include "CRcTex.h"
#include "CRenderer.h"
#include "CSpriteEff.h"
#include "CTransform.h"

CBuildingBase::CBuildingBase(DEVICE graphicDev)
    : CRenderObject(graphicDev)
{ }

CBuildingBase::CBuildingBase(const CBuildingBase& rhs)
    : CRenderObject(rhs)
{ }

CBuildingBase::~CBuildingBase()
{ }

HRESULT CBuildingBase::Ready_GameObject()
{
    if (FAILED(CRenderObject::Ready_GameObject()))
        return E_FAIL;

    m_SpriteEff->InitOutLine(D3DCOLOR_ARGB(255, 255, 255, 255), 0.05f);

    return S_OK;
}

void CBuildingBase::Initialize_GameObject()
{
    CRenderObject::Initialize_GameObject();
}

_int CBuildingBase::Update_GameObject(const _float& timeDelta)
{
    _int exit = CRenderObject::Update_GameObject(timeDelta);

    if (!m_IsPreview)
        CRenderer::GetInstance()->Add_RenderGroup(RENDER_PRIORITY_ALPHA, this);


    m_bHovering = Is_Hovering();

    return exit;
}

void CBuildingBase::LateUpdate_GameObject(const _float& timeDelta)
{
    CRenderObject::LateUpdate_GameObject(timeDelta);
}

void CBuildingBase::Render_GameObject()
{
    if (m_IsPreview)
    {
        if (m_TransformCom)
            m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());

        if (m_BufferCom)
            m_BufferCom->Render_Buffer();
    }
    else
    {

        Render_Setting();

        if (m_bPlayerOverlap == true && m_bHovering == true && m_OutlineEnabled == true)
            m_SpriteEff->OutLineEff(m_BufferCom);

        m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());

        if (m_BufferCom)
            m_BufferCom->Render_Buffer();

        Render_Reset();
    }

    if (m_BoxCollCom)
        m_BoxCollCom->Render();
}


bool CBuildingBase::Is_Hovering()
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

    _vec3 Min = m_BoxCollCom->Get_Transform()->Get_Pos() - m_BoxCollCom->Get_Size() * .5f;
    _vec3 Max = m_BoxCollCom->Get_Transform()->Get_Pos() + m_BoxCollCom->Get_Size() * .5f;


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



void CBuildingBase::OnBeginOverlap(CCollider* self, CCollider* other)
{
    CRenderObject::OnBeginOverlap(self, other);
    CPlayer* ply = dynamic_cast<CPlayer*>(other->Get_Owner());
    if (ply)
        m_bPlayerOverlap = true;
}

void CBuildingBase::OnStayOverlap(CCollider* self, CCollider* other)
{
    CRenderObject::OnStayOverlap(self, other);
}

void CBuildingBase::OnEndOverlap(CCollider* self, CCollider* other)
{
    CRenderObject::OnEndOverlap(self, other);

    CPlayer* ply = dynamic_cast<CPlayer*>(other->Get_Owner());

    if (m_bPlayerOverlap == true && ply != nullptr)
        m_bPlayerOverlap = false;

}

HRESULT CBuildingBase::Add_Component()
{
    if (FAILED(CRenderObject::Add_Component()))
        return E_FAIL;

    // BoxCollider
    m_BoxCollCom = CreateProtoComponent<CBoxCollider>(this, COMPONENTTYPE::BOX_COLL);
    NULL_CHECK_RETURN(m_BoxCollCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::BOX_COLL, m_BoxCollCom });

    // 
    m_SpriteEff = CreateProtoComponent<CSpriteEff>(this, COMPONENTTYPE::SPRITEEFFECT);
    NULL_CHECK_RETURN(m_SpriteEff, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::SPRITEEFFECT, m_SpriteEff });
    return S_OK;
}

void CBuildingBase::Free()
{
    CRenderObject::Free();
}
