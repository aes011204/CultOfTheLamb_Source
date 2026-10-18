#pragma once
#include "CUI.h"

//namespace Engine
//{
//    class CCombatStat;
//}

class CUICircle;

class CUISkillCount : public CUI
{
public:
    explicit CUISkillCount(DEVICE pGraphicDev);
    explicit CUISkillCount(const CUISkillCount& rhs);
    ~CUISkillCount() override;

    HRESULT Ready_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;

    HRESULT Add_Component();
    //void Set_PlayerCombatStat(CCombatStat* combatstat) { m_CombatStatCom = combatstat; }

public:
    static CUISkillCount* Create(DEVICE graphicDev);

private:
    static int m_nextID;

protected:
    void Free() override;

private:
    int m_SkillCount;
    CUICircle* m_UICircle;
    //CCombatStat* m_CombatStatCom;
};
