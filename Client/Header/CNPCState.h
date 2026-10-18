#pragma once
#include "CBaseState.h"

BEGIN(Engine)
class CTransform;
class CAnimator;
class CRigidbody;
END

class CNPCBase;
class CNPCController;
class CNPCStateMachine;
class CPlayer;

/// @brief 모든 NPC 상태 클래스의 기반이 되는 추상 클래스
/// @details
/// - NPC의 행동 패턴(State)을 정의하며, Enter, Update, Exit의 생명주기를 가진다.
/// - NPC 컨트롤러 및 상태 머신에 대한 참조를 멤버로 유지하여 하위 클래스에서 쉽게 접근할 수 있다.
class CNPCState : public CBaseState
{
protected:
    explicit CNPCState(CNPCBase* owner, CNPCStateMachine* stateMachine);
    ~CNPCState() override;

public:
    // 상태 진입 시 1회 호출되는 초기화 함수
    void Enter() override;
    
    // 매 프레임 호출되며, NPC의 행동을 처리하고 상태 전환 여부를 결정한다.
    _int Update_State(const _float& timeDelta) override;
    
    // 상태 종료 시 1회 호출되는 정리 함수
    void Exit() override;

protected:
    void Find_Player();

protected:
    void Free() override;

protected:
    CNPCBase*         m_Owner;
    CNPCController*   m_Controller;
    CNPCStateMachine* m_StateMachine;

    Engine::CTransform* m_OwnerTransform = nullptr;
    Engine::CAnimator*  m_OwnerAnimator  = nullptr;
    Engine::CRigidbody* m_OwnerRigidbody = nullptr;

    CPlayer*            m_Player          = nullptr;
    Engine::CTransform* m_PlayerTransform = nullptr;

    _float m_CurStateElapsedTime = 0.f;
};
