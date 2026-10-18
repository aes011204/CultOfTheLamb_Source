#include "pch.h"
#include "CEyeball_Die.h"
#include "CTransform.h"
#include "CAnimator.h"
#include "CEyeball.h"
#include "CMonsterController.h"

CEyeball_Die::CEyeball_Die(CMonsterBase* owner)
    : m_Owner(owner), m_Pattern_Cooldown(0)
{
}

CEyeball_Die::~CEyeball_Die()
{
}

void CEyeball_Die::Enter()
{
    CMonsterState::Enter();

    auto eyeball = static_cast<CEyeball*>(m_Owner);
    m_Controller = eyeball->Get_Controller();

    if (m_Controller)
    {
        auto animator = m_Controller->Get_OwnerAnimator();
        if (animator)
            animator->Play_Animation(L"Eyeball_Die", ANIMSTATE::ONCE);
    }

    m_fCurStateElapsedTime = 0.f;
}

_int CEyeball_Die::Update_State(const _float& timeDelta)
{
    m_fCurStateElapsedTime += timeDelta;

    if (m_fCurStateElapsedTime >= m_Pattern_Cooldown)
    {
        if (m_Controller->Get_OwnerAnimator()->Is_AnimationFinished())
        {
            auto eyeball = static_cast<CEyeball*>(m_Owner);
            if (eyeball)
                eyeball->Trigger_OnDeath();

            // 사망
            m_Owner->Mark_As_PendingKill();
        }
    }

    return CEyeball::PATTERN::EYEBALL_DEAD;
}

void CEyeball_Die::Exit()
{
    CMonsterState::Exit();

    // 상태 끝날 때
}

HRESULT CEyeball_Die::Ready_State()
{
    m_fMinStateTime = 0.5f;
    m_Pattern_Cooldown = 1.0f;

    return S_OK;
}

CEyeball_Die* CEyeball_Die::Create(CMonsterBase* owner)
{
    auto state = new CEyeball_Die(owner);

    if (FAILED(state->Ready_State()))
    {
        Safe_Release(state);
        MSG_BOX("Eyeball Die Create Failed");
        return nullptr;
    }

    return state;
}

void CEyeball_Die::Free()
{
    
}
