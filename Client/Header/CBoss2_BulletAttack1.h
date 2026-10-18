#pragma once

#include "CMonsterState.h"

class CBossController;
class CCharacterBase;

BEGIN(Engine)
class CGameObject;
class CRcTex;
END

class CBoss2_BulletAttack1 : public CMonsterState
{
public:
    enum ChainAttackAnim
    {
        ROAR,       // 시작
        HANDSLMA,   // 공격

        ChainAnim_END
    };

private:
    explicit CBoss2_BulletAttack1(CGameObject* owner);
    virtual ~CBoss2_BulletAttack1();

public:
    void Enter() override;
    _int Update_State(const _float& timeDelta) override;
    void Exit() override;

    HRESULT Ready_State() override;

public:
    void Play_Roar();
    void Play_HandSlam();

private:
    void SpawnBullets_Horizontal(DEVICE graphicDev);
    void SpawnBullets_Vertical(DEVICE graphicDev);

    void SpawnBullets_Vertical_BottomToTop(DEVICE graphicDev); 
    void SpawnBullets_Vertical_TopToBottom(DEVICE graphicDev); 


    void Spawn_SingleBullet(
        DEVICE graphicDev, const _vec3& pos, const _vec3 dir, _float speed);

public:
    static CBoss2_BulletAttack1* Create(CGameObject* owner);

private:
    void Free() override;

private:
    CGameObject*     m_Owner;

    _float           m_Pattern_Cooldown;
    ChainAttackAnim  m_ChainAnim = ChainAttackAnim::ChainAnim_END;

    _int             m_SlamCount = 0;
    _bool            m_IsFinished = false;
    _bool            m_HasFiredBullet = false;

};

