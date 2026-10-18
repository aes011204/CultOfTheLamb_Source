#pragma once

#include "CStateMachine.h"

class CNPCBase;
/// @brief NPC의 상태 전이를 관리하고 제어하는 상태 머신 클래스
/// @details
/// - NPC의 다양한 상태(Idle, Run, Work 등)를 등록하고 관리한다.
/// - RegisterState를 통해 상태를 등록하고, Change_State로 상태를 전환한다.
/// - Owner(NPC) 포인터를 보유하여 상태 간 데이터 공유를 돕는다.
class CNPCStateMachine : public CStateMachine
{
public:
    enum class NPC_STATE
    {
        IDLE,
        RUN,
        WALKPAST,       // 이동 중 플레이어와 가까워졌을 때
        BUILD,          // 건축
        CHOPWOOD,       // 벌목
        MINING,         // 채광
        EAT,            // 식사
        PRAY,           // 기도
        PRAY_PEEK,      // 기도 중 딴짓
        SPAWN,          // 소환
        RESCUED,        // 구출
        
        STATE_END
    };

protected:
    explicit CNPCStateMachine();
    ~CNPCStateMachine() override;

public:
    HRESULT Ready_StateMachine() override;
    _int    Update_StateMachine(const _float& timeDelta) override;
    void    Change_State(_uint dwStateKey) override;

public:
    static CNPCStateMachine* Create(CNPCBase* owner);

protected:
    void Free() override;

private:
    CNPCBase* m_Owner = nullptr;

public:
    void Set_Owner(CNPCBase* owner) { m_Owner = owner; }

    template<typename TState>
    HRESULT RegisterState(NPC_STATE stateEnum, CNPCBase* owner)
    {
        CBaseState* state = TState::Create(owner, this);
        if (!state)
            return E_FAIL;
        
        m_vecState[static_cast<_uint>(stateEnum)] = state;
        return S_OK;
    }
};
