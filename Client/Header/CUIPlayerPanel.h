#pragma once
#include "CUIPanel.h"

namespace Engine
{
    class CCombatStat;
}

class CUIPlayerPanel : public CUIPanel
{
public:
    explicit CUIPlayerPanel(DEVICE graphicDev);
    explicit CUIPlayerPanel(const CUIPlayerPanel& rhs);
    ~CUIPlayerPanel() override;

public:
    HRESULT Ready_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;

    virtual void Render_Editor() override;
    HRESULT Add_Component();
    void    Set_Resister(bool tf) { m_Resister = tf; }

public:
    static CUIPlayerPanel* Create(DEVICE graphicDev);

    void Set_PlayerCombatStat(CCombatStat* combatstat) { m_CombatStatCom = combatstat; }

protected:
    void Free() override;

private:
    CCombatStat* m_CombatStatCom;
    bool        m_Resister;
};
