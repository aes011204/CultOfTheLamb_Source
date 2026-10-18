#pragma once
#include "CNPCState.h"

/// @brief NPC가 기도 중 실눈 뜨고 쳐다보는 상태
/// - Pray 상태 중간에 한 번 재생됨 (Once)
class CNPCState_PrayPeek : public CNPCState
{
private:
    explicit CNPCState_PrayPeek(CNPCBase* owner, CNPCStateMachine* stateMachine);
    ~CNPCState_PrayPeek() override;

public:
    void Enter() override;
    _int Update_State(const _float& timeDelta) override;
    void Exit() override;

    HRESULT Ready_State() override;

public:
    static CNPCState_PrayPeek* Create(CNPCBase* owner, CNPCStateMachine* stateMachine);

private:
    void Free() override;
};
