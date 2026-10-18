#include "CFontMgr.h"

#include "CFont.h"

IMPLEMENT_SINGLETON(CFontMgr)

CFontMgr::CFontMgr()
{ }

CFontMgr::~CFontMgr()
{
    CFontMgr::Free();
}

HRESULT CFontMgr::Ready_Font(DEVICE         graphicDev,
                             const wstring& fontTag,
                             const wstring& fontType,
                             const _uint&   width,
                             const _uint&   height,
                             const _uint&   weight)
{
    CFont* font = Find_Font(fontTag);

    if (nullptr != font)
        return E_FAIL;

    font = CFont::Create(graphicDev, fontType, width, height, weight);

    if (nullptr == font)
        return E_FAIL;
    
    m_Fonts.insert({ fontTag, font });

    return S_OK;
}

void CFontMgr::Render_Font(const wstring& fontTag, const wstring& text, const _vec2* pos, D3DXCOLOR color)
{
    CFont* font = Find_Font(move(fontTag));

    font->Render_Font(text, pos, color);
}

void CFontMgr::Render_FontOutlined(const wstring& fontTag, const wstring& text, const _vec2* pos, D3DXCOLOR textColor, D3DXCOLOR outlineColor, int thickness)
{
    CFont* font = Find_Font(fontTag);
    if (!font)
        return;

    font->Render_FontOutlined(text, pos, textColor, outlineColor, thickness);
}

void CFontMgr::Render_FontWrapped(const wstring& fontTag, const wstring& text, const RECT& boxRect, D3DXCOLOR color)
{
    CFont* font = Find_Font(fontTag);

    if (font)
        font->Render_FontWrapped(text, boxRect, color);
}

CFont* CFontMgr::Find_Font(const wstring& fontTag)
{
    auto iter = find_if(m_Fonts.begin(), m_Fonts.end(), CTag_Finder(move(fontTag)));

    if (iter == m_Fonts.end())
        return nullptr;

    return iter->second;
}

void CFontMgr::Free()
{
    for_each(m_Fonts.begin(), m_Fonts.end(), CDeleteMap());
    m_Fonts.clear();
}
