#pragma once
#include "CBase.h"
#include "CStateMachine.h"
#include "Client_Enum.h"

class CMonsterBase;
class CMonsterState;

class CEligos_StateMachine :
    public CStateMachine
{
protected:
    explicit CEligos_StateMachine(CMonsterBase* owner);
    ~CEligos_StateMachine() override;

public:
    virtual HRESULT Ready_StateMachine()override;
    virtual _int    Update_StateMachine(const _float& timeDelta);
    void    LateUpdate_StateMachine(const _float& timeDelta);

    virtual void    Change_State(_uint changeStateKey);

    _uint Get_CurStateNum() { return m_CurStateKey; }

    CBaseState* Get_CurState() { return m_CurState; }
    _uint Get_PrevStateKey() { return  m_PrevStateKey; }

    CMonsterState* Get_State(_uint stateKey);
    _int GetCurStateNum();
private:
    CMonsterBase* m_pOwner;

protected:
    HRESULT Ready_BossStates();
public:
    static CEligos_StateMachine* Create(CMonsterBase* owner);


protected:
    void Free() override;
};

