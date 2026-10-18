#pragma once
#include "CBase.h"

class CLoading : public CBase
{
public:
    // 로딩 대상 종류 식별용 ID
    enum LOADINGID { LOADING_STAGE, LOADING_BOSS1, LOADING_BOSS2, LOADING_END };

private:
    explicit CLoading(DEVICE pGraphicDev);
    ~CLoading() override;

public:
    CRITICAL_SECTION* Get_Crt() { return &m_Crt; }
    LOADINGID         Get_LoadingID() { return m_LoadingID; }
    _bool             Get_Finish() { return m_IsFinish; }
    const wstring&    Get_String() { return m_LoadingText; }

public:
    HRESULT Ready_Loading(LOADINGID eID);
    _uint   Loading_ForState();
    _uint   Loading_ForBoss1();     // 보스 전용 로딩
    _uint   Loading_ForBoss2();     // 보스 전용 로딩
    _uint   Loading_ForFollower();  // 추종자 NPC 로딩

public:
    _uint                         Loading_ForState_Parallel();
    static unsigned int __stdcall Thread_Main(void* pArg);

private:
    DEVICE           m_GraphicDev;
    HANDLE           m_Thread;              // 로딩 스레드 핸들
    CRITICAL_SECTION m_Crt;                 // 스레드 간 동기화를 위한 임계영역 객체
    LOADINGID        m_LoadingID;           // 리소스 종류 ID
    _bool            m_IsFinish;            // 로딩 끝을 알리는 플래그
    wstring          m_LoadingText;         // 로딩 시 출력할 문자열

public:
    static CLoading* Create(DEVICE pGraphicDev, LOADINGID eID);

private:
    void Free() override;
};
