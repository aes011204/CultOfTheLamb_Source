#pragma once
#include "CMonsterState.h"

class CEligos;
class CEligos_StateMachine;

class CEligos_Dead :
    public CMonsterState
{
public:
    enum class PHASE { CHANGE, TALK, DISAPPEAR };
    PHASE m_ePhase;
private:
    explicit CEligos_Dead(CEligos* pOwner, CEligos_StateMachine* pStateMachine);
    virtual ~CEligos_Dead() override;

public:
    virtual void Enter();
    virtual void Exit();

public:
    virtual HRESULT Ready_State();
    virtual _int    Update_State(const _float& timeDelta);
    virtual void    LateUpdate_State(const _float& timeDelta);

public:
    static CEligos_Dead* Create(CEligos* pOwner, CEligos_StateMachine* pStateMachine);

private:
    virtual void Free();

private:
    CEligos* m_pOwner;
    CEligos_StateMachine* m_pStateMachine;

//public:
//    using   OnTalkStartCallback = function<void()>;
//    void    Set_OnTalkStart(OnTalkStartCallback callback) { m_OnTalkStart = callback; }
//
//    void    Set_DialogueFinished() { m_DialogueFinished = true; }
//
//private:
//    OnTalkStartCallback m_OnTalkStart = nullptr;
//    _bool               m_DialogueFinished = false;


};

