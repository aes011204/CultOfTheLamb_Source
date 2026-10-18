#include "pch.h"
#include "CBaseState.h"

CBaseState::CBaseState():m_dwCurStateKey(0),m_bStarted(true),m_bExited(false),m_bCanTransit(false)
{
}

CBaseState::~CBaseState()
{
}
