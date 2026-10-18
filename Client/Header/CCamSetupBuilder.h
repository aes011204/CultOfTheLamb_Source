#pragma once
#include "CCameraController.h"
#include "CCameraMovement.h"
#include "CGameObject.h"

// Builder 패턴을 이용한 CCameraMovement의 값 설정 클래스
class CCamSetupBuilder
{
public:
    /// @brief CCameraController를 받아와 카메라 타겟 설정 및 값 초기화
    /// @param camController  CCameraController 컴포넌트의 포인터
    /// @param LoadCurrent    true면 현재 카메라 설정을 가져와 추가 수정 (기본 true)
    CCamSetupBuilder(CCameraController* camController, _bool LoadCurrent = true)
        : m_CamController(camController),
          m_CamMovementCom(nullptr),
          m_TargetTransform(nullptr),
          m_Snap(false)
    {
        if (m_CamController)
        {
            // 컨트롤러를 통해 Movement 컴포넌트 확보
            m_CamMovementCom = dynamic_cast<CCameraMovement*>(
                m_CamController->Get_Owner()->Get_Component(ID_STATIC, COMPONENTTYPE::CAM_MOVEMENT));
        }

        if (LoadCurrent && m_CamMovementCom)
            m_CamPreset = m_CamMovementCom->Get_Preset();
        else
            m_CamPreset = CameraPresets::Default;
    }

    // --- 체이닝 메서드 : CameraPresets 이용 ---

    // 프리셋 통째로 로드
    // [현재 사용 안함]
    //CCamSetupBuilder& LoadPreset(const CAM_PRESET& preset)
    //{
    //    m_CamPreset = preset;
    //    return *this;
    //}

    // --- 체이닝 메서드 : 개별 제어 ---

    // 타겟 설정 (Null 이면 FreeMode로 설정됨)
    CCamSetupBuilder& SetTarget(Engine::CTransform* target)
    {
        m_TargetTransform = target;
        return *this;
    }

    // 타겟의 어느 지점을 바라볼건지에 대한 로컬 오프셋 설정
    CCamSetupBuilder& PivotOffset(const _vec3& offset)
    {
        m_CamPreset.pivotOffset = offset;
        return *this;
    }

    // 타겟과의 거리 설정
    CCamSetupBuilder& Distance(_float dist)
    {
        m_CamPreset.dist = dist;
        return *this;
    }

    // X축 회전 (Pitch) 설정 [단위: Degree]
    CCamSetupBuilder& RotX(_float angleDegree)
    {
        m_CamPreset.pitch = D3DXToRadian(angleDegree);
        return *this;
    }

    // Y축 회전 (Yaw) 설정 [단위: Degree]
    CCamSetupBuilder& RotY(_float angleDegree)
    {
        m_CamPreset.yaw = D3DXToRadian(angleDegree);
        return *this;
    }

    // 위치 추적 속도 (값이 클수록 빠르게 따라감)
    CCamSetupBuilder& LagSpeed(_float speed)
    {
        m_CamPreset.lagSpeed = speed;
        return *this;
    }

    // 줌 변경 속도 (값이 클수록 빠르게 변경됨)
    CCamSetupBuilder& ZoomSpeed(_float speed)
    {
        m_CamPreset.zoomSpeed = speed;
        return *this;
    }

    // 초기에 카메라를 타겟 위치로 즉시 이동
    // 초기 세팅용으로는 사용하고, 연출용으로는 보간 적용을 위해 사용하지 않는 것을 추천
    CCamSetupBuilder& Snap()
    {
        m_Snap = true;
        return *this;
    }

    // --- 최종 적용 ---

    // 설정한 값을 컴포넌트에 반영 (※ 반드시 마지막에 호출할 것)
    void Apply()
    {
        if (m_CamController && m_TargetTransform)
            m_CamController->Set_CamTarget(m_TargetTransform);
        else
            m_CamController->Set_CamTarget(nullptr);

        if (m_CamMovementCom)
        {
            m_CamMovementCom->Apply_Preset(m_CamPreset, m_Snap);

            if (!m_Snap) // Snap이 아닐 때에만, 연출 시작
                m_CamMovementCom->Start_Transition();
        }
    }

private:
    CCameraController* m_CamController;
    CCameraMovement*   m_CamMovementCom;

    Engine::CTransform* m_TargetTransform;

    CAM_PRESET m_CamPreset;
    _bool      m_Snap;
};
