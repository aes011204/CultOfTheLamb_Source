#include "pch.h"
#include "CBoss2_BulletAttack2.h"
#include "CBossController.h"
#include "CBossPhase2.h"
#include "CAnimator.h"
#include "CTimerMgr.h"
#include "CEyeball.h"
#include "CStateMachine.h"

CBoss2_BulletAttack2::CBoss2_BulletAttack2(CGameObject* owner)
    : m_Owner(owner), m_Pattern_Cooldown(0)
{
}

CBoss2_BulletAttack2::~CBoss2_BulletAttack2()
{
}

void CBoss2_BulletAttack2::Enter()
{
    CMonsterState::Enter();

    auto boss = static_cast<CBossPhase2*>(m_Owner);
    m_Controller = boss->Get_Controller();

    // 패턴 시작 시, 모든 Eyeball을 GroundMove로 변경
    boss->Command_Ground();

    if (m_Controller)
    {
        auto animator = m_Controller->Get_OwnerAnimator();
        if (animator)
            animator->Play_Animation(L"Boss2_Roar", ANIMSTATE::ONCE);
    }

    auto& eyeballs = boss->Get_Eyeballs();

    for (auto eyeball : eyeballs)
    {
        auto controller = eyeball->Get_Controller();
        if (controller && controller->Get_StateMachine())
        {
            controller->Get_StateMachine()->Change_State(
                CEyeball::PATTERN::EYEBALL_CENTER_ATTACK);
        }
    }

    m_fCurStateElapsedTime = 0.f;
    m_SlamCount = 0;
    m_ChainAnim = ROAR;

    Find_Player();
}

_int CBoss2_BulletAttack2::Update_State(const _float& timeDelta)
{
    m_fCurStateElapsedTime += timeDelta;

    if (m_IsFinished)
    {
        return CBossPhase2::PATTERN::PHASE2_IDLE;
    }

    auto animator = m_Controller->Get_OwnerAnimator();

    switch (m_ChainAnim)
    {
    case ROAR:
        PlayRoar();
        break;
    case HANDSLMA:
        PlayHandSlam();
        break;
    }

    return CBossPhase2::PATTERN::PHASE2_BULLET2;
}

void CBoss2_BulletAttack2::Exit()
{
    CMonsterState::Exit();

    // 상태 끝날 때
}

HRESULT CBoss2_BulletAttack2::Ready_State()
{
    m_fMinStateTime = 0.5f;
    m_Pattern_Cooldown = 5.f;

    m_ChainAnim = ROAR;
    m_SlamCount = 0;

    return S_OK;
}

void CBoss2_BulletAttack2::PlayRoar()
{
    auto animator = m_Controller->Get_OwnerAnimator();

    if (animator && animator->Is_AnimationFinished())
    {
        m_ChainAnim = ChainAttackAnim::HANDSLMA;
        animator->Play_Animation(L"Boss2_HandSlam", ANIMSTATE::LOOP);
    }

}

void CBoss2_BulletAttack2::PlayHandSlam()
{
    auto animator = m_Controller->Get_OwnerAnimator();

    if (m_fCurStateElapsedTime >= 8.f)
    {
        m_IsFinished = true;
    }

}

void CBoss2_BulletAttack2::Spawn_BounceBullets()
{

}

CBoss2_BulletAttack2* CBoss2_BulletAttack2::Create(CGameObject* owner)
{
    auto state = new CBoss2_BulletAttack2(owner);

    if (FAILED(state->Ready_State()))
    {
        Safe_Release(state);
        MSG_BOX("Boss Eyeball Create Failed");
        return nullptr;
    }

    return state;
}

void CBoss2_BulletAttack2::Free()
{
    
}
