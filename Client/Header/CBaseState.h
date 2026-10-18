#pragma once
#include "CBase.h"
#include "Engine_Define.h"

class CBaseState abstract :
    public CBase 
{
protected:
    explicit CBaseState();
    virtual ~CBaseState();

public:
    virtual void Enter() = 0;
    virtual void Exit() = 0;

    
public:
    virtual HRESULT Ready_State() { return S_OK; }
    virtual _int    Update_State(const _float& timeDelta) { return 0; }
    virtual void    LateUpdate_State(const _float& timeDelta) {}
    virtual void    Render_State() {}

public:
    virtual _uint Get_StateKey() const { return m_dwCurStateKey; }

protected:
    _uint m_dwCurStateKey;

    _bool m_bStarted;
    _bool m_bExited;
    _bool m_bCanTransit;



};

