#pragma once
#include "CUI.h"

class CUI_Skill : public CUI
{
public:

    explicit CUI_Skill(DEVICE graphicDev);
    explicit CUI_Skill(const CUI_Skill& rhs);
    ~CUI_Skill() override;

    HRESULT Ready_GameObject() override;
    _int Update_GameObject(const _float& timeDelta) override;
    void LateUpdate_GameObject(const _float& timeDelta) override;
    void Render_GameObject() override;
   // void Set_PlayerCombatStat(CCombatStat* combatStat) { m_CombatStatCom = combatStat; }
    HRESULT Add_Component();
    void Set_TextureIndex(_int index) { m_TextureIndex = index; }
public:
    static CUI_Skill* Create(DEVICE graphicDev);

protected:
    void Free() override;

private:
    _int m_TextureIndex;
    //  CCombatStat* m_CombatStatCom;

};


