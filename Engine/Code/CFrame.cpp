#include "CFrame.h"

CFrame::CFrame()
    : m_CallLimit(0.f), m_AccTimeDelta(0.f)
{ }

CFrame::~CFrame()
{ }

_bool CFrame::IsPermit_Call(const _float& timeDelta)
{
    m_AccTimeDelta += timeDelta;

    if (m_AccTimeDelta >= m_CallLimit)
    {
        m_AccTimeDelta = 0.f;

        return true;
    }

    return false;
}

HRESULT CFrame::Ready_Frame(const _float& callLimit)
{
    m_CallLimit = 1.f / callLimit;

    return S_OK;
}

CFrame* CFrame::Create(const _float& callLimit)
{
    auto frame = new CFrame;

    if (FAILED(frame->Ready_Frame(callLimit)))
    {
        Safe_Release(frame);
        MSG_BOX("Frame create Failed");
        return nullptr;
    }

    return frame;
}

void CFrame::Free()
{}
