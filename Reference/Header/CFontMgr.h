#pragma once
#include "CBase.h"
#include "Engine_Define.h"

BEGIN(Engine)
class CFont;

class ENGINE_DLL CFontMgr : public CBase
{
    DECLARE_SINGLETON(CFontMgr)

private:
    explicit CFontMgr();
    ~CFontMgr() override;

public:
    /// @brief 폰트를 생성하고 폰트 맵에 등록
    /// @param graphicDev       그래픽 디바이스
    /// @param fontTag          문자열 형식의 고유 태그
    /// @param fontType         서체
    /// @param width            글자 폭
    /// @param height           글자 높이
    /// @param weight           글자 굵기
    HRESULT Ready_Font(
        DEVICE         graphicDev,
        const wstring& fontTag,
        const wstring& fontType,
        const _uint&   width,
        const _uint&   height,
        const _uint&   weight);

    /// @brief 태그를 이용해 문자열을 화면에 출력
    /// @param fontTag          출력에 사용할 폰트의 태그
    /// @param text             출력할 문자열
    /// @param pos              출력 위치 (픽셀 단위)
    /// @param color            색상
    void Render_Font(
        const wstring& fontTag,
        const wstring& text,
        const _vec2*   pos,
        D3DXCOLOR      color);

    void Render_FontOutlined(
        const wstring& fontTag,
        const wstring& text,
        const _vec2*   pos,
        D3DXCOLOR      textColor,
        D3DXCOLOR      outlineColor,
        int            thickness);

    void Render_FontWrapped(
        const wstring& fontTag,
        const wstring& text,
        const RECT& boxRect,
        D3DXCOLOR color);

private:
    CFont* Find_Font(const wstring& fontTag);

private:
    map<wstring, CFont*> m_Fonts;

private:
    void Free() override;
};

END
