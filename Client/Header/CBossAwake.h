#pragma once
#include "CMonsterState.h"

class CBossPhase1;
class CBossStateMachine;

class CBossAwake :
    public CMonsterState
{
public:
    enum class PHASE {AWAKE,MOVE };
    PHASE m_ePhase;
private:
    explicit CBossAwake(CBossPhase1* pOwner, CBossStateMachine* pStateMachine);
    virtual ~CBossAwake();

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

    bool tmpAniDone = false;
    _vec3 dir;

    //
    float m_Speed = 3;
public:
    static CBossAwake* Create(CBossPhase1* pOwner, CBossStateMachine* pStateMachine);

private:
    virtual void Free();
};

