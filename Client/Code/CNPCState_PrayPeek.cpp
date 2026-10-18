#include "pch.h"
#include "CNPCState_PrayPeek.h"

#include "CAnimator.h"
#include "CNPCBase.h"
#include "CNPCController.h"
#include "CNPCStateMachine.h"

CNPCState_PrayPeek::CNPCState_PrayPeek(CNPCBase* owner, CNPCStateMachine* stateMachine)
    : CNPCState(owner, stateMachine)
{ }

CNPCState_PrayPeek::~CNPCState_PrayPeek()
{ }

void CNPCState_PrayPeek::Enter()
{
    CNPCState::Enter();

    if (m_OwnerAnimator && m_Controller)
    {
        wstring key = m_Controller->Get_AnimKeyPrefix() + L"PrayPeek";
        m_OwnerAnimator->Play_Animation(key, ANIMSTATE::ONCE);
    }
}

_int CNPCState_PrayPeek::Update_State(const _float& timeDelta)
{
    CNPCState::Update_State(timeDelta);

    // 애니메이션 종료 체크
    if (m_OwnerAnimator && m_OwnerAnimator->Is_AnimationFinished())
    {
        // 다시 Pray 상태로 돌아감 (Pray의 m_bSecondPhase는 true인 상태여야 함)
        return static_cast<_int>(CNPCStateMachine::NPC_STATE::PRAY);
    }

    return static_cast<_int>(CNPCStateMachine::NPC_STATE::PRAY_PEEK);
}

void CNPCState_PrayPeek::Exit()
{
    CNPCState::Exit();
}

HRESULT CNPCState_PrayPeek::Ready_State()
{
    return S_OK;
}

CNPCState_PrayPeek* CNPCState_PrayPeek::Create(CNPCBase* owner, CNPCStateMachine* stateMachine)
{
    auto instance = new CNPCState_PrayPeek(owner, stateMachine);

    if (FAILED(instance->Ready_State()))
    {
        Safe_Release(instance);
        return nullptr;
    }

    return instance;
}

void CNPCState_PrayPeek::Free()
{
    CNPCState::Free();
}
