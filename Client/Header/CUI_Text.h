#pragma once
#include "CUI.h"
class CUI_Text : public CUI
{
private:
    explicit CUI_Text(DEVICE graphicDev);
    explicit CUI_Text(const CUI_Text& rhs);
    ~CUI_Text() override;

public:
    HRESULT Ready_GameObject();
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;
    HRESULT Add_Component();

    void    Set_Color(D3DXCOLOR color) { m_D3dxColor = color; }
    void Set_Text(wstring text) { m_Text = text; }
    void Set_Font(wstring font) { m_Font = font; }
    static CUI_Text* Create(DEVICE graphicDev);

private:
    void Free() override;

private:
    wstring m_Font;
    wstring	m_Text;
    D3DXCOLOR m_D3dxColor;
};

