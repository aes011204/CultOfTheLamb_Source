#include "CDInputMgr.h"

IMPLEMENT_SINGLETON(CDInputMgr)

CDInputMgr::CDInputMgr()
    : m_InputSDK(nullptr),
      m_KeyBoardDev(nullptr),
      m_MouseDev(nullptr),
      m_KeyState{},
      m_PrevKeyState{},
      m_MouseState{},
      m_PrevMouseState{},
      m_hWnd(nullptr)
{}

CDInputMgr::~CDInputMgr()
{
    CDInputMgr::Free();
}

HRESULT CDInputMgr::Ready_InputDev(HINSTANCE hInst, HWND hWnd)
{
    if (FAILED(DirectInput8Create(hInst,
        DIRECTINPUT_VERSION,
        IID_IDirectInput8,
        (void**)&m_InputSDK,
        nullptr)))
        return E_FAIL;

    if (FAILED(m_InputSDK->CreateDevice(GUID_SysKeyboard, &m_KeyBoardDev, nullptr)))
        return E_FAIL;

    m_KeyBoardDev->SetDataFormat(&c_dfDIKeyboard);

    // 백그라운드 입력 못받게 막기
    //m_KeyBoardDev->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
    m_KeyBoardDev->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

    m_KeyBoardDev->Acquire();

    if (FAILED(m_InputSDK->CreateDevice(GUID_SysMouse, &m_MouseDev, nullptr)))
        return E_FAIL;

    m_MouseDev->SetDataFormat(&c_dfDIMouse);

    //m_MouseDev->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
    m_MouseDev->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

    m_MouseDev->Acquire();

    m_hWnd = hWnd;

    return S_OK;
}

void CDInputMgr::Update_InputDev()
{
    // 키보드 이전 프레임 상태 백업
    memcpy(m_PrevKeyState, m_KeyState, sizeof(m_KeyState));

    // 마우스 이전 프레임 상태 백업
    m_PrevMouseState = m_MouseState;

    // 키보드
    if (FAILED(m_KeyBoardDev->GetDeviceState(256, m_KeyState)))
    {
        // 포커스 잃었을 때 복구용
        m_KeyBoardDev->Acquire();
        m_KeyBoardDev->GetDeviceState(256, m_KeyState);
    }

    // 마우스
    if (FAILED(m_MouseDev->GetDeviceState(sizeof(m_MouseState), &m_MouseState)))
    {
        m_MouseDev->Acquire();
        m_MouseDev->GetDeviceState(sizeof(m_MouseState), &m_MouseState);
    }
}

void CDInputMgr::Free()
{
    Safe_Release(m_KeyBoardDev);
    Safe_Release(m_MouseDev);
    Safe_Release(m_InputSDK);
}
