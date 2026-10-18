#pragma once

#include "CCharacterBase.h"
#include "IHitFlashEff.h"

BEGIN(Engine)
class CTransform;
END

// 모든 몬스터들의 기본 베이스 (HumanMonster, Boss ..)
class CMonsterBase : public CCharacterBase, public IHitFlashEff
{
protected:
    explicit CMonsterBase(DEVICE graphicDev);
    explicit CMonsterBase(const CMonsterBase& rhs);
    virtual ~CMonsterBase();

public:
    HRESULT         Ready_GameObject() override;
    void            Initialize_GameObject() override;
    _int            Update_GameObject(const _float& timeDelta) override;
    void            LateUpdate_GameObject(const _float& timeDelta) override;

    void            Render_GameObject() override;

public:
    void             OnBeginOverlap(CCollider* self, CCollider* other) override;
    void             OnEndOverlap(CCollider* self, CCollider* other) override;

public:
    DEVICE          Get_GraphicDev() { return m_GraphicDev; }
    _float          Get_DistanceToTarget();
    _bool           Can_Attack() { return m_AttackTimer >= m_AttackCooldown; }

    virtual void    Chase_Target(const float& timeDelta, _float speed) {}

    _bool           Is_InAttackRange()      const;  // 공격 범위에 들어왔는지
    _bool           Is_InStoppingRange()    const;  // 멈출 범위
    _bool           Is_InChaseRange()       const;  // 따라갈 범위
    _bool           Is_OutofChaseRange()  const;  // Target 추적 그만할 위치. 필요한가?

    void            Set_StoppingDistance(_float distance) { m_StoppingDistance = distance; }
    void            Set_AttackRange(_float range) { m_AttackRange = range; }
    void            Set_ChaseRange(_float range) { m_ChaseRange = range; }

    _float          Get_StoppingDistance() const { return m_StoppingDistance; }
    _float          Get_AttackRange() const { return m_AttackRange; }
    _float          Get_ChaseRange() const { return m_ChaseRange; }

    // IHitFlashEff 구현부
    virtual void    FlashRenderStart(DWORD color = D3DCOLOR_ARGB(255, 255, 255, 255));
    virtual void    FlashRenderEnd() ;
    virtual void    AlphaRenderStart(float alpha = 0.8f) ;
    virtual void    AlphaRenderEnd();
    void            Set_Hurt(bool hurt) { m_bHurt = hurt; }

public:
    using   OnDeathCallback = function<void()>;
    void    Set_OnDeath(OnDeathCallback callback) { m_OnDeath = callback; }
    void    Trigger_OnDeath();

private:
    OnDeathCallback m_OnDeath = nullptr;

protected:
    void            Free() override;

protected:
    CGameObject*    m_Target;
    CTransform*     m_TargetTransform;

    _vec3           m_TargetDir{};

    // 공격 가능 범위 설정
    _float          m_StoppingDistance;  // Stop 위치 (원/근거리 다르게 설정)
    _float          m_AttackRange;       // 공격 가능 범위
    _float          m_ChaseRange;        // 추적 가능 범위

    // 공격 딜레이 설정
    _float          m_AttackCooldown;    // 몇초마다 공격할건지
    _float          m_AttackTimer;       // 현재 공격 타이머
    _bool           m_CanAttack;         // Timer >= Cooldown 공격이 가능한지 ?

protected:
    bool    m_OnDead = false;

private:
    bool    m_bHurt = false;
    float   HurtTime = 0;
    float   flashTime = 0.1f;

};

