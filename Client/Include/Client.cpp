// Client.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "pch.h"
#include "Client.h"
#include "framework.h"
#include "CFrameMgr.h"
#include "CImGuiManager.h"
#include "CMainApp.h"
#include "CTimerMgr.h"
#include "CMainEditorMgr.h"

#include <locale.h> // cmd창 출력용

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE g_hInst;                                  // 현재 인스턴스입니다.
WCHAR     szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR     szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.
HWND      g_hWnd;
FILE*     debug;

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM             MyRegisterClass(HINSTANCE hInstance);
BOOL             InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE     hInstance,
                      _In_opt_ HINSTANCE hPrevInstance,
                      _In_ LPWSTR        lpCmdLine,
                      _In_ int           nCmdShow)
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CLIENT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    //비율이슈
    SetProcessDPIAware();

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance(hInstance, nCmdShow))
        return FALSE;

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CLIENT));

    MSG msg;
    msg.message = WM_NULL;

    CMainApp* pMainApp = CMainApp::Create();

    if (nullptr == pMainApp)
        return FALSE;

    // 타이머 설치
    if (FAILED(CTimerMgr::GetInstance()->Ready_Timer(L"Timer_Immediate")))
        return E_FAIL;

    if (FAILED(CTimerMgr::GetInstance()->Ready_Timer(L"Timer_FPS60")))
        return E_FAIL;

    // 프레임 생성
    if (FAILED(CFrameMgr::GetInstance()->Ready_Frame(L"Frame60", 60.f)))
        return E_FAIL;

    // 기본 메시지 루프입니다:
    while (true)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (WM_QUIT == msg.message)
                break;

            if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            CTimerMgr::GetInstance()->Set_TimeDelta(L"Timer_Immediate");

            _float fTimer_Immediate = CTimerMgr::GetInstance()->Get_TimeDelta(L"Timer_Immediate");

            if (CFrameMgr::GetInstance()->IsPermit_Call(L"Frame60", fTimer_Immediate))
            {
                CTimerMgr::GetInstance()->Set_TimeDelta(L"Timer_FPS60");

                _float realDelta = CTimerMgr::GetInstance()->Get_RealTimeDelta();
                CTimerMgr::GetInstance()->Update_HitStop(realDelta);

                _float fTimer_FPS60 = CTimerMgr::GetInstance()->Get_TimeDelta(L"Timer_FPS60");

                if (CMainEditorMgr::GetInstance()->IsPlaying())
                {
                    _float scaledDelta = fTimer_FPS60 * CMainEditorMgr::GetInstance()->Get_TimeScale();

                    pMainApp->Update_MainApp(scaledDelta);
                    pMainApp->LateUpdate_MainApp(scaledDelta);
                }

                else // 일시정지
                {
                    pMainApp->Update_MainApp(0.f);
                    pMainApp->LateUpdate_MainApp(0.f);
                }
               
                pMainApp->Render_MainApp();
            }
        }
    }

    _ulong refCnt(0);

    if (refCnt = Safe_Release(pMainApp))
    {
        MSG_BOX("MainApp Release Failed");
        return FALSE;
    }

    return static_cast<int>(msg.wParam);
}

//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style         = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc   = WndProc;
    wcex.cbClsExtra    = 0;
    wcex.cbWndExtra    = 0;
    wcex.hInstance     = hInstance;
    wcex.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CLIENT));
    wcex.hCursor       = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName  = nullptr;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm       = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    g_hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

    RECT rc{ 0, 0, WINCX, WINCY };

    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

    int windowWidth = rc.right - rc.left;
    int windowHeight = rc.bottom - rc.top;

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);  
    int screenHeight = GetSystemMetrics(SM_CYSCREEN); 

    int centerX = (screenWidth - windowWidth) / 2;
    int centerY = (screenHeight - windowHeight) / 2;

    HWND hWnd = CreateWindowW(szWindowClass,
                              szTitle,
                              WS_OVERLAPPEDWINDOW,
                              centerX,
                              centerY,
                              rc.right - rc.left,
                              rc.bottom - rc.top,
                              nullptr,
                              nullptr,
                              hInstance,
                              nullptr);

    if (!hWnd)
        return FALSE;

    g_hWnd = hWnd;

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (CImGuiManager::GetInstance()->MsgProc(hWnd, message, wParam, lParam))
        return true;

    switch (message)
    {
#pragma region Debug Log
    case WM_CREATE:
    {
        AllocConsole();
        _tfreopen_s(&debug, _T("CONOUT$"), _T("w"), stdout);
        _tfreopen_s(&debug, _T("CONIN$"), _T("r"), stdin);
        _tfreopen_s(&debug, _T("CONERR"), _T("w"), stderr);
        _tsetlocale(LC_ALL, _T(""));

        HWND consoleWindow = GetConsoleWindow();
        if (consoleWindow)
        {

            SetWindowPos(consoleWindow,
                HWND_TOP,
                50,            //  X 좌표
                50,            //  Y 좌표
                800,           //  너비
                600,           //  높이
                SWP_SHOWWINDOW);
        }
    }
    break;
    case WM_CLOSE:
    {
        FreeConsole();
        DestroyWindow(hWnd);
    }
    break;
#pragma endregion
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // 메뉴 선택을 구문 분석합니다:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(g_hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;

    case WM_MOUSEWHEEL:  
        return 0;

    case WM_KEYDOWN:
    {
        switch (wParam)
        {
        case VK_ESCAPE:
            //DestroyWindow(g_hWnd);
            break;
        }
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;

    default:
        break;
    }
    return (INT_PTR)FALSE;
}
