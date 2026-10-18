#include "CCameraCalc.h"

#include "CDInputMgr.h"
#include "CTransform.h"

CCameraCalc::CCameraCalc(DEVICE graphicDev)
    : CComponent(graphicDev),

      m_ProjType(PROJ_PERSPECTIVE),
      m_FOV(D3DXToRadian(60.f)),
      m_Aspect(static_cast<_float>(WINCX) / WINCY),
      m_Near(0.2f),
      m_Far(1000.f),
      m_isDirty(true)
{
    D3DXMatrixIdentity(&m_matView);
    D3DXMatrixIdentity(&m_matProj);
}

CCameraCalc::CCameraCalc(const CCameraCalc& rhs)
    : CComponent(rhs),

      m_ProjType(rhs.m_ProjType),
      m_FOV(rhs.m_FOV),
      m_Aspect(rhs.m_Aspect),
      m_Near(rhs.m_Near),
      m_Far(rhs.m_Far),
      m_isDirty(true)
{
    D3DXMatrixIdentity(&m_matView);
    D3DXMatrixIdentity(&m_matProj);
}

CCameraCalc::~CCameraCalc()
{ }

HRESULT CCameraCalc::Ready_CameraCalc()
{
    return S_OK;
}

void CCameraCalc::Initialize_Component()
{
    // 초기 투영 행렬 계산
    Update_Proj_Matrix();
}

_int CCameraCalc::Update_Component(const _float& timeDelta)
{
    return 0;
}

void CCameraCalc::LateUpdate_Component()
{
    NULL_CHECK(m_GraphicDev);

    Update_View_Matrix();
    m_GraphicDev->SetTransform(D3DTS_VIEW, &m_matView);

    if (m_isDirty)
    {
        Update_Proj_Matrix();
        m_GraphicDev->SetTransform(D3DTS_PROJECTION, &m_matProj);

        m_isDirty = false;
    }
}

void CCameraCalc::Set_ProjType(PROJECTION_TYPE projType)
{
    if (m_ProjType != projType)
    {
        m_ProjType = projType;
        m_isDirty  = true;
    }
}

void CCameraCalc::Update_View_Matrix()
{
    NULL_CHECK(m_TransformCom);

    _vec3 eye  = m_TransformCom->Get_Pos();
    _vec3 look = m_TransformCom->Get_Look();
    _vec3 up   = m_TransformCom->Get_Up();
    _vec3 at   = eye + look;

    D3DXMatrixLookAtLH(&m_matView, &eye, &at, &up);
}

void CCameraCalc::Update_Proj_Matrix()
{
    switch (m_ProjType)
    {
    case PROJ_PERSPECTIVE:
    {
        D3DXMatrixPerspectiveFovLH(&m_matProj, m_FOV, m_Aspect, m_Near, m_Far);
    }
    break;
    case PROJ_ORTHOGRAPHIC:
    {
        D3DVIEWPORT9 viewport;
        m_GraphicDev->GetViewport(&viewport);

        D3DXMatrixOrthoLH(
            &m_matProj,
            static_cast<_float>(viewport.Width),
            static_cast<_float>(viewport.Height),
            m_Near,
            m_Far);
    }
    break;
    default: break;
    }
}

CCameraCalc* CCameraCalc::Create(DEVICE graphicDev)
{
    auto CameraCom = new CCameraCalc(graphicDev);

    if (FAILED(CameraCom->Ready_CameraCalc()))
    {
        Safe_Release(CameraCom);
        MSG_BOX("CameraCalc Create Failed");
        return nullptr;
    }

    return CameraCom;
}

CComponent* CCameraCalc::Clone()
{
    return new CCameraCalc(*this);
}

void CCameraCalc::Free()
{
    CComponent::Free();
}
