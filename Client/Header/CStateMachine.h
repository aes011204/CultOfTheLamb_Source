#pragma once
#include "CBase.h"

class CBaseState;

class CStateMachine : public CBase
{
protected:
    explicit CStateMachine();
    ~CStateMachine() override;

public:
    virtual HRESULT Ready_StateMachine() = 0;
    virtual _int    Update_StateMachine(const _float& timeDelta) = 0;
    virtual void    Change_State(_uint dwStateKey) = 0;

    _uint       Get_CurStateNum() { return m_CurStateKey; }
    CBaseState* Get_CurState() { return m_CurState; }

protected:
    vector<CBaseState*> m_vecState;
    CBaseState*         m_CurState;
    // CBaseState* m_pPrevState;

    _uint m_PrevStateKey;
    _uint m_CurStateKey;

protected:
    void Free() override;
};
