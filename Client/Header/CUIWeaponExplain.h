#pragma once
#include "CUI.h"
class CUIIcon;
class CSpriteEff;

class CUIWeaponExplain : public CUI
{
private:
    explicit CUIWeaponExplain(DEVICE graphicDev);
    explicit CUIWeaponExplain(const CUIWeaponExplain& rhs);
    ~CUIWeaponExplain() override;

public:
    void    Initialize_GameObject() override;
    HRESULT Ready_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;
    HRESULT Add_Component();
    void    Set_ItemID(int ID) { m_ItemID = ID; }
    void    Set_Active(_bool tf) { m_Active = tf; }
    _bool   Get_Active() { return m_Active; }
    static CUIWeaponExplain* Create(DEVICE graphicDev);

protected:
    void Free() override;

private:
    int m_ItemID;
    _float m_Alpha = 0.f;
    CSpriteEff* m_SpriteEffCom;
    _bool m_SoundEnded;
};
