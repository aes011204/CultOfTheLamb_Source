#include "pch.h"
#include "CBoss2_StateMachine.h"

#include "CBoss2_BulletAttack1.h"
#include "CBoss2_BulletAttack2.h"
#include "CBoss2_Idle.h"
#include "CBossPhase2.h"
#include "CMonsterState.h"
#include "CBoss2_Start.h"
#include "CBoss2_ChainAttack.h"
#include "CBoss2_Die.h"
#include "CBoss2_Follower.h"
#include "CBoss2_Groggy.h"
#include "CBoss2_SummonFollower.h"

CBoss2_StateMachine::CBoss2_StateMachine(CMonsterBase* owner)
    : m_Owner(owner)
{
}

CBoss2_StateMachine::~CBoss2_StateMachine()
{
    CBoss2_StateMachine::Free();
       
}

HRESULT CBoss2_StateMachine::Ready_StateMachine()
{
    m_vecState.resize(CBossPhase2::PATTERN::PHASE2_END);

    CMonsterState* state = nullptr;

    NULL_CHECK_RETURN(m_Owner, E_FAIL);

#pragma region 상태 등록
    {
        // Summon
        if (FAILED(RegisterState<CBoss2_SummonFollower>(CBossPhase2::PATTERN::PHASE2_SUMMON, m_Owner)))
            return E_FAIL;

        // Start
        if (FAILED(RegisterState<CBoss2_Start>(CBossPhase2::PATTERN::PHASE2_START, m_Owner)))
            return E_FAIL;

        // Idle
        if (FAILED(RegisterState<CBoss2_Idle>(CBossPhase2::PATTERN::PHASE2_IDLE, m_Owner)))
            return E_FAIL;

        // ChainAttack
        if (FAILED(RegisterState<CBoss2_ChainAttack>(CBossPhase2::PATTERN::PHASE2_CHAIN, m_Owner)))
            return E_FAIL;

        // Bullet1
        if (FAILED(RegisterState<CBoss2_BulletAttack1>(CBossPhase2::PATTERN::PHASE2_BULLET1, m_Owner)))
            return E_FAIL;

        // Bullet2
        if (FAILED(RegisterState<CBoss2_BulletAttack2>(CBossPhase2::PATTERN::PHASE2_BULLET2, m_Owner)))
            return E_FAIL;

        // Groggy
        if (FAILED(RegisterState<CBoss2_Groggy>(CBossPhase2::PATTERN::PHASE2_GROGGY, m_Owner)))
            return E_FAIL;

        // Die
        if (FAILED(RegisterState<CBoss2_Die>(CBossPhase2::PATTERN::PHASE2_DEAD, m_Owner)))
            return E_FAIL;

        // Follower
        if (FAILED(RegisterState<CBoss2_Follower>(CBossPhase2::PATTERN::PHASE2_FOLLOWER, m_Owner)))
            return E_FAIL;

    }
#pragma endregion
    

    // 초기 상태 설정
    m_CurState = m_vecState[CBossPhase2::PATTERN::PHASE2_SUMMON];
    m_CurStateKey = CBossPhase2::PATTERN::PHASE2_SUMMON;
    m_CurState->Enter();

    return S_OK;
}

_int CBoss2_StateMachine::Update_StateMachine(const _float& timeDelta)
{
    if (m_CurState)
    {
        _uint nextState = m_CurState->Update_State(timeDelta);

        if (nextState < CBossPhase2::PATTERN::PHASE2_END && nextState != m_CurStateKey)
        {
            Change_State(nextState);
        }
    }

    return 0;
}

void CBoss2_StateMachine::Change_State(_uint dwStateKey)
{
    if (m_CurStateKey == dwStateKey)
        return;

    if (m_vecState[dwStateKey] == nullptr || dwStateKey >= CBossPhase2::PATTERN::PHASE2_END)
        return;

    // 기존 상태 정리
    if (m_CurState)
        m_CurState->Exit();

    m_PrevStateKey = m_CurStateKey;
    m_CurStateKey = dwStateKey;

    m_CurState = m_vecState[dwStateKey];
    m_CurState->Enter();
}

CBoss2_StateMachine* CBoss2_StateMachine::Create(CMonsterBase* owner)
{
    CBoss2_StateMachine* stateMachine = new CBoss2_StateMachine(owner);

    if (FAILED(stateMachine->Ready_StateMachine()))
    {
        Safe_Release(stateMachine);
        MSG_BOX("Boss2 State Machine Create Failed");
        return nullptr;
    }

    return stateMachine;
}

void CBoss2_StateMachine::Free()
{
    CStateMachine::Free();

}
