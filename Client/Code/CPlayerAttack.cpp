#include "pch.h"
#include "CPlayerAttack.h"

#include "CAttackBuilder.h"
#include "CCombatStat.h"
#include "CGameObject.h"
#include "CPlayerInput.h"
#include "CPlayerMovement.h"
#include "CSoundMgr.h"
#include "CState.h"
#include "CWeaponEquip.h"

CPlayerAttack::CPlayerAttack(DEVICE graphicDev)
    : CComponent(graphicDev),

      m_OwnerStateCom(nullptr),
      m_OwnerCombatStatCom(nullptr),
      m_OwnerWeaponCom(nullptr),
      m_OwnerInputCom(nullptr),
      m_OwnerMovementCom(nullptr),

      m_CurrentAttackObj(nullptr),

      m_AttackInfo{},
      m_ComboIndex(0),
      m_MaxCombo(3),

      m_Timer(0.f),
      m_IsHitboxActive(false),
      m_CanNextCombo(false),
      m_IsNextComboReserved(false)
{}

CPlayerAttack::CPlayerAttack(const CPlayerAttack& rhs)
    : CComponent(rhs),

      m_OwnerStateCom(nullptr),
      m_OwnerCombatStatCom(nullptr),
      m_OwnerWeaponCom(nullptr),
      m_OwnerInputCom(nullptr),
      m_OwnerMovementCom(nullptr),

      m_CurrentAttackObj(nullptr),

      m_ComboIndex(rhs.m_ComboIndex),
      m_MaxCombo(rhs.m_MaxCombo),

      m_Timer(rhs.m_Timer),
      m_IsHitboxActive(rhs.m_IsHitboxActive),
      m_CanNextCombo(rhs.m_CanNextCombo),
      m_IsNextComboReserved(rhs.m_IsNextComboReserved)
{
    copy_n(&rhs.m_AttackInfo[0][0],
           static_cast<_int>(WEAPON_MAX) * static_cast<_int>(ATTACK_MAX),
           &m_AttackInfo[0][0]);
}

CPlayerAttack::~CPlayerAttack()
{ }

HRESULT CPlayerAttack::Ready_PlayerCombat()
{
    // =========================================================
    // [1] 검 (WEAPON_SWORD) 데이터 설정
    // =========================================================

    // 강공격
    m_AttackInfo[WEAPON_SWORD][ATTACK_HEAVY].state            = ACTORSTATE::PLAYER_SWORD_HEAVY;
    m_AttackInfo[WEAPON_SWORD][ATTACK_HEAVY].duration         = 0.02f * 70;
    m_AttackInfo[WEAPON_SWORD][ATTACK_HEAVY].hitboxStartTime  = 0.5f;
    m_AttackInfo[WEAPON_SWORD][ATTACK_HEAVY].hitboxEndTime    = 0.6f;
    m_AttackInfo[WEAPON_SWORD][ATTACK_HEAVY].comboInputTime   = 0.0f; // 강공격은 연계 없음
    m_AttackInfo[WEAPON_SWORD][ATTACK_HEAVY].damageMultiplier = 3.f;
    m_AttackInfo[WEAPON_SWORD][ATTACK_HEAVY].knockbackPower   = 50.f;
    m_AttackInfo[WEAPON_SWORD][ATTACK_HEAVY].spawnOffset      = 1.3f;
    m_AttackInfo[WEAPON_SWORD][ATTACK_HEAVY].hitboxScale      = _vec3(1.f, 1.f, 1.f);

    m_AttackInfo[WEAPON_SWORD][ATTACK_HEAVY].particle.key      = L"HitEffect_Star";
    m_AttackInfo[WEAPON_SWORD][ATTACK_HEAVY].particle.frames   = 14;
    m_AttackInfo[WEAPON_SWORD][ATTACK_HEAVY].particle.interval = 0.03f;
    m_AttackInfo[WEAPON_SWORD][ATTACK_HEAVY].particle.scale    = _vec3(1.5f, 1.5f, 1.5f);

    // 1타
    m_AttackInfo[WEAPON_SWORD][ATTACK_COMBO1].state            = ACTORSTATE::PLAYER_SWORD_COMBO1;
    m_AttackInfo[WEAPON_SWORD][ATTACK_COMBO1].duration         = 0.02f * 31;
    m_AttackInfo[WEAPON_SWORD][ATTACK_COMBO1].hitboxStartTime  = 0.02f;
    m_AttackInfo[WEAPON_SWORD][ATTACK_COMBO1].hitboxEndTime    = 0.02f * 8;
    m_AttackInfo[WEAPON_SWORD][ATTACK_COMBO1].comboInputTime   = 0.3f;
    m_AttackInfo[WEAPON_SWORD][ATTACK_COMBO1].damageMultiplier = 1.f;
    m_AttackInfo[WEAPON_SWORD][ATTACK_COMBO1].knockbackPower   = 30.f;
    m_AttackInfo[WEAPON_SWORD][ATTACK_COMBO1].spawnOffset      = 1.2f; // 가까움
    m_AttackInfo[WEAPON_SWORD][ATTACK_COMBO1].hitboxScale      = _vec3(0.85f, 0.85f, 0.85f);

    m_AttackInfo[WEAPON_SWORD][ATTACK_COMBO1].particle.key      = L"HitEffect_Star";
    m_AttackInfo[WEAPON_SWORD][ATTACK_COMBO1].particle.frames   = 14;
    m_AttackInfo[WEAPON_SWORD][ATTACK_COMBO1].particle.interval = 0.03f;
    m_AttackInfo[WEAPON_SWORD][ATTACK_COMBO1].particle.scale    = _vec3(1.5f, 1.5f, 1.5f);

    // 인수 : HitStop은 일단 각 무기 1타에만 넣어놓을게요
    m_AttackInfo[WEAPON_SWORD][ATTACK_COMBO1].hitStopDuration = 0.01f;   // 0.01초 동안
    m_AttackInfo[WEAPON_SWORD][ATTACK_COMBO1].hitStopScale    = 0.3f;       // 30%로 슬로우

    // 2타
    m_AttackInfo[WEAPON_SWORD][ATTACK_COMBO2].state            = ACTORSTATE::PLAYER_SWORD_COMBO2;
    m_AttackInfo[WEAPON_SWORD][ATTACK_COMBO2].duration         = 0.02f * 38;
    m_AttackInfo[WEAPON_SWORD][ATTACK_COMBO2].hitboxStartTime  = 0.02f;
    m_AttackInfo[WEAPON_SWORD][ATTACK_COMBO2].hitboxEndTime    = 0.02f * 8;
    m_AttackInfo[WEAPON_SWORD][ATTACK_COMBO2].comboInputTime   = 0.3f;
    m_AttackInfo[WEAPON_SWORD][ATTACK_COMBO2].damageMultiplier = 1.f;
    m_AttackInfo[WEAPON_SWORD][ATTACK_COMBO2].knockbackPower   = 30.f;
    m_AttackInfo[WEAPON_SWORD][ATTACK_COMBO2].spawnOffset      = 1.2f;
    m_AttackInfo[WEAPON_SWORD][ATTACK_COMBO2].hitboxScale      = _vec3(0.85f, 0.85f, 0.85f);

    m_AttackInfo[WEAPON_SWORD][ATTACK_COMBO2].particle.key      = L"HitEffect_Star";
    m_AttackInfo[WEAPON_SWORD][ATTACK_COMBO2].particle.frames   = 14;
    m_AttackInfo[WEAPON_SWORD][ATTACK_COMBO2].particle.interval = 0.03f;
    m_AttackInfo[WEAPON_SWORD][ATTACK_COMBO2].particle.scale    = _vec3(1.5f, 1.5f, 1.5f);

    // 3타
    m_AttackInfo[WEAPON_SWORD][ATTACK_COMBO3].state            = ACTORSTATE::PLAYER_SWORD_COMBO3;
    m_AttackInfo[WEAPON_SWORD][ATTACK_COMBO3].duration         = 0.02f * 38;
    m_AttackInfo[WEAPON_SWORD][ATTACK_COMBO3].hitboxStartTime  = 0.02f * 4;
    m_AttackInfo[WEAPON_SWORD][ATTACK_COMBO3].hitboxEndTime    = 0.02f * 11;
    m_AttackInfo[WEAPON_SWORD][ATTACK_COMBO3].comboInputTime   = 0.3f;
    m_AttackInfo[WEAPON_SWORD][ATTACK_COMBO3].damageMultiplier = 1.f;
    m_AttackInfo[WEAPON_SWORD][ATTACK_COMBO3].knockbackPower   = 30.f;
    m_AttackInfo[WEAPON_SWORD][ATTACK_COMBO3].spawnOffset      = 1.2f;
    m_AttackInfo[WEAPON_SWORD][ATTACK_COMBO3].hitboxScale      = _vec3(0.85f, 0.85f, 0.85f);

    m_AttackInfo[WEAPON_SWORD][ATTACK_COMBO3].particle.key      = L"HitEffect_Star";
    m_AttackInfo[WEAPON_SWORD][ATTACK_COMBO3].particle.frames   = 14;
    m_AttackInfo[WEAPON_SWORD][ATTACK_COMBO3].particle.interval = 0.03f;
    m_AttackInfo[WEAPON_SWORD][ATTACK_COMBO3].particle.scale    = _vec3(1.5f, 1.5f, 1.5f);

    // =========================================================
    // [2] 건틀릿 (GAUNTLET) 데이터 설정
    // =========================================================

    //강공격
    m_AttackInfo[WEAPON_GAUNTLET][ATTACK_HEAVY].state            = ACTORSTATE::PLAYER_GAUNTLET_HEAVY;
    m_AttackInfo[WEAPON_GAUNTLET][ATTACK_HEAVY].duration         = 0.02f * 70;
    m_AttackInfo[WEAPON_GAUNTLET][ATTACK_HEAVY].hitboxStartTime  = 0.4f;
    m_AttackInfo[WEAPON_GAUNTLET][ATTACK_HEAVY].hitboxEndTime    = 0.6f;
    m_AttackInfo[WEAPON_GAUNTLET][ATTACK_HEAVY].comboInputTime   = 0.4f;
    m_AttackInfo[WEAPON_GAUNTLET][ATTACK_HEAVY].damageMultiplier = 5.f;
    m_AttackInfo[WEAPON_GAUNTLET][ATTACK_HEAVY].knockbackPower   = 50.f;
    m_AttackInfo[WEAPON_GAUNTLET][ATTACK_HEAVY].spawnOffset      = 1.2f;
    m_AttackInfo[WEAPON_GAUNTLET][ATTACK_HEAVY].hitboxScale      = _vec3(0.85f, 0.85f, 0.85f);

    m_AttackInfo[WEAPON_GAUNTLET][ATTACK_HEAVY].particle.key      = L"HitEffect_Star";
    m_AttackInfo[WEAPON_GAUNTLET][ATTACK_HEAVY].particle.frames   = 14;
    m_AttackInfo[WEAPON_GAUNTLET][ATTACK_HEAVY].particle.interval = 0.03f;
    m_AttackInfo[WEAPON_GAUNTLET][ATTACK_HEAVY].particle.scale    = _vec3(1.5f, 1.5f, 1.5f);

    //1타
    m_AttackInfo[WEAPON_GAUNTLET][ATTACK_COMBO1].state            = ACTORSTATE::PLAYER_GAUNTLET_COMBO1;
    m_AttackInfo[WEAPON_GAUNTLET][ATTACK_COMBO1].duration         = 0.02f * 41;
    m_AttackInfo[WEAPON_GAUNTLET][ATTACK_COMBO1].hitboxStartTime  = 0.02f * 5;
    m_AttackInfo[WEAPON_GAUNTLET][ATTACK_COMBO1].hitboxEndTime    = 0.02f * 12;
    m_AttackInfo[WEAPON_GAUNTLET][ATTACK_COMBO1].comboInputTime   = 0.4f;
    m_AttackInfo[WEAPON_GAUNTLET][ATTACK_COMBO1].damageMultiplier = 2.f;
    m_AttackInfo[WEAPON_GAUNTLET][ATTACK_COMBO1].knockbackPower   = 30.f;
    m_AttackInfo[WEAPON_GAUNTLET][ATTACK_COMBO1].spawnOffset      = 1.2f;
    m_AttackInfo[WEAPON_GAUNTLET][ATTACK_COMBO1].hitboxScale      = _vec3(0.85f, 0.85f, 0.85f);

    m_AttackInfo[WEAPON_GAUNTLET][ATTACK_COMBO1].particle.key      = L"HitEffect_Star";
    m_AttackInfo[WEAPON_GAUNTLET][ATTACK_COMBO1].particle.frames   = 14;
    m_AttackInfo[WEAPON_GAUNTLET][ATTACK_COMBO1].particle.interval = 0.03f;
    m_AttackInfo[WEAPON_GAUNTLET][ATTACK_COMBO1].particle.scale    = _vec3(1.5f, 1.5f, 1.5f);

    m_AttackInfo[WEAPON_GAUNTLET][ATTACK_COMBO1].hitStopDuration = 0.01f;
    m_AttackInfo[WEAPON_GAUNTLET][ATTACK_COMBO1].hitStopScale    = 0.3f;

    //2타
    m_AttackInfo[WEAPON_GAUNTLET][ATTACK_COMBO2].state            = ACTORSTATE::PLAYER_GAUNTLET_COMBO2;
    m_AttackInfo[WEAPON_GAUNTLET][ATTACK_COMBO2].duration         = 0.02f * 43;
    m_AttackInfo[WEAPON_GAUNTLET][ATTACK_COMBO2].hitboxStartTime  = 0.02f * 12;
    m_AttackInfo[WEAPON_GAUNTLET][ATTACK_COMBO2].hitboxEndTime    = 0.02f * 24;
    m_AttackInfo[WEAPON_GAUNTLET][ATTACK_COMBO2].comboInputTime   = 0.4f;
    m_AttackInfo[WEAPON_GAUNTLET][ATTACK_COMBO2].damageMultiplier = 2.f;
    m_AttackInfo[WEAPON_GAUNTLET][ATTACK_COMBO2].knockbackPower   = 30.f;
    m_AttackInfo[WEAPON_GAUNTLET][ATTACK_COMBO2].spawnOffset      = 1.2f;
    m_AttackInfo[WEAPON_GAUNTLET][ATTACK_COMBO2].hitboxScale      = _vec3(0.85f, 0.85f, 0.85f);

    m_AttackInfo[WEAPON_GAUNTLET][ATTACK_COMBO2].particle.key      = L"HitEffect_Star";
    m_AttackInfo[WEAPON_GAUNTLET][ATTACK_COMBO2].particle.frames   = 14;
    m_AttackInfo[WEAPON_GAUNTLET][ATTACK_COMBO2].particle.interval = 0.03f;
    m_AttackInfo[WEAPON_GAUNTLET][ATTACK_COMBO2].particle.scale    = _vec3(1.5f, 1.5f, 1.5f);

    //3타
    m_AttackInfo[WEAPON_GAUNTLET][ATTACK_COMBO3].state            = ACTORSTATE::PLAYER_GAUNTLET_COMBO3;
    m_AttackInfo[WEAPON_GAUNTLET][ATTACK_COMBO3].duration         = 0.02f * 46;
    m_AttackInfo[WEAPON_GAUNTLET][ATTACK_COMBO3].hitboxStartTime  = 0.02f * 20;
    m_AttackInfo[WEAPON_GAUNTLET][ATTACK_COMBO3].hitboxEndTime    = 0.02f * 27;
    m_AttackInfo[WEAPON_GAUNTLET][ATTACK_COMBO3].comboInputTime   = 0.4f;
    m_AttackInfo[WEAPON_GAUNTLET][ATTACK_COMBO3].damageMultiplier = 4.f;
    m_AttackInfo[WEAPON_GAUNTLET][ATTACK_COMBO3].knockbackPower   = 50.f;
    m_AttackInfo[WEAPON_GAUNTLET][ATTACK_COMBO3].spawnOffset      = 1.2f;
    m_AttackInfo[WEAPON_GAUNTLET][ATTACK_COMBO3].hitboxScale      = _vec3(0.85f, 0.85f, 0.85f);

    m_AttackInfo[WEAPON_GAUNTLET][ATTACK_COMBO3].particle.key      = L"HitEffect_Star";
    m_AttackInfo[WEAPON_GAUNTLET][ATTACK_COMBO3].particle.frames   = 14;
    m_AttackInfo[WEAPON_GAUNTLET][ATTACK_COMBO3].particle.interval = 0.03f;
    m_AttackInfo[WEAPON_GAUNTLET][ATTACK_COMBO3].particle.scale    = _vec3(1.5f, 1.5f, 1.5f);

    return S_OK;
}

_int CPlayerAttack::Update_Component(const _float& timeDelta)
{
    // 매 프레임 작업 없음. 스태틱 컴포넌트로 사용할 것
    return 0;
}

void CPlayerAttack::LateUpdate_Component()
{}

HRESULT CPlayerAttack::Init_OwnerComponent()
{
    m_OwnerStateCom = dynamic_cast<CState*>(
        m_Owner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::STATE));

    m_OwnerCombatStatCom = dynamic_cast<CCombatStat*>(
        m_Owner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::COMBATSTAT));

    m_OwnerWeaponCom = dynamic_cast<CWeaponEquip*>(
        m_Owner->Get_Component(ID_STATIC, COMPONENTTYPE::WEAPON_EQUIP));

    m_OwnerInputCom = dynamic_cast<CPlayerInput*>(
        m_Owner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::PL_INPUT));

    m_OwnerMovementCom = dynamic_cast<CPlayerMovement*>(
        m_Owner->Get_Component(ID_STATIC, COMPONENTTYPE::PL_MOVEMENT));

    HRESULT result = S_OK;

    if (nullptr == m_OwnerStateCom)
    {
        MessageBoxW(nullptr,
                    L"CPlayerAttack: Owner State Not Found",
                    L"System Message",
                    MB_ICONERROR);

        result = E_FAIL;
    }

    if (nullptr == m_OwnerCombatStatCom)
    {
        MessageBoxW(nullptr,
                    L"CPlayerAttack: Owner CombatStat Not Found",
                    L"System Message",
                    MB_ICONERROR);

        result = E_FAIL;
    }

    if (nullptr == m_OwnerWeaponCom)
    {
        MessageBoxW(nullptr,
                    L"CPlayerAttack: Owner WeaponEquip Not Found",
                    L"System Message",
                    MB_ICONERROR);

        result = E_FAIL;
    }

    if (nullptr == m_OwnerInputCom)
    {
        MessageBoxW(nullptr,
                    L"CPlayerAttack: Owner PlayerInput Not Found",
                    L"System Message",
                    MB_ICONERROR);

        result = E_FAIL;
    }

    if (nullptr == m_OwnerMovementCom)
    {
        MessageBoxW(nullptr,
                    L"CPlayerAttack: Owner PlayerMovement Not Found",
                    L"System Message",
                    MB_ICONERROR);

        result = E_FAIL;
    }

    return result;
}

ACTORSTATE CPlayerAttack::Start_NormalAttack()
{
    if (!m_OwnerWeaponCom || !m_OwnerWeaponCom->Get_WeaponEquipped()) return ACTORSTATE::STATE_END;

    Reset_AttackData();

    // 무기가 어떤건지 CWeaponEquip의 ID 확인
    const _int weaponIdx = Get_WeaponIndex(m_OwnerWeaponCom->Get_CurrentWeaponID());
    if (weaponIdx < 0) return ACTORSTATE::STATE_END;

    // 마우스 위치 X값이 플레이어보다 작거나 같으면 LEFT, 크면 RIGHT로 설정
    const _vec3& vMouseDir = m_OwnerInputCom->Get_MouseDir();
    if (vMouseDir.x <= 0.f)
        m_OwnerStateCom->Change_Dir(ACTORDIR::LEFT);
    else
        m_OwnerStateCom->Change_Dir(ACTORDIR::RIGHT);

    // 1타(COMBO_1) 시작
    m_ComboIndex = static_cast<_int>(ATTACK_COMBO1);

    // 일단 마찰 줘서 정지
    m_OwnerMovementCom->Stop();

    // 마우스 방향으로 도약
    m_OwnerMovementCom->ImpulseMove(m_OwnerInputCom->Get_MouseDir(), 450.f);

    // Controller에서 상태를 변경하도록 상태값 반환
    return m_AttackInfo[weaponIdx][ATTACK_COMBO1].state;
}

void CPlayerAttack::Try_NextCombo()
{
    if (m_CanNextCombo)
        m_IsNextComboReserved = true;
}

_bool CPlayerAttack::Update_NormalAttack(const _float& timeDelta)
{
    if (!m_OwnerWeaponCom || !m_OwnerCombatStatCom || !m_OwnerInputCom) return false;

    // 무기가 어떤건지 CWeaponEquip의 ID 확인
    const _int weaponIdx = Get_WeaponIndex(m_OwnerWeaponCom->Get_CurrentWeaponID());
    if (weaponIdx < 0) return false;

    // 콤보 인덱스 유효성 체크
    if (m_ComboIndex < ATTACK_COMBO1 || m_ComboIndex > m_MaxCombo) return false;

    m_Timer += timeDelta;

    const AttackInfo& info = m_AttackInfo[weaponIdx][m_ComboIndex];

    _float atkSpeed = m_OwnerCombatStatCom->Get_AttackSpeed();
    if (atkSpeed < 0.1f) atkSpeed = 0.1f;

    // 공격 오브젝트 제어
    if (m_Timer >= info.hitboxStartTime / atkSpeed &&
        m_Timer <= info.hitboxEndTime / atkSpeed)
    {
        if (!m_IsHitboxActive)
        {
            m_IsHitboxActive = true;

            //!! 검 기본공격의 경우 좌우만 판단해서 히트박스를 만들면 되기에 mouseDir의 x성분만 가져와 사용
            _vec3 mouseDir = m_OwnerInputCom->Get_MouseDir();
            mouseDir.z     = 0.f;
            D3DXVec3Normalize(&mouseDir, &mouseDir);

            Create_AttackObj(info, mouseDir);
        }
    }
    else
    {
        if (m_IsHitboxActive)
            m_IsHitboxActive = false;
    }

    // 선입력 허용
    if (m_Timer >= info.comboInputTime / atkSpeed && m_ComboIndex < m_MaxCombo)
        m_CanNextCombo = true;

    // 다음 콤보
    if (m_Timer >= info.comboInputTime && m_IsNextComboReserved)
    {
        ++m_ComboIndex;
        m_Timer               = 0.f;
        m_IsHitboxActive      = false;
        m_CanNextCombo        = false;
        m_IsNextComboReserved = false;

        // 마우스 위치 X값이 플레이어보다 작거나 같으면 LEFT, 크면 RIGHT로 설정
        const _vec3& vMouseDir = m_OwnerInputCom->Get_MouseDir();
        if (vMouseDir.x <= 0.f)
            m_OwnerStateCom->Change_Dir(ACTORDIR::LEFT);
        else
            m_OwnerStateCom->Change_Dir(ACTORDIR::RIGHT);

        // 일단 마찰 줘서 정지
        m_OwnerMovementCom->Stop();

        // 마우스 방향으로 도약
        if (weaponIdx == 1 && m_ComboIndex == 3)
            m_OwnerMovementCom->ImpulseMove(m_OwnerInputCom->Get_MouseDir(), 600.f);
        else
            m_OwnerMovementCom->ImpulseMove(m_OwnerInputCom->Get_MouseDir(), 450.f);

        m_OwnerStateCom->Change_State(m_AttackInfo[weaponIdx][m_ComboIndex].state);

        return true;
    }

    // 종료
    if (m_Timer >= info.duration / atkSpeed)
    {
        Reset_AttackData();
        return false;
    }

    return true;
}

void CPlayerAttack::Start_HeavyAttack()
{
    if (!m_OwnerWeaponCom || !m_OwnerWeaponCom->Get_WeaponEquipped()) return;

    Reset_AttackData();

    const _int weaponIdx = Get_WeaponIndex(m_OwnerWeaponCom->Get_CurrentWeaponID());
    if (weaponIdx < 0) return;

    m_ComboIndex = ATTACK_HEAVY;
    m_OwnerStateCom->Change_State(m_AttackInfo[weaponIdx][ATTACK_HEAVY].state);
}

_bool CPlayerAttack::Update_HeavyAttack(const _float& timeDelta)
{
    if (!m_OwnerWeaponCom || !m_OwnerCombatStatCom || !m_OwnerInputCom) return false;

    // 마우스 위치 X값이 플레이어보다 작거나 같으면 LEFT, 크면 RIGHT로 설정
    const _vec3& vMouseDir = m_OwnerInputCom->Get_MouseDir();
    if (vMouseDir.x <= 0.f)
        m_OwnerStateCom->Change_Dir(ACTORDIR::LEFT);
    else
        m_OwnerStateCom->Change_Dir(ACTORDIR::RIGHT);

    const _int weaponIdx = Get_WeaponIndex(m_OwnerWeaponCom->Get_CurrentWeaponID());
    if (weaponIdx < 0) return false;

    m_Timer += timeDelta;

    const AttackInfo& info = m_AttackInfo[weaponIdx][ATTACK_HEAVY];

    _float atkSpeed = m_OwnerCombatStatCom->Get_AttackSpeed();
    if (atkSpeed < 0.1f) atkSpeed = 0.1f;

    // 공격 오브젝트 제어
    if (m_Timer >= info.hitboxStartTime / atkSpeed &&
        m_Timer <= info.hitboxEndTime / atkSpeed)
    {
        if (!m_IsHitboxActive)
        {
            m_IsHitboxActive = true;
            Create_AttackObj(info, m_OwnerInputCom->Get_MouseDir());
        }
    }
    else
    {
        if (m_IsHitboxActive)
            m_IsHitboxActive = false;
    }

    // 종료 체크
    if (m_Timer >= info.duration / atkSpeed)
    {
        Reset_AttackData();
        return false;
    }

    return true;
}

void CPlayerAttack::Cancel_Attack()
{
    Reset_AttackData();
}

_int CPlayerAttack::Get_WeaponIndex(_int weaponID)
{
    switch (weaponID)
    {
    case 3001: return WEAPON_SWORD;
    case 3002: return WEAPON_GAUNTLET;
    default: return -1;
    }
}

void CPlayerAttack::Reset_AttackData()
{
    m_Timer = 0.f;

    if (m_IsHitboxActive)
    {
        m_IsHitboxActive = false;

        // 공격 오브젝트 수명이 아직 남아있으면, 강제로 히트박스를 소멸
        // 소멸 시, Create_Hitbox() 에서 등록해놓은 콜백 함수가 자동으로 실행되어
        // m_CurrentHitbox = nullptr로 안전하게 초기화함
        if (m_CurrentAttackObj)
        {
            m_CurrentAttackObj->Mark_As_PendingKill();
            m_CurrentAttackObj = nullptr;
        }
    }

    m_CanNextCombo        = false;
    m_IsNextComboReserved = false;
}

void CPlayerAttack::Create_AttackObj(const AttackInfo& attackInfo, const _vec3& vAttackDir)
{
    if (!m_OwnerCombatStatCom) return;

    CAttackBuilder(m_GraphicDev, m_Owner)
        // 박스 콜라이더 사용
        .Box()
        //!! 실제 공격 오브젝트를 여기서 직접 관리하고 싶어서 가져옴. 다른 곳에선 안쓸거면 필요 없음
        .Track(m_CurrentAttackObj)
        // 데미지 설정
        .Damage(m_OwnerCombatStatCom->Get_Attack() * attackInfo.damageMultiplier)
        // 넉백 설정
        .Knockback(attackInfo.knockbackPower)
        // 지속시간 설정
        .LifeTime(attackInfo.hitboxEndTime - attackInfo.hitboxStartTime)
        // 공격 오브젝트 크기 설정
        .Scale(attackInfo.hitboxScale)
        // 공격하는 쪽을 기준으로 위치를 지정하겠다
        // 반대되는 기능으로 절대 위치 기준을 정하는 Position(_vec3)가 있다
        .InstigatorPos()
        // 공격자로부터 얼마나 떨어져 있는지 설정
        .Offset(vAttackDir * attackInfo.spawnOffset)
        // 콜라이더 충돌 시 바로 파괴되어야 하는지 설정
        .DestroyOnHit(false)
        // 파티클 설정
        .Particle(attackInfo.particle.key,
                  attackInfo.particle.frames,
                  attackInfo.particle.interval,
                  attackInfo.particle.scale,
                  attackInfo.particle.offset)
        .HitStop(attackInfo.hitStopDuration, attackInfo.hitStopScale)
        // 모든 설정을 마치고 생성
        .Spawn();

    // 현재 attackBox가 죽을 때, 이쪽의 m_CurrentHitbox가 아직 현재 attackBox를 가리키고 있으면
    // 이를 nullptr로 안전하게 초기화하는 람다함수를 등록
    if (m_CurrentAttackObj)
    {
        CAttackBase* pBox = m_CurrentAttackObj;
        m_CurrentAttackObj->Set_OnDestroyCallback([this, pBox]()
        {
            if (m_CurrentAttackObj == pBox)
                m_CurrentAttackObj = nullptr;
        });
    }

    // 공격 사운드를 히트박스의 생성과 동시에 재생
    Play_AttackSound();
}

void CPlayerAttack::Play_AttackSound()
{
    switch (m_ComboIndex)
    {
    case 3:
        CSoundMgr::GetInstance()->Play_Once(
            L"Player/Player_Attack_3.wav",
            SOUND_EFFECT,
            1.f);
        break;
    default:
        CSoundMgr::GetInstance()->Play_Once(
            L"Player/Player_Attack_0.wav",
            SOUND_EFFECT,
            1.f);
        break;
    }
}

CPlayerAttack* CPlayerAttack::Create(DEVICE graphicDev)
{
    auto playerAttack = new CPlayerAttack(graphicDev);

    if (FAILED(playerAttack->Ready_PlayerCombat()))
    {
        Safe_Release(playerAttack);
        MSG_BOX("PlayerCombat Create Failed");
        return nullptr;
    }

    return playerAttack;
}

CComponent* CPlayerAttack::Clone()
{
    return new CPlayerAttack(*this);
}

void CPlayerAttack::Free()
{
    m_OwnerStateCom      = nullptr;
    m_OwnerCombatStatCom = nullptr;
    m_OwnerWeaponCom     = nullptr;
    m_OwnerInputCom      = nullptr;
    m_OwnerMovementCom   = nullptr;

    // CPlayerAttack이 현재 히트박스보다 먼저 죽는 경우
    // Set_OnDestroyCallback이 아무것도 실행하지 말도록 nullptr로 콜백할 함수를 지움
    if (m_CurrentAttackObj)
    {
        m_CurrentAttackObj->Set_OnDestroyCallback(nullptr);
        m_CurrentAttackObj = nullptr;
    }

    CComponent::Free();
}
