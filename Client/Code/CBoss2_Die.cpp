#include "pch.h"
#include "CBoss2_Die.h"
#include "CBossController.h"
#include "CBossPhase2.h"
#include "CAnimator.h"

CBoss2_Die::CBoss2_Die(CGameObject* owner)
    : m_Owner(owner), m_Pattern_Cooldown(0)
{

}

CBoss2_Die::~CBoss2_Die()
{

}

void CBoss2_Die::Enter()
{
    CMonsterState::Enter();

    auto boss = static_cast<CBossPhase2*>(m_Owner);
    m_Controller = boss->Get_Controller();

    if (m_Controller)
    {
        auto animator = m_Controller->Get_OwnerAnimator();
        if (animator)
            animator->Play_Animation(L"Boss2_Dead", ANIMSTATE::ONCE);
    }

    boss->Trigger_OnDead();

    m_fCurStateElapsedTime = 0.f;
}

_int CBoss2_Die::Update_State(const _float& timeDelta)
{
    m_fCurStateElapsedTime += timeDelta;

    auto animator = m_Controller->Get_OwnerAnimator();

    if (animator && animator->Is_AnimationFinished())
    {
        return CBossPhase2::PATTERN::PHASE2_FOLLOWER;
    }

    return CBossPhase2::PATTERN::PHASE2_DEAD;
}

void CBoss2_Die::Exit()
{
    CMonsterState::Exit();

    // 상태 끝날 때
}

HRESULT CBoss2_Die::Ready_State()
{
    m_fMinStateTime = 0.5f;
    m_Pattern_Cooldown = 30.f;

    return S_OK;
}

CBoss2_Die* CBoss2_Die::Create(CGameObject* owner)
{
    auto state = new CBoss2_Die(owner);

    if (FAILED(state->Ready_State()))
    {
        Safe_Release(state);
        MSG_BOX("Boss2 Die Create Failed");
        return nullptr;
    }

    return state;
}

void CBoss2_Die::Free()
{
    
}
