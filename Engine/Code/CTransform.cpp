#include "CTransform.h"

CTransform::CTransform()
    : m_Scale(1.f, 1.f, 1.f),
      m_Rotation(0.f, 0.f, 0.f, 1.f),
      m_EulerRad(0.f, 0.f, 0.f),
      m_PrevScale(m_Scale),
      m_PrevRotation(m_Rotation),
      m_IsDirty(true)
{
    ZeroMemory(m_Info, sizeof(_vec3) * static_cast<_uint>(INFO_END));
    ZeroMemory(m_PrevInfo, sizeof(_vec3) * static_cast<_uint>(INFO_END));

    D3DXMatrixIdentity(&m_matWorld);
    D3DXMatrixIdentity(&m_matRot);
}

CTransform::CTransform(DEVICE graphicDev)
    : CComponent(graphicDev),
      m_Scale(1.f, 1.f, 1.f),
      m_Rotation(0.f, 0.f, 0.f, 1.f),
      m_EulerRad(0.f, 0.f, 0.f),
      m_PrevScale(m_Scale),
      m_PrevRotation(m_Rotation),
      m_IsDirty(true)
{
    ZeroMemory(m_Info, sizeof(_vec3) * static_cast<_uint>(INFO_END));
    ZeroMemory(m_PrevInfo, sizeof(_vec3) * static_cast<_uint>(INFO_END));

    D3DXMatrixIdentity(&m_matWorld);
    D3DXMatrixIdentity(&m_matRot);
}

CTransform::CTransform(const CTransform& rhs)
    : CComponent(rhs),
      m_Scale(rhs.m_Scale),
      m_Rotation(rhs.m_Rotation),
      m_matRot(rhs.m_matRot),
      m_matWorld(rhs.m_matWorld),
      m_EulerRad(rhs.m_EulerRad),
      m_PrevScale(rhs.m_Scale),
      m_PrevRotation(rhs.m_Rotation),
      m_IsDirty(true)
{
    memcpy(m_Info, rhs.m_Info, sizeof(_vec3) * static_cast<_uint>(INFO_END));
    memcpy(m_PrevInfo, m_Info, sizeof(_vec3) * static_cast<_uint>(INFO_END));
}

CTransform::~CTransform()
{ }

HRESULT CTransform::Ready_Transform()
{
    D3DXMatrixIdentity(&m_matWorld);

    for (_uint i = 0; i < INFO_END; ++i)
    {
        // _vec3가 정확히 3개의 float로 이루어져 있기에, _vec3 단위로 통째로 옮기기
        memcpy(&m_Info[i], &m_matWorld.m[i][0], sizeof(_vec3));

        m_PrevInfo[i] = m_Info[i];
    }

    return S_OK;
}

_int CTransform::Update_Component(const _float& timeDelta)
{
    m_IsDirty = false;

    if ((m_IsDirty = Check_Dirty()) == true)
        Compute_WorldMatrix();

    return 0;
}

void CTransform::LateUpdate_Component()
{
    if ((m_IsDirty = Check_Dirty()) == true)
        Compute_WorldMatrix();

    // 현재 값을 이전 값으로 저장
    for (size_t i = 0; i < INFO_END; ++i)
    {
        m_PrevInfo[i] = m_Info[i];
    }

    m_PrevScale    = m_Scale;
    m_PrevRotation = m_Rotation;
}

_vec3 CTransform::Get_Rotation() const
{
    return {
        asinf(Clamp(-m_matRot._23, -1.f, 1.f)), // Pitch (X회전)
        atan2f(m_matRot._13, m_matRot._33),     // Yaw (Y회전)
        atan2f(m_matRot._21, m_matRot._22)      // Roll (Z회전)
    };
}

_float CTransform::Get_Rotation(ROTATION eType) const
{
    switch (eType)
    {
    case ROT_X:
        return asinf(Clamp(-m_matRot._23, -1.f, 1.f));
    case ROT_Y:
        return atan2f(m_matRot._13, m_matRot._33);
    case ROT_Z:
        return atan2f(m_matRot._21, m_matRot._22);
    default:
        return -1.f;
    }
}

void CTransform::Get_Info(INFO type, _vec3* info)
{
    memcpy(info, &m_matWorld.m[type][0], sizeof(_vec3));
}

void CTransform::Set_Rotation(const _vec3& rad)
{
    m_EulerRad = rad;
    D3DXQuaternionRotationYawPitchRoll(&m_Rotation,
                                       rad.y,
                                       rad.x,
                                       rad.z);
    m_IsDirty = true;
}

void CTransform::Set_RotationAxis(ROTATION axis, float v)
{
    switch (axis)
    {
    case ROT_X:
        m_EulerRad.x = v;
        break;
    case ROT_Y:
        m_EulerRad.y = v;
        break;
    case ROT_Z:
        m_EulerRad.z = v;
        break;
    }

    Set_Rotation(m_EulerRad);
}

void CTransform::Move_Pos(const _vec3& dir, const _float& timeDelta, const _float& speed)
{
    m_Info[INFO_POS] += dir * timeDelta * speed;
}

void CTransform::Rotation(ROTATION rotType, const _float& radian, const _float& timeDelta)
{
    D3DXVECTOR3 axis(0.f, 0.f, 0.f);

    switch (rotType)
    {
    case ROT_X:
        axis = D3DXVECTOR3(1.f, 0.f, 0.f);
        break;
    case ROT_Y:
        axis = D3DXVECTOR3(0.f, 1.f, 0.f);
        break;
    case ROT_Z:
        axis = D3DXVECTOR3(0.f, 0.f, 1.f);
        break;
    default:
        return;
    }

    const _float angle = radian * timeDelta;

    _quat deltaRot;

    D3DXQuaternionRotationAxis(&deltaRot, &axis, angle);

    // [Local Rotation]  (오브젝트 로컬 축 기준)
    // Local Rotation: Current * Delta
    D3DXQuaternionMultiply(&m_Rotation, &m_Rotation, &deltaRot);

    D3DXQuaternionNormalize(&m_Rotation, &m_Rotation);
}

void CTransform::Rotation_World(ROTATION rotType, const _float& radian, const _float& timeDelta)
{
    D3DXVECTOR3 axis(0.f, 0.f, 0.f);

    switch (rotType)
    {
    case ROT_X:
        axis = D3DXVECTOR3(1.f, 0.f, 0.f);
        break;
    case ROT_Y:
        axis = D3DXVECTOR3(0.f, 1.f, 0.f);
        break;
    case ROT_Z:
        axis = D3DXVECTOR3(0.f, 0.f, 1.f);
        break;
    default:
        return;
    }

    const _float angle = radian * timeDelta;

    _quat deltaRot;

    D3DXQuaternionRotationAxis(&deltaRot, &axis, angle);

    // [World Rotation]  (월드 축 기준)
    // World Rotation: Delta * Current
    D3DXQuaternionMultiply(&m_Rotation, &deltaRot, &m_Rotation);

    D3DXQuaternionNormalize(&m_Rotation, &m_Rotation);
}

void CTransform::Chase_Target(const _vec3* targetPos, const _float& timeDelta, const _float& speed)
{
    const float epsMove      = 1e-6f;               // 이보다 가까우면 이동하지 않음
    const float epsRotDist   = 0.01f;               // 이 거리보다 가까우면 회전 멈춤 (데드존)
    const float maxTurnSpeed = D3DXToRadian(720.f); // 초당 회전 한계 (rad/s)

    // [1] 이동: 등속 이동 + 남은 거리 클램프
    _vec3 dir  = *targetPos - m_Info[INFO_POS];
    float dist = D3DXVec3Length(&dir);

    if (dist > epsMove)
    {
        D3DXVec3Normalize(&dir, &dir);

        // 거리가 멀 때 :
        // speed * dt가 작으므로 등속 이동
        // 거리가 가까울 때
        // speed * dt > dist 가 되는 순간 가까워짐에 따라 감속 이동, 마지막 도착점에 정확히 0으로 스냅
        const float step = min(speed * timeDelta, dist);
        m_Info[INFO_POS] += dir * step;
    }

    // 데드존 안이면 방향 정렬 생략
    if (dist < epsRotDist)
        return;

    const _quat targetRot  = Compute_LookAtTarget_Quat(targetPos);
    _quat       currentRot = m_Rotation;
    D3DXQuaternionNormalize(&currentRot, &currentRot);

    _quat invCurrentRot;
    D3DXQuaternionInverse(&invCurrentRot, &currentRot);

    _quat deltaRot;

    // D3DXQuaternionMultiply(&Out, &Q1, &Q2)는 Out = Q2 * Q1
    // deltaRot = invCurrentRot * targetRot. (Q1=targetRot, Q2=invCurrentRot)
    // 즉, invCurrentRot를 targetRot에 좌측곱
    // 이유: 현재 자세(currentRot)에서 목표 자세(targetRot)로 가기 위한 '로컬(Local) 회전 차이' deltaRot를 계산.
    // (수식: targetRot = currentRot * deltaRot  ->  양변에 invCurrentRot 좌측곱  ->  invCurrentRot * targetRot = deltaRot)
    D3DXQuaternionMultiply(&deltaRot, &targetRot, &invCurrentRot);

    D3DXQuaternionNormalize(&deltaRot, &deltaRot);

    // 프레임 당 이동 제한 (= 각속도 제한)
    const float dot     = Clamp(fabsf(deltaRot.w), -1.f, 1.f);
    const float angle   = 2.f * acosf(dot);
    const float maxStep = maxTurnSpeed * timeDelta;

    if (angle > 1e-6f)
    {
        const float t = min(1.f, maxStep / angle);

        _quat       stepRot;
        const _quat identity(0, 0, 0, 1);

        // identity(시작점) -> deltaRot(최종 목표 회전)로 slerp(t)
        // slerp으로 한 프레임마다 조금씩 도는 회전 분위값을 만들어 stepRot에 저장
        D3DXQuaternionSlerp(&stepRot, &identity, &deltaRot, t);

        // m_qRot_new = m_qRot_old * stepRot. (Q1=stepRot, Q2=m_qRot_old)
        // 즉, stepRot을 m_qRot에 우측곱
        // 이유: 이전에 계산한 '로컬' 회전(deltaRot)을 기반으로 한 프레임 스텝(stepRot)을
        // 현재 로컬 좌표계에 누적(갱신)하기 위해 우측에 곱합니다.
        D3DXQuaternionMultiply(&m_Rotation, &stepRot, &m_Rotation);

        D3DXQuaternionNormalize(&m_Rotation, &m_Rotation);
    }
}

void CTransform::LookAt(const _vec3& targetPos)
{
    _vec3 pos  = m_Info[INFO_POS];
    _vec3 look = targetPos - pos;
    D3DXVec3Normalize(&look, &look);

    if (D3DXVec3LengthSq(&look) == 0.f)
        return;

    _vec3 right;

    D3DXVec3Cross(&right, &g_WorldUp, &look);
    D3DXVec3Normalize(&right, &right);

    if (D3DXVec3LengthSq(&right) == 0.f)
        right = g_WorldRight;

    _vec3 up;
    D3DXVec3Cross(&up, &look, &right);
    D3DXVec3Normalize(&up, &up);

    _matrix matRot;
    D3DXMatrixIdentity(&matRot);

    matRot._11 = right.x;
    matRot._12 = right.y;
    matRot._13 = right.z;

    matRot._21 = up.x;
    matRot._22 = up.y;
    matRot._23 = up.z;

    matRot._31 = look.x;
    matRot._32 = look.y;
    matRot._33 = look.z;

    D3DXQuaternionRotationMatrix(&m_Rotation, &matRot);

    m_IsDirty = true;
}

_quat CTransform::Compute_LookAtTarget_Quat(const _vec3* pTargetPos)
{
    // 현재 위치에서 타깃을 바라보는 정규화된 전방 벡터를 구한 후,
    // BuildLookRotation을 사용하여 Z축 회전(Roll) 없이 타깃을 바라보는 기준 행렬을 만든다.

    _matrix matTarget;
    _quat   rotation;

    // 타깃 방향 정규화
    _vec3 forward = *pTargetPos - m_Info[INFO_POS];
    D3DXVec3Normalize(&forward, &forward);

    // Roll이 고정된 상태로 타깃을 바라보는 기준 회전 행렬 생성 (World Up 고정)
    matTarget = BuildLookRotation(forward, _vec3(0.f, 1.f, 0.f));

    // 행렬을 목표 회전 쿼터니언으로 변환
    D3DXQuaternionRotationMatrix(&rotation, &matTarget);
    D3DXQuaternionNormalize(&rotation, &rotation);

    return rotation;
}

_matrix CTransform::BuildLookRotation(const _vec3& inputForward, const _vec3& inputWorldUp)
{
    _matrix outRotMat;
    D3DXMatrixIdentity(&outRotMat);

    _vec3 forwardDir = inputForward;
    if (D3DXVec3LengthSq(&forwardDir) < 1e-12f) return outRotMat;
    D3DXVec3Normalize(&forwardDir, &forwardDir);

    _vec3 upDir = inputWorldUp;
    if (fabsf(D3DXVec3Dot(&forwardDir, &upDir)) > 0.9995f)
        upDir = fabsf(forwardDir.y) > 0.9f ? _vec3(1.f, 0.f, 0.f) : _vec3(0.f, 1.f, 0.f);

    // LH 외적 순서 : right = up × forward, up = forward × right
    _vec3 rightDir;
    D3DXVec3Cross(&rightDir, &upDir, &forwardDir);
    if (D3DXVec3LengthSq(&rightDir) < 1e-12f)
    {
        // 특수한 경우 보정
        upDir = _vec3(0.f, 0.f, 1.f);
        D3DXVec3Cross(&rightDir, &upDir, &forwardDir);
    }
    D3DXVec3Normalize(&rightDir, &rightDir);

    _vec3 newUpDir;
    D3DXVec3Cross(&newUpDir, &forwardDir, &rightDir);
    D3DXVec3Normalize(&newUpDir, &newUpDir);

    // 출력 회전행렬의 각 행에 right, up, forward를 기록
    outRotMat._11 = rightDir.x;
    outRotMat._12 = rightDir.y;
    outRotMat._13 = rightDir.z;
    outRotMat._21 = newUpDir.x;
    outRotMat._22 = newUpDir.y;
    outRotMat._23 = newUpDir.z;
    outRotMat._31 = forwardDir.x;
    outRotMat._32 = forwardDir.y;
    outRotMat._33 = forwardDir.z;

    return outRotMat;
}

void CTransform::Compute_WorldMatrix()
{
    D3DXMatrixIdentity(&m_matWorld);

    // 1. 축 벡터 초기화 (월드 행렬의 기본 기저 벡터)
    for (_uint i = 0; i < INFO_POS; ++i)
    {
        // m_vInfo[INFO_RIGHT/UP/LOOK] 에 단위 벡터 (1,0,0), (0,1,0), (0,0,1) 복사
        memcpy(&m_Info[i], &m_matWorld.m[i][0], sizeof(_vec3));
    }

    // 2. 크기(Scale) 적용
    for (_uint i = 0; i < INFO_POS; ++i)
    {
        D3DXVec3Normalize(&m_Info[i], &m_Info[i]);
        // 각 축을 정규화한 뒤, m_vScale 의 각 성분을 곱하여 스케일 적용
        m_Info[i] *= *((_float*)&m_Scale + i);
    }

    // 3. 회전(Rotation) 변환 (쿼터니온 기반)
    _quat q = m_Rotation;

    D3DXQuaternionNormalize(&q, &q);
    D3DXMatrixRotationQuaternion(&m_matRot, &q);

    // 스케일이 적용된 축 벡터들을 쿼터니언 회전 행렬로 변환
    for (_uint i = 0; i < INFO_POS; ++i)
    {
        // D3DXVec3TransformNormal 사용: 위치 정보(m_vInfo[INFO_POS]가 아님)는 변환하지 않음
        D3DXVec3TransformNormal(&m_Info[i], &m_Info[i], &m_matRot);
    }

    // 4. 위치(Translation) 변환
    // m_vInfo[INFO_POS]는 이미 외부 함수(Move_Pos/Set_Pos)에 의해 제어되고 있으므로,
    // 이 단계에서는 별도의 행렬 계산 없이 m_vInfo[INFO_POS]에 저장된 최종 위치를 사용함.

    // 5. 월드 행렬에 복사 (Right, Up, Look, Pos 순서)
    for (_uint i = 0; i < INFO_END; ++i)
    {
        // 계산된 m_vInfo[i] 벡터를 월드 행렬의 i번째 행(Row)에 그대로 복사
        memcpy(&m_matWorld.m[i][0], &m_Info[i], sizeof(_vec3));
    }
}

bool CTransform::Check_Dirty()
{
    // 위치 비교
    if (HasVectorChanged(m_Info[INFO_POS], m_PrevInfo[INFO_POS])) return true;
    // 크기 비교
    if (HasVectorChanged(m_Scale, m_PrevScale)) return true;
    // 회전(쿼터니온) 비교
    if (HasQuatChanged(m_Rotation, m_PrevRotation)) return true;

    return false;
}

bool CTransform::HasVectorChanged(const _vec3& now, const _vec3& prev, float eps)
{
    return fabsf(now.x - prev.x) > eps
           || fabsf(now.y - prev.y) > eps
           || fabsf(now.z - prev.z) > eps;
}

bool CTransform::HasQuatChanged(const _quat& now, const _quat& prev, float eps)
{
    // q와 -q는 동일한 회전 -> 쿼터니온 내적의 절댓값 사용
    float d = fabsf(D3DXQuaternionDot(&now, &prev));

    d = Clamp(d, -1.f, 1.f);

    // 각도 차 = 2*acos(dot)
    float angle = 2.f * acosf(d);

    return angle > eps;
}

CTransform* CTransform::Create(DEVICE graphicDev)
{
    auto pTransform = new CTransform(graphicDev);

    if (FAILED(pTransform->Ready_Transform()))
    {
        Safe_Release(pTransform);
        MSG_BOX("CTransform Create Failed");
        return nullptr;
    }

    return pTransform;
}

CComponent* CTransform::Clone()
{
    return new CTransform(*this);
}

void CTransform::Free()
{
    CComponent::Free();
}
