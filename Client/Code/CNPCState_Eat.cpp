#include "pch.h"
#include "CNPCState_Eat.h"

#include "CAnimator.h"
#include "CDroppedItem.h"
#include "CNPCBase.h"
#include "CNPCController.h"
#include "CNPCStateMachine.h"
#include "CRigidbody.h"
#include "CSoundMgr.h"

CNPCState_Eat::CNPCState_Eat(CNPCBase* owner, CNPCStateMachine* stateMachine)
    : CNPCState(owner, stateMachine)
{ }

CNPCState_Eat::~CNPCState_Eat()
{ }

void CNPCState_Eat::Enter()
{
    CNPCState::Enter();

    if (m_OwnerAnimator && m_Controller)
    {
        wstring key = m_Controller->Get_AnimKeyPrefix() + L"Eat";
        m_OwnerAnimator->Play_Animation(key, ANIMSTATE::ONCE);

        // Enter 시점에 바로 음식 삭제 처리
        CGameObject* target = m_Controller->Get_TargetObject();
        auto         item   = dynamic_cast<CDroppedItem*>(target);

        if (item && !item->Is_PendingDestroy())
            item->Mark_As_PendingKill();

        m_Controller->Set_TargetObject(nullptr);
    }

    if (m_OwnerRigidbody)
        m_OwnerRigidbody->Set_Friction(0.3f);

    CSoundMgr::GetInstance()->Play_Once(L"Follower/food_eat3.wav",
                                        SOUND_EFFECT,
                                        0.4f);
}

_int CNPCState_Eat::Update_State(const _float& timeDelta)
{
    CNPCState::Update_State(timeDelta);

    if (m_OwnerAnimator)
    {
        if (m_OwnerAnimator->Is_AnimationFinished())
            return static_cast<_int>(CNPCStateMachine::NPC_STATE::IDLE);
    }

    return static_cast<_int>(CNPCStateMachine::NPC_STATE::EAT);
}

void CNPCState_Eat::Exit()
{
    CNPCState::Exit();
}

HRESULT CNPCState_Eat::Ready_State()
{
    return S_OK;
}

CNPCState_Eat* CNPCState_Eat::Create(CNPCBase* owner, CNPCStateMachine* stateMachine)
{
    auto instance = new CNPCState_Eat(owner, stateMachine);

    if (FAILED(instance->Ready_State()))
    {
        Safe_Release(instance);
        return nullptr;
    }

    return instance;
}

void CNPCState_Eat::Free()
{
    CNPCState::Free();
}
