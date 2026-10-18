#pragma once
#include "CMonsterState.h"

class CEligos;
class CEligos_StateMachine;

class CEligos_Move :
    public CMonsterState
{
private:
    explicit CEligos_Move(CEligos* pOwner, CEligos_StateMachine* pStateMachine);
    virtual ~CEligos_Move();

public:
    virtual void Enter();
    virtual void Exit();


public:
    virtual HRESULT Ready_State();
    virtual _int    Update_State(const _float& timeDelta);
    virtual void    LateUpdate_State(const _float& timeDelta);
    virtual void    Render_State();

private:
    CEligos* m_pOwner;
    CEligos_StateMachine* m_pStateMachine;

public:
    static CEligos_Move* Create(CEligos* pOwner, CEligos_StateMachine* pStateMachine);

private:
    virtual void Free();
};

