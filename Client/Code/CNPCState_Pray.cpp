#include "pch.h"
#include "CNPCState_Pray.h"

#include "CAnimator.h"
#include "CConstructCircle.h"
#include "CDroppedItem.h"
#include "CInteractionChecker.h"
#include "CNPCBase.h"
#include "CNPCController.h"
#include "CNPCStateMachine.h"
#include "CSoundMgr.h"
#include "CTransform.h"
#include "CWorldInteractive.h"

CNPCState_Pray::CNPCState_Pray(CNPCBase* owner, CNPCStateMachine* stateMachine)
    : CNPCState(owner, stateMachine),
      m_Timer(0.f)
{ }

CNPCState_Pray::~CNPCState_Pray()
{ }

void CNPCState_Pray::Enter()
{
    CNPCState::Enter();

    if (m_OwnerAnimator && m_Controller)
    {
        wstring key = m_Controller->Get_AnimKeyPrefix() + L"Pray";
        m_OwnerAnimator->Play_Animation(key, ANIMSTATE::LOOP);
    }

    CSoundMgr::GetInstance()->Play_Once(L"Follower/Follower_Praying.wav", SOUND_EFFECT, 0.5f);

    m_Timer = 0.f;
}

_int CNPCState_Pray::Update_State(const _float& timeDelta)
{
    CNPCState::Update_State(timeDelta);

    if (m_Owner)
    {
        if (CInteractionChecker* checker = m_Owner->Get_InteractionChecker())
        {
            // 건축 부지 탐색
            CConstructCircle* nearestCircle = checker->Get_NearestConstruction(m_OwnerTransform->Get_Pos());
            if (nearestCircle)
            {
                if (m_Controller)
                {
                    _vec3 targetPos = nearestCircle->Get_Transform()->Get_Pos();
                    m_Controller->Set_TargetPos(targetPos);
                    m_Controller->Set_TargetObject(nearestCircle);
                    m_Controller->Set_TargetType(CNPCController::NPC_TARGET_TYPE::CONSTRUCTION);
                    return static_cast<_int>(CNPCStateMachine::NPC_STATE::RUN);
                }
            }

            // 음식 탐색
            if (m_Controller->Is_FoodSearchAvailable())
            {
                CDroppedItem* nearestFood = checker->Get_NearestFood(m_OwnerTransform->Get_Pos());
                if (nearestFood)
                {
                    _vec3 itemPos = nearestFood->Get_Transform()->Get_Pos();
                    m_Controller->Set_TargetPos(itemPos);
                    m_Controller->Set_TargetObject(nearestFood);
                    m_Controller->Set_TargetType(CNPCController::NPC_TARGET_TYPE::FOOD);

                    m_Controller->Reset_FoodSearchCooldown(); // 15초 쿨다운

                    return static_cast<_int>(CNPCStateMachine::NPC_STATE::RUN);
                }
            }

            // 다른 작업(채광, 벌목) 탐색
            CWorldInteractive* nearestInteractive = nullptr;
            switch (m_Controller->Get_Work())
            {
            case CNPCController::NPC_WORK::CHOPWOOD:
                nearestInteractive = checker->Get_NearestTree(m_OwnerTransform->Get_Pos());
                if (nearestInteractive)
                {
                    _vec3 targetPos = nearestInteractive->Get_Transform()->Get_Pos();
                    m_Controller->Set_TargetPos(targetPos);
                    m_Controller->Set_TargetObject(nearestInteractive);
                    m_Controller->Set_TargetType(CNPCController::NPC_TARGET_TYPE::TREE);

                    return static_cast<_int>(CNPCStateMachine::NPC_STATE::RUN);
                }
                break;

            case CNPCController::NPC_WORK::MINING:
                nearestInteractive = checker->Get_NearestRock(m_OwnerTransform->Get_Pos());
                if (nearestInteractive)
                {
                    _vec3 targetPos = nearestInteractive->Get_Transform()->Get_Pos();
                    m_Controller->Set_TargetPos(targetPos);
                    m_Controller->Set_TargetObject(nearestInteractive);
                    m_Controller->Set_TargetType(CNPCController::NPC_TARGET_TYPE::ROCK);

                    return static_cast<_int>(CNPCStateMachine::NPC_STATE::RUN);
                }
                break;

            default: break;
            }
        }
    }

    m_Timer += timeDelta;

    if (m_Timer >= 5.f)
    {
        // 5초 기도 후 PRAY_PEEK로 전환 (Once 재생 후 다시 돌아옴)
        return static_cast<_int>(CNPCStateMachine::NPC_STATE::PRAY_PEEK);
    }

    return static_cast<_int>(CNPCStateMachine::NPC_STATE::PRAY);
}

void CNPCState_Pray::Exit()
{
    CNPCState::Exit();

    CSoundMgr::GetInstance()->Stop_Sound(L"Follower/Follower_Praying.wav");
}

HRESULT CNPCState_Pray::Ready_State()
{
    return S_OK;
}

CNPCState_Pray* CNPCState_Pray::Create(CNPCBase* owner, CNPCStateMachine* stateMachine)
{
    auto instance = new CNPCState_Pray(owner, stateMachine);

    if (FAILED(instance->Ready_State()))
    {
        Safe_Release(instance);
        return nullptr;
    }

    return instance;
}

void CNPCState_Pray::Free()
{
    CSoundMgr::GetInstance()->Stop_Sound(L"Follower/Follower_Praying.wav");

    CNPCState::Free();
}
