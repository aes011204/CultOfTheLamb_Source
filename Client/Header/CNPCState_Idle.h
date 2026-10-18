#pragma once
#include "CNPCState.h"

/// @brief NPC의 대기(Idle) 상태
class CNPCState_Idle : public CNPCState
{
private:
    explicit CNPCState_Idle(CNPCBase* owner, CNPCStateMachine* stateMachine);
    ~CNPCState_Idle() override;

public:
    void Enter() override;
    _int Update_State(const _float& timeDelta) override;
    void Exit() override;

    HRESULT Ready_State() override;

public:
    static CNPCState_Idle* Create(CNPCBase* owner, CNPCStateMachine* stateMachine);

private:
    void Free() override;

private:
    _float m_IdleEndTimer;             // 자동 Pray 상태 진입 타이머
};
