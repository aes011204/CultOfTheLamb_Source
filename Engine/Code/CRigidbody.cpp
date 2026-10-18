#include "CRigidbody.h"

#include "CTransform.h"

CRigidbody::CRigidbody(DEVICE graphicDev)
    : CComponent(graphicDev),
      m_UseAccel(true),
      m_UseFriction(true),
      m_UseGravity(true),
      m_OnGround(true),
      m_Mass(1.f),
      m_Friction(0.00f),
      m_Force(0.f, 0.f, 0.f),
      m_Accel(0.f, 0.f, 0.f),
      m_Vel(0.f, 0.f, 0.f),
      m_VelLimit(10.f, 100.f, 10.f),
      m_Gravity(0.f, -200.f, 0.f)
{ }

CRigidbody::CRigidbody(const CRigidbody& rhs)
    : CComponent(rhs),
      m_UseAccel(rhs.m_UseAccel),
      m_UseFriction(rhs.m_UseFriction),
      m_UseGravity(rhs.m_UseGravity),
      m_OnGround(rhs.m_OnGround),
      m_Mass(rhs.m_Mass),
      m_Friction(rhs.m_Friction),
      m_Force(rhs.m_Force),
      m_Accel(rhs.m_Accel),
      m_Vel(rhs.m_Vel),
      m_VelLimit(rhs.m_VelLimit),
      m_Gravity(rhs.m_Gravity)
{ }

CRigidbody::~CRigidbody()
{ }

HRESULT CRigidbody::Ready_Rigidbody()
{
    return S_OK;
}

_int CRigidbody::Update_Component(const _float& timeDelta)
{
    if (m_Mass <= 0.f) return 0;

    // 가속도 스위치 t/f로 가속도 적용
    if (m_UseAccel)
    {
        // 가속도 계산 (a = F / m)
        m_Accel = m_Force / m_Mass;

        // 가속도를 속도에 적용
        m_Vel += m_Accel * timeDelta;
    }
    else
        m_Vel = m_Force / m_Mass;

    _vec3 gravityDir = m_Gravity;

    // 중력이 0인 경우 안전 처리
    if (D3DXVec3LengthSq(&gravityDir) < 1e-6f)
        gravityDir = _vec3(0.f, -1.f, 0.f);
    else
        D3DXVec3Normalize(&gravityDir, &gravityDir);

    // 중력 스위치 t/f로 중력 적용
    if (m_UseGravity)
    {
        if (m_OnGround) // 지상
        {
            // 중력 방향과 속도의 내적을 구해 중력 방향 속도 성분 추출
            const _float dotY = D3DXVec3Dot(&m_Vel, &gravityDir);

            // 지면 위에서는 중력 방향 속도를 제거하여 미끄러짐/떨림 방지
            if (dotY > 0.f)
                m_Vel -= gravityDir * dotY;

            if (D3DXVec3LengthSq(&m_Vel) < 1e-6f)
                m_Vel = { 0.f, 0.f, 0.f };
        }
        else // 공중
        {
            // 중력 가속도 누적 (Y축 가속)
            m_Vel += m_Gravity * timeDelta;
        }
    }

    // 마찰력 스위치 t/f로 마찰력 적용
    if (m_UseFriction)
    {
        //const _vec3 forceXZ    = { m_Force.x, 0.f, m_Force.z };
        //const _bool hasForceXZ = (D3DXVec3LengthSq(&forceXZ) > 1e-4f);

        if (m_OnGround && D3DXVec3LengthSq(&m_Vel) > 1e-4f)
        {
            _vec3 frictionDir = -m_Vel; // 운동 방향의 반대
            D3DXVec3Normalize(&frictionDir, &frictionDir);

            // 마찰력 크기
            _float gravityAccelScalar = D3DXVec3Length(&m_Gravity);

            // 감속량 계산
            _float frictionDeltaVel = m_Friction * gravityAccelScalar * timeDelta;

            _float currentSpeed = D3DXVec3Length(&m_Vel);

            // 마찰력이 남은 속도보다 크면 정지, 아니면 감속
            if (currentSpeed <= frictionDeltaVel)
                m_Vel = { 0.f, 0.f, 0.f };
            else
                m_Vel += frictionDir * frictionDeltaVel;
        }
    }

    // 속도 분해 : Y / XZ
    _float speedY = D3DXVec3Dot(&m_Vel, &gravityDir);
    _vec3  velY   = gravityDir * speedY;
    _vec3  velXZ  = m_Vel - velY;

    // Y축(중력 방향) 속도 제한
    if (D3DXVec3Length(&velY) > m_VelLimit.y)
    {
        D3DXVec3Normalize(&velY, &velY);
        velY *= m_VelLimit.y;
    }

    // X, Z축(평면) 속도 제한
    if (D3DXVec3Length(&velXZ) > m_VelLimit.x)
    {
        D3DXVec3Normalize(&velXZ, &velXZ);
        velXZ *= m_VelLimit.x; // m_VelLimit.x = m_VelLimit.z 일 때
    }

    // 재합성
    m_Vel = velY + velXZ;

    // 위치 갱신
    NULL_CHECK_RETURN(m_TransformCom, 0);

    _vec3 pos = m_TransformCom->Get_Pos();
    pos += m_Vel * timeDelta;
    m_TransformCom->Set_Pos(pos);

    m_Force = { 0.f, 0.f, 0.f };

    return 0;
}

void CRigidbody::LateUpdate_Component()
{ }

void CRigidbody::Render_Editor()
{ }

CRigidbody* CRigidbody::Create(DEVICE graphicDev)
{
    auto rigidbody = new CRigidbody(graphicDev);

    if (FAILED(rigidbody->Ready_Rigidbody()))
    {
        Safe_Release(rigidbody);
        MSG_BOX("Rigidbody Create Failed");
        return nullptr;
    }

    return rigidbody;
}

CComponent* CRigidbody::Clone() { return new CRigidbody(*this); }

void CRigidbody::Free()
{
    CComponent::Free();
}
