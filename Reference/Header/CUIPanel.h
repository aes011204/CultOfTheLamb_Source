#pragma once
#include "CUI.h"

BEGIN(Engine)
class ENGINE_DLL CUIPanel : public CUI
{
public:
    explicit CUIPanel(DEVICE graphicDev);
    explicit CUIPanel(const CUIPanel& rhs);
    ~CUIPanel() override;

public:
    HRESULT Ready_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;

public:
    virtual void AddChild(CUI* ui);

protected:
    void Free() override;

protected:
    vector<CUI*> m_Children;
};

END
