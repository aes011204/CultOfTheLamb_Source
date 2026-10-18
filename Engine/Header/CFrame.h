#pragma once
#include "CBase.h"
#include "Engine_Define.h"

BEGIN(Engine)
class ENGINE_DLL CFrame : public CBase
{
private:
    explicit CFrame();
    ~CFrame() override;

public:
    _bool IsPermit_Call(const _float& timeDelta);

public:
    HRESULT Ready_Frame(const _float& callLimit);

private:
    _float m_CallLimit;
    _float m_AccTimeDelta;

public:
    static CFrame* Create(const _float& callLimit);

private:
    void Free() override;
};

END
