#include "pch.h"
#include "CNPCState_Build.h"

#include "CAnimator.h"
#include "CGameObject.h"
#include "CNPCBase.h"
#include "CNPCController.h"
#include "CNPCStateMachine.h"
#include "CSoundMgr.h"

CNPCState_Build::CNPCState_Build(CNPCBase* owner, CNPCStateMachine* stateMachine)
    : CNPCState(owner, stateMachine)
{ }

CNPCState_Build::~CNPCState_Build()
{ }

void CNPCState_Build::Enter()
{
    CNPCState::Enter();

    if (m_OwnerAnimator && m_Controller)
    {
        wstring key = m_Controller->Get_AnimKeyPrefix() + L"Build";
        m_OwnerAnimator->Play_Animation(key, ANIMSTATE::LOOP);
    }
}

_int CNPCState_Build::Update_State(const _float& timeDelta)
{
    CNPCState::Update_State(timeDelta);

#pragma region 건축 효과음. 0.53초마다 재생
            g_Timers[TIMER_SOUND_FOL_CONSTRUCT] += timeDelta;

            if (g_Timers[TIMER_SOUND_FOL_CONSTRUCT] >= 0.53f)
            {
                CSoundMgr::GetInstance()->Play_Once(L"Construction/Hammering_16.wav", SOUND_EFFECT, 0.5f);
                g_Timers[TIMER_SOUND_FOL_CONSTRUCT] = 0.f;
            }
#pragma endregion

    // Check Target Validity
    if (m_Controller)
    {
        CGameObject* target = m_Controller->Get_TargetObject();
        if (!target || target->Is_PendingDestroy())
        {
            m_Controller->Set_TargetObject(nullptr);
            return static_cast<_int>(CNPCStateMachine::NPC_STATE::IDLE);
        }
    }
    else
    {
        return static_cast<_int>(CNPCStateMachine::NPC_STATE::IDLE);
    }

    return static_cast<_int>(CNPCStateMachine::NPC_STATE::BUILD);
}

void CNPCState_Build::Exit()
{
    CNPCState::Exit();
}

HRESULT CNPCState_Build::Ready_State()
{
    return S_OK;
}

CNPCState_Build* CNPCState_Build::Create(CNPCBase* owner, CNPCStateMachine* stateMachine)
{
    auto instance = new CNPCState_Build(owner, stateMachine);

    if (FAILED(instance->Ready_State()))
    {
        Safe_Release(instance);
        return nullptr;
    }

    return instance;
}

void CNPCState_Build::Free()
{
    CNPCState::Free();
}
