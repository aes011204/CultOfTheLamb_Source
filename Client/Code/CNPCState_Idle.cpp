#include "pch.h"
#include "CNPCState_Idle.h"

#include "CAnimator.h"
#include "CConstructCircle.h"
#include "CDroppedItem.h"
#include "CDropSystem.h"
#include "CInteractionChecker.h"
#include "CNPCBase.h"
#include "CNPCController.h"
#include "CNPCStateMachine.h"
#include "CRigidbody.h"
#include "CTransform.h"
#include "CWorldInteractive.h"

CNPCState_Idle::CNPCState_Idle(CNPCBase* owner, CNPCStateMachine* stateMachine)
    : CNPCState(owner, stateMachine),
      m_IdleEndTimer(0.f)
{ }

CNPCState_Idle::~CNPCState_Idle()
{ }

void CNPCState_Idle::Enter()
{
    CNPCState::Enter();

    if (m_OwnerAnimator && m_Controller)
    {
        wstring key = m_Controller->Get_AnimKeyPrefix() + L"Idle";
        m_OwnerAnimator->Play_Animation(key, ANIMSTATE::LOOP);
    }

    if (m_OwnerRigidbody)
        m_OwnerRigidbody->Set_Friction(0.3f);
}

_int CNPCState_Idle::Update_State(const _float& timeDelta)
{
    CNPCState::Update_State(timeDelta);

    m_IdleEndTimer += timeDelta;

    if (m_Owner)
    {
        // 상호작용 검사 작동

        if (CInteractionChecker* checker = m_Owner->Get_InteractionChecker())
        {
            CConstructCircle* nearestCircle = checker->Get_NearestConstruction(m_OwnerTransform->Get_Pos());

            // 건축 부지 탐색
            if (nearestCircle)
            {
                if (m_Controller)
                {
                    _vec3 targetPos = nearestCircle->Get_Transform()->Get_Pos();
                    m_Controller->Set_TargetPos(targetPos);
                    m_Controller->Set_TargetObject(nearestCircle); // Set Target Object
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

                    m_Controller->Reset_FoodSearchCooldown(); // 15초 쿨다운 설정

                    return static_cast<_int>(CNPCStateMachine::NPC_STATE::RUN);
                }
            }

            // 작업 탐색
            CWorldInteractive* nearestInteractive = nullptr;

            switch (m_Controller->Get_Work())
            {
            case CNPCController::NPC_WORK::CHOPWOOD:
            {
                nearestInteractive = checker->Get_NearestTree(m_OwnerTransform->Get_Pos());

                if (nearestInteractive)
                {
                    _vec3 targetPos = nearestInteractive->Get_Transform()->Get_Pos();
                    m_Controller->Set_TargetPos(targetPos);
                    m_Controller->Set_TargetObject(nearestInteractive);
                    m_Controller->Set_TargetType(CNPCController::NPC_TARGET_TYPE::TREE);

                    return static_cast<_int>(CNPCStateMachine::NPC_STATE::RUN);
                }
            }
            break;

            case CNPCController::NPC_WORK::MINING:
            {
                nearestInteractive = checker->Get_NearestRock(m_OwnerTransform->Get_Pos());

                if (nearestInteractive)
                {
                    _vec3 targetPos = nearestInteractive->Get_Transform()->Get_Pos();
                    m_Controller->Set_TargetPos(targetPos);
                    m_Controller->Set_TargetObject(nearestInteractive);
                    m_Controller->Set_TargetType(CNPCController::NPC_TARGET_TYPE::ROCK);

                    return static_cast<_int>(CNPCStateMachine::NPC_STATE::RUN);
                }
            }
            break;

            case CNPCController::NPC_WORK::PRAY:
            {
                // 지정된 위치로 이동
                _vec3 targetPos = m_Controller->Get_PrayTargetPos();
                m_Controller->Set_TargetPos(targetPos);
                m_Controller->Set_TargetObject(nullptr); // 특정 오브젝트가 아님
                m_Controller->Set_TargetType(CNPCController::NPC_TARGET_TYPE::PRAY_SITE);

                return static_cast<_int>(CNPCStateMachine::NPC_STATE::RUN);
            }

            default: break;
            }

            // 5초 이상 Idle 상태라면 자동으로 주어진 일이 아닌 다른 일을 하러 감
            if (m_IdleEndTimer >= 5.f)
            {
                m_IdleEndTimer = 0.f; // 리셋

                // 자신의 본래 작업이 아닌 다른 작업이 있는지 먼저 확인
                CWorldInteractive*       altInteractive = nullptr;
                CNPCController::NPC_WORK myWork         = m_Controller->Get_Work();

                // CHOPWOOD가 원래 작업이면 MINING 확인, 그 반대도 마찬가지
                if (myWork == CNPCController::NPC_WORK::CHOPWOOD)
                {
                    altInteractive = checker->Get_NearestRock(m_OwnerTransform->Get_Pos());
                    if (altInteractive)
                    {
                        // 돌 채광으로 이동
                        m_Controller->Set_TargetPos(altInteractive->Get_Transform()->Get_Pos());
                        m_Controller->Set_TargetObject(altInteractive);
                        m_Controller->Set_TargetType(CNPCController::NPC_TARGET_TYPE::ROCK);
                        return static_cast<_int>(CNPCStateMachine::NPC_STATE::RUN);
                    }
                }
                else if (myWork == CNPCController::NPC_WORK::MINING)
                {
                    altInteractive = checker->Get_NearestTree(m_OwnerTransform->Get_Pos());
                    if (altInteractive)
                    {
                        // 나무 벌목으로 이동
                        m_Controller->Set_TargetPos(altInteractive->Get_Transform()->Get_Pos());
                        m_Controller->Set_TargetObject(altInteractive);
                        m_Controller->Set_TargetType(CNPCController::NPC_TARGET_TYPE::TREE);
                        return static_cast<_int>(CNPCStateMachine::NPC_STATE::RUN);
                    }
                }

                // 다른 작업도 없으면 기도로 이동
                _vec3 targetPos = m_Controller->Get_PrayTargetPos();
                m_Controller->Set_TargetPos(targetPos);
                m_Controller->Set_TargetObject(nullptr);
                m_Controller->Set_TargetType(CNPCController::NPC_TARGET_TYPE::PRAY_SITE);

                return static_cast<_int>(CNPCStateMachine::NPC_STATE::RUN);
            }
        }
    }

    return static_cast<_int>(CNPCStateMachine::NPC_STATE::IDLE);
}

void CNPCState_Idle::Exit()
{
    CNPCState::Exit();
}

HRESULT CNPCState_Idle::Ready_State()
{
    return S_OK;
}

CNPCState_Idle* CNPCState_Idle::Create(CNPCBase* owner, CNPCStateMachine* stateMachine)
{
    auto instance = new CNPCState_Idle(owner, stateMachine);

    if (FAILED(instance->Ready_State()))
    {
        Safe_Release(instance);
        return nullptr;
    }

    return instance;
}

void CNPCState_Idle::Free()
{
    CNPCState::Free();
}
