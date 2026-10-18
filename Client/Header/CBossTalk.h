#pragma once
#include "CMonsterState.h"


class CBossPhase1;
class CBossStateMachine;

class CBossTalk :
    public CMonsterState
{
public:
    enum class PHASE { IDLE, TALK, CAHANGE };
    PHASE m_ePhase;
private:
    explicit CBossTalk(CBossPhase1* pOwner, CBossStateMachine* pStateMachine);
    virtual ~CBossTalk();

public:
    virtual void Enter();
    virtual void Exit();

    void Set_Phase(PHASE phase) { m_ePhase = phase; }


public:
    virtual HRESULT Ready_State();
    virtual _int    Update_State(const _float& timeDelta);
    virtual void    LateUpdate_State(const _float& timeDelta);
    virtual void    Render_State();

private:
    CBossPhase1* m_pOwner;
    CBossStateMachine* m_pStateMachine;

    bool Is_Talk = false;

public:
    static CBossTalk* Create(CBossPhase1* pOwner, CBossStateMachine* pStateMachine);

private:
    virtual void Free();

};

