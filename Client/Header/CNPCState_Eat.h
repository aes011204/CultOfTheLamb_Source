#pragma once
#include "CNPCState.h"

/// @brief NPC의 먹기(Eat) 상태
class CNPCState_Eat : public CNPCState
{
private:
    explicit CNPCState_Eat(CNPCBase* owner, CNPCStateMachine* stateMachine);
    ~CNPCState_Eat() override;

public:
    void Enter() override;
    _int Update_State(const _float& timeDelta) override;
    void Exit() override;

    HRESULT Ready_State() override;

public:
    static CNPCState_Eat* Create(CNPCBase* owner, CNPCStateMachine* stateMachine);

private:
    void Free() override;
};
