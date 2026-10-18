#include "pch.h"
#include "CBossDead.h"
#include "CMonsterState.h"
#include "CBossPhase1.h"
#include "CBossStateMachine.h"
#include "CTransform.h"
#include "CAnimator.h"
#include "CSceneMgr.h"
#include "CSpriteAnimation.h"


CBossDead::CBossDead(CBossPhase1* pOwner, CBossStateMachine* pStateMachine)
{
    m_pOwner = pOwner;
    m_pStateMachine = pStateMachine;
}

CBossDead::~CBossDead()
{
}

void CBossDead::Enter()
{
    CMonsterState::Enter();

    m_pOwnerAnimatorCom->Play_Animation(L"BossGroggy_start", ANIMSTATE::ONCE);

    m_ePhase = PHASE::DOWN;
}

void CBossDead::Exit()
{
    CMonsterState::Exit();
}

HRESULT CBossDead::Ready_State()
{
    m_fMinStateTime = 0.f;

    m_pOwnerTransformCom = static_cast<CTransform*>(m_pOwner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));
    m_pOwnerAnimatorCom = static_cast<CAnimator*>(m_pOwner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::ANIMATOR));

    return S_OK;
}

_int CBossDead::Update_State(const _float& timeDelta)
{
    CMonsterState::Update_State(timeDelta);

    if (m_pOwnerAnimatorCom->Get_CurAnimation() == nullptr)
        return CBossPhase1::BOSSPHASE1::BOSSDEAD;

    switch (m_ePhase)
    {
    case PHASE::DOWN:

        if (m_pOwnerAnimatorCom->Get_CurFrame() == m_pOwnerAnimatorCom->Get_MaxFrame() - 1)
        {
            m_ePhase = PHASE::TALK;

            if (m_OnTalkStart)
                m_OnTalkStart();
        }
            
        break;

    case PHASE::TALK:
        m_pOwnerAnimatorCom->Play_Animation(L"BossDead", ANIMSTATE::LOOP);

        if (m_DialogueFinished)
        {
            m_ePhase = PHASE::DISAPPEAR;
        }
        break;

    case PHASE::DISAPPEAR:
        m_pOwnerAnimatorCom->Play_Animation(L"BossMove_Down", ANIMSTATE::ONCE);

        if (m_pOwnerAnimatorCom->Get_CurFrame() == m_pOwnerAnimatorCom->Get_MaxFrame() - 25)
        {
            CSceneMgr::GetInstance()->Change_SceneMgr(SCENETYPE::SC_BOSS_STAGE2);

            return CBossPhase1::BOSSPHASE1::BOSS_END;   // 보스 끝남 
        }
    }

    return CBossPhase1::BOSSPHASE1::BOSSDEAD;
}

void CBossDead::LateUpdate_State(const _float& timeDelta)
{

}

CBossDead* CBossDead::Create(CBossPhase1* pOwner, CBossStateMachine* pStateMachine)
{
    auto BossDead = new CBossDead(pOwner, pStateMachine);

    if (FAILED(BossDead->Ready_State()))
    {
        MSG_BOX("BossDead Create Failed");
        Safe_Release(BossDead);
        return nullptr;
    }

    return BossDead;
}

void CBossDead::Free()
{
}
