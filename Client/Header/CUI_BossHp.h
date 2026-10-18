#pragma once

#include "CUI.h"

BEGIN(Engine)
class CCombatStat;
END

class CUI_BossHp : public CUI
{
private:
    explicit CUI_BossHp(DEVICE graphicDev, CCombatStat* combatStat);

    // 특정 몬스터한테 1:1로 매칭될 거기 때문에 복사 생성자가 안필요할듯?
    explicit CUI_BossHp(const CUI_BossHp& rhs) = delete;
    virtual ~CUI_BossHp();

public:
    HRESULT Ready_GameObject() override;
    void    Initialize_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;

private:
    HRESULT Add_Component();

public:
    static CUI_BossHp* Create(DEVICE graphicDev, CCombatStat* combatStat);

protected:
    void Free() override;

private:
    CGameObject*    m_Owner;
    CCombatStat*    m_OwnerCombatStat;

    CRcTex*         m_Buffer_Hp;
    CRcTex*         m_Buffer_Delay;

    _float          m_HpRatio;
    _float          m_HpDelayRatio;   
    _float          m_HpDelayLerpSpeed;

};

