#pragma once
#include "CNPCState.h"

/// @brief NPC의 벌목(ChopWood) 상태
class CNPCState_ChopWood : public CNPCState
{
private:
    explicit CNPCState_ChopWood(CNPCBase* owner, CNPCStateMachine* stateMachine);
    ~CNPCState_ChopWood() override;

public:
    void Enter() override;
    _int Update_State(const _float& timeDelta) override;
    void Exit() override;

    HRESULT Ready_State() override;

public:
    static CNPCState_ChopWood* Create(CNPCBase* owner, CNPCStateMachine* stateMachine);

private:
    void Free() override;

private:
    bool m_bAttackFlag = false;
};
