#pragma once
#include "CNPCState.h"

/// @brief NPC의 채광(Mining) 상태
class CNPCState_Mining : public CNPCState
{
private:
    explicit CNPCState_Mining(CNPCBase* owner, CNPCStateMachine* stateMachine);
    ~CNPCState_Mining() override;

public:
    void Enter() override;
    _int Update_State(const _float& timeDelta) override;
    void Exit() override;

    HRESULT Ready_State() override;

public:
    static CNPCState_Mining* Create(CNPCBase* owner, CNPCStateMachine* stateMachine);

private:
    void Free() override;

private:
    bool m_bAttackFlag = false;
};
