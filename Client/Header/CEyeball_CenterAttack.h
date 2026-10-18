#pragma once

#include "CMonsterState.h"

class CMonsterController;
class CMonsterBase;
class CBoss2_Bullet;

class CEyeball_CenterAttack : public CMonsterState
{
    enum class Eyeball_Anim
    {
        ATTACK_ANTI,
        ATTACK_LOOP,
        ATTACK_END,

        EYEBALL_ATTACK_END
    };

private:
    explicit     CEyeball_CenterAttack(CMonsterBase* owner);
    virtual     ~CEyeball_CenterAttack();

public:
    void         Enter() override;
    _int         Update_State(const _float& timeDelta) override;
    void         Exit() override;
                 
    HRESULT      Ready_State() override;

private:
    void         Move_Center(const _float& timeDelta);
    void         Grow_Size(const _float& timeDelta);
    void         Spawn_BounceBullets();

public:
    static CEyeball_CenterAttack* Create(CMonsterBase* owner);

private:
    void Free() override;

private:
    DEVICE                  m_GraphicDev;

    CMonsterBase*           m_Owner;

    // 이동 관련
    _vec3                   m_TargetPos = { 0.f, 0.f, 0.f };
    _float                  m_MoveSpeed = 5.f;
    _bool                   m_HasReachedCenter = false; // 가운데에 도착했는지

    // 크기
    _float                  m_CurrentScale = 1.f;
    _float                  m_TargetScale = 2.f; // 최대 크기
    _float                  m_GrowSpeed = 0.5f;

    // Bullet
    _float                  m_FireTimer = 0.f;
    _float                  m_FireInterval = 0.5f;      
    _int                    m_FireCount = 0;
    _int                    m_MaxFireCount = 6;         
    _bool                   m_IsFinished = false;
};

