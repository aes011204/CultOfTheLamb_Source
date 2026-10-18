#pragma once
#include "CUI.h"

class CPlayerInput;
class CPlayerSkill;

class CUICharging : public CUI
{
private:
    explicit CUICharging(DEVICE graphicDev);
    explicit CUICharging(const CUICharging& rhs);
    ~CUICharging() override;

public:
    void    Initialize_GameObject() override;
    HRESULT Ready_GameObject() override;
    void    Render_GameObject() override;
    int     Update_GameObject(const _float& timeDelta) override;
    HRESULT Add_Component();

    void    Set_PlayerSkill(CPlayerSkill* playerSkill) { m_PlayerSkill = playerSkill; }
    void    Set_Dir(_vec3 dir) { m_ArrowDir = dir; }
    static CUICharging* Create(DEVICE graphicDev);

    
protected:
    void Free() override;

private:
    CPlayerSkill* m_PlayerSkill;
    float         m_ChargeRatio;
    CPlayerInput* m_PlayerInputCom;
    _vec3         m_ArrowLength;
    _vec3         m_ArrowDir;
    
};


