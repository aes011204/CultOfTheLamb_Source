#pragma once

#include "CStateMachine.h"

class CMonsterBase;
class CMonsterState;

BEGIN(Engine)
class CGameObject;
END

class CEyeball_StateMachine : public CStateMachine
{
private:
    explicit CEyeball_StateMachine(CMonsterBase* owner);
    virtual ~CEyeball_StateMachine() override;

public:
    HRESULT Ready_StateMachine() override;
    _int    Update_StateMachine(const _float& timeDelta) override;
    void    Change_State(_uint dwStateKey) override;

public:
    static CEyeball_StateMachine* Create(CMonsterBase* owner);

protected:
    void Free() override;

private:
    CMonsterBase* m_Owner;

    template<typename TState>
    HRESULT RegisterState(_uint stateIndex, CMonsterBase* owner)
    {
        CMonsterState* state = TState::Create(owner);
        if (!state)
        {
            MSG_BOX("State Create Faield");
            return E_FAIL;
        }

        m_vecState[stateIndex] = state;

        return S_OK;
    }
};

