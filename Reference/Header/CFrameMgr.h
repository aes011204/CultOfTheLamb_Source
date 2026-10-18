#pragma once
#include "CBase.h"
#include "Engine_Define.h"

BEGIN(Engine)
class CFrame;

class ENGINE_DLL CFrameMgr : public CBase
{
    DECLARE_SINGLETON(CFrameMgr)

private:
    explicit CFrameMgr();
    ~CFrameMgr() override;

public:
    _bool   IsPermit_Call(const wstring& frameTag, const _float& timeDelta);
    HRESULT Ready_Frame(const wstring& frameTag, const _float& callLimit);

private:
    CFrame* Find_Frame(const wstring& frameTag);

private:
    map<wstring, CFrame*> m_Frames;

public:
    void Free() override;
};

END
