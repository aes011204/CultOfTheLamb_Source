#include "pch.h"
#include "CCameraMovement.h"

#include "CCameraController.h"
#include "CTransform.h"

CCameraMovement::CCameraMovement(DEVICE graphicDev)
    : CComponent(graphicDev),

      m_TargetTransformCom(nullptr),

      m_CurrentPivotPos(0.f, 0.f, 0.f),
      m_CurrentDistance(0.f),

      m_ShakeAmp(0.f),
      m_ShakeDecay(0.f),
      m_ShakeFreqX(0.f),
      m_ShakeFreqY(0.f),
      m_ShakeTimeAcc(0.f),
      m_ShakeOffset(0.f, 0.f, 0.f)
{ }

CCameraMovement::CCameraMovement(const CCameraMovement& rhs)
    : CComponent(rhs),

      m_TargetTransformCom(nullptr),

      m_CamPreset(rhs.m_CamPreset),
      m_CurrentPivotPos(rhs.m_CurrentPivotPos),
      m_CurrentDistance(rhs.m_CurrentDistance),

      m_ShakeAmp(0.f),
      m_ShakeDecay(0.f),
      m_ShakeFreqX(0.f),
      m_ShakeFreqY(0.f),
      m_ShakeTimeAcc(0.f),
      m_ShakeOffset(0.f, 0.f, 0.f)
{ }

CCameraMovement::~CCameraMovement()
{ }

HRESULT CCameraMovement::Ready_CameraMovement()
{
    return S_OK;
}

void CCameraMovement::Initialize_Component()
{ }

_int CCameraMovement::Update_Component(const _float& timeDelta)
{
    return 0;
}

void CCameraMovement::LateUpdate_Component()
{ }

void CCameraMovement::Set_Target(CTransform* targetTransform)
{
    m_TargetTransformCom = targetTransform;
    // Reset_Camera();
}

void CCameraMovement::Set_RotX(_float rotX)
{
    m_CamPreset.pitch = rotX;
    Clamp_Pitch();
}

void CCameraMovement::Apply_Preset(const CAM_PRESET& preset, _bool snap)
{
    m_CamPreset = preset;

    Clamp_Pitch();

    if (snap)
        Update_TargetMode(0.f);
}

void CCameraMovement::Start_CamShake(_float amplitude, _float decay, _float freqX, _float freqY)
{
    if (amplitude <= m_ShakeAmp)
        return;

    m_ShakeAmp     = amplitude;
    m_ShakeDecay   = decay;
    m_ShakeFreqX   = freqX;
    m_ShakeFreqY   = freqY;
    m_ShakeTimeAcc = 0.f;
}

void CCameraMovement::Move_FreeMode(const _vec3& dir, _float speed, _float timeDelta)
{
    NULL_CHECK(m_TransformCom);

    _vec3 moveDir(0.f, 0.f, 0.f);

    if (dir.z != 0.f) moveDir += m_TransformCom->Get_Look() * dir.z;
    if (dir.x != 0.f) moveDir += m_TransformCom->Get_Right() * dir.x;
    if (dir.y != 0.f) moveDir += m_TransformCom->Get_Up() * dir.y;

    if (D3DXVec3Length(&moveDir) > 0.f) // 0벡터 정규화 방지
        D3DXVec3Normalize(&moveDir, &moveDir);

    m_TransformCom->Move_Pos(moveDir, timeDelta, speed);
}

void CCameraMovement::RotateX_FreeMode(_float angleDelta)
{
    NULL_CHECK(m_TransformCom);

    m_CamPreset.pitch += angleDelta;
    Clamp_Pitch();
    m_TransformCom->Set_Rotation(_vec3(m_CamPreset.pitch, m_CamPreset.yaw, 0.f));
}

void CCameraMovement::RotateY_FreeMode(_float angleDelta)
{
    NULL_CHECK(m_TransformCom);

    m_CamPreset.yaw += angleDelta;
    m_TransformCom->Set_Rotation(_vec3(m_CamPreset.pitch, m_CamPreset.yaw, 0.f));
}

_bool CCameraMovement::Update_TargetMode(const _float& timeDelta)
{
    NULL_CHECK_RETURN(m_TransformCom, false);
    NULL_CHECK_RETURN(m_TargetTransformCom, false);


    Update_Ease(timeDelta);

    Update_DynamicTargetMode(timeDelta);

    // 피벗 위치 계산
    _vec3 targetPos = m_TargetTransformCom->Get_Pos() + m_CamPreset.pivotOffset;

    // 감속 보간
    if (timeDelta > 0.f)
    {
        m_CurrentPivotPos = Engine::Lerp_Damp(m_CurrentPivotPos,
                                              targetPos,
                                              m_CamPreset.lagSpeed,
                                              timeDelta);

        m_CurrentDistance = Engine::Lerp_Damp(m_CurrentDistance,
                                              m_CamPreset.dist,
                                              m_CamPreset.zoomSpeed,
                                              timeDelta);
    }
    else // timeDelta <= 0.f 경우 현재 위치를 목표 위치로 즉시 설정
    {
        m_CurrentPivotPos = targetPos;
        m_CurrentDistance = m_CamPreset.dist;
    }

    // 카메라 흔들림 업데이트
    Update_Shake(timeDelta);

    // 회전 행렬 생성
    _matrix matRot;
    D3DXMatrixRotationYawPitchRoll(&matRot, m_CamPreset.yaw, m_CamPreset.pitch, 0.f);

    // 흔들림 보정 로직
    _vec3 worldShake;
    D3DXVec3TransformNormal(&worldShake, &m_ShakeOffset, &matRot);

    // 최종 위치 계산

    // 로컬 거리
    _vec3 localDist = { 0.f, 0.f, -m_CurrentDistance };

    // (로컬 거리) * 회전 = 카메라와 피벗 사이의 월드 오프셋
    _vec3 worldDistOffset;
    D3DXVec3TransformCoord(&worldDistOffset, &localDist, &matRot);

    // 최종 위치 = 피벗 위치 + 거리 오프셋 + 흔들림
    _vec3 finalPos = m_CurrentPivotPos + worldDistOffset + worldShake;

    m_TransformCom->Set_Pos(finalPos);

    _vec3 shakePivotPos = m_CurrentPivotPos + worldShake;

    if (m_CamPreset.dist > 0.01f)
        m_TransformCom->LookAt(shakePivotPos);
    else
        m_TransformCom->Set_Rotation(_vec3(m_CamPreset.pitch, m_CamPreset.yaw, 0.f));

    return true;
}

void CCameraMovement::Clamp_Pitch()
{
    constexpr _float rotXLimit = D3DXToRadian(89.f);

    m_CamPreset.pitch = Engine::Clamp(m_CamPreset.pitch, -rotXLimit, rotXLimit);
}

void CCameraMovement::Update_Shake(const _float& timeDelta)
{
    // 진폭이 없으면 로직 수행 X
    if (m_ShakeAmp <= 0.f)
    {
        m_ShakeAmp     = 0.f;
        m_ShakeOffset  = _vec3(0.f, 0.f, 0.f);
        m_ShakeTimeAcc = 0.f;
        return;
    }

    // 시간 누적
    m_ShakeTimeAcc += timeDelta;

    // 진폭 감쇠
    m_ShakeAmp -= m_ShakeDecay * timeDelta;

    if (m_ShakeAmp < 0.f)
        m_ShakeAmp = 0.f;

    // 오프셋 계산 (Sin 파동 이용)
    const _float offsetX = sinf(m_ShakeTimeAcc * m_ShakeFreqX) * m_ShakeAmp;

    // Y축은 cos를 사용하여 X축과 위상차를 둠
    const _float offsetY = cosf(m_ShakeTimeAcc * m_ShakeFreqY) * m_ShakeAmp;

    m_ShakeOffset = _vec3(offsetX, offsetY, 0.f);
}

CCameraMovement* CCameraMovement::Create(DEVICE graphicDev)
{
    auto camMovement = new CCameraMovement(graphicDev);

    if (FAILED(camMovement->Ready_CameraMovement()))
    {
        Safe_Release(camMovement);
        MSG_BOX("CameraMovement Create Failed");
        return nullptr;
    }

    return camMovement;
}

CComponent* CCameraMovement::Clone()
{
    return new CCameraMovement(*this);
}

void CCameraMovement::Free()
{
    m_TargetTransformCom = nullptr;

    CComponent::Free();
}

void CCameraMovement::Set_OnTransitionEnd(OnTransitionEnd callback)
{
    m_OnTransitionEnd = callback;
    m_IsInTransition = true;
}

_bool CCameraMovement::Check_TransitionComplete(_float threshold)
{
    if (!m_IsInTransition || !m_OnTransitionEnd)
        return false;

    // 거리 체크
    _float distDiff = abs(m_CurrentDistance - m_CamPreset.dist);

    // 위치 체크
    _vec3 targetPos = m_TargetTransformCom->Get_Pos() + m_CamPreset.pivotOffset;
    _vec3 posDiffVec = m_CurrentPivotPos - targetPos;
    _float posDiff = D3DXVec3Length(&posDiffVec);

    if (distDiff < threshold && posDiff < threshold && m_OnTransitionEnd)
    {
        auto callback = m_OnTransitionEnd;

        m_OnTransitionEnd = nullptr;
        m_IsInTransition  = false;

        callback();

        return true;
    }
    return false;
}

void CCameraMovement::Update_DynamicTargetMode(const _float& timeDelta)
{
    if (m_UseDynamicDistance && m_SecondTargetTransform)
    {
        _vec3 playerPos = m_TargetTransformCom->Get_Pos();
        _vec3 bossPos   = m_SecondTargetTransform->Get_Pos();

        _vec3  vecDistance = (bossPos - playerPos);
        _float distance    = D3DXVec3Length(&vecDistance);

        // 거리 비율 계산
        _float ratio = Engine::Clamp(
            (distance - m_PlayerDistMin) / (m_PlayerDistMax - m_PlayerDistMin),
            0.f,
            1.f);

        // 카메라 거리 보간
        m_CamPreset.dist = m_DynamicCamDistMin + ratio * (m_DynamicCamDistMax - m_DynamicCamDistMin);
    }
}

void CCameraMovement::Set_DynamicDistanceParams(_float camMin, _float camMax, _float distMin, _float distMax)
{
    m_DynamicCamDistMin = camMin;
    m_DynamicCamDistMax = camMax;

    m_PlayerDistMin = distMin;
    m_PlayerDistMax = distMax;
}

void CCameraMovement::Disable_DynamicDistance()
{
    m_UseDynamicDistance    = false;
    m_SecondTargetTransform = nullptr;
}

void CCameraMovement::Ease_ToPreset(CameraPreset& camTargetPreset, float duration, function<float(float)> ease)
{
    m_UsePresetEase = true;
    m_EaseTime = 0;
    m_EaseDuration = duration;

    m_EaseStartPreset= m_CamPreset;
    m_EaseEndPreset= camTargetPreset;

    m_easeFunc = ease;

    Start_Transition();
}
void CCameraMovement::Update_Ease(const _float& timeDelta)
{
    if (!m_UsePresetEase)
        return;

    m_EaseTime += timeDelta;

    float radio = m_EaseTime / m_EaseDuration;

    radio = clamp(radio, 0.f, 1.f);
    float result =0 ;
    if (m_easeFunc)
        result = m_easeFunc(radio);

    m_CamPreset.pivotOffset = Lerp(m_EaseStartPreset.pivotOffset, m_EaseEndPreset.pivotOffset, result);

    m_CamPreset.dist = Lerp(m_EaseStartPreset.dist, m_EaseEndPreset.dist, result);

    m_CamPreset.pitch = Lerp(m_EaseStartPreset.pitch, m_EaseEndPreset.pitch, result);

    m_CamPreset.yaw = Lerp(m_EaseStartPreset.yaw, m_EaseEndPreset.yaw, result);

    m_CamPreset.pivotOffset = Lerp(m_EaseStartPreset.pivotOffset, m_EaseEndPreset.pivotOffset, result);

    if (radio >= 1.f)
    {
        m_UsePresetEase = false;
        m_CamPreset = m_EaseEndPreset;

        if (m_OnTransitionEnd)
            m_OnTransitionEnd();
    }
}
