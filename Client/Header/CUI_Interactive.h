#pragma once
#include "CUI.h"
class CUI_Interactive : public CUI
{
private:
    explicit CUI_Interactive(DEVICE graphicDev);
    explicit CUI_Interactive(const CUI_Interactive& rhs);
    ~CUI_Interactive() override;

public:
    HRESULT Ready_GameObject();
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;
    HRESULT Add_Component();
    void        Set_TextureIndex(int textureIndex) { m_textureIndex = textureIndex; }
    static CUI_Interactive* Create(DEVICE graphicDev);

private:
    void Free() override;
private:
    int m_textureIndex;

};

