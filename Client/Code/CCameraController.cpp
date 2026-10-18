#include "pch.h"
#include "CCameraController.h"

#include "CCameraCalc.h"
#include "CCameraMovement.h"
#include "CDInputMgr.h"
#include "CGameObject.h"
#include "CTransform.h"

CCameraController::CCameraController(DEVICE graphicDev)
    : CComponent(graphicDev),

      m_OwnerCamCalcCom(nullptr),
      m_OwnerCamMovementCom(nullptr),

      m_CamMode(CAM_FREE),
      m_FreeMoveSpeed(20.f),
      m_FreeRotSpeed(90.f) // 초당 90도 회전
{ }

CCameraController::CCameraController(const CCameraController& rhs)
    : CComponent(rhs),

      m_OwnerCamCalcCom(nullptr),
      m_OwnerCamMovementCom(nullptr),

      m_CamMode(rhs.m_CamMode),
      m_FreeMoveSpeed(rhs.m_FreeMoveSpeed),
      m_FreeRotSpeed(rhs.m_FreeRotSpeed)
{ }

CCameraController::~CCameraController()
{ }

HRESULT CCameraController::Ready_CameraController()
{
    return S_OK;
}

void CCameraController::Initialize_Component()
{
    m_OwnerCamCalcCom = dynamic_cast<CCameraCalc*>(
        m_Owner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::CAM_CALC));

    m_OwnerCamMovementCom = dynamic_cast<CCameraMovement*>(
        m_Owner->Get_Component(ID_STATIC, COMPONENTTYPE::CAM_MOVEMENT));

    NULL_CHECK_MSG(
        m_OwnerCamCalcCom,
        L"CCameraController: Owner's CamCalc caching has failed");

    NULL_CHECK_MSG(
        m_OwnerCamMovementCom,
        L"CCameraController: Owner's CamMovement caching has failed");
}

_int CCameraController::Update_Component(const _float& timeDelta)
{
    NULL_CHECK_RETURN(m_OwnerCamCalcCom, -1);
    NULL_CHECK_RETURN(m_OwnerCamMovementCom, -1);

    switch (m_CamMode)
    {
    case CAM_FREE:
        Control_FreeMode(timeDelta);
        break;

    case CAM_TARGET:
        Control_TargetMode(timeDelta);
        break;

    default: break;
    }

    m_OwnerCamMovementCom->Check_TransitionComplete(1.f);

    return 0;
}

void CCameraController::LateUpdate_Component()
{ }

void CCameraController::Set_CamTarget(Engine::CTransform* targetTransform)
{
    if (nullptr == m_OwnerCamMovementCom)
    {
        m_OwnerCamMovementCom = dynamic_cast<CCameraMovement*>(
            m_Owner->Get_Component(ID_STATIC, COMPONENTTYPE::CAM_MOVEMENT));
    }

    NULL_CHECK(m_OwnerCamMovementCom);

    // Movement 컴포넌트에 타겟 정보 갱신
    m_OwnerCamMovementCom->Set_Target(targetTransform);

    // 타겟 유무에 따른 모드 자동 전환
    if (nullptr != targetTransform)
    {
        m_CamMode = CAM_TARGET;
        // m_OwnerCamMovementCom->Reset_Camera();
    }
    else
        m_CamMode = CAM_FREE;
}

void CCameraController::Set_CamMode(CAM_MODE camMode)
{
    if (m_CamMode == camMode) return;

    m_CamMode = camMode;
}

void CCameraController::Set_ProjType(PROJECTION_TYPE projType)
{
    if (nullptr == m_OwnerCamCalcCom)
    {
        m_OwnerCamCalcCom = dynamic_cast<CCameraCalc*>(
            m_Owner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::CAM_CALC));
    }

    NULL_CHECK(m_OwnerCamCalcCom);

    m_OwnerCamCalcCom->Set_ProjType(projType);
}

void CCameraController::Control_FreeMode(const _float& timeDelta)
{
    NULL_CHECK(m_OwnerCamCalcCom);
    NULL_CHECK(m_OwnerCamMovementCom);

    auto inputMgr = CDInputMgr::GetInstance();

    // isRShiftPressed == true 면 회전 모드
    bool isRShiftPressed = (inputMgr->Get_DIKeyState(DIK_RSHIFT) & 0x80);

    if (isRShiftPressed)
    {
        // ============================
        // [회전] Shift + 방향키
        // ============================
        _float rotSpeedRad = D3DXToRadian(m_FreeRotSpeed) * timeDelta;

        // 상하 키 -> X축 회전 (Pitch)
        if (inputMgr->Get_DIKeyState(DIK_UP) & 0x80)
            m_OwnerCamMovementCom->RotateX_FreeMode(-rotSpeedRad); // 위쪽을 보도록

        if (inputMgr->Get_DIKeyState(DIK_DOWN) & 0x80)
            m_OwnerCamMovementCom->RotateX_FreeMode(rotSpeedRad);  // 아래쪽을 보도록

        // 좌우 키 -> Y축 회전 (Yaw)
        if (inputMgr->Get_DIKeyState(DIK_LEFT) & 0x80)
            m_OwnerCamMovementCom->RotateY_FreeMode(-rotSpeedRad);

        if (inputMgr->Get_DIKeyState(DIK_RIGHT) & 0x80)
            m_OwnerCamMovementCom->RotateY_FreeMode(rotSpeedRad);
    }
    else
    {
        // ============================
        // [이동] 방향키 + PageUp/Dn
        // ============================
        _vec3 moveDir(0.f, 0.f, 0.f);

        // Z축 (전후)
        if (inputMgr->Get_DIKeyState(DIK_UP) & 0x80) moveDir.z += 1.f;
        if (inputMgr->Get_DIKeyState(DIK_DOWN) & 0x80) moveDir.z -= 1.f;

        // X축 (좌우)
        if (inputMgr->Get_DIKeyState(DIK_RIGHT) & 0x80) moveDir.x += 1.f;
        if (inputMgr->Get_DIKeyState(DIK_LEFT) & 0x80) moveDir.x -= 1.f;

        // Y축 (승강) - PageUp / PageDown
        if (inputMgr->Get_DIKeyState(DIK_PGUP) & 0x80) moveDir.y += 1.f;
        if (inputMgr->Get_DIKeyState(DIK_PGDN) & 0x80) moveDir.y -= 1.f;

        if (D3DXVec3LengthSq(&moveDir) > 0.f)
        {
            D3DXVec3Normalize(&moveDir, &moveDir);
            m_OwnerCamMovementCom->Move_FreeMode(moveDir, m_FreeMoveSpeed, timeDelta);
        }
    }
}

void CCameraController::Control_TargetMode(const _float& timeDelta)
{
    if (false == m_OwnerCamMovementCom->Update_TargetMode(timeDelta))
        m_CamMode = CAM_FREE;
}

CCameraController* CCameraController::Create(DEVICE graphicDev)
{
    auto camController = new CCameraController(graphicDev);

    if (FAILED(camController->Ready_CameraController()))
    {
        Safe_Release(camController);
        MSG_BOX("CameraController Create Failed");
        return nullptr;
    }

    return camController;
}

CComponent* CCameraController::Clone()
{
    return new CCameraController(*this);
}

void CCameraController::Free()
{
    m_OwnerCamCalcCom     = nullptr;
    m_OwnerCamMovementCom = nullptr;

    CComponent::Free();
}
