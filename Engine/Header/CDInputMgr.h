#pragma once
#include "CBase.h"
#include "Engine_Define.h"

BEGIN(Engine)
/// @brief DirectInput 기반의 키보드 및 마우스 입력을 관리하는 싱글톤 매니저 클래스
class ENGINE_DLL CDInputMgr : public CBase
{
    DECLARE_SINGLETON(CDInputMgr)

private:
    explicit CDInputMgr();
    ~CDInputMgr() override;

public:
    /// @brief 특정 키의 현재 입력 상태를 반환
    /// @param keyState DIK_XXX 계열의 키 상수 (스캔 코드 기반)
    /// @return 눌림 상태면 상위 비트(0x80)가 1인 값 (= true), 아니면 0 (= false)
    /// @note 주의!! DIKEYBOARD_XXX 계열은 사용하지 말 것. (하위 8비트만 남아 오작동 가능)
    _ubyte Get_DIKeyState(_ubyte keyState)
    {
        return m_KeyState[keyState];
    }

    _ubyte Get_DIMouseState(MOUSEKEYSTATE mouseInputState)
    {
        return m_MouseState.rgbButtons[mouseInputState];
    }

    // 현재 마우스의 특정 축 좌표를 반환
    _long Get_DIMouseMove(MOUSEMOVESTATE mouseMoveState)
    {
        return *(reinterpret_cast<_long*>(&m_MouseState) + mouseMoveState);
    }

    // ---------- 키보드 ----------
    bool KeyDown(_ubyte key)
    {
        return  (m_KeyState[key] & 0x80) && !(m_PrevKeyState[key] & 0x80);
    }

    bool KeyPress(_ubyte key)
    {
        return (m_KeyState[key] & 0x80) != 0;
    }

    bool KeyUp(_ubyte key)
    {
        return !(m_KeyState[key] & 0x80) && (m_PrevKeyState[key] & 0x80);
    }

    // ---------- 마우스 ----------
    bool MouseDown(MOUSEKEYSTATE btn)
    {
        return  (m_MouseState.rgbButtons[btn] & 0x80)
            && !(m_PrevMouseState.rgbButtons[btn] & 0x80);
    }

    bool MousePress(MOUSEKEYSTATE btn)
    {
        return (m_MouseState.rgbButtons[btn] & 0x80) != 0;
    }

    bool MouseUp(MOUSEKEYSTATE btn)
    {
        return !(m_MouseState.rgbButtons[btn] & 0x80)
            && (m_PrevMouseState.rgbButtons[btn] & 0x80);
    }
    
    HWND Get_HWND() { return m_hWnd; }

public:
    HRESULT Ready_InputDev(HINSTANCE hInst, HWND hWnd);
    void    Update_InputDev();

private:
    LPDIRECTINPUT8 m_InputSDK;

    LPDIRECTINPUTDEVICE8 m_KeyBoardDev;
    LPDIRECTINPUTDEVICE8 m_MouseDev;

    _ubyte m_KeyState[256];
    _ubyte m_PrevKeyState[256];

    DIMOUSESTATE m_MouseState;
    DIMOUSESTATE m_PrevMouseState;

    HWND m_hWnd;

public:
    void Free() override;
};

END
