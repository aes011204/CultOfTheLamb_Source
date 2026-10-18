#include "pch.h"
#include "CMainCamera.h"

#include "CCameraCalc.h"
#include "CCameraController.h"
#include "CCameraMovement.h"
#include "CCreateHelper.h"
#include "CDInputMgr.h"
#include "CTransform.h"

CMainCamera::CMainCamera(DEVICE graphicDev)
    : CGameObject(graphicDev),

      m_TransformCom(nullptr),
      m_CamCalc(nullptr),
      m_CamController(nullptr),
      m_CamMovement(nullptr)
{ }

CMainCamera::CMainCamera(const CMainCamera& rhs)
    : CGameObject(rhs),

      m_TransformCom(nullptr),
      m_CamCalc(nullptr),
      m_CamController(nullptr),
      m_CamMovement(nullptr)
{ }

CMainCamera::~CMainCamera()
{ }

HRESULT CMainCamera::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    return S_OK;
}

_int CMainCamera::Update_GameObject(const _float& timeDelta)
{
    _int exit = CGameObject::Update_GameObject(timeDelta);

    return exit;
}

void CMainCamera::LateUpdate_GameObject(const _float& timeDelta)
{
    CGameObject::LateUpdate_GameObject(timeDelta);
}

void CMainCamera::Render_Editor()
{
    CGameObject::Render_Editor();

    ImGui::Text("=== Main Camera === ");
}

HRESULT CMainCamera::Add_Component()
{
    // transform
    m_TransformCom = CreateProtoComponent<CTransform>(this, COMPONENTTYPE::TRANSFORM);
    NULL_CHECK_RETURN(m_TransformCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::TRANSFORM, m_TransformCom });

    // Camera Calculator
    m_CamCalc = CreateProtoComponent<CCameraCalc>(this, COMPONENTTYPE::CAM_CALC);
    NULL_CHECK_RETURN(m_CamCalc, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::CAM_CALC, m_CamCalc });

    // Camera Movement
    m_CamMovement = CreateProtoComponent<CCameraMovement>(this, COMPONENTTYPE::CAM_MOVEMENT);
    NULL_CHECK_RETURN(m_CamMovement, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::CAM_MOVEMENT, m_CamMovement });

    // Camera Controller
    m_CamController = CreateProtoComponent<CCameraController>(this, COMPONENTTYPE::CAM_CONTROLLER);
    NULL_CHECK_RETURN(m_CamController, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::CAM_CONTROLLER, m_CamController });

    return S_OK;
}

CMainCamera* CMainCamera::Create(DEVICE graphicDev)
{
    auto mainCamera = new CMainCamera(graphicDev);

    if (FAILED(mainCamera->Ready_GameObject()))
    {
        MSG_BOX("MainCamera Create Failed");
        Safe_Release(mainCamera);
        return nullptr;
    }

    return mainCamera;
}

void CMainCamera::Free()
{
    m_TransformCom  = nullptr;
    m_CamCalc       = nullptr;
    m_CamController = nullptr;
    m_CamMovement   = nullptr;

    CGameObject::Free();
}
