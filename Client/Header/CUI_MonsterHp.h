#pragma once

#include "CUI.h"

BEGIN(Engine)
class CCombatStat;
END

class CUI_MonsterHp : public CUI
{
private:
    explicit CUI_MonsterHp(DEVICE graphicDev, CCombatStat* combat);

    // 특정 몬스터한테 1:1로 매칭될 거기 때문에 복사 생성자가 안필요할듯?
    explicit CUI_MonsterHp(const CUI_MonsterHp& rhs) = delete;
    virtual ~CUI_MonsterHp();

public:
    HRESULT Ready_GameObject() override;
    void    Initialize_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;
    
public:
    void    Set_YOffset(_float offset) { m_YOffset = offset; }

private:
    HRESULT Add_Component();

public:
    static CUI_MonsterHp* Create(DEVICE graphicDev, CCombatStat* combat);

protected:
    void Free() override;

private:
    CGameObject*    m_Owner;

    CCombatStat*    m_OwnerCombatStat;
    CTransform*     m_OwnerTransform;

    CRcTex*         m_Buffer_Hp;
    CRcTex*         m_Buffer_Delay;

    _float          m_HpRatio;
    _float          m_HpDelayRatio;   
    _float          m_HpDelaySpeed;

    _float          m_YOffset;
    _vec3           m_BarSize;

    _matrix         m_BillboardMatrix; // 빌보드 쓸지말지?
};

