#pragma once
#include "CMonsterState.h"
class CEligos;
class CEligos_StateMachine;

namespace Engine
{
    class CCombatStat;
}

class CEligos_Idle :
    public CMonsterState
{
private:
    explicit CEligos_Idle(CEligos* pOwner, CEligos_StateMachine* pStateMachine);
    virtual ~CEligos_Idle();

public:
    virtual void Enter();
    virtual void Exit();


public:
    virtual HRESULT Ready_State();
    virtual _int    Update_State(const _float& timeDelta);
    virtual void    LateUpdate_State(const _float& timeDelta);
    virtual void    Render_State();

private:
private:
    CEligos* m_pOwner;
    CEligos_StateMachine* m_pStateMachine;
    CCombatStat* m_pOwnerStateCom;

    static int ChangeAPattern;

public:
    static CEligos_Idle* Create(CEligos* pOwner, CEligos_StateMachine* pStateMachine);

private:
    virtual void Free();
};

