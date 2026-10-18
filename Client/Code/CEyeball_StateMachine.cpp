#include "pch.h"
#include "CEyeball_StateMachine.h"
#include "CEyeball.h"
#include "CEyeball_Attack.h"
#include "CEyeball_CenterAttack.h"
#include "CEyeball_Die.h"
#include "CEyeball_Enter.h"
#include "CEyeball_Exit.h"
#include "CEyeball_StartGround.h"
#include "CEyeball_Idle.h"
#include "CEyeball_Start.h"
#include "CEyeball_StartExit.h"
#include "CMonsterState.h"
#include "CEyeball_Ground_Move.h"

CEyeball_StateMachine::CEyeball_StateMachine(CMonsterBase* owner)
    : m_Owner(owner)
{
}

CEyeball_StateMachine::~CEyeball_StateMachine()
{
    CEyeball_StateMachine::Free();
       
}

HRESULT CEyeball_StateMachine::Ready_StateMachine()
{
    m_vecState.resize(CEyeball::PATTERN::EYEBALL_END);

    CMonsterState* state = nullptr;

    NULL_CHECK_RETURN(m_Owner, E_FAIL);

#pragma region 상태 등록
    {
        // Start
        if (FAILED(RegisterState<CEyeball_Start>(CEyeball::PATTERN::EYEBALL_START, m_Owner)))
            return E_FAIL;

        // Idle
        if (FAILED(RegisterState<CEyeball_Idle>(CEyeball::PATTERN::EYEBALL_IDLE, m_Owner)))
            return E_FAIL;

        // Start Exit
        if (FAILED(RegisterState<CEyeball_StartExit>(CEyeball::PATTERN::EYEBALL_START_EXIT, m_Owner)))
            return E_FAIL;

        // Exit
        if (FAILED(RegisterState<CEyeball_Exit>(CEyeball::PATTERN::EYEBALL_EXIT, m_Owner)))
            return E_FAIL;

        // Enter
        if (FAILED(RegisterState<CEyeball_Enter>(CEyeball::PATTERN::EYEBALL_ENTER, m_Owner)))
            return E_FAIL;

        // Move -> Idle로 대체. 일단은
        //if (FAILED(RegisterState<CEyeball>(CEyeball::PATTERN::EYEBALL_MOVE, m_Owner)))
        //    return E_FAIL;

        // Start Ground
        if (FAILED(RegisterState<CEyeball_StartGround>(CEyeball::PATTERN::EYEBALL_START_GROUND, m_Owner)))
            return E_FAIL;

        // Ground Move -> 땅속에서 랜덤으로 움직이게.
        if (FAILED(RegisterState<CEyeball_Ground_Move>(CEyeball::PATTERN::EYEBALL_GROUND_MOVE, m_Owner)))
            return E_FAIL;

        // Attack
        if (FAILED(RegisterState<CEyeball_Attack>(CEyeball::PATTERN::EYEBALL_ATTACK, m_Owner)))
            return E_FAIL;

        // Center Attack
        if (FAILED(RegisterState<CEyeball_CenterAttack>(CEyeball::PATTERN::EYEBALL_CENTER_ATTACK, m_Owner)))
            return E_FAIL;

        // Dead
        if (FAILED(RegisterState<CEyeball_Die>(CEyeball::PATTERN::EYEBALL_DEAD, m_Owner)))
            return E_FAIL;

        
    }
#pragma endregion
    

    // 초기 상태 설정
    m_CurState = m_vecState[CEyeball::PATTERN::EYEBALL_START];
    m_CurStateKey = CEyeball::PATTERN::EYEBALL_START;
    m_CurState->Enter();

    return S_OK;
}

_int CEyeball_StateMachine::Update_StateMachine(const _float& timeDelta)
{
    if (m_CurState)
    {
        _uint nextState = m_CurState->Update_State(timeDelta);

        if (nextState < CEyeball::PATTERN::EYEBALL_END && nextState != m_CurStateKey)
        {
            Change_State(nextState);
        }
    }

    return 0;
}

void CEyeball_StateMachine::Change_State(_uint dwStateKey)
{
    if (m_vecState[dwStateKey] == nullptr || dwStateKey >= CEyeball::PATTERN::EYEBALL_END)
        return;

    // 기존 상태 정리
    if (m_CurState)
        m_CurState->Exit();

    m_PrevStateKey = m_CurStateKey;
    m_CurStateKey = dwStateKey;
    m_CurState = m_vecState[dwStateKey];

    m_CurState->Enter();
}

CEyeball_StateMachine* CEyeball_StateMachine::Create(CMonsterBase* owner)
{
    CEyeball_StateMachine* stateMachine = new CEyeball_StateMachine(owner);

    if (FAILED(stateMachine->Ready_StateMachine()))
    {
        Safe_Release(stateMachine);
        MSG_BOX("Eyeball State Machine Create Failed");
        return nullptr;
    }

    return stateMachine;
}

void CEyeball_StateMachine::Free()
{
    CStateMachine::Free();

}
