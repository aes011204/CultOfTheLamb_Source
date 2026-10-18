#include "CTimer.h"

CTimer::CTimer()
    : m_FrameTime{}, m_FixTime{}, m_LastTime{}, m_CpuTick{},
      m_TimeDelta(0.f)
{ }

CTimer::~CTimer()
{ }

HRESULT CTimer::Ready_Timer()
{
    QueryPerformanceCounter(&m_FrameTime);  // ex) 1077
    QueryPerformanceCounter(&m_LastTime);   // ex) 1085
    QueryPerformanceCounter(&m_FixTime);	// ex) 1090

    QueryPerformanceFrequency(&m_CpuTick);	// CPU Tick 값을 얻어오는 함수

    return S_OK;
}

void CTimer::Update_Timer()
{
    QueryPerformanceCounter(&m_FrameTime);  // ex) 1500

    if (m_FrameTime.QuadPart - m_FixTime.QuadPart >= m_CpuTick.QuadPart)
    {
        QueryPerformanceFrequency(&m_CpuTick);
        m_FixTime = m_FrameTime;
    }

    m_TimeDelta = static_cast<_float>(m_FrameTime.QuadPart - m_LastTime.QuadPart)
        / static_cast<_float>(m_CpuTick.QuadPart);

    m_LastTime = m_FrameTime;
}

CTimer* CTimer::Create()
{
    auto timer = new CTimer;

    if (FAILED(timer->Ready_Timer()))
    {
        MSG_BOX("Timer Create Failed");
        Engine::Safe_Release(timer);
        return nullptr;
    }

    return timer;
}

void CTimer::Free()
{ }
