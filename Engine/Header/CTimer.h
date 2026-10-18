#pragma once
#include "CBase.h"
#include "Engine_Define.h"

BEGIN(Engine)
class ENGINE_DLL CTimer : public CBase
{
private:
    explicit CTimer();
    ~CTimer() override;

public:
    _float Get_TimeDelta() const { return m_TimeDelta; }

public:
    HRESULT Ready_Timer();
    void    Update_Timer();

private:
    LARGE_INTEGER m_FrameTime;
    LARGE_INTEGER m_FixTime;
    LARGE_INTEGER m_LastTime;
    LARGE_INTEGER m_CpuTick;

    _float m_TimeDelta;

public:
    static CTimer* Create();
private:
    void Free() override;
};

END
