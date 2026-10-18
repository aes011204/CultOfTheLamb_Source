#include "CFont.h"

CFont::CFont(DEVICE graphicDev)
    : m_GraphicDev(graphicDev), m_Sprite(nullptr), m_Font(nullptr)
{
    m_GraphicDev->AddRef();
}

CFont::~CFont()
{ }

HRESULT CFont::Ready_Font(const wstring& fontType,
                          const _uint&   width,
                          const _uint&   height,
                          const _uint&   weight)
{
    D3DXFONT_DESC fontDesc{};

    fontDesc.CharSet = HANGUL_CHARSET;
    fontDesc.Width   = width;
    fontDesc.Height  = static_cast<_int>(height);
    fontDesc.Weight  = weight;
    lstrcpy(fontDesc.FaceName, fontType.c_str());

    if (FAILED(D3DXCreateFontIndirect(m_GraphicDev, &fontDesc, &m_Font)))
    {
        MSG_BOX("Font Create Failed");
        return E_FAIL;
    }

    if (FAILED(D3DXCreateSprite(m_GraphicDev, &m_Sprite)))
    {
        MSG_BOX("Sprite Create Failed");
        return E_FAIL;
    }

    return S_OK;
}

void CFont::Render_Font(const wstring& text, const _vec2* pos, D3DXCOLOR color)
{
    //RECT rc{ static_cast<_long>(pos->x), static_cast<_long>(pos->y), 0, 0 };

    //m_Sprite->Begin(D3DXSPRITE_ALPHABLEND);

    //m_Font->DrawTextW(m_Sprite, text.c_str(), static_cast<_int>(text.length()), &rc, DT_NOCLIP, color);
    //// DT_NOCLIP : 사각형 영역에 의한 클리핑 없이 출력 (정렬 옵션 없음)
    //// 다른 정렬 옵션을 사용할경우 rc 값을
    //// 전부 채워 RECT 전체를 구성할 것

    //m_Sprite->End();


    RECT rc;
    rc.left = static_cast<LONG>(pos->x - 500);
    rc.top = static_cast<LONG>(pos->y - 200);
    rc.right = static_cast<LONG>(pos->x + 500);
    rc.bottom = static_cast<LONG>(pos->y + 200);

    m_Sprite->Begin(D3DXSPRITE_ALPHABLEND);

    m_Font->DrawTextW(
        m_Sprite,
        text.c_str(),
        static_cast<int>(text.length()),
        &rc,
        DT_CENTER | DT_VCENTER,     // 중앙기준
        color
    );

    m_Sprite->End();
}

void CFont::Render_FontOutlined(const wstring& text,
    const _vec2* pos,
    D3DXCOLOR textColor,
    D3DXCOLOR outlineColor,
    int thickness)
{
    RECT baseRc;
    baseRc.left = static_cast<LONG>(pos->x - 500);
    baseRc.top = static_cast<LONG>(pos->y - 200);
    baseRc.right = static_cast<LONG>(pos->x + 500);
    baseRc.bottom = static_cast<LONG>(pos->y + 200);

    m_Sprite->Begin(D3DXSPRITE_ALPHABLEND);

    // ---- 1) 외곽선 8방향 렌더 ----
    const int offset = thickness; // 1 또는 2 정도 추천

    const int offsetX[8] = { -offset,  offset,  0,       0,      -offset,  offset, -offset,  offset };
    const int offsetY[8] = { 0,        0,      -offset,  offset, -offset, -offset,  offset,  offset };

    for (int i = 0; i < 8; ++i)
    {
        RECT rc = baseRc;
        rc.left += offsetX[i];
        rc.right += offsetX[i];
        rc.top += offsetY[i];
        rc.bottom += offsetY[i];

        m_Font->DrawTextW(
            m_Sprite,
            text.c_str(),
            static_cast<int>(text.length()),
            &rc,
            DT_CENTER | DT_VCENTER,
            outlineColor
        );
    }

    // ---- 2) 원래 텍스트 중앙에 한 번 더 ----
    m_Font->DrawTextW(
        m_Sprite,
        text.c_str(),
        static_cast<int>(text.length()),
        &baseRc,
        DT_CENTER | DT_VCENTER,
        textColor
    );

    m_Sprite->End();
}

void CFont::Render_FontWrapped(const wstring& text, const RECT& boxRect, D3DXCOLOR color)
{
    m_Sprite->Begin(D3DXSPRITE_ALPHABLEND);
    RECT rc = boxRect;
    m_Font->DrawTextW(
        m_Sprite,
        text.c_str(),
        static_cast<int>(text.length()),
        &rc,
        DT_LEFT | DT_WORDBREAK, // 자동 줄바꿈
        color
    );
    m_Sprite->End();
}

CFont* CFont::Create(DEVICE         graphicDev,
                     const wstring& fontType,
                     const _uint&   width,
                     const _uint&   height,
                     const _uint&   weight)
{
    auto font = new CFont(graphicDev);

    if (FAILED(font->Ready_Font(fontType, width, height, weight)))
    {
        Safe_Release(font);
        MSG_BOX("Font Create Failed");
        return nullptr;
    }

    return font;
}

void CFont::Free()
{
    Safe_Release(m_Font);
    Safe_Release(m_Sprite);
    Safe_Release(m_GraphicDev);
}
