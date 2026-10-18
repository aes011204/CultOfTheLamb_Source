#pragma once
#include "CNPCState.h"

/// @brief NPC가 건물을 짓거나 작업하는 상태
class CNPCState_Build : public CNPCState
{
public:
    explicit CNPCState_Build(CNPCBase* owner, CNPCStateMachine* stateMachine);
    ~CNPCState_Build() override;

public:
    void Enter() override;
    _int Update_State(const _float& timeDelta) override;
    void Exit() override;

    HRESULT Ready_State() override;

public:
    static CNPCState_Build* Create(CNPCBase* owner, CNPCStateMachine* stateMachine);

protected:
    void Free() override;
};
