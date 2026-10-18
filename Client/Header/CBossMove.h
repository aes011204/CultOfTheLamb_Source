#pragma once
#include "CMonsterState.h"

class CBossPhase1;
class CBossStateMachine;

class CBossMove :
    public CMonsterState
{
public:
    enum class PHASE { DOWN, WAIT,UP };
    PHASE m_ePhase;
private:
    explicit CBossMove(CBossPhase1* pOwner, CBossStateMachine* pStateMachine);
    virtual ~CBossMove();

public:
    virtual void Enter();
    virtual void Exit();


public:
    virtual HRESULT Ready_State();
    virtual _int    Update_State(const _float& timeDelta);
    virtual void    LateUpdate_State(const _float& timeDelta);
    virtual void    Render_State() {}

    void RandomMove(const _float& timeDelta, _vec3 BaseCenter, int BaseLenght, float interval);
private:
    CBossPhase1* m_pOwner;
    CBossStateMachine* m_pStateMachine;
public:
    static CBossMove* Create(CBossPhase1* pOwner, CBossStateMachine* pStateMachine);

private:
    virtual void Free();

};

