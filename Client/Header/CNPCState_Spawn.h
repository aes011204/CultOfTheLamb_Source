#pragma once
#include "CNPCState.h"

// NPC의 소환(Spawn) 연출 상태
class CNPCState_Spawn : public CNPCState
{
private:
    explicit CNPCState_Spawn(CNPCBase* owner, CNPCStateMachine* stateMachine);
    ~CNPCState_Spawn() override;

public:
    void Enter() override;
    _int Update_State(const _float& timeDelta) override;
    void Exit() override;

    HRESULT Ready_State() override;

public:
    static CNPCState_Spawn* Create(CNPCBase* owner, CNPCStateMachine* stateMachine);

private:
    void Free() override;
};
