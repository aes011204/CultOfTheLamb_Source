#include "CTimerMgr.h"

#include "CTimer.h"

IMPLEMENT_SINGLETON(CTimerMgr)

static const wchar_t* DEFAULT_FRAME_TIMER = L"Timer_FPS60";

CTimerMgr::CTimerMgr()
{ }

CTimerMgr::~CTimerMgr()
{
    CTimerMgr::Free();
}

_float CTimerMgr::Get_TimeDelta(const wstring& timerTag)
{
    CTimer* timer = Find_Timer(move(timerTag));

    if (nullptr == timer)
        return 0.f;

    return timer->Get_TimeDelta() * m_TimeScale;
}

void CTimerMgr::Set_TimeDelta(const wstring& timerTag)
{
    CTimer* timer = Find_Timer(move(timerTag));

    if (nullptr == timer)
        return;

    timer->Update_Timer();
}

_float CTimerMgr::Get_TimeDelta()
{
    return Get_TimeDelta(DEFAULT_FRAME_TIMER);
}

void CTimerMgr::Set_TimeDelta()
{
    Set_TimeDelta(DEFAULT_FRAME_TIMER);
}

HRESULT CTimerMgr::Ready_Timer(const wstring& timerTag)
{
    CTimer* timer = Find_Timer(timerTag);

    if (timer != nullptr)
        return E_FAIL;

    timer = CTimer::Create();

    if (nullptr == timer)
        return E_FAIL;

    m_Timers.insert({ timerTag, timer });

    return S_OK;
}

CTimer* CTimerMgr::Find_Timer(const wstring& timerTag)
{
    auto iter
        = find_if(m_Timers.begin(), m_Timers.end(), CTag_Finder(move(timerTag)));

    if (iter == m_Timers.end())
        return nullptr;

    return iter->second;
}

void CTimerMgr::Free()
{
    for_each(m_Timers.begin(), m_Timers.end(), CDeleteMap());
    m_Timers.clear();
}

_float CTimerMgr::Get_RealTimeDelta()
{
    CTimer* timer = Find_Timer(DEFAULT_FRAME_TIMER);

    if (timer == nullptr)
        return 0.f;

    return timer->Get_TimeDelta();
}

// Ex) duration = 0.2, scale = 0.1이라면 -> 0.2초 동안 10% 속도로 슬로우 모션
void CTimerMgr::Start_HitStop(_float duration, _float scale)
{
    m_HitStopDuration = duration;
    m_HitStopScale = scale;
    m_TimeScale = scale;
}

void CTimerMgr::Update_HitStop(_float realDelta)
{
    if (m_HitStopDuration > 0.f)
    {
        m_HitStopDuration -= realDelta;

        if (m_HitStopDuration <= 0.f)
        {
            m_HitStopDuration = 0.f;
            m_TimeScale = 1.f; // 원래대로 복귀
        }
    }
}
