#pragma once
#include "CComponent.h"

BEGIN(Engine)
class CTransform;
END

// 카메라 설정 프리셋 구조체
typedef struct CameraPreset
{
    // 타겟 오프셋
    _vec3 pivotOffset = { 0.f, 0.f, 0.f };
    // 거리
    _float dist = 12.f;
    // X축 회전
    _float pitch = D3DXToRadian(45.f);
    // Y축 회전
    _float yaw = 0.f;
    // 추적 속도
    _float lagSpeed = 10.f;
    // 줌 속도
    _float zoomSpeed = 5.f;

    // 기본 생성자
    CameraPreset() = default;

    // 사용자 정의 생성자
    CameraPreset(_vec3  _offset,
                 _float _dist,
                 _float _rotX,
                 _float _rotY,
                 _float _lagSpeed,
                 _float _zoomSpeed)
        : pivotOffset(_offset),
          dist(_dist),
          pitch(_rotX),
          yaw(_rotY),
          lagSpeed(_lagSpeed),
          zoomSpeed(_zoomSpeed) {}
} CAM_PRESET;

// 네임 스페이스 CameraPresets 안에 프리셋 저장
BEGIN(CameraPresets)
inline const CAM_PRESET Default = CAM_PRESET();

// 예시
inline const CAM_PRESET Combat =
    { _vec3(0, 1, 0), 6.f, D3DXToRadian(20.f), 0.f, 10.f, 5.f };

// 예시
inline const CAM_PRESET Boss =
    { _vec3(0, 5, 0), 18.f, D3DXToRadian(60.f), 0.f, 10.f, 5.f };
END

/// @brief 카메라의 움직임(이동, 회전, 추적, 흔들림)을 제어하는 컴포넌트
/// @details
/// 타겟 추적(Target Mode)과 자유 시점(Free Mode) 이동 로직을 제공한다.
/// 카메라 프리셋(CameraPreset)을 통해 다양한 시점 설정을 손쉽게 적용할 수 있다.
/// 카메라 흔들림(Shake) 효과를 지원한다.
class CCameraMovement : public CComponent
{
private:
    explicit CCameraMovement(DEVICE graphicDev);
    explicit CCameraMovement(const CCameraMovement& rhs);
    ~CCameraMovement() override;

public:
    HRESULT Ready_CameraMovement();
    void    Initialize_Component() override;
    _int    Update_Component(const _float& timeDelta) override;
    void    LateUpdate_Component() override;

public:  // --- Getter ---
    const _vec3& Get_PivotOffset() const { return m_CamPreset.pivotOffset; }
    _float       Get_TargetDistance() const { return m_CamPreset.dist; }
    _float       Get_RotX() const { return m_CamPreset.pitch; }
    _float       Get_RotY() const { return m_CamPreset.yaw; }
    _float       Get_LagSpeed() const { return m_CamPreset.lagSpeed; }
    _float       Get_ZoomSpeed() const { return m_CamPreset.zoomSpeed; }

    const CAM_PRESET& Get_Preset() const { return m_CamPreset; }

public:  // --- Setter ---
    /// @brief 추적할 타겟의 Transform 컴포넌트 설정
    /// @param targetTransform 타겟 Transform 포인터
    void Set_Target(CTransform* targetTransform);

    void Set_PivotOffset(const _vec3& offset) { m_CamPreset.pivotOffset = offset; }
    void Set_TargetDistance(_float dist) { m_CamPreset.dist = dist; }
    void Set_RotX(_float rotX);
    void Set_RotY(_float rotY) { m_CamPreset.yaw = rotY; }
    void Set_LagSpeed(_float speed) { m_CamPreset.lagSpeed = speed; }
    void Set_ZoomSpeed(_float speed) { m_CamPreset.zoomSpeed = speed; }

public:  // --- 공용 기능 ---

    /// @brief CAM_PRESET 구조체로 프리셋 적용
    /// @param preset     카메라 프리셋 구조체
    /// @param snap       true면 보간 없이 즉시 이동
    void Apply_Preset(const CAM_PRESET& preset, _bool snap = false);

    /// @brief 카메라 상태 초기화 (Reset 용도: 스테이지 진입 등)
    /// @param preset  초기화할 프리셋 (기본값: 기본 생성자)
    /// @param snap    true면 보간 없이 즉시 이동
    void Reset_Camera(const CAM_PRESET& preset = CAM_PRESET(), _bool snap = true) { Apply_Preset(preset, snap); }

    /// @brief 카메라 흔들기 기능 시작
    /// @param amplitude  진동 크기 (강도)
    /// @param decay      감쇠 속도 (클수록 빨리 멈춤)
    /// @param freqX      좌우(Horizontal) 진동수
    /// @param freqY      상하(Vertical) 진동수
    void Start_CamShake(_float amplitude = 0.5f, _float decay = 1.f, _float freqX = 50.f, _float freqY = 25.f);

public:  // --- 자유 모드 기능 ---

    /// @brief [자유 모드] 카메라 이동
    /// @param dir        이동 방향 벡터
    /// @param speed      이동 속도
    /// @param timeDelta  시간 델타
    void Move_FreeMode(const _vec3& dir, _float speed, _float timeDelta);

    /// @brief [자유 모드] X축 회전 (Pitch)
    /// @param angleDelta  매 프레임 회전할 각도 (Radian * timeDelta)
    void RotateX_FreeMode(_float angleDelta);

    /// @brief [자유 모드] Y축 회전 (Yaw)
    /// @param angleDelta  매 프레임 회전할 각도 (Radian * timeDelta)
    void RotateY_FreeMode(_float angleDelta);

public:  // --- 타겟 모드 기능 ---

    /// @brief [타겟 모드] 타겟을 따라가는 이동 로직 수행
    /// @details 
    /// 타겟의 위치와 설정된 오프셋, 거리, 각도를 기반으로 목표 위치를 계산한다.
    /// 현재 위치에서 목표 위치로 LagSpeed를 적용하여 부드럽게 보간 이동한다.
    /// @param timeDelta 시간 델타
    /// @return 성공적으로 위치 갱신시 true, 타겟 유실 등 문제 발생시 false
    _bool Update_TargetMode(const _float& timeDelta);

private: // --- 내부 기능 ---
    void Clamp_Pitch(); // Pitch 각도 제한 ( -89도 ~ 89도 )
    void Update_Shake(const _float& timeDelta); // 카메라 흔들림 효과 갱신

private: // --- 외부 컴포넌트 캐싱 ---
    Engine::CTransform* m_TargetTransformCom;

private: // --- 멤버 변수 ---
    CAM_PRESET m_CamPreset;

private: // --- 현재 값 : 보간 계산 중 현재 적용되는 값 ---
    _vec3  m_CurrentPivotPos;   // 현재 바라보고 있는 피벗 위치
    _float m_CurrentDistance;   // 현재 카메라 거리

private: // --- 카메라 흔들기 변수 ---
    _float m_ShakeAmp;          // 현재 진폭
    _float m_ShakeDecay;        // 감쇠 속도
    _float m_ShakeFreqX;        // 좌우 진동수
    _float m_ShakeFreqY;        // 상하 진동수
    _float m_ShakeTimeAcc;      // 시간 누적
    _vec3  m_ShakeOffset;       // 계산된 오프셋

public:
    static CCameraMovement* Create(DEVICE graphicDev);
    CComponent*             Clone() override;

private:
    void Free() override;

#pragma region 카메라 연출

public:
    using OnTransitionEnd = function<void()>;
    void  Set_OnTransitionEnd(OnTransitionEnd callback);

    _bool Check_TransitionComplete(_float threshold = 0.1f);

    void Start_Transition()
    {
        m_IsInTransition = true;
    }

private:
    _bool           m_IsInTransition = false;  // 연출 중인지?
    OnTransitionEnd m_OnTransitionEnd = nullptr;

public:
    void        Update_DynamicTargetMode(const _float& timeDelta);
    void        Set_SecondTarget(CTransform* transform) { m_SecondTargetTransform = transform; }
    void        Set_DynamicDistanceParams(_float camMin, _float camMax, _float distMin, _float distMax);

    void        Enable_DynamicDistance(_bool enable) { m_UseDynamicDistance = enable; }
    void        Disable_DynamicDistance();


private:
    // 거리 비교해서 카메라 Distance 조절
    CTransform* m_SecondTargetTransform = nullptr;
    _bool       m_UseDynamicDistance = false;

    // 카메라 거리 최소, 최대치
    _float      m_DynamicCamDistMin = 12.f;
    _float      m_DynamicCamDistMax = 22.f;

    // 플레이어 - 보스 거리 범위
    _float      m_PlayerDistMin = 5.f;
    _float      m_PlayerDistMax = 20.f;


#pragma endregion
    public:
    void Ease_ToPreset(CameraPreset& camPreset, float duration, function<float(float)>);

    void Update_Ease(const _float& timeDelta);


    private:
    bool m_UsePresetEase = false;
    float m_EaseTime=0;
    float m_EaseDuration=1;

    CameraPreset m_EaseStartPreset;
    CameraPreset m_EaseEndPreset;

    function<float(float)> m_easeFunc;
};

