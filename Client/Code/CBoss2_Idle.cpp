#include "pch.h"
#include "CBoss2_Idle.h"

#include "CBossController.h"
#include "CBossPhase2.h"
#include "CAnimator.h"
#include "CRandomMgr.h"

CBoss2_Idle::CBoss2_Idle(CGameObject* owner)
    : m_Owner(owner), m_Pattern_Cooldown(0)
{
}

CBoss2_Idle::~CBoss2_Idle()
{
}

void CBoss2_Idle::Enter()
{
    CMonsterState::Enter();

    auto boss = static_cast<CBossPhase2*>(m_Owner);
    m_Controller = boss->Get_Controller();

    if (m_Controller)
    {
        auto animator = m_Controller->Get_OwnerAnimator();
        if (animator)
            animator->Play_Animation(L"Boss2_Idle", ANIMSTATE::LOOP);
    }

    m_fCurStateElapsedTime = 0.f;

    m_Pattern_Cooldown = CRandomMgr::GetInstance()->Get_Int_Random(m_CooldownMin, m_CooldownMax);
}

_int CBoss2_Idle::Update_State(const _float& timeDelta)
{
    m_fCurStateElapsedTime += timeDelta;

    if (m_fCurStateElapsedTime >= m_Pattern_Cooldown)
    {
        return CBossPhase2::PATTERN::PHASE2_CHAIN;
    }

    return CBossPhase2::PATTERN::PHASE2_IDLE;
}

void CBoss2_Idle::Exit()
{
    CMonsterState::Exit();

    // 상태 끝날 때
}

HRESULT CBoss2_Idle::Ready_State()
{
    m_fMinStateTime = 0.5f;

    m_CooldownMin = 8;
    m_CooldownMax = 13;

    m_Pattern_Cooldown = 10.f;

    return S_OK;
}

CBoss2_Idle* CBoss2_Idle::Create(CGameObject* owner)
{
    auto state = new CBoss2_Idle(owner);

    if (FAILED(state->Ready_State()))
    {
        Safe_Release(state);
        MSG_BOX("Boss Idle Create Failed");
        return nullptr;
    }

    return state;
}

void CBoss2_Idle::Free()
{
    
}
