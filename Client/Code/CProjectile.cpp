#include "pch.h"
#include "CProjectile.h"
#include "CTransform.h"
#include "Client_Enum.h"

CProjectile::CProjectile()
{
}

CProjectile::CProjectile(DEVICE graphicDev)
    : CComponent(graphicDev),
    m_BulletTransform(nullptr), m_UpdateFlag(false),
    m_ProjectileOwner(nullptr), m_MoveType(PROJ_TYPE::TYPE_END), m_Speed(0.f), m_Direction({ 0,1,0 }),
    m_OrbitCenter({ 0,0,0 }), m_StartAngle(0.f), m_Radius(0.f), m_Target(nullptr), m_TurnSpeed(0.f)
    , isSetDesc(false)
{
}

CProjectile::CProjectile(const CProjectile& rhs)
    : CComponent(rhs),
    m_BulletTransform(rhs.m_BulletTransform), m_UpdateFlag(rhs.m_UpdateFlag),
    m_ProjectileOwner(rhs.m_ProjectileOwner), m_MoveType(rhs.m_MoveType), m_Speed(rhs.m_Speed), m_Direction(rhs.m_Direction),
    m_OrbitCenter(rhs.m_OrbitCenter), m_StartAngle(rhs.m_StartAngle), m_Radius(rhs.m_Radius), m_Target(rhs.m_Target), m_TurnSpeed(rhs.m_TurnSpeed)
    , isSetDesc(rhs.isSetDesc)
{
}

CProjectile::~CProjectile()
{
}

HRESULT CProjectile::Ready_Projectile()
{

    return S_OK;
}

_int CProjectile::Update_Component(const _float& timeDelta)
{
    if (m_UpdateFlag == false)
    {
        m_BossTransform = dynamic_cast<CTransform*>(m_ProjectileOwner->Get_Component(COMPONENTID::ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));
        m_UpdateFlag = true;
    }


    switch (m_MoveType)
    {
    case PROJ_TYPE::LINEAR:
    {
        _vec3 dir = m_Direction * timeDelta * m_Speed;
        m_TransformCom->Set_Pos(m_TransformCom->Get_Pos() + dir );

    }
        break;
    case PROJ_TYPE::ORBIT:
    {

        m_StartAngle += timeDelta * m_Speed;
        m_StartAngle = fmodf(m_StartAngle,360.f);
        _vec3 circle = { cos(D3DXToRadian(m_StartAngle)) * m_Radius, 0.f, sin(D3DXToRadian(m_StartAngle)) * m_Radius };
        m_TransformCom->Set_Pos(m_OrbitCenter + circle);


    }
        break;
    case PROJ_TYPE::HOMING:
        // 일단 없음
        if (m_Target == nullptr)
            break;

        CTransform* targetTransform =
            dynamic_cast<CTransform*>(m_Target->Get_Component(COMPONENTID::ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));

        if (targetTransform == nullptr)
            break;

        _vec3 myPos = m_TransformCom->Get_Pos();
        _vec3 targetPos = targetTransform->Get_Pos();

        // 1️ 목표 방향
        _vec3 targetDir = targetPos - myPos;
        D3DXVec3Normalize(&targetDir, &targetDir);

        // 2️ 현재 방향
        _vec3 curDir = m_Direction;
        D3DXVec3Normalize(&curDir, &curDir);

        // 3️ 방향 보간 (회전)
        float turnRatio = m_TurnSpeed * timeDelta;
        turnRatio = min(turnRatio, 1.f);

        _vec3 newDir = curDir + (targetDir - curDir) * turnRatio;
        D3DXVec3Normalize(&newDir, &newDir);

        m_Direction = newDir;

        // 4️ 전진
        _vec3 move = m_Direction * m_Speed * timeDelta;
        m_TransformCom->Set_Pos(myPos + move);
    
        break;
    }

    return 0;
}

void CProjectile::LateUpdate_Component()
{
}

CProjectile* CProjectile::Create(DEVICE graphicDev)
{
    auto Projectile = new CProjectile(graphicDev);

    if (FAILED(Projectile->Ready_Projectile()))
    {
        Safe_Release(Projectile);
        MSG_BOX("Projectile Create Failed");
        return nullptr;
    }

    return Projectile;
}

CComponent* CProjectile::Clone()
{
    return new CProjectile(*this);
}

void CProjectile::Free()
{
    Safe_Release(m_ProjectileOwner);

    CComponent::Free();
}
