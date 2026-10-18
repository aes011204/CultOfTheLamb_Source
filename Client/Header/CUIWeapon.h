#pragma once
#include "CUI.h"

namespace Engine
{
    class CCombatStat;
}

class CUIWeapon : public CUI
{
public:

    explicit CUIWeapon(DEVICE pGraphicDev);
    explicit CUIWeapon(const CUIWeapon& rhs);
    ~CUIWeapon() override;

    HRESULT Ready_GameObject() override;
    _int Update_GameObject(const _float& timeDelta) override;
    void LateUpdate_GameObject(const _float& timeDelta) override;
    void Render_GameObject() override;
    void Set_PlayerCombatStat(CCombatStat* combatStat) { m_CombatStatCom = combatStat; }
    HRESULT Add_Component();

public:
    static CUIWeapon* Create(DEVICE graphicDev);

protected:
    void Free() override;

private:
    int          m_WeaponID;
    static int   m_NextID;
    CCombatStat* m_CombatStatCom;

};

