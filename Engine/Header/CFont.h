#pragma once
#include "CBase.h"
#include "Engine_Define.h"

BEGIN(Engine)
class ENGINE_DLL CFont : public CBase
{
private:
    explicit CFont(DEVICE graphicDev);
    ~CFont() override;

public:
    /// @brief 폰트 객체를 초기화하는 함수
    /// @param fontType     서체
    /// @param width        글자 폭
    /// @param height       글자 높이
    /// @param weight       글자 굵기
    HRESULT Ready_Font(const wstring& fontType,
                       const _uint&   width,
                       const _uint&   height,
                       const _uint&   weight);

    /// @brief 문자열을 화면에 출력하는 함수
    /// @param text         문자열
    /// @param pos          위치
    /// @param color        색상
    void Render_Font(const wstring& text,
                     const _vec2*   pos,
                     D3DXCOLOR      color);

    void Render_FontOutlined(const wstring& text,
                             const _vec2* pos,
                             D3DXCOLOR textColor,
                             D3DXCOLOR outlineColor,
                             int thickness);

    void Render_FontWrapped(
        const wstring& text,
        const RECT& boxRect,  
        D3DXCOLOR color);

private:
    DEVICE       m_GraphicDev;
    LPD3DXSPRITE m_Sprite;    // 2D 렌더링용 스프라이트 객체
    LPD3DXFONT   m_Font;      // 폰트 COM 객체

public:
    static CFont* Create(DEVICE         graphicDev,
                         const wstring& fontType,
                         const _uint&   width,
                         const _uint&   height,
                         const _uint&   weight);

private:
    void Free() override;
};

END
