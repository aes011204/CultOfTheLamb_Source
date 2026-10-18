#include "pch.h"
#include "CLoading.h"
#include <chrono>
#include <future>
#include "CAnimator.h"
#include "CBoxCol.h"
#include "CProtoMgr.h"
#include "CResourceMgr.h"
#include "CTexture.h"

CLoading::CLoading(DEVICE pGraphicDev)
    : m_GraphicDev(pGraphicDev),
    m_Thread(nullptr),
    m_Crt{},
    m_LoadingID(LOADING_END),
    m_IsFinish(false)
{
    m_GraphicDev->AddRef();
}

CLoading::~CLoading()
{
}

HRESULT CLoading::Ready_Loading(LOADINGID eID)
{
    // 스레드 동기화를 위한 임계영역(크리티컬 섹션) 초기화
    InitializeCriticalSection(&m_Crt);

    // 로딩 스레드 생성
    m_Thread = (HANDLE)_beginthreadex(
        nullptr,	        // 보안속성(핸들의 상속 여부, NULL인 경우 상속에서 제외)
        0,		            // 스택 크기 (0 = OS 기본값 사용)
        Thread_Main,		// 쓰레드 함수(__stdcall 호출 규약)
        this,	            // 쓰레드 함수를 이용하여 가공할 데이터 주소
        0,		            // 쓰레드 생성 및 실행을 조절하는 flag
        nullptr);	        // 쓰레드 id 반환

    m_LoadingID = eID;

    return S_OK;
}

_uint CLoading::Loading_ForState()
{
    // 시작 시간 체크
    auto start = chrono::high_resolution_clock::now();

    m_LoadingText = L"Resource Loading...";

    // 이제 리소스 추가는 Resource Manager에서 진행
    if (FAILED(CResourceMgr::GetInstance()->Ready_Resource(m_GraphicDev)))
        return E_FAIL;

    // 끝 시간 체크
    auto end = chrono::high_resolution_clock::now();
    // 걸린 시간 출력
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);

    OutputDebugString((L"Loading Time: " + to_wstring(duration.count()) + L"ms\n").c_str());

    m_IsFinish = true;

    return S_OK;
}

_uint CLoading::Loading_ForBoss1()
{
    auto start = chrono::high_resolution_clock::now();

	// 이제 보스 1,2 동시에 로딩
	
    m_LoadingText = L"Boss Loading...";
    auto resourceMgr = CResourceMgr::GetInstance();

    vector<future<HRESULT>> futures;

    futures.push_back(async(launch::async, [&]() {
        return resourceMgr->Ready_BossP1(m_GraphicDev);
        }));

    //futures.push_back(async(launch::async, [&]() {
    //    return resourceMgr->Ready_BossP2(m_GraphicDev);
    //    }));
	//
    //futures.push_back(async(launch::async, [&]() {
    //    return resourceMgr->Ready_BossP2_Eyeball(m_GraphicDev);
    //    }));
    //
    //futures.push_back(async(launch::async, [&]() {
    //    return resourceMgr->Ready_BossP2_Follower(m_GraphicDev);
    //    }));

    // 병렬 태스크 완료 대기
    for (auto& future : futures)
    {
        if (FAILED(future.get()))
            return E_FAIL;
    }

	// 로딩 시간 체크
	auto end = chrono::high_resolution_clock::now();
	auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
	OutputDebugString((L"Loading Time: " + to_wstring(duration.count()) + L"ms\n").c_str());

    m_IsFinish = true;

    return S_OK;
}

_uint CLoading::Loading_ForBoss2()
{
    auto start = chrono::high_resolution_clock::now();

    m_LoadingText = L"Boss2 Loading...";
    auto resourceMgr = CResourceMgr::GetInstance();

    vector<future<HRESULT>> futures;

    futures.push_back(async(launch::async, [&]() {
        return resourceMgr->Ready_BossP2(m_GraphicDev);
        }));

    futures.push_back(async(launch::async, [&]() {
            return resourceMgr->Ready_BossP2_Eyeball(m_GraphicDev);
        }));

    futures.push_back(async(launch::async, [&]() {
        return resourceMgr->Ready_BossP2_Follower(m_GraphicDev);
        }));

    // 병렬 태스크 완료 대기
    for (auto& future : futures)
    {
        if (FAILED(future.get()))
            return E_FAIL;
    }

    // 로딩 시간 체크
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    OutputDebugString((L"Loading Time: " + to_wstring(duration.count()) + L"ms\n").c_str());

    m_IsFinish = true;

    return S_OK;
}

_uint CLoading::Loading_ForState_Parallel()
{
    auto start = chrono::high_resolution_clock::now();

    m_LoadingText = L"텍스처 로딩 중...";
    auto resourceMgr = CResourceMgr::GetInstance();

    // 버퍼는 먼저 준비
    if (FAILED(resourceMgr->Ready_Buffer(m_GraphicDev)))
        return E_FAIL;

    if (FAILED(resourceMgr->Ready_Common(m_GraphicDev)))
        return E_FAIL;

    if (FAILED(resourceMgr->Ready_Item(m_GraphicDev)))
        return E_FAIL;

    vector<future<HRESULT>> futures;

    futures.push_back(async(launch::async, [&]() {
        return resourceMgr->Ready_Player(m_GraphicDev);
        }));
    futures.push_back(async(launch::async, [&]() {
        return resourceMgr->Ready_Monster(m_GraphicDev);
        }));
    futures.push_back(async(launch::async, [&]() {
        return resourceMgr->Ready_UI(m_GraphicDev);
        }));
    futures.push_back(async(launch::async, [&]() {
        return resourceMgr->Ready_Terrain(m_GraphicDev);
        }));
    futures.push_back(async(launch::async, [&]() {
        return resourceMgr->Ready_Decoration(m_GraphicDev);
        }));
    futures.push_back(async(launch::async, [&]() {
        return resourceMgr->Ready_Particle(m_GraphicDev);
        }));
    futures.push_back(async(launch::async, [&]() {
        return resourceMgr->Ready_Building(m_GraphicDev);
        }));
    futures.push_back(async(launch::async, [&]() {
        return resourceMgr->Ready_Follower(m_GraphicDev);
        }));
    //futures.push_back(async(launch::async, [&]() {
    //    return resourceMgr->Ready_MiniBoss(m_GraphicDev);
    //    }));

    // 병렬 태스크 완료 대기
    for (size_t i = 0; i < futures.size(); ++i)
    {
        if (FAILED(futures[i].get()))
        {
            wstring msg = L"[Parallel Loading Failed] Task Index: " + to_wstring(i) + L"\n";
            OutputDebugString(msg.c_str());
            return E_FAIL;
        }
    }

    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);

    OutputDebugString((L"Loading Time: " + to_wstring(duration.count()) + L"ms\n").c_str());

    m_IsFinish = true;

    return S_OK;
}

unsigned CLoading::Thread_Main(void* pArg)
{
    auto pLoading = static_cast<CLoading*>(pArg);

    _uint iFlag(0);

    // 임계영역 진입 (다른 스레드의 접근 차단)
    EnterCriticalSection(pLoading->Get_Crt());

    switch (pLoading->Get_LoadingID())
    {
    case LOADING_STAGE:
        //iFlag = pLoading->Loading_ForState();
        iFlag = pLoading->Loading_ForState_Parallel();
        break;
    case LOADING_BOSS1:
        iFlag = pLoading->Loading_ForBoss1();
        break;
    case LOADING_BOSS2:
        iFlag = pLoading->Loading_ForBoss2();
        break;
    default:
        break;
    }

    // 임계영역 해제 (다른 스레드 접근 허용)
    LeaveCriticalSection(pLoading->Get_Crt());

    //_endthreadex(0);
    // -> _beginthreadex()로 생성된 스레드는 함수가 return 될 때
    // 내부적으로 _endthreadex() 가 자동 호출되어 정리됨.
    // 따라서 명시적으로 호출할 필요가 없음

    return iFlag;
}

CLoading* CLoading::Create(DEVICE pGraphicDev, LOADINGID eID)
{
    auto pLoading = new CLoading(pGraphicDev);

    if (FAILED(pLoading->Ready_Loading(eID)))
    {
        Safe_Release(pLoading);

        MSG_BOX("Loading Create Failed");
        return nullptr;
    }

    return pLoading;
}

void CLoading::Free()
{
    // 생성한 스레드가 완전히 끝날 때까지 대기
    WaitForSingleObject(m_Thread, INFINITE);

    // 스레드 핸들 닫기 (핸들 리소스 해제)
    CloseHandle(m_Thread);

    // 크리티컬 섹션 삭제 (운영체제 리소스 해제)
    DeleteCriticalSection(&m_Crt);

    Safe_Release(m_GraphicDev);
}
