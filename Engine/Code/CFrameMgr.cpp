#include "CFrameMgr.h"

#include "CFrame.h"

IMPLEMENT_SINGLETON(CFrameMgr)

CFrameMgr::CFrameMgr()
{ }

CFrameMgr::~CFrameMgr()
{
    CFrameMgr::Free();
}

_bool CFrameMgr::IsPermit_Call(const wstring& frameTag, const _float& timeDelta)
{
    CFrame* frame = Find_Frame(move(frameTag));
    if (nullptr == frame)
        return false;

    return frame->IsPermit_Call(timeDelta);
}

HRESULT CFrameMgr::Ready_Frame(const wstring& frameTag, const _float& callLimit)
{
    CFrame* frame = Find_Frame(frameTag);

    if (nullptr != frame)
        return E_FAIL;

    frame = CFrame::Create(callLimit);
    if (nullptr == frame)
        return E_FAIL;

    m_Frames.insert({ frameTag, frame });

    return S_OK;
}

CFrame* CFrameMgr::Find_Frame(const wstring& frameTag)
{
    auto iter = find_if(m_Frames.begin(), m_Frames.end(), CTag_Finder(move(frameTag)));

    if (iter == m_Frames.end())
        return nullptr;

    return iter->second;
}

void CFrameMgr::Free()
{
    for_each(m_Frames.begin(), m_Frames.end(), CDeleteMap());
    m_Frames.clear();
}
