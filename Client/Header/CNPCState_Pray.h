#pragma once
#include "CNPCState.h"

/// @brief NPC가 기도하는 상태
/// - 5초 기도 -> PrayPeek -> 5초 기도 -> Idle
class CNPCState_Pray : public CNPCState
{
private:
    explicit CNPCState_Pray(CNPCBase* owner, CNPCStateMachine* stateMachine);
    ~CNPCState_Pray() override;

public:
    void Enter() override;
    _int Update_State(const _float& timeDelta) override;
    void Exit() override;

    HRESULT Ready_State() override;

public:
    static CNPCState_Pray* Create(CNPCBase* owner, CNPCStateMachine* stateMachine);

private:
    void Free() override;

private:
    _float m_Timer;                 // 자동 상태 넘기기 타이머
};
