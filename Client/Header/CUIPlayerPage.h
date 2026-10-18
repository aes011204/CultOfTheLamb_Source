#pragma once
#include "CUI.h"
#include "CUIPlayerImage.h"

class CUITabMenu;
class CUIPlayerPanel;
class CUIIcon;

class CUIPlayerPage : public CUI
{
private:
    explicit CUIPlayerPage(DEVICE pGraphicDev, CUI* owner);
    explicit CUIPlayerPage(const CUIPlayerPage& rhs, CUI* owner);
    ~CUIPlayerPage() override;

public:
    HRESULT Add_Component();
    HRESULT Ready_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;
    void    Initialize();

    static CUIPlayerPage* Create(DEVICE graphicDev, CUI* onwer);
    void                  Free() override;

private:
    CUIPlayerImage* m_PlayerUI;
    CUIIcon*        m_PlayerBG;
    CUIIcon*        m_LockUI;
    CUIIcon*        m_UnlockUI;
    CUIIcon*        m_OutfitUI;
    CUIIcon*        m_WeaponUI;
    CUIPlayerPanel* m_PlayerPanel;
    CUI*            m_Owner;
};
