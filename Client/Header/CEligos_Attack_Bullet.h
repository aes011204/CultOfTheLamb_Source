#pragma once
#include "CMonsterState.h"

class CEligos;
class CEligos_StateMachine;

class CEligos_Attack_Bullet :
    public CMonsterState
{
public:
    enum class PHASE { MOVE, SHOOT, READY };
    PHASE m_ePhase;
private:
    explicit CEligos_Attack_Bullet(CEligos* pOwner, CEligos_StateMachine* pStateMachine);
    virtual ~CEligos_Attack_Bullet();

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
    static CEligos_Attack_Bullet* Create(CEligos* pOwner, CEligos_StateMachine* pStateMachine);

private:
    virtual void Free();
};

