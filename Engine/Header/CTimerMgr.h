#pragma once
#include "CBase.h"
#include "Engine_Define.h"

BEGIN(Engine)
class CTimer;

class ENGINE_DLL CTimerMgr : public CBase
{
    DECLARE_SINGLETON(CTimerMgr)

private:
    explicit CTimerMgr();
    ~CTimerMgr() override;

public:
    _float Get_TimeDelta(const wstring& timerTag);
    void   Set_TimeDelta(const wstring& timerTag);

    // 60FPS TimeDelta 바로 반환하는 편의용 오버로드
    _float Get_TimeDelta();  
    void   Set_TimeDelta();

public:
    HRESULT Ready_Timer(const wstring& timerTag);

private:
    CTimer* Find_Timer(const wstring& timerTag);

private:
    map<wstring, CTimer*> m_Timers;

private:
    void Free() override;

#pragma region HitStop
public:
    void    Set_TimeScale(_float timeScale) { m_TimeScale = timeScale; }
    _float  Get_TimeScale() { return m_TimeScale; }
    _float  Get_RealTimeDelta(); // TimeScale 미적용 값

    // [ 시간 배율이 0이면 완전 정지, 0.1이면 슬로우 ]
    void    Start_HitStop(_float duration, _float scale = 0.f);
    void    Update_HitStop(_float realDelta); // 실제 시간으로 업데이트

private:
    // 시간 배율(0 : 정지, 1 : 정상)
    _float  m_TimeScale = 1.f; 
    _float  m_HitStopDuration = 0.f;
    _float  m_HitStopScale = 0.f;       // HitStop 도중 TimeScale


#pragma endregion

};

END
