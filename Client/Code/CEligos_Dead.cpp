#include "pch.h"
#include "CEligos_Dead.h"
#include "CMonsterState.h"
#include "CEligos.h"
#include "CEligos_StateMachine.h"
#include "CTransform.h"
#include "CAnimator.h"
#include "CSceneMgr.h"
#include "CSpriteAnimation.h"

CEligos_Dead::CEligos_Dead(CEligos* pOwner, CEligos_StateMachine* pStateMachine)
{
    m_pOwner = pOwner;
    m_pStateMachine = pStateMachine;
}

CEligos_Dead::~CEligos_Dead()
{

}

void CEligos_Dead::Enter()
{
    CMonsterState::Enter();
    // 일단 지금은 
    m_pOwnerAnimatorCom->Play_Animation(L"BossGroggy_start", ANIMSTATE::ONCE);

    m_ePhase = PHASE::CHANGE;
}

void CEligos_Dead::Exit()
{
    CMonsterState::Exit();
}

HRESULT CEligos_Dead::Ready_State()
{
    m_fMinStateTime = 0.f;

    m_pOwnerTransformCom = static_cast<CTransform*>(m_pOwner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));
    m_pOwnerAnimatorCom = static_cast<CAnimator*>(m_pOwner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::ANIMATOR));

    return S_OK;
}

_int CEligos_Dead::Update_State(const _float& timeDelta)
{
    CMonsterState::Update_State(timeDelta);

    if (m_pOwnerAnimatorCom->Get_CurAnimation() == nullptr)
        return CEligos::ELIGOSSTATE::BOSSDEAD;

    switch (m_ePhase)
    {
    case PHASE::CHANGE:

        if (m_pOwnerAnimatorCom->Get_CurFrame() == m_pOwnerAnimatorCom->Get_MaxFrame() - 1)
        {
            m_ePhase = PHASE::TALK;

            //if (m_OnTalkStart)
            //    m_OnTalkStart();
        }

        break;

    case PHASE::TALK:
        m_pOwnerAnimatorCom->Play_Animation(L"BossDead", ANIMSTATE::LOOP);

       // if (m_DialogueFinished)
        {
            m_ePhase = PHASE::DISAPPEAR;
        }
        break;

    case PHASE::DISAPPEAR:
        m_pOwnerAnimatorCom->Play_Animation(L"BossMove_Down", ANIMSTATE::ONCE);

        if (m_pOwnerAnimatorCom->Get_CurFrame() == m_pOwnerAnimatorCom->Get_MaxFrame() - 1)
        {
            //CSceneMgr::GetInstance()->Change_SceneMgr(SCENETYPE::SC_BOSS_STAGE2);

            return CEligos::ELIGOSSTATE::BOSSIDLE;   // 보스 끝남 
        }
    }

    return CEligos::ELIGOSSTATE::BOSSDEAD;
}

void CEligos_Dead::LateUpdate_State(const _float& timeDelta)
{
}

CEligos_Dead* CEligos_Dead::Create(CEligos* pOwner, CEligos_StateMachine* pStateMachine)
{
    auto BossDead = new CEligos_Dead(pOwner, pStateMachine);

    if (FAILED(BossDead->Ready_State()))
    {
        MSG_BOX("BossDead Create Failed");
        Safe_Release(BossDead);
        return nullptr;
    }

    return BossDead;
}

void CEligos_Dead::Free()
{

}
