#pragma once

#include "CMonsterState.h"

class CMonsterController;
class CCharacterBase;
class CMonsterBase;

BEGIN(Engine)
class CGameObject;
END

class CEyeball_Idle : public CMonsterState
{
private:
    explicit CEyeball_Idle(CMonsterBase* owner);
    virtual ~CEyeball_Idle();

public:
    void Enter() override;
    _int Update_State(const _float& timeDelta) override;
    void Exit() override;

    HRESULT Ready_State() override;

public:
    static CEyeball_Idle* Create(CMonsterBase* owner);

private:
    void Free() override;

private:
    CMonsterBase*       m_Owner;
    _float              m_Pattern_Cooldown;

    _vec3               m_Current_Direction;
    _vec3               m_Target_Direction;

    _float              m_Direction_ChangeTimer;
    _float              m_Direction_Timer;
    _float              m_MoveSpeed;
    _float              m_LerpSpeed;
};

