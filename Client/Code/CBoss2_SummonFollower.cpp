#include "pch.h"
#include "CBoss2_SummonFollower.h"
#include "CBossController.h"
#include "CBossPhase2.h"
#include "CAnimator.h"
#include "CBoss_SlaveCow.h"
#include "CBoss_SlaveElephant.h"
#include "CBoss_SlavePig.h"
#include "CCreateHelper.h"
#include "CEyeball.h"
#include "CCreateHelper.h"
#include "CTransform.h"

CBoss2_SummonFollower::CBoss2_SummonFollower(CMonsterBase* owner)
    : m_Owner(owner), m_Pattern_Cooldown(0)
{

}

CBoss2_SummonFollower::~CBoss2_SummonFollower()
{
}

HRESULT CBoss2_SummonFollower::Ready_State()
{
    m_fMinStateTime = 0.5f;
    m_Pattern_Cooldown = 3.0f;

    return S_OK;
}

void CBoss2_SummonFollower::Enter()
{
    CMonsterState::Enter();

    auto boss = static_cast<CBossPhase2*>(m_Owner);
    m_Controller = boss->Get_Controller();

    if (m_Controller)
    {
        auto animator = m_Controller->Get_OwnerAnimator();

        if (animator)
        {
            animator->Play_Animation(L"Boss2_Roar_Alive", ANIMSTATE::ONCE);
        }
    }

    // 카메라 줌인
    boss->Trigger_OnSummonStart();

    m_fCurStateElapsedTime = 0.f;

    m_DialogueFinished = false;
    m_IsSummon = false;

    m_Phase = SummonPhase::ROAR;
}

_int CBoss2_SummonFollower::Update_State(const _float& timeDelta)
{
    auto boss = static_cast<CBossPhase2*>(m_Owner);
    auto animator = m_Controller->Get_OwnerAnimator();

    switch (m_Phase)
    {
    case SummonPhase::ROAR:
        if (animator && animator->Is_AnimationFinished())
        {
            animator->Play_Animation(L"Boss2_Idle_Alive", ANIMSTATE::LOOP);

            m_Phase = SummonPhase::DIALOGUE;
            boss->Trigger_OnSummonRoarEnd();
        }
        break;

    case SummonPhase::DIALOGUE:
        if (m_DialogueFinished)
        {
            m_Phase = SummonPhase::SUMMON;

            if (animator)
                animator->Play_Animation(L"Boss2_Summon", ANIMSTATE::ONCE);
        }
        break;

    case SummonPhase::SUMMON:

        if (!m_IsSummon && animator && animator->Get_CurFrame() >= 22)
        {
            m_IsSummon = true;

            boss->Trigger_OnSummonAnimStart();

            Summon_Follower();
        }

        // Summon 끝나면 Start로 전환
        if (animator && animator->Is_AnimationFinished())
        {
            // 끝났을 때 카메라
            boss->Trigger_OnSummonAnimEnd();

            return CBossPhase2::PATTERN::PHASE2_START;
        }

        break;
    }

    return CBossPhase2::PATTERN::PHASE2_SUMMON;
}

void CBoss2_SummonFollower::Exit()
{
    CMonsterState::Exit();

}

void CBoss2_SummonFollower::Summon_Follower()
{
    // 왼쪽
    {
        auto cow = CBoss_SlaveCow::Create(m_Owner->Get_GraphicDev());
        cow->Get_Transform()->Set_Pos(_vec3(34.1f, 3.5f, 43.7f));
        SpawnObjectOnLayer(cow, OBJTYPE::NPC);

        auto elephant = CBoss_SlaveElephant::Create(m_Owner->Get_GraphicDev());
        elephant->Get_Transform()->Set_Pos(_vec3(36.8f, 3.7f, 46.6f));
        SpawnObjectOnLayer(elephant, OBJTYPE::NPC);

        auto pig = CBoss_SlavePig::Create(m_Owner->Get_GraphicDev());
        pig->Get_Transform()->Set_Pos(_vec3(36.7f, 3.4f, 42.f));
        SpawnObjectOnLayer(pig, OBJTYPE::NPC);
    }

    // 오른쪽
    {
        auto cow = CBoss_SlaveCow::Create(m_Owner->Get_GraphicDev());
        cow->Get_Transform()->Set_Pos(_vec3(64.3f, 3.5f, 42.1f));
        SpawnObjectOnLayer(cow, OBJTYPE::NPC);

        auto elephant = CBoss_SlaveElephant::Create(m_Owner->Get_GraphicDev());
        elephant->Get_Transform()->Set_Pos(_vec3(62.4f, 3.7f, 40.0f));
        SpawnObjectOnLayer(elephant, OBJTYPE::NPC);

        auto pig = CBoss_SlavePig::Create(m_Owner->Get_GraphicDev());
        pig->Get_Transform()->Set_Pos(_vec3(62.5f, 3.4f, 45.5f));
        SpawnObjectOnLayer(pig, OBJTYPE::NPC);
    }
    
}

CBoss2_SummonFollower* CBoss2_SummonFollower::Create(CMonsterBase* owner)
{
    auto state = new CBoss2_SummonFollower(owner);

    if (FAILED(state->Ready_State()))
    {
        Safe_Release(state);
        MSG_BOX("Boss Summon Create Failed");
        return nullptr;
    }

    return state;
}

void CBoss2_SummonFollower::Free()
{
    
}
