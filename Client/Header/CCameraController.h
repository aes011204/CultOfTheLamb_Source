#pragma once
#include "CComponent.h"

BEGIN(Engine)
class CCameraCalc;
class CTransform;
END

class CCameraMovement;

/// @brief 카메라의 전반적인 동작 모드(Target/Free)를 관리하고 제어하는 컨트롤러 역할의 다이나믹 컴포넌트
/// @details
/// 사용자 입력(키보드/마우스)을 받아 카메라를 이동시키거나,
/// 특정 게임 상황에 맞춰 카메라 모드를 전환하는 역할을 수행한다.
/// CCameraCalc와 CCameraMovement 컴포넌트와 연동하여 동작한다.
class CCameraController : public CComponent
{
private:
    explicit CCameraController(DEVICE graphicDev);
    explicit CCameraController(const CCameraController& rhs);
    ~CCameraController() override;

public:
    HRESULT Ready_CameraController();
    void    Initialize_Component() override;
    _int    Update_Component(const _float& timeDelta) override;
    void    LateUpdate_Component() override;

public:  // --- Getter ---
    CAM_MODE Get_CamMode() const { return m_CamMode; } //  현재 카메라 모드 반환

public:  // --- Getter : Imgui 관리용. 타 용도로 사용 금지 ---
    Engine::CCameraCalc* Get_OwnerCamCalc() const { return m_OwnerCamCalcCom; }
    CCameraMovement*     Get_OwnerCamMovement() const { return m_OwnerCamMovementCom; }

public:  // --- Setter ---
    /// @brief 추적할 타겟 설정
    /// @param targetTransform  타겟의 Transform 컴포넌트
    void Set_CamTarget(Engine::CTransform* targetTransform);

    /// @brief 카메라 모드 변경 (Target Mode <-> Free Mode)
    /// @param camMode  변경할 카메라 모드
    void Set_CamMode(CAM_MODE camMode);

    /// @brief 투영 타입 변경 (Perspective <-> Orthographic)
    /// @param projType  변경할 투영 타입
    void Set_ProjType(PROJECTION_TYPE projType);

private: // --- 내부 로직 ---

    // [자유 모드] WASD 키 입력에 따른 카메라 이동 처리
    void Control_FreeMode(const _float& timeDelta);

    // [타겟 모드] 타겟 추적 처리 및 예외 상황 핸들링
    void Control_TargetMode(const _float& timeDelta);

private: // --- Owner 컴포넌트 캐싱 ---
    Engine::CCameraCalc* m_OwnerCamCalcCom;
    CCameraMovement*     m_OwnerCamMovementCom;

private: // --- 일반 멤버 변수 ---
    CAM_MODE m_CamMode;

    _float m_FreeMoveSpeed; // 자유 시점 이동 속도
    _float m_FreeRotSpeed;  // 자유 시점 회전 속도 (Degree/sec)

public:
    static CCameraController* Create(DEVICE graphicDev);
    CComponent*               Clone() override;

private:
    void Free() override;


};
