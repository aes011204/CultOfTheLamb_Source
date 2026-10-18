#pragma once

#include "CMonsterState.h"

class CBossController;
class CCharacterBase;

BEGIN(Engine)
class CGameObject;
END

class CBoss2_Follower : public CMonsterState
{
private:
    explicit CBoss2_Follower(CGameObject* owner);
    virtual ~CBoss2_Follower();

public:
    void Enter() override;
    _int Update_State(const _float& timeDelta) override;
    void Exit() override;

    HRESULT Ready_State() override;

private:
    void Start_Dialogue();

public:
    static CBoss2_Follower* Create(CGameObject* owner);

private:
    void Free() override;

private:
    CGameObject*    m_Owner;
    _bool           m_DialogueStarted = false;
    _bool           m_IsFinished = false;
    _bool           m_Unconverted = false;

    _vec3           m_StartPos = _vec3(0.f, 0.f, 0.f);
    _vec3           m_TargetPos = _vec3(0.f, 0.f, 0.f);
    _float          m_MoveProgress = 0.f;

};

