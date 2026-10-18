#pragma once
#include "CBase.h"
#include "CStateMachine.h"
#include "Client_Enum.h"

class CBossPhase1;
class CMonsterState;
class CMonsterBase;

class CBossStateMachine : public CStateMachine
{
protected:
    explicit CBossStateMachine(CMonsterBase* owner);
    virtual ~CBossStateMachine();

public:
    virtual HRESULT Ready_StateMachine();
    _int    Update_StateMachine(const _float& timeDelta);
    void    LateUpdate_StateMachine(const _float& timeDelta);
    virtual void Change_State(_uint changeStateKey);
    _int GetCurStateNum();

    CBaseState* Get_CurState() { return m_CurState; }
    _uint Get_PrevStateKey() { return  m_PrevStateKey; }

    CMonsterState* Get_State(_uint stateKey);

private:
    CMonsterBase* m_pOwner;

protected:
    HRESULT Ready_BossStates();

public:
    static CBossStateMachine* Create(CMonsterBase* owner);

protected:
    virtual void Free();
};

