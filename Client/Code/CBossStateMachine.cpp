#include "pch.h"
#include "CBossStateMachine.h"
#include "CBossPhase1.h"
#include "CMonsterState.h"

#include "CBossAwake.h"
#include "CBossBulletAttack.h"
#include "CBossChainAttack.h"
#include "CBossDead.h"
#include "CBossGroggy.h"    
#include "CBossIdle.h"
#include "CBossMove.h"
#include "CBossTalk.h"

CBossStateMachine::CBossStateMachine(CMonsterBase* owner)
    : m_pOwner(owner)
{
}

CBossStateMachine::~CBossStateMachine()
{
    CBossStateMachine::Free();
}

HRESULT CBossStateMachine::Ready_StateMachine()
{
    Ready_BossStates();

    m_CurState = m_vecState[CBossPhase1::BOSSPHASE1::TALK];

    m_CurState->Enter();
    
    return S_OK;
}

_int CBossStateMachine::Update_StateMachine(const _float& timeDelta)
{
    // 씬메니져의 리퀘스트 씬 같은 느낌?
    if (m_CurState)
    {
        _uint nextState = m_CurState->Update_State(timeDelta);

        if (nextState == CBossPhase1::BOSSPHASE1::BOSS_END)
        {
            return nextState;
        }

        if (nextState < CBossPhase1::BOSSPHASE1::BOSS_END && nextState != m_CurStateKey)
        {
            Change_State(nextState);    
        }
    }

    return (_int)m_CurStateKey;
}

void CBossStateMachine::LateUpdate_StateMachine(const _float& timeDelta)
{
    m_CurState->LateUpdate_State(timeDelta);
}

void CBossStateMachine::Change_State(_uint changeStateKey)
{
    // 체인지 씬이랑 비슷한 느낌

    if (m_CurStateKey == changeStateKey)
        return; // 이전이랑 같은 상황

    if(m_vecState[changeStateKey] == nullptr || changeStateKey >= CBossPhase1::BOSSPHASE1::BOSS_END)
        return; // 인덱스 범위 확인

    // 이전 상태 나가기
    if (m_CurState)
       m_CurState->Exit();

    // 이전 상태 등록
    //m_pPrevState = m_CurState;
    m_PrevStateKey = m_CurStateKey;
    m_CurStateKey = changeStateKey;

    // 새 상태로 가져오기
    m_CurState = m_vecState[changeStateKey];
    m_CurState->Enter();

}

_int CBossStateMachine::GetCurStateNum()
{
    { return m_CurState->Get_StateKey(); }
}

CMonsterState* CBossStateMachine::Get_State(_uint stateKey)
{
    if (stateKey < m_vecState.size())
        return static_cast<CMonsterState*>(m_vecState[stateKey]);

    return nullptr;
}

HRESULT CBossStateMachine::Ready_BossStates()
{
    m_vecState.resize(CBossPhase1::BOSSPHASE1::BOSS_END);
    //m_vecState.resize(2);

    CMonsterState* pState = nullptr;

    CBossPhase1* phase1 = dynamic_cast<CBossPhase1*>(m_pOwner);
    NULL_CHECK_RETURN(phase1, E_FAIL);

    if (!(pState = CBossAwake::Create(phase1, this)))
        return E_FAIL;

    m_vecState[CBossPhase1::BOSSPHASE1::BOSSAWAKE] = pState;

    if (nullptr == (pState = CBossBulletAttack::Create(phase1, this)))
        return E_FAIL;
    m_vecState[CBossPhase1::BOSSPHASE1::BULLETATTACK] = pState;
    
    if (nullptr == (pState = CBossChainAttack::Create(phase1, this)))
        return E_FAIL;
    m_vecState[CBossPhase1::BOSSPHASE1::CHAINATTACK] = pState;
    
    if (nullptr == (pState = CBossDead::Create(phase1, this)))
        return E_FAIL;
    m_vecState[CBossPhase1::BOSSPHASE1::BOSSDEAD] = pState;
    
    if (nullptr == (pState = CBossIdle::Create(phase1, this)))
        return E_FAIL;
    m_vecState[CBossPhase1::BOSSPHASE1::BOSSIDLE] = pState;
    
    if (nullptr == (pState = CBossMove::Create(phase1, this)))
        return E_FAIL;
    m_vecState[CBossPhase1::BOSSPHASE1::BOSSMOVE] = pState;
    
    if (nullptr == (pState = CBossGroggy::Create(phase1, this)))
        return E_FAIL;
    m_vecState[CBossPhase1::BOSSPHASE1::BOSSGROGGRY] = pState;
    
    if (nullptr == (pState = CBossTalk::Create(phase1, this)))
        return E_FAIL;
    m_vecState[CBossPhase1::BOSSPHASE1::TALK] = pState;

    return S_OK;
}


CBossStateMachine* CBossStateMachine::Create(CMonsterBase* owner)
{
    auto StateMachine = new CBossStateMachine(owner);
    if (FAILED(StateMachine->Ready_StateMachine()))
    {
        Safe_Release(StateMachine);
        MSG_BOX("StateMachine Created Failed");
        return nullptr;
    }

    return StateMachine;

}

void CBossStateMachine::Free()
{

    CStateMachine::Free();
}
