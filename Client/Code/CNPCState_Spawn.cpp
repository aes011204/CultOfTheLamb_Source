#include "pch.h"
#include "CNPCState_Spawn.h"

#include "CAnimator.h"
#include "CNPCBase.h"
#include "CNPCController.h"
#include "CNPCStateMachine.h"
#include "CSoundMgr.h"
#include "CSpriteAnimation.h"
#include "CVillageStateMgr.h"

CNPCState_Spawn::CNPCState_Spawn(CNPCBase* owner, CNPCStateMachine* stateMachine)
    : CNPCState(owner, stateMachine)
{ }

CNPCState_Spawn::~CNPCState_Spawn()
{ }

void CNPCState_Spawn::Enter()
{
    CNPCState::Enter();

    if (m_OwnerAnimator && m_Controller)
    {
        wstring key = m_Controller->Get_AnimKeyPrefix() + L"Spawn";
        m_OwnerAnimator->Play_Animation(key, ANIMSTATE::ONCE);
    }

    CSoundMgr::GetInstance()->Play_Once(L"Follower/Follower_Start_Indo.wav",
                                        SOUND_EFFECT,
                                        0.38f);
}

_int CNPCState_Spawn::Update_State(const _float& timeDelta)
{
    CNPCState::Update_State(timeDelta);

    // 애니메이션 종료 시 IDLE로 전환
    if (m_OwnerAnimator && m_OwnerAnimator->Get_CurAnimation()->Get_AnimState() == ANIMSTATE::ONCE)
    {
        if (m_OwnerAnimator->Is_AnimationFinished())
        {
            if (m_Owner)
                CVillageStateMgr::GetInstance()->Save_NPC(m_Owner->Get_Name(), true, true);

            return static_cast<_int>(CNPCStateMachine::NPC_STATE::IDLE);
        }
    }

    return static_cast<_int>(CNPCStateMachine::NPC_STATE::SPAWN);
}

void CNPCState_Spawn::Exit()
{
    CNPCState::Exit();
}

HRESULT CNPCState_Spawn::Ready_State()
{
    return S_OK;
}

CNPCState_Spawn* CNPCState_Spawn::Create(CNPCBase* owner, CNPCStateMachine* stateMachine)
{
    auto instance = new CNPCState_Spawn(owner, stateMachine);

    if (FAILED(instance->Ready_State()))
    {
        Safe_Release(instance);
        return nullptr;
    }

    return instance;
}

void CNPCState_Spawn::Free()
{
    CNPCState::Free();
}
