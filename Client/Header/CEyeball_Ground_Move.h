#pragma once

#include "CMonsterState.h"

class CMonsterController;
class CCharacterBase;
class CMonsterBase;

BEGIN(Engine)
class CGameObject;
END

class CEyeball_Ground_Move : public CMonsterState
{
private:
    explicit CEyeball_Ground_Move(CMonsterBase* owner);
    virtual ~CEyeball_Ground_Move();

public:
    void Enter() override;
    _int Update_State(const _float& timeDelta) override;
    void Exit() override;

    HRESULT Ready_State() override;

public:
    static CEyeball_Ground_Move* Create(CMonsterBase* owner);

private:
    void Free() override;

private:
    CMonsterBase*       m_Owner;
    _float              m_Pattern_Cooldown;

    _vec3               m_StartPos{};
    _vec3               m_TargetDir{};
    _vec3               m_TargetPos{};

    _float              m_LerpSpeed;
    _float              m_MaxMoveDistance;

    _float              m_Direction_Timer = 0.f;
    _bool               m_IsUnderground = false;

    _float              m_ParticleTimer = 0.f;
};

