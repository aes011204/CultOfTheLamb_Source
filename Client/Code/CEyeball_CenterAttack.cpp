#include "pch.h"
#include "CEyeball_CenterAttack.h"
#include "CTransform.h"
#include "CAnimator.h"
#include "CBoss2_Bullet.h"
#include "CEyeball.h"
#include "CAttackBuilder.h"
#include "CBossPhase2.h"
#include "CMonsterController.h"
#include "CProjectile.h"
#include "CRandomMgr.h"
#include "CTimerMgr.h"

CEyeball_CenterAttack::CEyeball_CenterAttack(CMonsterBase* owner)
    : m_Owner(owner)
{
    
}

CEyeball_CenterAttack::~CEyeball_CenterAttack()
{
}

void CEyeball_CenterAttack::Enter()
{
    CMonsterState::Enter();

    auto eyeball = static_cast<CEyeball*>(m_Owner);
    m_Controller = eyeball->Get_Controller();

    if (m_Controller)
    {
        auto animator = m_Controller->Get_OwnerAnimator();

        if (animator)
            animator->Play_Animation(L"Eyeball_Attack_Loop", ANIMSTATE::LOOP);
    }

    auto boss = dynamic_cast<CBossPhase2*>(eyeball->Get_Boss());

    if (boss && boss->Get_Controller())
    {
        _vec3 bossPos = boss->Get_Controller()->Get_OwnerTransform()->Get_Pos();

        m_TargetPos = bossPos;
        m_TargetPos.z -= 10.f;
        m_TargetPos.y = 2.f;  
    }

    m_fCurStateElapsedTime = 0.f;
    m_HasReachedCenter = false;
    m_CurrentScale = 1.f;

    m_FireTimer = 0.f;
    m_FireCount = 0;
    m_IsFinished = false;

    Find_Player();

}

_int CEyeball_CenterAttack::Update_State(const _float& timeDelta)
{
    m_fCurStateElapsedTime += timeDelta;

    if (m_IsFinished)
        return CEyeball::PATTERN::EYEBALL_GROUND_MOVE;

    /*CTransform* transform = m_Controller->Get_OwnerTransform();
    auto animator = m_Controller->Get_OwnerAnimator();*/
    
    // 가운데로 먼저 이동
    if (!m_HasReachedCenter)
    {
        Move_Center(timeDelta);
    }
    else
    {
        Grow_Size(timeDelta);

        m_FireTimer += timeDelta;
        if (m_FireTimer >= m_FireInterval && m_FireCount < m_MaxFireCount)
        {
            Spawn_BounceBullets();
            m_FireTimer = 0.f;
            m_FireCount++;
        }
    }

    // 발사 끝
    if (m_FireCount >= m_MaxFireCount)
    {
        m_IsFinished = true;
    }

    return CEyeball::PATTERN::EYEBALL_CENTER_ATTACK;
}

void CEyeball_CenterAttack::Exit()
{
    CMonsterState::Exit();

    // 원래 크기로
    auto eyeball = static_cast<CEyeball*>(m_Owner);
    if (eyeball && eyeball->Get_Transform())
    {
        eyeball->Get_Transform()->Set_Scale({ 1.f, 1.f, 1.f });
    }

}

HRESULT CEyeball_CenterAttack::Ready_State()
{
    m_fMinStateTime = 0.5f;
    
    return S_OK;
}

void CEyeball_CenterAttack::Move_Center(const _float& timeDelta)
{
    CTransform* transform = m_Controller->Get_OwnerTransform();
    if (!transform) return;

    _vec3 curPos = transform->Get_Pos();
    _vec3 dir = m_TargetPos - curPos;

    _float distance = D3DXVec3Length(&dir);

    if (distance <= 0.5f)
    {
        m_HasReachedCenter = true;
        transform->Set_Pos(m_TargetPos);
        return;
    }

    D3DXVec3Normalize(&dir, &dir);
    curPos += dir * m_MoveSpeed * timeDelta;
    transform->Set_Pos(curPos);
}

void CEyeball_CenterAttack::Grow_Size(const _float& timeDelta)
{
    if (m_CurrentScale >= m_TargetScale) return;

    m_CurrentScale += m_GrowSpeed * timeDelta;

    if (m_CurrentScale > m_TargetScale)
        m_CurrentScale = m_TargetScale;

    CTransform* transform = m_Controller->Get_OwnerTransform();
    if (transform)
    {
        transform->Set_Scale({ m_CurrentScale, m_CurrentScale, m_CurrentScale });
    }

}

void CEyeball_CenterAttack::Spawn_BounceBullets()
{
    auto eyeball = static_cast<CEyeball*>(m_Owner);
    NULL_CHECK(eyeball);

    _vec3 eyeballPos = m_Controller->Get_OwnerTransform()->Get_Pos();

    // 일단 8번만
    for (_int i = 0; i < 8; i++)
    {
        _float horizonAngle = static_cast<_float>(CRandomMgr::GetInstance()->Get_Int_Random(0, 360));

        // 60 ~ 80도
        _float verticalAngle = 60.f + static_cast<_float>(CRandomMgr::GetInstance()->Get_Int_Random(0, 20));

        _float speed = 10.f + static_cast<_float>(CRandomMgr::GetInstance()->Get_Int_Random(0, 5));

        _vec3 velocity;
        velocity.x = cosf(D3DXToRadian(horizonAngle)) * cosf(D3DXToRadian(verticalAngle)) * speed;
        velocity.y = sinf(D3DXToRadian(verticalAngle)) * speed;
        velocity.z = sinf(D3DXToRadian(horizonAngle)) * cosf(D3DXToRadian(verticalAngle)) * speed;

        CBoss2_Bullet* bullet = nullptr;

        CAttackBuilder(eyeball->Get_GraphicDev(), m_Owner)
            .SetType<CBoss2_Bullet>()
            .Track(bullet)
            .Position(eyeballPos)
            .Scale(0.2f)
            .Damage(1.f)
            .Knockback(30.f)
            .LifeTime(10.f)
            .CollisionLayer(COLLISION_LAYERTYPE::BOSSPROJECTILE)
            .DestroyOnHit(true)
            .Particle(L"HitEffect_Star", 14)
            .Spawn();

        if (bullet)
        {
            bullet->Set_GravityMode(true);
            bullet->Set_Velocity(velocity);
            bullet->Set_MaxBounce(3);
            bullet->Start_Shoot();
            bullet->Initialize_GameObject();
        }
    }

}

CEyeball_CenterAttack* CEyeball_CenterAttack::Create(CMonsterBase* owner)
{
    auto state = new CEyeball_CenterAttack(owner);

    if (FAILED(state->Ready_State()))
    {
        Safe_Release(state);
        MSG_BOX("Boss Attack Create Failed");
        return nullptr;
    }

    return state;
}

void CEyeball_CenterAttack::Free()
{
    
}
