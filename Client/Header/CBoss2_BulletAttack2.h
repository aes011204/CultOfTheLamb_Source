#pragma once

#include "CMonsterState.h"

class CBossController;
class CCharacterBase;

BEGIN(Engine)
class CGameObject;
class CRcTex;
END

class CBoss2_BulletAttack2 : public CMonsterState
{
public:
    enum ChainAttackAnim
    {
        ROAR,       // 시작
        HANDSLMA,   // 공격

        ChainAnim_END
    };

private:
    explicit CBoss2_BulletAttack2(CGameObject* owner);
    virtual ~CBoss2_BulletAttack2();

public:
    void Enter() override;
    _int Update_State(const _float& timeDelta) override;
    void Exit() override;

    HRESULT Ready_State() override;

public:
    void PlayRoar();
    void PlayHandSlam();

    void Spawn_BounceBullets();

public:
    static CBoss2_BulletAttack2* Create(CGameObject* owner);

private:
    void Free() override;

private:
    CGameObject*     m_Owner;

    _float           m_Pattern_Cooldown;
    ChainAttackAnim  m_ChainAnim = ChainAttackAnim::ChainAnim_END;

    _int             m_SlamCount = 0;
    _bool            m_IsFinished = false;

private:
    _float           m_FireTimer = 0.f;
    _float           m_FireInterval = 0.3f;
    _int             m_FireCount = 0;
    _int             m_MaxFireCount = 5;

};

