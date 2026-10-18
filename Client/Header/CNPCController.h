#pragma once
#include "CComponent.h"
#include "CNPCStateMachine.h"

BEGIN(Engine)
class CCombatStat;
class CAnimator;
class CTransform;
END

/// @brief NPC의 AI 판단, 작업(Work), 목표 지점, 애니메이션 설정을 총괄하는 컨트롤러 컴포넌트
/// @details
/// [주요 기능]
/// 1. 작업(Work) 관리
///    - NPC_WORK 열거체를 통해 NONE, CHOPWOOD(벌목), MINING(채광), PRAY(기도) 등의 작업을 부여할 수 있다.
///    - Set_Work / Get_Work를 통해 현재 부여된 임무를 설정 및 확인한다.
/// 
/// 2. 목표 지점 및 이동 관리
///    - Set_TargetPos를 통해 NPC가 이동해야 할 목적지 좌표를 설정한다.
///    - 이는 상태 머신에서 참조하여 이동 로직을 수행하는 데 사용된다.
/// 
/// 3. 상호작용 대상 관리
///    - Set_TargetObject를 통해 나무, 바위, 건물 등 상호작용할 대상 오브젝트를 지정한다.
///    - 벌목, 채광, 건축 등 특정 대상이 필요한 상태에서 이 오브젝트를 참조한다.
/// 
/// 4. 애니메이션 리소스 관리
///    - Set_AnimKeyPrefix를 통해 애니메이션 키를 설정한다.
///    - 이를 통해 동일한 FSM 로직을 공유하면서도 다른 외형과 애니메이션을 출력할 수 있다.
/// 
/// 5. 상태 머신 연동
///    - Get_CurNPCState를 통해 현재 NPCStateMachine의 상태를 제공한다.
class CNPCController : public CComponent
{
public:
    // NPC가 할 기본 일
    enum class NPC_WORK
    {
        NONE,
        CHOPWOOD,       // 벌목
        MINING,         // 채광
        PRAY            // 기도
    };

    // NPC가 다가갈 타겟
    enum class NPC_TARGET_TYPE
    {
        NONE,
        CONSTRUCTION,   // 건축
        FOOD,           // 음식
        TREE,           // 나무
        ROCK,           // 돌
        PRAY_SITE       // 기도 장소
    };

private:
    explicit CNPCController(DEVICE graphicDev);
    explicit CNPCController(const CNPCController& rhs);
    ~CNPCController() override;

public:
    HRESULT Ready_Controller();
    void    Initialize_Component() override;
    _int    Update_Component(const _float& timeDelta) override;

public:  // --- Owner 컴포넌트 Getter ---
    CTransform*  Get_OwnerTransform();
    CAnimator*   Get_OwnerAnimator();
    CCombatStat* Get_OwnerCombatStat();

public:  // --- Getter / Setter ---
    CStateMachine* Get_StateMachine() { return m_StateMachine; }

    // MPC용 FSM의 상태 반환
    CNPCStateMachine::NPC_STATE Get_CurNPCState();

    NPC_WORK        Get_Work() const { return m_Work; }                     // NPC_WORK 중 할 작업 반환
    NPC_TARGET_TYPE Get_TargetType() const { return m_TargetType; }         // 다가갈 타겟 종류 반환

    const wstring& Get_AnimKeyPrefix() const { return m_AnimKeyPrefix; }

    CGameObject* Get_TargetObject() { return m_TargetObject; }              // 타겟 오브젝트 반환
    const _vec3& Get_TargetPos() const { return m_TargetPos; }              // 타겟 위치 반환
    const _vec3& Get_PrayTargetPos() const { return m_PrayTargetPos; }      // 기도 위치 반환

    void Set_StateMachine(CStateMachine* stateMachine);

    void Set_Work(NPC_WORK work) { m_Work = work; }                         // NPC_WORK 중 할 작업 지정
    void Set_TargetType(NPC_TARGET_TYPE type) { m_TargetType = type; }      // 다가갈 타겟 설정

    void Set_AnimKeyPrefix(const wstring& prefix) { m_AnimKeyPrefix = prefix; }

    void Set_TargetObject(CGameObject* obj) { m_TargetObject = obj; }       // 타겟 오브젝트 설정
    void Set_TargetPos(const _vec3& pos) { m_TargetPos = pos; }             // 타겟 위치 설정
    void Set_PrayTargetPos(const _vec3& pos) { m_PrayTargetPos = pos; }     // 기도 위치 설정

    // 음식 탐색 쿨다운 관리
    _bool Is_FoodSearchAvailable() const { return m_FoodSearchCooldown <= 0.f; }
    void  Reset_FoodSearchCooldown() { m_FoodSearchCooldown = 20.f; }

private:
    void  Cache_Components();
    _uint Get_CurrentState(); // 현재 내부에서만 사용중

public:
    static CNPCController* Create(DEVICE graphicDev);
    CComponent*            Clone() override;

private:
    void Free() override;

private: // --- 소유하고 있는 상태 머신 관리 클래스---
    CStateMachine* m_StateMachine;

private: // --- Owner 컴포넌트 캐싱 ---
    CAnimator*   m_AnimatorCom;
    CCombatStat* m_CombatStatCom;

private: // --- 멤버 변수 ---
    NPC_WORK        m_Work;
    NPC_TARGET_TYPE m_TargetType;

    wstring m_AnimKeyPrefix;

    _bool m_ComponentsCached;

private: // --- 갈 곳의 위치, 오브젝트가 타겟인 경우 오브젝트 저장---
    CGameObject* m_TargetObject;

    _vec3 m_TargetPos;
    _vec3 m_PrayTargetPos;

    _float m_FoodSearchCooldown;
};
