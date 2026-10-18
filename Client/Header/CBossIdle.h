#pragma once
#include "CMonsterState.h"

class CBossPhase1;
class CBossStateMachine;

namespace Engine
{
    class CCombatStat;
}

class CBossIdle :
    public CMonsterState
{
private:
    explicit CBossIdle(CBossPhase1* pOwner, CBossStateMachine* pStateMachine);
    virtual ~CBossIdle();

public:
    virtual void Enter();
    virtual void Exit();

public:
    virtual HRESULT Ready_State();
    virtual _int    Update_State(const _float& timeDelta);
    virtual void    LateUpdate_State(const _float& timeDelta);
    virtual void    Render_State() {}

private:
    CBossPhase1*    m_pOwner;
    CBossStateMachine* m_pStateMachine;
    CCombatStat* m_pOwnerStateCom;


    static int ChangeAPattern;
public:
    static CBossIdle* Create(CBossPhase1* pOwner, CBossStateMachine* pStateMachine);

private:
    virtual void Free();

};

