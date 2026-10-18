#pragma once
#include "CNPCState.h"

/// @brief NPC의 구출(Rescued) 상태.
/// @details
/// - 구출 애니메이션을 재생한 뒤, 재생이 끝나면 오브젝트를 소멸(PendingKill)시킨다.
class CNPCState_Rescued : public CNPCState
{
private:
    explicit CNPCState_Rescued(CNPCBase* owner, CNPCStateMachine* stateMachine);
    ~CNPCState_Rescued() override;

public:
    void Enter() override;
    _int Update_State(const _float& timeDelta) override;
    void Exit() override;

    HRESULT Ready_State() override;

public:
    static CNPCState_Rescued* Create(CNPCBase* owner, CNPCStateMachine* stateMachine);

private:
    void Free() override;

private:
    _int m_AnimStep = 0;

    _bool m_CanPlayRescueSound = false;
};
