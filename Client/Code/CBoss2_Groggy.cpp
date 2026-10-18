#include "pch.h"
#include "CBoss2_Groggy.h"
#include "CBossController.h"
#include "CBossPhase2.h"
#include "CAnimator.h"
#include "CWarningChain.h"
#include "CTransform.h"
#include "CCreateHelper.h"
#include "CTimerMgr.h"

CBoss2_Groggy::CBoss2_Groggy(CGameObject* owner)
    : m_Owner(owner), m_Pattern_Cooldown(0)
{
}

CBoss2_Groggy::~CBoss2_Groggy()
{
}

void CBoss2_Groggy::Enter()
{
    CMonsterState::Enter();

    auto boss = static_cast<CBossPhase2*>(m_Owner);
    m_Controller = boss->Get_Controller();

    if (m_Controller)
    {
        auto animator = m_Controller->Get_OwnerAnimator();
        if (animator)
            animator->Play_Animation(L"Boss2_DownStart", ANIMSTATE::ONCE);
    }

    m_fCurStateElapsedTime = 0.f;

}

_int CBoss2_Groggy::Update_State(const _float& timeDelta)
{
    m_fCurStateElapsedTime += timeDelta;

    auto animator = m_Controller->Get_OwnerAnimator();

    if (animator && animator->Is_AnimationFinished())
    {
        animator->Play_Animation(L"Boss2_DownIdle", ANIMSTATE::LOOP);
    }

    return CBossPhase2::PATTERN::PHASE2_GROGGY;
}

void CBoss2_Groggy::Exit()
{
    CMonsterState::Exit();

    // 상태 끝날 때
}

HRESULT CBoss2_Groggy::Ready_State()
{
    m_fMinStateTime = 0.5f;
    m_Pattern_Cooldown = 5.f;

    return S_OK;
}

CBoss2_Groggy* CBoss2_Groggy::Create(CGameObject* owner)
{
    auto state = new CBoss2_Groggy(owner);

    if (FAILED(state->Ready_State()))
    {
        Safe_Release(state);
        MSG_BOX("Boss Eyeball Create Failed");
        return nullptr;
    }

    return state;
}

void CBoss2_Groggy::Free()
{
    
}
