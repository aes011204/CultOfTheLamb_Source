#pragma once

#include "CMonsterState.h"

class CMonsterController;
class CCharacterBase;
class CMonsterBase;

BEGIN(Engine)
class CGameObject;
END

class CEyeball_StartExit : public CMonsterState
{
private:
    explicit CEyeball_StartExit(CMonsterBase* owner);
    virtual ~CEyeball_StartExit();

public:
    void Enter() override;
    _int Update_State(const _float& timeDelta) override;
    void Exit() override;

    HRESULT Ready_State() override;

public:
    static CEyeball_StartExit* Create(CMonsterBase* owner);

private:
    void Free() override;

private:
    CMonsterBase*       m_Owner;
    _float              m_Pattern_Cooldown;

    _vec3               m_StartPos = _vec3(0.f, 0.f, 0.f);
    _vec3               m_StartTargetPos = _vec3(0.f, 0.f, 0.f);
    _float              m_LerpSpeed;
    _bool               m_IsReachedTarget;
    _bool               m_IsAnimFinished;
    _bool               m_IsFinalPosSet;

    const _vec3         FLOOR_POS1 = _vec3(0.f, -1.f, 0.f);
    const _vec3         FLOOR_POS2 = _vec3(0.f, -1.f, 0.f);

    _bool               m_DescendStarted = false;
    _float              m_DescendTime = 0.f;
    _float              m_DescendDuration = 0.3f;
    _vec3               m_DescendStartPos = _vec3(0.f, 0.f, 0.f);

};

