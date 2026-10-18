#include "pch.h"
#include "CFloaterMonster.h"
#include "CCombatStat.h"
#include "CAnimator.h"
#include "CCreateHelper.h"
#include "CPlayer.h"
#include "CState.h"
#include "CTexture.h"
#include "CTransform.h"
#include "CRigidbody.h"
#include "CAttackBuilder.h"
#include "CParticleHelper.h"
#include "CUI_MonsterHp.h"

CFloaterMonster::CFloaterMonster(DEVICE graphicDev)
    : CMonsterBase(graphicDev),
      m_RigidbodyCom(nullptr),
      m_AttackSpawned(false),
      m_HpBar(nullptr)
{ }

CFloaterMonster::CFloaterMonster(const CFloaterMonster& rhs)
    : CMonsterBase(rhs),
      m_RigidbodyCom(nullptr),
      m_AttackSpawned(false),
      m_HpBar(nullptr)
{ }

CFloaterMonster::~CFloaterMonster()
{ }

HRESULT CFloaterMonster::Ready_GameObject()
{
    if (FAILED(CMonsterBase::Ready_GameObject()))
        return E_FAIL;

    m_TransformCom->Set_Pos(_vec3(5.f, 1.f, 7.f));
    m_TransformCom->Set_Scale(_vec3(1.8f, 1.8f, 1.2f));

    m_CombatStatCom->Set_MaxHp(750.f);
    m_CombatStatCom->Set_Speed(65.f);

    //!! Rigidbody에서 중력만 사용 안함
    m_RigidbodyCom->Set_UseAccel(true);
    m_RigidbodyCom->Set_UseFriction(true);
    m_RigidbodyCom->Set_UseGravity(false);

    m_RigidbodyCom->Set_Friction(0.3f);

    m_TransformCom->Set_RotationAxis(ROT_X, D3DXToRadian(45.f));

    // 공격 주기 설정
    m_AttackCooldown = 2.f;
    m_AttackTimer    = 0.f;

    // 거리 설정
    Set_StoppingDistance(2.f);
    Set_AttackRange(2.5f);
    Set_ChaseRange(10.f);

    return S_OK;
}

void CFloaterMonster::Initialize_GameObject()
{
    CMonsterBase::Initialize_GameObject();

    m_TargetTransform = dynamic_cast<CTransform*>(
        CManagement::GetInstance()->Get_Component(ID_DYNAMIC,
                                                  LAYERTYPE::GAMELOGIC,
                                                  OBJTYPE::PLAYER,
                                                  COMPONENTTYPE::TRANSFORM));

    if (m_TargetTransform)
        m_Target = m_TargetTransform->Get_Owner();

    // Hp Bar 추가
    /*m_HpBar = CUI_MonsterHp::Create(m_GraphicDev, this);
    if (m_HpBar)
    {
        m_HpBar->Set_YOffset(1.7f);

        SpawnUIOnLayer(m_HpBar, OBJTYPE::UI);
    }*/
}

_int CFloaterMonster::Update_GameObject(const _float& timeDelta)
{
    _int exit = CMonsterBase::Update_GameObject(timeDelta);

    if (Check_CommonTransitions())
        return exit;

    Update_Direction();

    ACTORSTATE curState = m_StateCom->Get_State();

    switch (curState)
    {
    case ACTORSTATE::IDLE: Update_State_Idle(timeDelta);
        break;
    case ACTORSTATE::RUN: Update_State_Run(timeDelta);
        break;
    case ACTORSTATE::ATTACK: Update_State_Attack(timeDelta);
        break;
    case ACTORSTATE::HIT: Update_State_Hit(timeDelta);
        break;
    case ACTORSTATE::DEAD: Update_State_Dead(timeDelta);
        break;
    }

    return exit;
}

void CFloaterMonster::LateUpdate_GameObject(const _float& timeDelta)
{
    CMonsterBase::LateUpdate_GameObject(timeDelta);
}

void CFloaterMonster::Render_GameObject()
{
    CMonsterBase::Render_GameObject();
}

_bool CFloaterMonster::Check_CommonTransitions()
{
    ACTORSTATE curState = m_StateCom->Get_State();

    if (m_CombatStatCom->Is_Dead())
    {
        if (curState != ACTORSTATE::DEAD)
            m_StateCom->Change_State(ACTORSTATE::DEAD);

        return false;
    }

    if (curState != ACTORSTATE::HIT && m_CombatStatCom->Is_Knockback())
    {
        m_StateCom->Change_State(ACTORSTATE::HIT);

        return true;
    }

    return false;
}

void CFloaterMonster::Update_State_Idle(const _float timeDelta)
{
    m_AttackTimer += timeDelta;

    // 공격 가능하고 거리 안에 있으면
    if (Is_InAttackRange() && Can_Attack())
    {
        m_StateCom->Change_State(ACTORSTATE::ATTACK);
        m_AttackTimer = 0.f;
    }
    // 추적 범위 안에 있으면 RUN
    else if (Is_InChaseRange())
        m_StateCom->Change_State(ACTORSTATE::RUN);
}

void CFloaterMonster::Update_State_Run(const _float timeDelta)
{
    m_AttackTimer += timeDelta;

    // 멈춤 거리 밖이면 추적
    if (!Is_InStoppingRange())
        Chase_Target(timeDelta, m_CombatStatCom->Get_Speed());
    // 공격 가능
    if (Is_InAttackRange() && Can_Attack())
    {
        m_StateCom->Change_State(ACTORSTATE::ATTACK);
        m_AttackTimer = 0.f;
    }
    // 추적 범위 밖
    else if (Is_OutofChaseRange())
        m_StateCom->Change_State(ACTORSTATE::IDLE);
}

void CFloaterMonster::Update_State_Attack(const _float timeDelta)
{
    if (m_AnimatorCom->Get_CurFrame() == 2 && !m_AttackSpawned)
    {
        CAttackBuilder(m_GraphicDev, this)
            .SetType<CAttackBox>()
            .Damage(1.f)
            .LifeTime(0.2f)
            .DestroyOnHit(true)
            .InstigatorPos()
            .Offset(m_TargetDir * 0.6f)
            .Knockback(50.f)
            .Scale(_vec3(1.f, 1.f, 1.f))
            .Particle(L"HitEffect_Star", 14, 0.04f)
            .Spawn();

        m_AttackSpawned = true; // 한 프레임에 2개 생성해버려서
    }

    if (m_AnimatorCom->Is_AnimationFinished())
    {
        m_StateCom->Change_State(ACTORSTATE::IDLE);
        m_AttackSpawned = false;
    }
}

void CFloaterMonster::Update_State_Hit(const _float timeDelta)
{
    // 애니메이션이 끝나고, 넉백이 다 밀렸을 때
    if (m_AnimatorCom->Is_AnimationFinished() && !m_CombatStatCom->Is_Knockback())
        m_StateCom->Change_State(ACTORSTATE::IDLE);
}

void CFloaterMonster::Update_State_Dead(const _float timeDelta)
{
    if (m_StateCom->Get_State() == ACTORSTATE::DEAD)
    {
        Trigger_OnDeath();
        Mark_As_PendingKill();
    }
}

void CFloaterMonster::Update_Direction()
{
    if (!m_TargetTransform || !m_TransformCom)
        return;

    ACTORSTATE curState = m_StateCom->Get_State();

    // Idle, Run일 떄에만 방향 전환 허용
    bool canChangeDir = false;
    canChangeDir |= curState == ACTORSTATE::IDLE;
    canChangeDir |= curState == ACTORSTATE::RUN;

    if (!canChangeDir)
        return;

    if (m_TargetDir.x > 0.f)
        m_StateCom->Change_Dir(ACTORDIR::RIGHT);
    else
        m_StateCom->Change_Dir(ACTORDIR::LEFT);
}

void CFloaterMonster::Animation_Setting()
{
    // 애니메이션 생성
    m_AnimatorCom->Create_Animation(L"Floater_Idle", 32, 0.04f, _vec2(0.8f, 0.9f));
    m_AnimatorCom->Create_Animation(L"Floater_Hit", 25, 0.04f, _vec2(0.8f, 0.9f));
    m_AnimatorCom->Create_Animation(L"Floater_Attack", 34, 0.02f, _vec2(1.5f, 1.6f), _vec2(0.f, 0.63f));

    // State -> Animation 연동
    m_StateCom->Set_AnimInfo(ACTORSTATE::IDLE, L"Floater_Idle", ANIMSTATE::LOOP);
    m_StateCom->Set_AnimInfo(ACTORSTATE::RUN, L"Floater_Idle", ANIMSTATE::LOOP);
    m_StateCom->Set_AnimInfo(ACTORSTATE::HIT, L"Floater_Hit", ANIMSTATE::ONCE);
    m_StateCom->Set_AnimInfo(ACTORSTATE::ATTACK, L"Floater_Attack", ANIMSTATE::ONCE);
}

void CFloaterMonster::OnBeginOverlap(CCollider* self, CCollider* other)
{
    CMonsterBase::OnBeginOverlap(self, other);
}

void CFloaterMonster::OnEndOverlap(CCollider* self, CCollider* other)
{
    CMonsterBase::OnEndOverlap(self, other);
}

void CFloaterMonster::Chase_Target(const float& timeDelta, _float speed)
{
    if (!m_TargetTransform || !m_TransformCom)
        return;

    _vec3 pos       = m_TransformCom->Get_Pos();
    _vec3 targetPos = m_TargetTransform->Get_Pos();
    //m_TransformCom->Chase_Target(&targetPos, timeDelta, speed);

    _vec3 dir = targetPos - pos;
    dir.y     = 0.f;

    D3DXVec3Normalize(&dir, &dir);

    _vec3 moveForce = dir * m_CombatStatCom->Get_Speed();
    m_RigidbodyCom->Add_Force(moveForce);
}

HRESULT CFloaterMonster::Add_Component()
{
    if (FAILED(CMonsterBase::Add_Component()))
        return E_FAIL;

    // texture
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_FLOATER_MONSTER);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);

    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_FLOATER_MONSTER, m_TextureCom });

    m_AnimatorCom->Set_TextureType(COMPONENTTYPE::TEX_FLOATER_MONSTER);

    // Rigidbody
    m_RigidbodyCom = CreateProtoComponent<CRigidbody>(this, COMPONENTTYPE::RIGIDBODY);
    NULL_CHECK_RETURN(m_RigidbodyCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::RIGIDBODY, m_RigidbodyCom });

    return S_OK;
}

CFloaterMonster* CFloaterMonster::Create(DEVICE graphicDev)
{
    auto floater = new CFloaterMonster(graphicDev);

    if (FAILED(floater->Ready_GameObject()))
    {
        MSG_BOX("Floater Create Failed");
        Safe_Release(floater);
        return nullptr;
    }

    return floater;
}

void CFloaterMonster::Free()
{
    CMonsterBase::Free();
}
