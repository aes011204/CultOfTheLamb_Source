#pragma once
#include "CComponent.h"
#include <CPlayerMovement.h>

class CPlayerInput;
class CWeaponEquip;
class CFireball;
class CUICharging;

BEGIN(Engine)
class CAttackBase;
class CCombatStat;
class CState;
END

class CPlayerSkill : public CComponent
{
public:
    struct SkillInfo
    {
        ACTORSTATE state;

        _float duration;
        _float damageMultiplier;
        _float knockbackPower;
        _float spawnOffset;
        _vec3 hitboxScale;
        _float skillspeed;
        _float chargeTime;

        PARTICLEDATA particle;
    };

    enum PL_SKILL_INDEX
    {
        SKILL_CHARGING,
        SKILL_SINGLE,
        SKILL_TRIPLE,

        SKILL_MAX
    };
private:
    explicit CPlayerSkill(DEVICE graphicDev);
    explicit CPlayerSkill(const CPlayerSkill& rhs);
    ~CPlayerSkill() override;


public:
    HRESULT Ready_PlayerCombat();
    _int    Update_Component(const _float& timeDelta) override;
    void    LateUpdate_Component() override;

public:
    HRESULT Init_OwnerComponent();
    
public:
    void Set_SingleSkill(_bool tf) { m_SingleSkillEnable = tf; }
    void Set_TripleSkill(_bool tf) { m_TripleSkillEnable = tf; }

    _bool Get_SingleSkillAvailability() { return m_SingleSkillEnable; }
    _bool Get_TripleSkillAvailability() { return m_TripleSkillEnable; }

    float Get_ChargeTime() { return m_ChargeTimer; }
    float Get_MaxChargeTime() { return m_MaxChargeTime; }
    SkillInfo Get_SkillInfo() { return m_SkillInfo[m_SkillIndex]; }
    _bool Get_IsCharging() { return m_IsCharging; }
	_bool Get_IsSkillUnlocked() { return m_IsSkillUnlocked; }
public:
    ACTORSTATE  Start_Charging();
    ACTORSTATE  Update_Charging(const float& timeDelta);

    ACTORSTATE  Start_SingleSkill();
    _bool       Update_SingleSkill(const float& timeDelta);

    ACTORSTATE  Start_TripleSkill();
    _bool       Update_TripleSkill(const float& timeDelta);

    ACTORSTATE  Evaluate_ChargeLevel(); 
public:
    void Set_CurrentTimer(_float timer) { m_SkillTimer = timer; }
    void Cancel_Attack();
    void Set_SkillUnlocked(bool unlocked) { m_IsSkillUnlocked = unlocked; }
    _int Get_CurrentCharges() { return m_CurrentCharges; }
    void Set_CurrentCharges(_int charge) { m_CurrentCharges = charge; }
private:
    void Reset_AttackData();
   // void Create_Projectile(const SkillInfo& skillInfo, const _vec3& SkillDir);
    CFireball* Create_Fireball(const SkillInfo& skillInfo, DEVICE graphicDev, CGameObject* instigator, _vec3 skillDir);

private:
    CPlayerInput* m_OwnerInputCom;
    CState* m_OwnerStateCom;
    CCombatStat* m_OwnerCombatStatCom;
    CPlayerMovement* m_OwnerPlayerMovement;
    CFireball* m_Fireball;
private:
    CAttackBase* m_CurrentAttackObj;

private:
    _bool     m_IsSkillUnlocked;
    SkillInfo m_SkillInfo[SKILL_MAX];
    _float    m_SkillTimer;
    _float    m_ChargeTimer;
    _float    m_MaxChargeTime;
    _bool     m_ProjectileFired;
    _int      m_SkillIndex;
    _bool     m_IsCharged;
    _bool     m_IsCharging;

    _bool     m_SingleSkillEnable;
    _bool     m_TripleSkillEnable;
    _bool     m_SingleFired;
    _bool    m_TripleFired;

    vector<CFireball*> m_Fireballs;

public:
    static CPlayerSkill* Create(DEVICE graphicDev);
    CComponent* Clone() override;
    
private:
    void Free() override;

private:
    CUICharging* m_LeftChargingUI;
    CUICharging* m_CenterChargingUI;
    CUICharging* m_RightChargingUI;

    _int m_CurrentCharges;
};
