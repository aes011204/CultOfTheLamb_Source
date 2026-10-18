#pragma once

#include "CMonsterState.h"

class CMonsterController;
class CCharacterBase;
class CMonsterBase;

BEGIN(Engine)
class CGameObject;
END

class CEyeball_Start : public CMonsterState
{
private:
    explicit CEyeball_Start(CMonsterBase* owner);
    virtual ~CEyeball_Start();

public:
    void Enter() override;
    _int Update_State(const _float& timeDelta) override;
    void Exit() override;

    HRESULT Ready_State() override;

public:
    static CEyeball_Start* Create(CMonsterBase* owner);

private:
    void Free() override;

private:
    CMonsterBase*   m_Owner;
    _float          m_Pattern_Cooldown;

    _vec3           m_StartPos;
    _vec3           m_StartTargetPos;
    _float          m_LerpSpeed;
    _bool           m_IsReachedTarget;

    _float          m_MoveTime = 0.f;
    _float          m_MoveDuration = 1.f;

    
};

