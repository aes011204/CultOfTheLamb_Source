#include "pch.h"
#include "CEyeball_Exit.h"
#include "CBossController.h"
#include "CAnimator.h"
#include "CEyeball.h"

CEyeball_Exit::CEyeball_Exit(CMonsterBase* owner)
    : m_Owner(owner), m_Pattern_Cooldown(0)
{
}

CEyeball_Exit::~CEyeball_Exit()
{
}

void CEyeball_Exit::Enter()
{
    CMonsterState::Enter();

    auto eyeball = static_cast<CEyeball*>(m_Owner);
    m_Controller = eyeball->Get_Controller();

    if (m_Controller)
    {
        auto animator = m_Controller->Get_OwnerAnimator();
        if (animator)
            animator->Play_Animation(L"Eyeball_Exit", ANIMSTATE::ONCE);
    }

    m_fCurStateElapsedTime = 0.f;
}

_int CEyeball_Exit::Update_State(const _float& timeDelta)
{
    m_fCurStateElapsedTime += timeDelta;

    auto animator = m_Controller->Get_OwnerAnimator();
    if (animator)
    {
        if (animator->Is_AnimationFinished())
        {
            return CEyeball::PATTERN::EYEBALL_START_GROUND;
        }
    }

    return CEyeball::PATTERN::EYEBALL_EXIT;
}

void CEyeball_Exit::Exit()
{
    CMonsterState::Exit();

    // 상태 끝날 때
}

HRESULT CEyeball_Exit::Ready_State()
{
    m_fMinStateTime = 0.5f;
    m_Pattern_Cooldown = 3.0f;

    return S_OK;
}

CEyeball_Exit* CEyeball_Exit::Create(CMonsterBase* owner)
{
    auto state = new CEyeball_Exit(owner);

    if (FAILED(state->Ready_State()))
    {
        Safe_Release(state);
        MSG_BOX("Boss Idle Create Failed");
        return nullptr;
    }

    return state;
}

void CEyeball_Exit::Free()
{
    
}
