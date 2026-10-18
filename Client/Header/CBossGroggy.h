#pragma once
#include "CMonsterState.h"

class CBossPhase1;
class CBossStateMachine;

class CBossGroggy :
    public CMonsterState
{
public:
    enum class PHASE { DOWN, LOOP, UP };
    PHASE m_ePhase;
private:
    explicit CBossGroggy(CBossPhase1* pOwner, CBossStateMachine* pStateMachine);
    virtual ~CBossGroggy();

public:
    virtual void Enter();
    virtual void Exit();


public:
    virtual HRESULT Ready_State();
    virtual _int    Update_State(const _float& timeDelta);
    virtual void    LateUpdate_State(const _float& timeDelta);
    virtual void    Render_State();

private:
    CBossPhase1* m_pOwner;
    CBossStateMachine* m_pStateMachine;
public:
    static CBossGroggy* Create(CBossPhase1* pOwner, CBossStateMachine* pStateMachine);

private:
    virtual void Free();

};

