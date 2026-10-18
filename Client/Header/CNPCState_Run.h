#pragma once
#include "CNPCState.h"

/// @brief NPC의 이동(Run) 상태
class CNPCState_Run : public CNPCState
{
private:
    explicit CNPCState_Run(CNPCBase* owner, CNPCStateMachine* stateMachine);
    ~CNPCState_Run() override;

public:
    void Enter() override;
    _int Update_State(const _float& timeDelta) override;
    void Exit() override;

    HRESULT Ready_State() override;

public:
    static CNPCState_Run* Create(CNPCBase* owner, CNPCStateMachine* stateMachine);

private:
    void Set_WalkPastCoolTime(_float time) { m_WalkPastCooldown = time; }

private:
    void Free() override;

private:
   _float m_WalkPastCooldown = 0.f;
};
