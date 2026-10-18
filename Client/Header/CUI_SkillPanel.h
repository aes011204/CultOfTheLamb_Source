#pragma once
#include "CUI.h"

class CPlayerSkill;
class CUIIcon;

class CUI_SkillPanel : public CUI
{
private:
    explicit CUI_SkillPanel(DEVICE graphicDev);
    explicit CUI_SkillPanel(const CUI_SkillPanel& rhs);
    ~CUI_SkillPanel() override;

public:
    void Initialize_GameObject() override;
    HRESULT Ready_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;

    static CUI_SkillPanel* Create(DEVICE graphicDev);


private:
    HRESULT Add_Component();

protected:
    void Free() override;

private:
    CPlayerSkill* m_PlayerSkillCom;
    CUIIcon* m_SingleSkillUI;
    CUIIcon* m_TripleSkillUI;
};


