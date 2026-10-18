#pragma once
#include "CUI.h"

namespace Engine
{
class CCombatStat;
}

class CUICircle : public CUI
{
public:

    explicit CUICircle(DEVICE pGraphicDev);
    explicit CUICircle(const CUICircle& rhs);
    ~CUICircle() override;

    HRESULT Ready_GameObject() override;
    _int Update_GameObject(const _float& timeDelta) override;
    void LateUpdate_GameObject(const _float& timeDelta) override;
    void Render_GameObject() override;
    void Set_PlayerCombatStat(CCombatStat* combatstat) { m_CombatStatCom = combatstat; }
    HRESULT Add_Component();

public:
    static CUICircle* Create(DEVICE graphicDev);

private:
    static int         m_nextID;
    CCombatStat* m_CombatStatCom;

protected:
    void Free() override;

};

