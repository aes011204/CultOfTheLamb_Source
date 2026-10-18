#pragma once

#include "CMonsterState.h"

class CMonsterController;
class CMonsterBase;
class CBoss2_Bullet;

class CEyeball_Attack : public CMonsterState
{
    enum class Eyeball_Anim
    {
        ATTACK_ANTI,
        ATTACK_LOOP,
        ATTACK_END,

        EYEBALL_ATTACK_END
    };

    enum class Attack_Type
    {
        DISPERSION, //분산
        CIRCLE,
        BURST,

        ATTACK_TYPE_END
    };

private:
    explicit     CEyeball_Attack(CMonsterBase* owner);
    virtual     ~CEyeball_Attack();

public:
    void        Enter() override;
    _int        Update_State(const _float& timeDelta) override;
    void        Exit() override;

    HRESULT     Ready_State() override;

public:
    void        Play_Anti();
    void        Play_Loop();
    void        Play_End();

public:
    void        Bullet_Dispresion();
    void        Bullet_Circle();
    void        Bullet_Brust();

public:
    static CEyeball_Attack* Create(CMonsterBase* owner);

private:
    void Attack_Reset(CAnimator* animator);
    void Random_Movement(const _float& timeDelta);

private:
    void Free() override;

private:
    DEVICE                  m_GraphicDev;

    CMonsterBase*           m_Owner;
    _float                  m_Pattern_Cooldown;

    Eyeball_Anim            m_EyeballAnim;
    _bool                   m_HasFiredBullet;

    _int                    m_AttackCount;
    _bool                   m_IsFinished;

    Attack_Type             m_Bullet_AttackType;
    _float                  m_circleTimer = 0.f;
    _int                    m_Spawn_BulletCount = 0;
    vector<CBoss2_Bullet*>  m_CircleBullets;

    // 랜덤 이동
    _vec3                   m_Current_Direction;
    _vec3                   m_Target_Direction;

    _float                  m_Direction_ChangeTimer;
    _float                  m_Direction_Timer;      
    _float                  m_MoveSpeed;            
    _float                  m_LerpSpeed;        

    _int                    m_TotalAttackCount = 0;
    _int                    m_MaxAttackCount = 0;
};

