#include "CCombatStat.h"
#include "CAnimator.h"
#include "CGameObject.h"
#include "CManagement.h"
#include "CRigidbody.h"
#include "CState.h"
#include "CTimerMgr.h"
#include "CTransform.h"

class CMonsterBase;

CCombatStat::CCombatStat(DEVICE graphicDev)
    : CComponent(graphicDev), m_OwnerRigidbody(nullptr),
      m_Attack(0),
      m_AttackSpeed(1.f),
      m_Hp(0),
      m_MaxHp(6),
      m_Mp(0),
      m_MaxMp(10),
      m_Speed(20),
      m_IsDead(false), m_CanHitState(true), m_IsInvincible(false), m_KnockbackTimer(0)
{
}

CCombatStat::CCombatStat(const CCombatStat& rhs)
    : CComponent(rhs), m_OwnerRigidbody(nullptr),
      m_Attack(0),
      m_AttackSpeed(1.f),
      m_Hp(0),
      m_MaxHp(rhs.m_MaxHp),
      m_Mp(0),
      m_MaxMp(rhs.m_MaxMp),
      m_Speed(rhs.m_Speed),
      m_IsDead(false), m_CanHitState(true), m_IsInvincible(false), m_KnockbackTimer(0)
{
    m_Hp = m_MaxHp;
    m_Mp = m_MaxMp;

    m_IsDead = false;
}

CCombatStat::~CCombatStat()
{ }

HRESULT CCombatStat::Ready_StateComponent(DEVICE graphicDev)
{
    // TODO : 객체마다 스탯 설정해주기
    // ex) Player -> m_CombatStat->Set_Hp(100.f);
    // ex) Player -> m_CombatStat->Set_Attack(10.f);

    return S_OK;
}

void CCombatStat::Initialize_Component()
{
    CComponent::Initialize_Component();

    if (!m_OwnerRigidbody && m_Owner)
    {
        m_OwnerRigidbody = dynamic_cast<CRigidbody*>(
            m_Owner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::RIGIDBODY));
    }
}

_int CCombatStat::Update_Component(const _float& timeDelta)
{
    _int exit = CComponent::Update_Component(timeDelta);

    // 넉백 타이머 세팅
    if (m_KnockbackTimer > 0.f)
    {
        m_KnockbackTimer -= timeDelta;
        if (m_KnockbackTimer < 0.f)
            m_KnockbackTimer = 0.f;
    }

    return exit;
}

void CCombatStat::LateUpdate_Component()
{
    CComponent::LateUpdate_Component();
}

void CCombatStat::Take_Damage(const HITDATA& hitData)
{
    if (m_IsInvincible) // 무적 체크 -> AttackBase에서 하도록 변경하긴 했음
        return;

    if (m_IsDead)
        return;

    if (hitData.damage <= 0.f)
        return;

    // 데미지
    Apply_Damage(hitData);

    // 넉백
    Apply_Knockback(hitData);

    // Hit 상태로 전환. 보스는 false로 설정
    if (m_CanHitState && m_Owner)
    {
        CState* state = dynamic_cast<CState*>(
            m_Owner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::STATE));

        if (state)
            state->Change_State(ACTORSTATE::HIT);
    }
}

void CCombatStat::Apply_Damage(const HITDATA& hitData)
{
    m_Hp -= hitData.damage;
    if (m_Hp < 0.f)
        m_Hp = 0.f;

    m_DirForItem = hitData.dir;

    if (OnDamaged)
        OnDamaged();

    if (m_Hp <= 0.f && !m_IsDead)
        On_Dead();
}

void CCombatStat::Apply_Knockback(const HITDATA& hitData)
{
    if (hitData.knockback > 0.f && m_OwnerRigidbody)
    {
        _vec3 knockbackDir = hitData.dir;
        knockbackDir.y = 0.f;  
        D3DXVec3Normalize(&knockbackDir, &knockbackDir);  

        _float mass = m_OwnerRigidbody->Get_Mass();
        _vec3 knockbackVel = knockbackDir * (hitData.knockback / mass);

        m_OwnerRigidbody->Set_Velocity(knockbackVel);
        m_KnockbackTimer = hitData.knockbackTimer;
    }
}

void CCombatStat::On_Dead()
{
    m_IsDead = true;

    if (m_Owner)
    {
        CState* state = dynamic_cast<CState*>(
            m_Owner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::STATE));

        if (state)
            state->Change_State(ACTORSTATE::DEAD);

     //  // 각자 클래스에서 애니메이션 끝났을 때 판단해서 추가로 처리하기
     //  CMonsterBase* monster = dynamic_cast<CMonsterBase*>(m_Owner);
     //  if (monster)
     //  {
	 //
	//	CScene* scene = CManagement::GetInstance()->Get_Scene();
	//	CStage_Dungeon* dungeonStage = dynamic_cast<CStage_Dungeon*>(scene);
	 //
	//	if (dungeonStage)
	//	{
	//		dungeonStage->Get_RunLootLog().Add(m_itemInst);
	//	}
     //  }
	 //

    }
}

CCombatStat* CCombatStat::Create(DEVICE graphicDev)
{
    CCombatStat* state = new CCombatStat(graphicDev);

    if (FAILED(state->Ready_StateComponent(graphicDev)))
    {
        Safe_Release(state);
        MSG_BOX("CombatStat Create Failed");
        return nullptr;
    }

    return state;
}

CComponent* CCombatStat::Clone()
{
    return new CCombatStat(*this);
}

void CCombatStat::Free()
{
    CComponent::Free();
}
