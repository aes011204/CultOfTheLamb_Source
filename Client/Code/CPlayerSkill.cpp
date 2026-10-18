#include "pch.h"
#include "CPlayerSkill.h"
#include "CAttackBuilder.h"
#include "CCombatStat.h"
#include "CFireball.h"
#include "CGameObject.h"
#include "CLayer.h"
#include "CPlayerInput.h"
#include "CSoundMgr.h"
#include "CState.h"
#include "CTimerMgr.h"
#include "CUICharging.h"

CPlayerSkill::CPlayerSkill(DEVICE graphicDev)
    : CComponent(graphicDev),
      m_OwnerInputCom(nullptr),
      m_OwnerStateCom(nullptr),
      m_OwnerCombatStatCom(nullptr),
      m_Fireball(nullptr),
      m_CurrentAttackObj(nullptr),
      m_IsSkillUnlocked(false),
      m_SkillInfo{},
      m_SkillTimer(0),
      m_ChargeTimer(0),
      m_MaxChargeTime(5.f),
      m_ProjectileFired(false),
      m_SkillIndex(0.f),
      m_IsCharged(false),
      m_IsCharging(false),
      m_SingleSkillEnable(false),
      m_TripleSkillEnable(false),
      m_SingleFired(false),
      m_TripleFired(false),
      m_LeftChargingUI(nullptr),
      m_CenterChargingUI(nullptr),
      m_RightChargingUI(nullptr),
      m_CurrentCharges(0)
{ }

CPlayerSkill::CPlayerSkill(const CPlayerSkill& rhs)
    : CComponent(rhs),
      m_OwnerInputCom(nullptr),
      m_OwnerStateCom(nullptr),
      m_OwnerCombatStatCom(nullptr),
      m_Fireball(nullptr),
      m_CurrentAttackObj(nullptr),
      m_IsSkillUnlocked(false),
      m_SkillInfo{},
      m_SkillTimer(rhs.m_SkillTimer),
      m_ChargeTimer(rhs.m_ChargeTimer),
      m_MaxChargeTime(5.f),
      m_ProjectileFired(false),
      m_SkillIndex(rhs.m_SkillIndex),
      m_IsCharged(false),
      m_IsCharging(false),
      m_SingleSkillEnable(false),
      m_TripleSkillEnable(false),
      m_SingleFired(false),
      m_TripleFired(false),
      m_LeftChargingUI(nullptr),
      m_CenterChargingUI(nullptr),
      m_RightChargingUI(nullptr),
      m_CurrentCharges(0)
{
    copy_n(&rhs.m_SkillInfo[0], static_cast<_int>(SKILL_MAX), &m_SkillInfo[0]);
}

CPlayerSkill::~CPlayerSkill() {}

HRESULT CPlayerSkill::Ready_PlayerCombat()
{
    //충전
    m_SkillInfo[SKILL_CHARGING].state            = ACTORSTATE::PLAYER_SKILL_CHARGING;
    m_SkillInfo[SKILL_CHARGING].duration         = 0.5f;
    m_SkillInfo[SKILL_CHARGING].damageMultiplier = 2.f;
    m_SkillInfo[SKILL_CHARGING].knockbackPower   = 50.f;
    m_SkillInfo[SKILL_CHARGING].spawnOffset      = 0.8f;

    // 1발
    m_SkillInfo[SKILL_SINGLE].state            = ACTORSTATE::PLAYER_SKILL_SINGLE;
    m_SkillInfo[SKILL_SINGLE].duration         = 0.5f;
    m_SkillInfo[SKILL_SINGLE].damageMultiplier = 2.f;
    m_SkillInfo[SKILL_SINGLE].knockbackPower   = 50.f;
    m_SkillInfo[SKILL_SINGLE].spawnOffset      = 0.8f;
    m_SkillInfo[SKILL_SINGLE].hitboxScale      = _vec3(0.6f, 0.6f, 0.6f);
    m_SkillInfo[SKILL_SINGLE].chargeTime       = 0.15f;
    // 3발
    m_SkillInfo[SKILL_TRIPLE].state            = ACTORSTATE::PLAYER_SKILL_TRIPLE;
    m_SkillInfo[SKILL_TRIPLE].duration         = 0.5f;
    m_SkillInfo[SKILL_TRIPLE].damageMultiplier = 2.f;
    m_SkillInfo[SKILL_TRIPLE].knockbackPower   = 50.f;
    m_SkillInfo[SKILL_TRIPLE].spawnOffset      = 0.8f;
    m_SkillInfo[SKILL_TRIPLE].hitboxScale      = _vec3(0.6f, 0.6f, 0.6f);
    m_SkillInfo[SKILL_TRIPLE].chargeTime       = 0.6f;
    return S_OK;
}

_int CPlayerSkill::Update_Component(const _float& timeDelta)
{
    return 0;
}

void CPlayerSkill::LateUpdate_Component()
{
    CComponent::LateUpdate_Component();
}

HRESULT CPlayerSkill::Init_OwnerComponent()
{
    m_OwnerStateCom      = dynamic_cast<CState*>(m_Owner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::STATE));
    m_OwnerCombatStatCom = dynamic_cast<CCombatStat*>(m_Owner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::COMBATSTAT));
    m_OwnerInputCom      = dynamic_cast<CPlayerInput*>(m_Owner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::PL_INPUT));
    m_OwnerPlayerMovement = dynamic_cast<CPlayerMovement*>(m_Owner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::PL_MOVEMENT));

    m_LeftChargingUI = CUICharging::Create(m_GraphicDev);
    m_LeftChargingUI->Set_PlayerSkill(this);
    m_LeftChargingUI->Initialize_GameObject();

    CManagement::GetInstance()->Get_Scene()->Get_Layer(LAYERTYPE::UI)->Add_GameObject(OBJTYPE::UI, m_LeftChargingUI);

    m_CenterChargingUI = CUICharging::Create(m_GraphicDev);
    m_CenterChargingUI->Set_PlayerSkill(this);
    m_CenterChargingUI->Initialize_GameObject();

    CManagement::GetInstance()->Get_Scene()->Get_Layer(LAYERTYPE::UI)->Add_GameObject(OBJTYPE::UI, m_CenterChargingUI);

    m_RightChargingUI = CUICharging::Create(m_GraphicDev);
    m_RightChargingUI->Set_PlayerSkill(this);
    m_RightChargingUI->Initialize_GameObject();

    CManagement::GetInstance()->Get_Scene()->Get_Layer(LAYERTYPE::UI)->Add_GameObject(OBJTYPE::UI, m_RightChargingUI);

    HRESULT result = S_OK;

    if (nullptr == m_OwnerStateCom)
    {
        MessageBoxW(nullptr,
                    L"CPlayerSkill:Owner State Not Found",
                    L"System Message",
                    MB_ICONERROR);

        result = E_FAIL;
    }

    if (nullptr == m_OwnerCombatStatCom)
    {
        MessageBoxW(nullptr,
                    L"CPlayerSkill:Owner CombatStat Not Found",
                    L"System Message",
                    MB_ICONERROR);

        result = E_FAIL;
    }

    if (nullptr == m_OwnerInputCom)
    {
        MessageBoxW(nullptr,
                    L"CPlayerSkill:Owner PlayerInput Not Found",
                    L"System Message",
                    MB_ICONERROR);

        result = E_FAIL;
    }

    return result;
}

ACTORSTATE CPlayerSkill::Start_Charging()
{
    Reset_AttackData();

    if (!m_IsSkillUnlocked)
        return ACTORSTATE::STATE_END;

    // 슬로우모션 시작
    CTimerMgr::GetInstance()->Set_TimeScale(0.8f);

    CCombatStat* playerCombatStat = dynamic_cast<CCombatStat*>(m_Owner->Get_Component(COMPONENTID::ID_DYNAMIC, COMPONENTTYPE::COMBATSTAT));

    if (m_CurrentCharges <= 0)
        return ACTORSTATE::STATE_END;

    if (!m_IsCharging)
        m_ChargeTimer = 0.f;

    m_IsCharging = true;
    m_CenterChargingUI->Set_IsActive(true);

    CSoundMgr::GetInstance()->Play_Once(L"Player/Player_Skill_Charge.wav",
                                        SOUND_EFFECT,
                                        0.5f);

    return m_SkillInfo[SKILL_CHARGING].state;
}

ACTORSTATE CPlayerSkill::Update_Charging(const float& timeDelta)
{
    _vec3 vMouseDir = m_OwnerInputCom->Get_MouseDir();

    D3DXVec3Normalize(&vMouseDir, &vMouseDir);

    m_CenterChargingUI->Set_Dir(vMouseDir);

    D3DXVec3Normalize(&vMouseDir, &vMouseDir);
    float baseAngle = atan2f(vMouseDir.z, vMouseDir.x);

    float offset     = D3DXToRadian(30.f);
    float leftAngle  = baseAngle - offset;
    float RightAngle = baseAngle + offset;

    _vec3 vLeft  = { cosf(leftAngle), 0.f, sinf(leftAngle) };
    _vec3 vRight = { cosf(RightAngle), 0.f, sinf(RightAngle) };

    m_LeftChargingUI->Set_Dir(vLeft);
    m_RightChargingUI->Set_Dir(vRight);

    if (vMouseDir.x <= 0.f)
        m_OwnerStateCom->Change_Dir(ACTORDIR::LEFT);
    else
        m_OwnerStateCom->Change_Dir(ACTORDIR::RIGHT);

    if (!m_IsCharging)
    {
        m_LeftChargingUI->Set_IsActive(false);
        m_CenterChargingUI->Set_IsActive(false);
        m_RightChargingUI->Set_IsActive(false);

        return ACTORSTATE::IDLE;
    }
    m_ChargeTimer += timeDelta;

    if (m_ChargeTimer >= m_MaxChargeTime)
    {
        m_ChargeTimer = m_MaxChargeTime;
        m_IsCharging  = false;
        m_LeftChargingUI->Set_IsActive(false);
        m_CenterChargingUI->Set_IsActive(false);
        m_RightChargingUI->Set_IsActive(false);
        return ACTORSTATE::IDLE;
    }
    if (m_ChargeTimer >= m_SkillInfo[SKILL_TRIPLE].chargeTime)
    {
        m_LeftChargingUI->Set_IsActive(true);
        m_CenterChargingUI->Set_IsActive(true);
        m_RightChargingUI->Set_IsActive(true);
        return ACTORSTATE::PLAYER_SKILL_TRIPLE;
    }

    if (m_ChargeTimer >= m_SkillInfo[SKILL_SINGLE].chargeTime)
        return ACTORSTATE::PLAYER_SKILL_SINGLE;

    return ACTORSTATE::PLAYER_SKILL_CHARGING;
}

ACTORSTATE CPlayerSkill::Start_SingleSkill()
{
    if (!m_IsCharging)
        return ACTORSTATE::STATE_END;

    if (!m_IsSkillUnlocked)
        return ACTORSTATE::STATE_END;

    if (m_ChargeTimer >= m_SkillInfo[SKILL_SINGLE].chargeTime)
    {
        m_SkillTimer = 0;
        m_IsCharging = true;
        m_CenterChargingUI->Set_IsActive(true);
        m_SingleFired = false;
        m_SkillIndex  = SKILL_SINGLE;
    }

    CSoundMgr::GetInstance()->Play_Once(L"Player/Player_Skill_Shoot.wav",
                                        SOUND_EFFECT,
                                        0.5f);

    return m_SkillInfo[SKILL_SINGLE].state;
}

_bool CPlayerSkill::Update_SingleSkill(const float& timeDelta)
{
    for (auto iter = m_Fireballs.begin(); iter != m_Fireballs.end();)
    {
        if ((*iter)->Is_PendingDestroy())
            iter = m_Fireballs.erase(iter);
        else
            ++iter;
    }

    m_SkillTimer += timeDelta;
    m_ChargeTimer += timeDelta;

    const SkillInfo& skillInfo = m_SkillInfo[SKILL_SINGLE];
    _vec3            MouseDir  = m_OwnerInputCom->Get_MouseDir();

    D3DXVec3Normalize(&MouseDir, &MouseDir);

    if (!m_SingleFired)
    {
        CFireball* fireball = Create_Fireball(m_SkillInfo[m_SkillIndex], m_GraphicDev, m_Owner, MouseDir);
        m_Fireballs.push_back(fireball);

        m_IsCharging = false;

        m_CenterChargingUI->Set_IsActive(false);

        CCombatStat* playerCombatStat = dynamic_cast<CCombatStat*>(m_Owner->Get_Component(COMPONENTID::ID_DYNAMIC, COMPONENTTYPE::COMBATSTAT));

        // playerCombatStat->Set_CurrentCharges(-1);
        m_CurrentCharges -= 1;
        m_SingleFired       = true;
        m_SingleSkillEnable = false;
    }

    if (m_SkillTimer >= m_SkillInfo[SKILL_SINGLE].duration)
        return false;

    return true;
}

ACTORSTATE CPlayerSkill::Start_TripleSkill()
{
    if (!m_IsCharging)
        return ACTORSTATE::STATE_END;

    if (!m_IsSkillUnlocked)
        return ACTORSTATE::STATE_END;

    if (m_ChargeTimer >= m_SkillInfo[SKILL_TRIPLE].chargeTime)
    {
        m_SkillTimer  = 0;
        m_IsCharging  = true;
        m_TripleFired = false;
        m_CenterChargingUI->Set_IsActive(true);
        m_LeftChargingUI->Set_IsActive(true);
        m_RightChargingUI->Set_IsActive(true);
        m_SkillIndex = SKILL_TRIPLE;
    }

    CSoundMgr::GetInstance()->Play_Once(L"Player/Player_Skill_Shoot.wav",
                                        SOUND_EFFECT,
                                        0.6f);

    return m_SkillInfo[SKILL_TRIPLE].state;
}

_bool CPlayerSkill::Update_TripleSkill(const _float& timeDelta)
{
    if (!m_OwnerCombatStatCom || !m_OwnerInputCom) return false;

    for (auto iter = m_Fireballs.begin(); iter != m_Fireballs.end();)
    {
        if ((*iter)->Is_PendingDestroy())
            iter = m_Fireballs.erase(iter);
        else
            ++iter;
    }

    m_SkillTimer += timeDelta;
    m_ChargeTimer += timeDelta;

    const SkillInfo& skillInfo = m_SkillInfo[SKILL_TRIPLE];

    _vec3 MouseDir = m_OwnerInputCom->Get_MouseDir();

    D3DXVec3Normalize(&MouseDir, &MouseDir);
    float baseAngle = atan2f(MouseDir.z, MouseDir.x);

    float offset     = D3DXToRadian(15.f);
    float leftAngle  = baseAngle - offset;
    float RightAngle = baseAngle + offset;

    _vec3 vLeft  = { cosf(leftAngle), 0.f, sinf(leftAngle) };
    _vec3 vRight = { cosf(RightAngle), 0.f, sinf(RightAngle) };

    if (!m_TripleFired)
    {
        CFireball* center = Create_Fireball(m_SkillInfo[m_SkillIndex], m_GraphicDev, m_Owner, MouseDir);
        CFireball* left   = Create_Fireball(m_SkillInfo[m_SkillIndex], m_GraphicDev, m_Owner, vLeft);
        CFireball* right  = Create_Fireball(m_SkillInfo[m_SkillIndex], m_GraphicDev, m_Owner, vRight);

        m_Fireballs.push_back(center);
        m_Fireballs.push_back(left);
        m_Fireballs.push_back(right);

        m_IsCharging = false;

        m_CenterChargingUI->Set_IsActive(false);
        m_LeftChargingUI->Set_IsActive(false);
        m_RightChargingUI->Set_IsActive(false);

        CCombatStat* playerCombatStat = dynamic_cast<CCombatStat*>(m_Owner->Get_Component(COMPONENTID::ID_DYNAMIC, COMPONENTTYPE::COMBATSTAT));
        //playerCombatStat->Set_CurrentCharges(-1);
        m_CurrentCharges -= 1;
        m_TripleFired       = true;
        m_TripleSkillEnable = false;
    }

    if (m_SkillTimer >= m_SkillInfo[SKILL_TRIPLE].duration)
        return false;

    return true;
}

ACTORSTATE CPlayerSkill::Evaluate_ChargeLevel()
{
    return ACTORSTATE::STATE_END;
}

void CPlayerSkill::Cancel_Attack()
{
    Reset_AttackData();
}

void CPlayerSkill::Reset_AttackData()
{
    m_IsCharging = false;

    m_CenterChargingUI->Set_IsActive(false);
    m_LeftChargingUI->Set_IsActive(false);
    m_RightChargingUI->Set_IsActive(false);

    m_SkillTimer  = 0.f;
    m_ChargeTimer = 0.f;

    CTimerMgr::GetInstance()->Set_TimeScale(1.f);
}

CFireball* CPlayerSkill::Create_Fireball(const SkillInfo& skillInfo, DEVICE graphicDev, CGameObject* instigator, _vec3 skillDir)
{
    CFireball* fireball = nullptr;

    CAttackBuilder(graphicDev, instigator)
        .SetType<CFireball>() // .Box 박스 충돌체 소환하던걸 이제 Fireball, 이게 Sphere를 상속받아서 자체적으로 충돌체가 있어
        .Track(fireball)
        .Damage(
            dynamic_cast<CCombatStat*>(instigator->Get_Component(ID_DYNAMIC, COMPONENTTYPE::COMBATSTAT))->Get_Attack() * skillInfo.damageMultiplier)
        .Knockback(skillInfo.knockbackPower)
        .Scale(skillInfo.hitboxScale)
        .InstigatorPos()
        .Offset(skillDir * skillInfo.spawnOffset)
        .LifeTime(3.f)
        .DestroyOnHit(true)
        .CollisionLayer(COLLISION_LAYERTYPE::PLAYERPROJECTILE)
        .Spawn();

    if (fireball)
    {
        auto projectile = dynamic_cast<CProjectile*>(fireball->Get_Component(ID_STATIC, COMPONENTTYPE::PROJECTILE));
        if (projectile)
        {
            BulletDesc d;
            d.ProjectileOwner = instigator;
            d.moveType        = PROJ_TYPE::LINEAR;
            d.Speed           = 10.f;
            d.Direction       = skillDir;
            projectile->SetMoveDesc(d);
        }

        auto transform = fireball->Get_Transform();
        if (transform)
        {
            //transform->Set_Scale(0.7f, 0.7f, 0.f);
            transform->Set_Rotation(_vec3{ D3DXToRadian(30.f), 0.f, 0.f });
        }
    }

    return fireball;
}

CPlayerSkill* CPlayerSkill::Create(DEVICE graphicDev)
{
    auto playerskill = new CPlayerSkill(graphicDev);

    if (FAILED(playerskill->Ready_PlayerCombat()))
    {
        Safe_Release(playerskill);
        MSG_BOX("PlayerCombat Create Failed");
        return nullptr;
    }

    return playerskill;
}

CComponent* CPlayerSkill::Clone()
{
    return new CPlayerSkill(*this);
}

void CPlayerSkill::Free()
{
    m_OwnerStateCom      = nullptr;
    m_OwnerCombatStatCom = nullptr;
    m_OwnerInputCom      = nullptr;

    //총알은 레이어에 추가해서 CLayer LateUpdate에서 삭제

    CComponent::Free();
}
