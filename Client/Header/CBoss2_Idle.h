#pragma once

#include "CMonsterState.h"

class CBossController;
class CCharacterBase;

BEGIN(Engine)
class CGameObject;
END

class CBoss2_Idle : public CMonsterState
{
private:
    explicit CBoss2_Idle(CGameObject* owner);
    virtual ~CBoss2_Idle();

public:
    void     Enter() override;
    _int     Update_State(const _float& timeDelta) override;
    void     Exit() override;

    HRESULT  Ready_State() override;

public:
    static CBoss2_Idle* Create(CGameObject* owner);

private:
    void Free() override;

private:
    CGameObject*    m_Owner;

    _float          m_Pattern_Cooldown;
    _int            m_CooldownMin = 10; // Min, Max 사이에 랜덤하게
    _int            m_CooldownMax = 15;

};

