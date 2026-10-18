#pragma once
#include "CUI.h"

namespace Engine
{
    class CCombatStat;
}

class CUIMp : public CUI
{
public:

    explicit CUIMp(DEVICE pGraphicDev);
    explicit CUIMp(const CUIMp& rhs);
    ~CUIMp() override;

    HRESULT Ready_GameObject() override;
    _int Update_GameObject(const _float& timeDelta) override;
    void LateUpdate_GameObject(const _float& timeDelta) override;
    void Render_GameObject() override;

    HRESULT Add_Component();
    void Set_PlayerCombatStat(CCombatStat* combatstat) { m_CombatStatCom = combatstat; }
public:
    static CUIMp* Create(DEVICE graphicDev);

private:
    static int         m_nextID;

protected:
    void Free() override;

private:
    CCombatStat* m_CombatStatCom;
};

