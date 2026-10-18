#pragma once

#include "CComponent.h"
#include "Engine_Define.h"

BEGIN(Engine)
class CAnimator;
class CRigidbody;

// 공격 설정용, AttackBuilder로 스폰할 때 사용
typedef struct tagAttackData
{
    _float damage;     // 데미지
    _float knockback;  // 넉백량
    _float duration;   // 넉백 유지 시간(필요하면 사용 ㄱㄱ)

    _float hitStopDuration;
    _float hitStopScale;

    PARTICLEDATA hitParticle;

    tagAttackData()
        : damage(0.f),
          knockback(0.f),
          duration(0.f), hitStopDuration(0), hitStopScale(0)
    {
    }
} ATTACKDATA;

// 이건 충돌 구조에서 알아서 채워지는 Hit Data
typedef struct tagHitData
{
    _vec3  dir;              // 넉백 방향
    _float damage;           // 데미지
    _float knockback;        // 넉백량
    _float knockbackTimer;         // 넉백 유지 시간

    CGameObject* attacker;   // 때린 놈
    CGameObject* hitter;     // 맞은 놈

    tagHitData()
        : dir(0.f, 0.f, 0.f),
        damage(0.f),
        knockback(0.f),
        knockbackTimer(0.f),
        attacker(nullptr),
        hitter(nullptr)
    {}

    // AttackData에서 HitData로 변환하는 편의 생성자
    tagHitData(const ATTACKDATA& attackData)
        : dir(0.f, 0.f, 0.f),
        damage(attackData.damage),
        knockback(attackData.knockback),
        knockbackTimer(attackData.duration),
        attacker(nullptr),
        hitter(nullptr)
    {}

} HITDATA;

/**
 * @brief GameObject의 전투 상태 스테이터스 컴포넌트
 */
class ENGINE_DLL CCombatStat : public CComponent
{
private:
    explicit CCombatStat(DEVICE graphicDev);
    explicit CCombatStat(const CCombatStat& rhs);
    ~CCombatStat() override;

public:
    HRESULT Ready_StateComponent(DEVICE graphicDev);
    void    Initialize_Component() override;
    _int    Update_Component(const _float& timeDelta) override;
    void    LateUpdate_Component() override;

public:
    _float  Get_Attack() { return m_Attack; }
    _float  Get_AttackSpeed() { return m_AttackSpeed; }
    _float  Get_Hp() { return m_Hp; }
    _float  Get_MaxHp() { return m_MaxHp; }
    _float  Get_Mp() { return m_Mp; }
    _float  Get_Speed() { return m_Speed; }
    _float  Get_MaxMp() { return m_MaxMp; }
  //  _int    Get_CurrentCharges() { return m_CurrentCharges; }

    //  아이템용
    _vec3   Get_Dir() { return m_DirForItem; }
    void    Set_OnDamagedEvent(function<void()> event) { OnDamaged = event; }

    _bool   Is_Dead() { return m_IsDead; }
    _bool   Is_Invincible() const { return m_IsInvincible; }
    _bool   Is_Knockback() const { return m_KnockbackTimer > 0.f; }

    _bool   Can_HitState() { return m_CanHitState; }
    void    Set_HitState(_bool flag) { m_CanHitState = flag; }

    void    Set_Attack(_float attack) { m_Attack = attack; }
    void    Set_AttackSpeed(_float attackSpeed) { m_AttackSpeed = attackSpeed; }
	void    Set_Hp(_float hp) { if (hp > m_MaxHp)m_Hp = m_MaxHp; else m_Hp = hp; }
    void    Set_MaxHp(_float maxHp) { m_MaxHp = maxHp; Set_Hp(maxHp); }
    void    Set_Mp(_float mp) { m_Mp = mp; }
    void    Set_MaxMp(_float maxMp) { m_MaxMp = maxMp; Set_Mp(maxMp); }
    void    Set_Speed(_float speed) { m_Speed = speed; }
    void    Set_Invincible(bool invincible) { m_IsInvincible = invincible; }
  //  void    Set_CurrentCharges(int chargeCount) { m_CurrentCharges += chargeCount; }

    void    Add_Attack(float attack) { m_Attack += attack; }

public:
    void    Take_Damage(const HITDATA& hitData);

    void    Apply_Damage(const HITDATA& hitData);
    void    Apply_Knockback(const HITDATA& hitData);
    void    On_Dead();

public:
    static CCombatStat* Create(DEVICE graphicDev);

    CComponent* Clone() override;

protected:
    void Free() override;

private:
    CRigidbody* m_OwnerRigidbody;

    _float  m_Attack;
    _float  m_AttackSpeed;   // 공격 속도

    _float  m_Hp;
    _float  m_MaxHp = 100.f;
    _float  m_Mp;
    _float  m_MaxMp = 10.f;
    _float  m_Speed;

  //  _int    m_CurrentCharges = 0 ;   // 스킬 사용 가능 횟수
  //  _int    m_MaxCharges = 0;

    _bool   m_IsDead;
    _bool   m_CanHitState; // 보스는 false로 설정해줘야 Hit상태로 변환 안됨

    _bool   m_IsInvincible; // 무적
    _float  m_KnockbackTimer;

    //  아이템용
    _vec3 m_DirForItem = {0,0,0};
    function<void()> OnDamaged;
};

END
