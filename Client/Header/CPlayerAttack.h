#pragma once
#include "CComponent.h"

BEGIN(Engine)
class CAttackBase;
class CCombatStat;
class CState;
END

class CPlayerMovement;
class CPlayerInput;
class CWeaponEquip;

/// @brief 플레이어의 전투 및 공격 로직을 담당하는 컴포넌트
/// @details
/// 무기 사용, 콤보 시스템, 공격 판정 생성 등 전투와 관련된 모든 기능을 수행합니다.
/// - 공격 실행: 일반 공격(콤보) 및 강공격을 시작하고 관리합니다.
/// - 콤보 시스템: 타이밍에 맞는 입력으로 이어지는 연속 공격 로직을 처리합니다.
/// - 히트박스 관리: 공격 애니메이션의 특정 프레임에 맞춰 공격 판정(AttackObj)을 생성하고 제거합니다.
/// - 데이터 관리: 각 무기별 공격 속도, 데미지, 범위 등의 데이터를 관리합니다.
/// @note
/// CPlayerController의 명령에 따라 공격을 수행하며, 내부적으로 CAttackBase 객체를 생성하여 실제 타격 판정을 처리합니다.
class CPlayerAttack : public CComponent
{
public:
    // 무기 공격 정보 구조체
    struct AttackInfo
    {
        ACTORSTATE  state;           // 해당 공격의 애니메이션 상태

        _float      duration;            // 공격 지속 시간
        _float      hitboxStartTime;     // 히트박스가 켜지는 시간
        _float      hitboxEndTime;       // 히트박스가 꺼지는 시간
        _float      comboInputTime;      // 다음 콤보 선입력 허용 시작 시간
        _float      damageMultiplier;    //TODO 공격력 배율. 나중에 아이템 정보와 연동하기
        _float      knockbackPower;      // 넉백 힘
        _float      knockbackTimer;      // 넉백 밀려나는 시간
        _float      spawnOffset;         // 생성 거리 오프셋 (얼마나 앞에 생성되는지)
        _vec3       hitboxScale;         // 공격 오브젝트 크기 (예: 1, 1, 1)

        PARTICLEDATA particle;
        _float      hitStopDuration = 0.f;
        _float      hitStopScale = 0.f;
    };

    // [AttackInfo 관리용] 무기 종류 enum
    enum PL_WEAPON_INDEX
    {
        WEAPON_SWORD,       // 무기 : 검
        WEAPON_GAUNTLET,    // 무기 : 건틀릿

        WEPON_CRAFTING,      // 곡괭이, 도끼 , 체집시 사용할 공격

        WEAPON_MAX
    };

    // [AttackInfo 관리용] 공격 종류 enum
    enum PL_ATTACK_INDEX
    {
        ATTACK_HEAVY,       // 강공격
        ATTACK_COMBO1,      // 일반 1타
        ATTACK_COMBO2,      // 일반 2타
        ATTACK_COMBO3,      // 일반 3타

        ATTACK_MAX
    };

private:
    explicit CPlayerAttack(DEVICE graphicDev);
    explicit CPlayerAttack(const CPlayerAttack& rhs);
    ~CPlayerAttack() override;

public:
    HRESULT Ready_PlayerCombat();
    _int    Update_Component(const _float& timeDelta) override;
    void    LateUpdate_Component() override;

public:  // --- 초기 세팅 ---
    HRESULT Init_OwnerComponent(); // Owner의 컴포넌트를 가져와 사용하기 위한 초기 세팅 함수

public:  // --- Getter ---
    // 에디터 등 외부에서 바로 수정 가능하도록 Reference로 반환
    AttackInfo& Get_AttackInfo_Ref(PL_WEAPON_INDEX weaponIdx, PL_ATTACK_INDEX attackIdx) { return m_AttackInfo[weaponIdx][attackIdx]; }

    _int   Get_ComboIndex() const { return m_ComboIndex; }
    _float Get_CurrentTimer() const { return m_Timer; }
    _bool  Get_IsHitboxActive() const { return m_IsHitboxActive; }
    _bool  Get_CanNextCombo() const { return m_CanNextCombo; }
    _bool  Get_IsNextComboReserved() const { return m_IsNextComboReserved; }

public:  // --- Setter ---
    void Set_AttackInfo(PL_WEAPON_INDEX weaponIdx, PL_ATTACK_INDEX attackIdx, const AttackInfo& info) { m_AttackInfo[weaponIdx][attackIdx] = info; }
    void Set_ComboIndex(_int comboIndex) { m_ComboIndex = comboIndex; }
    void Set_CurrentTimer(_float timer) { m_Timer = timer; }
    void Set_IsHitboxActive(_bool isActive) { m_IsHitboxActive = isActive; }
    void Set_CanNextCombo(_bool canNext) { m_CanNextCombo = canNext; }
    void Set_IsNextComboReserved(_bool isReserved) { m_IsNextComboReserved = isReserved; }

public:  // --- 동작 명령 함수 (Controller가 호출) ---

    ACTORSTATE Start_NormalAttack();                         // 일반공격 시작
    void       Try_NextCombo();                              // 일반공격 콤보 예약 (일반공격 중 클릭시 호출)
    _bool      Update_NormalAttack(const _float& timeDelta); // 일반공격 중 매 프레임 호출. false면 종료

    void    Start_HeavyAttack();                          // 강공격 시작
    _bool   Update_HeavyAttack(const _float& timeDelta);  // 강공격 중 매 프레임 호출. false면 종료

    void Cancel_Attack(); // 공격이 강제 취소될 때 직접 호출

private: // --- 내부 로직 ---
    _int Get_WeaponIndex(_int weaponID);
    void Reset_AttackData();

    /// @brief 공격 정보 구조체와 계산된 방향을 받아 공격 오브젝트 생성
    /// @param attackInfo 현재 공격의 설정 데이터 (데미지 배율, 크기, 오프셋 등)
    /// @param vAttackDir 계산된 공격 방향 벡터
    void Create_AttackObj(const AttackInfo& attackInfo, const _vec3& vAttackDir);

    void Play_AttackSound(); // 공격 사운드 재생

private: // --- Owner Component 캐싱 ---
    Engine::CState*      m_OwnerStateCom;
    Engine::CCombatStat* m_OwnerCombatStatCom;
    CWeaponEquip*        m_OwnerWeaponCom;
    CPlayerInput*        m_OwnerInputCom;
    CPlayerMovement*     m_OwnerMovementCom;

private: // --- 다른 오브젝트 캐싱 ---
    Engine::CAttackBase* m_CurrentAttackObj; // 현재 활성화된 공격 오브젝트

private: // --- 멤버 변수 ---
    // 무기 공격 데이터 저장소
    AttackInfo m_AttackInfo[WEAPON_MAX][ATTACK_MAX];

    _int   m_ComboIndex;            // 현재 PL_ATTACK_INDEX를 int로 저장
    _int   m_MaxCombo;              // 일반공격 콤보 최대값 설정
    _float m_Timer;                 // 공격 시간 타이머

    _bool m_IsHitboxActive;         // 현재 히트박스가 켜져 있는가 여부
    _bool m_CanNextCombo;           // 다음 콤보 선입력 허용 여부
    _bool m_IsNextComboReserved;    // 다음 콤보가 예약되었는지 여부

public:
    static CPlayerAttack* Create(DEVICE graphicDev);
    CComponent*           Clone() override;

private:
    void Free() override;
};
