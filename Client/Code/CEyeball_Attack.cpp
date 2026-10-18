#include "pch.h"
#include "CEyeball_Attack.h"
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

const _int CIRCLE_BULLET_MAX = 12;

CEyeball_Attack::CEyeball_Attack(CMonsterBase* owner)
    : m_Owner(owner),
      m_Pattern_Cooldown(0),
      m_LerpSpeed(0),
      m_EyeballAnim(Eyeball_Anim::EYEBALL_ATTACK_END),
      m_HasFiredBullet(false), m_AttackCount(0), m_IsFinished(false),
      m_Bullet_AttackType(Attack_Type::ATTACK_TYPE_END), m_Direction_ChangeTimer(0), m_Direction_Timer(0),
      m_MoveSpeed(0), m_Current_Direction({0.f, 0.f, 0.f}),
      m_Target_Direction({0.f, 0.f, 0.f}), m_GraphicDev(nullptr)
{
}

CEyeball_Attack::~CEyeball_Attack()
{
}

void CEyeball_Attack::Enter()
{
    CMonsterState::Enter();

    auto eyeball = static_cast<CEyeball*>(m_Owner);
    m_Controller = eyeball->Get_Controller();

    if (m_Controller)
    {
        auto animator = m_Controller->Get_OwnerAnimator();

        if (animator)
        {
            animator->Play_Animation(L"Eyeball_Anti", ANIMSTATE::ONCE);
        }
    }

    m_fCurStateElapsedTime = 0.f;

    // 공격 랜덤
    _int randomType = CRandomMgr::GetInstance()->Get_Int_Random(0, 2);
    m_Bullet_AttackType = static_cast<Attack_Type>(randomType);

    m_EyeballAnim = Eyeball_Anim::ATTACK_ANTI;
    m_TotalAttackCount = 0;
    m_MaxAttackCount = CRandomMgr::GetInstance()->Get_Int_Random(2, 3); // 2 ~ 3회 눈알별로 각자 랜덤
    m_IsFinished = false;

    Find_Player(); // 플레이어 찾기
    m_GraphicDev = m_Owner->Get_GraphicDev();
    m_CircleBullets.reserve(8);

}

_int CEyeball_Attack::Update_State(const _float& timeDelta)
{
    m_fCurStateElapsedTime += timeDelta;

    CTransform* transform = m_Controller->Get_OwnerTransform();
    auto animator = m_Controller->Get_OwnerAnimator();
    
    if (m_IsFinished)
        return CEyeball::PATTERN::EYEBALL_GROUND_MOVE;

    // 모든 공격 애니메이션에서 랜덤 이동
    Random_Movement(timeDelta);

    switch (m_EyeballAnim)
    {
    case Eyeball_Anim::ATTACK_ANTI:
        Play_Anti();
        break;

    case Eyeball_Anim::ATTACK_LOOP:
        Play_Loop();
        break;

    case Eyeball_Anim::ATTACK_END:
        Play_End();
        break;

    }

    /*if (m_fCurStateElapsedTime >= m_Pattern_Cooldown)
    {
        return CEyeball::PATTERN::EYEBALL_MOVE; 
    }*/

    return CEyeball::PATTERN::EYEBALL_ATTACK;
}

void CEyeball_Attack::Exit()
{
    CMonsterState::Exit();

    // 상태 끝날 때

}

HRESULT CEyeball_Attack::Ready_State()
{
    m_fMinStateTime = 0.5f;
    m_Pattern_Cooldown = 1.0f;

    m_EyeballAnim = Eyeball_Anim::ATTACK_ANTI;

    m_HasFiredBullet = false;
    m_AttackCount = 0;
    m_IsFinished = false;

    // 랜덤 이동
    m_Direction_ChangeTimer = 1.5f;
    m_Direction_Timer = 0.f;
    m_MoveSpeed = 1;
    m_LerpSpeed = 0.05f;
    
    return S_OK;
}

void CEyeball_Attack::Play_Anti()
{
    auto animator = m_Controller->Get_OwnerAnimator();

    if (animator && animator->Is_AnimationFinished())
    {
        m_EyeballAnim = Eyeball_Anim::ATTACK_LOOP;
        animator->Play_Animation(L"Eyeball_Attack_Loop", ANIMSTATE::LOOP);
    }
}

void CEyeball_Attack::Play_Loop()
{
    // 공격
    auto animator = m_Controller->Get_OwnerAnimator();
    NULL_CHECK(animator);

    switch (m_Bullet_AttackType)
    {
    case Attack_Type::DISPERSION:
    {
        _int curFrame = animator->Get_CurFrame();

        // 20 프레임에 한번만 쏘기
        if (curFrame >= 20 && !m_HasFiredBullet && m_AttackCount < 3)
        {
            Bullet_Dispresion();

            m_HasFiredBullet = true;
            m_AttackCount++;
        }

        if (curFrame >= 40)
        {
            m_HasFiredBullet = false;
        }

        if (m_AttackCount >= 3)
        {
            Attack_Reset(animator);
        }
        break;
    }
        
    case Attack_Type::CIRCLE:
    {
        Bullet_Circle();

        if (m_Spawn_BulletCount >= CIRCLE_BULLET_MAX)
            Attack_Reset(animator);

        break;
    }

    case Attack_Type::BURST:
    {
        // 추가 예정
        Bullet_Brust();

        if (m_Spawn_BulletCount >= CIRCLE_BULLET_MAX)
            Attack_Reset(animator);

        break;
    }

    }
    
}

void CEyeball_Attack::Play_End()
{
    m_HasFiredBullet = false;

    auto animator = m_Controller->Get_OwnerAnimator();
    if (!animator || !animator->Is_AnimationFinished())
        return;

    // 보스가 특수 패턴 중이면 바로 끝내기
    auto eyeball = static_cast<CEyeball*>(m_Owner);
    auto boss = dynamic_cast<CBossPhase2*>(eyeball->Get_Boss());

    if (boss && boss->Is_InSpecialPattern())
    {
        m_IsFinished = true;
        return;  
    }

    _int randomType = CRandomMgr::GetInstance()->Get_Int_Random(0, 2);
    m_Bullet_AttackType = static_cast<Attack_Type>(randomType);

    m_EyeballAnim = Eyeball_Anim::ATTACK_ANTI;
    animator->Play_Animation(L"Eyeball_Anti", ANIMSTATE::ONCE);
    m_AttackCount = 0;
    m_Spawn_BulletCount = 0;

    m_TotalAttackCount++;
    if (m_TotalAttackCount >= m_MaxAttackCount)
    {
        m_IsFinished = true;
    }

}

void CEyeball_Attack::Bullet_Dispresion()
{
    // 8개 탄환 360도로 분산하게
    _vec3 eyeballPos = m_Controller->Get_OwnerTransform()->Get_Pos();

    _int bulletCount = 8;
    _float angleStep = 360.f / static_cast<_float>(bulletCount);

    for (_int i = 0; i < bulletCount; ++i)
    {
        _float angle = D3DXToRadian(angleStep * i);
        _vec3 direction(cosf(angle), 0.f, sinf(angle));

        CBoss2_Bullet* bullet = nullptr;

        CAttackBuilder(m_GraphicDev, m_Owner)
            .SetType<CBoss2_Bullet>()
            .Track(bullet)
            .Position(eyeballPos)
            .Scale(0.2f)
            .Damage(1.f)
            .Knockback(50.f)
            .LifeTime(5.f)
            .CollisionLayer(COLLISION_LAYERTYPE::BOSSPROJECTILE)
            .DestroyOnHit(true)
            .Particle(L"HitEffect_Star", 14)
            .Spawn();

        if (bullet)
        {
            CProjectile* projectileCom = dynamic_cast<CProjectile*>(
                bullet->Get_Component(ID_STATIC, COMPONENTTYPE::PROJECTILE));

            if (projectileCom)
            {
                BulletDesc desc;
                desc.ProjectileOwner = m_Owner;
                desc.moveType = PROJ_TYPE::LINEAR;
                desc.Speed = 15.f;
                desc.Direction = direction;

                projectileCom->SetMoveDesc(desc);
            }

            bullet->Start_Shoot();
            bullet->Initialize_GameObject();
        }

    }

}

void CEyeball_Attack::Bullet_Circle()
{
    // 탄환 순차 생성 후 완료하면 Shoot
    _float delta = CTimerMgr::GetInstance()->Get_TimeDelta();
    m_circleTimer += delta;

    if (m_circleTimer < 0.1f)
        return;

    m_circleTimer = 0.f;

    if (m_Spawn_BulletCount >= CIRCLE_BULLET_MAX)
        return;

    _vec3 dir = m_pPlayerTransformCom->Get_Pos() - m_Owner->Get_Transform()->Get_Pos();
    D3DXVec3Normalize(&dir, &dir);

    _vec3 eyeballPos = m_Controller->Get_OwnerTransform()->Get_Pos();

    _float angleStep = 360.f / CIRCLE_BULLET_MAX;
    _float angle = D3DXToRadian(angleStep * m_Spawn_BulletCount);

    _vec3 circleDir;
    circleDir.x = cosf(angle);
    circleDir.y = 0.f;
    circleDir.z = sinf(angle);

    _float radius = 2.5f;
    _vec3 spawnPos = eyeballPos + circleDir * radius;

    CBoss2_Bullet* bullet = nullptr;

    CAttackBuilder(m_GraphicDev, m_Owner)
        .SetType<CBoss2_Bullet>()
        .Track(bullet)
        .Position(spawnPos)
        .Damage(1.f)
        .Knockback(50.f)
        .Scale(0.2f)
        .LifeTime(3.f)
        .CollisionLayer(COLLISION_LAYERTYPE::BOSSPROJECTILE)
        .DestroyOnHit(true)
        .Particle(L"HitEffect_Star", 14, 0.02)
        .Spawn();

    if (bullet)
    {
        CProjectile* projectileCom = dynamic_cast<CProjectile*>(
            bullet->Get_Component(ID_STATIC, COMPONENTTYPE::PROJECTILE));

        if (projectileCom)
        {
            BulletDesc desc;
            desc.ProjectileOwner = m_Owner;
            desc.moveType = PROJ_TYPE::LINEAR;
            desc.Speed = 15.f;
            desc.Direction = dir;

            projectileCom->SetMoveDesc(desc);
        }

        bullet->Initialize_GameObject(); // 크기 설정때문에 호출해야함

        m_CircleBullets.push_back(bullet);
    }

    m_Spawn_BulletCount++;

    if (m_Spawn_BulletCount >= CIRCLE_BULLET_MAX)
    {
        for (auto* bullet : m_CircleBullets)
        {
            if (bullet)
            {
                bullet->Start_Shoot();
            }
        }
        m_CircleBullets.clear(); // 발사 후 클리어
    }

}

void CEyeball_Attack::Bullet_Brust()
{
    // 그냥 계속 쏘기
    _float delta = CTimerMgr::GetInstance()->Get_TimeDelta();
    m_circleTimer += delta;

    if (m_circleTimer < 0.1f)
        return;

    m_circleTimer = 0.f;

    if (m_Spawn_BulletCount >= CIRCLE_BULLET_MAX)
        return;

    _vec3 dir = m_pPlayerTransformCom->Get_Pos() - m_Owner->Get_Transform()->Get_Pos();
    D3DXVec3Normalize(&dir, &dir);

    _vec3 eyeballPos = m_Controller->Get_OwnerTransform()->Get_Pos();

    CBoss2_Bullet* bullet = nullptr;

    CAttackBuilder(m_GraphicDev, m_Owner)
        .SetType<CBoss2_Bullet>()
        .Track(bullet)
        .Position(eyeballPos)
        .Damage(1.f)
        .Knockback(50.f)
        .Scale(0.2f)
        .LifeTime(3.f)
        .CollisionLayer(COLLISION_LAYERTYPE::BOSSPROJECTILE)
        .DestroyOnHit(true)
        .Particle(L"HitEffect_Star", 14, 0.02)
        .Spawn();

    if (bullet)
    {
        CProjectile* projectileCom = dynamic_cast<CProjectile*>(
            bullet->Get_Component(ID_STATIC, COMPONENTTYPE::PROJECTILE));

        if (projectileCom)
        {
            BulletDesc desc;
            desc.ProjectileOwner = m_Owner;
            desc.moveType = PROJ_TYPE::LINEAR;
            desc.Speed = 15.f;
            desc.Direction = dir;

            projectileCom->SetMoveDesc(desc);
        }

        bullet->Start_Shoot();
        bullet->Initialize_GameObject(); // 크기 설정때문에 호출해야함

        m_Spawn_BulletCount++;
    }
}

CEyeball_Attack* CEyeball_Attack::Create(CMonsterBase* owner)
{
    auto state = new CEyeball_Attack(owner);

    if (FAILED(state->Ready_State()))
    {
        Safe_Release(state);
        MSG_BOX("Boss Attack Create Failed");
        return nullptr;
    }

    return state;
}

void CEyeball_Attack::Attack_Reset(CAnimator* animator)
{
    m_EyeballAnim = Eyeball_Anim::ATTACK_END;
    animator->Play_Animation(L"Eyeball_Attack_End", ANIMSTATE::ONCE);
    m_Spawn_BulletCount = 0;
}

void CEyeball_Attack::Random_Movement(const _float& timeDelta)
{
    m_fCurStateElapsedTime += timeDelta;
    m_Direction_Timer += timeDelta;

    // 이동
    CTransform* transform = m_Controller->Get_OwnerTransform();
    NULL_CHECK(transform);

    // 일정 시간마다 새로운 방향 랜덤
    if (m_Direction_Timer >= m_Direction_ChangeTimer)
    {
        _vec3 currentPos = transform->Get_Pos();

        // 최대 5번 시도 (벽 없는 방향 찾기)
        for (int attempt = 0; attempt < 5; ++attempt)
        {
            _float randomAngle = static_cast<_float>(CRandomMgr::GetInstance()->Get_Int_Random(0, 360));
            _float radian = D3DXToRadian(randomAngle);
            _vec3 newDir = _vec3(cosf(radian), 0.f, sinf(radian));

            // 벽 체크
            auto eyeball = static_cast<CEyeball*>(m_Owner);
            if (!eyeball->Trace_HitWall(currentPos, newDir, 5.f))
            {
                m_Target_Direction = newDir;
                break;
            }
        }

        m_Direction_Timer = 0.f;
    }

    // Lerp으로 자연스럽게 방향 전환시켜보기
    m_Current_Direction = _vec3(
        m_Current_Direction.x + (m_Target_Direction.x - m_Current_Direction.x) * m_LerpSpeed,
        0.f,
        m_Current_Direction.z + (m_Target_Direction.z - m_Current_Direction.z) * m_LerpSpeed);

    D3DXVec3Normalize(&m_Current_Direction, &m_Current_Direction);

    transform->Move_Pos(m_Current_Direction, timeDelta, m_MoveSpeed);
}

void CEyeball_Attack::Free()
{
    
}
