#include "pch.h"
#include "CNPCStateMachine.h"

#include "CBaseState.h"
#include "Client_Struct.h"
#include "CNPCBase.h"
#include "CNPCState_Build.h"
#include "CNPCState_ChopWood.h"
#include "CNPCState_Eat.h"
#include "CNPCState_Idle.h"
#include "CNPCState_Mining.h"
#include "CNPCState_Pray.h"
#include "CNPCState_PrayPeek.h"
#include "CNPCState_Rescued.h"
#include "CNPCState_Run.h"
#include "CNPCState_Spawn.h"
#include "CNPCState_WalkPast.h"
#include "CVillageStateMgr.h"

CNPCStateMachine::CNPCStateMachine()
{ }

CNPCStateMachine::~CNPCStateMachine()
{ }

HRESULT CNPCStateMachine::Ready_StateMachine()
{
    m_vecState.resize(static_cast<_uint>(NPC_STATE::STATE_END));

    if (FAILED(RegisterState<CNPCState_Idle>(NPC_STATE::IDLE, m_Owner)))
        return E_FAIL;

    if (FAILED(RegisterState<CNPCState_Run>(NPC_STATE::RUN, m_Owner)))
        return E_FAIL;

    if (FAILED(RegisterState<CNPCState_WalkPast>(NPC_STATE::WALKPAST, m_Owner)))
        return E_FAIL;

    if (FAILED(RegisterState<CNPCState_Build>(NPC_STATE::BUILD, m_Owner)))
        return E_FAIL;

    if (FAILED(RegisterState<CNPCState_ChopWood>(NPC_STATE::CHOPWOOD, m_Owner)))
        return E_FAIL;

    if (FAILED(RegisterState<CNPCState_Mining>(NPC_STATE::MINING, m_Owner)))
        return E_FAIL;

    if (FAILED(RegisterState<CNPCState_Eat>(NPC_STATE::EAT, m_Owner)))
        return E_FAIL;

    if (FAILED(RegisterState<CNPCState_Pray>(NPC_STATE::PRAY, m_Owner)))
        return E_FAIL;

    if (FAILED(RegisterState<CNPCState_PrayPeek>(NPC_STATE::PRAY_PEEK, m_Owner)))
        return E_FAIL;

    if (FAILED(RegisterState<CNPCState_Spawn>(NPC_STATE::SPAWN, m_Owner)))
        return E_FAIL;

    if (FAILED(RegisterState<CNPCState_Rescued>(NPC_STATE::RESCUED, m_Owner)))
        return E_FAIL;

    // 초기 상태
    NPC_STATE startState = NPC_STATE::SPAWN;

    // 저장된 상태 확인
    if (m_Owner)
    {
        SavedNpc* pState = CVillageStateMgr::GetInstance()->Get_NPCState(m_Owner->Get_Name());
        if (pState && pState->firstSpawnAnimPlayed)
            startState = NPC_STATE::IDLE;
    }

    // 만약 SPAWN 상태가 등록되지 않았다면 IDLE로 (안전장치)
    if (m_vecState[static_cast<_uint>(startState)] == nullptr)
        startState = NPC_STATE::IDLE;

    m_CurState    = m_vecState[static_cast<_uint>(startState)];
    m_CurStateKey = static_cast<_uint>(startState);
    m_CurState->Enter();

    return S_OK;
}

_int CNPCStateMachine::Update_StateMachine(const _float& timeDelta)
{
    if (m_CurState)
    {
        _uint nextState = m_CurState->Update_State(timeDelta);

        if (nextState != m_CurStateKey)
            Change_State(nextState);
    }

    return 0;
}

void CNPCStateMachine::Change_State(_uint dwStateKey)
{
    if (m_CurStateKey == dwStateKey)
        return;

    // 범위 체크 및 nullptr 체크
    if (dwStateKey >= m_vecState.size() || m_vecState[dwStateKey] == nullptr)
        return;

    // 기존 상태 정리
    if (m_CurState)
        m_CurState->Exit();

    m_PrevStateKey = m_CurStateKey;
    m_CurStateKey  = dwStateKey;

    m_CurState = m_vecState[dwStateKey];
    m_CurState->Enter();
}

CNPCStateMachine* CNPCStateMachine::Create(CNPCBase* owner)
{
    auto instance = new CNPCStateMachine;
    instance->Set_Owner(owner);

    if (FAILED(instance->Ready_StateMachine()))
    {
        Safe_Release(instance);
        return nullptr;
    }

    return instance;
}

void CNPCStateMachine::Free()
{
    CStateMachine::Free();
}
