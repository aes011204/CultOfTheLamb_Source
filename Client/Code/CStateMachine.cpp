#include "pch.h"
#include "CStateMachine.h"
#include "CBaseState.h"

CStateMachine::CStateMachine() :m_CurState(nullptr),m_PrevStateKey(0),m_CurStateKey(0)
{
}

CStateMachine::~CStateMachine()
{
}

void CStateMachine::Free()
{
    for_each(m_vecState.begin(), m_vecState.end(), [](CBaseState*& state)
        {
        Safe_Release(state); 
        });
    m_vecState.clear();
}
