#include "pch.h"
#include "CNPCController.h"

#include "CAnimator.h"
#include "CCombatStat.h"
#include "CNPCBase.h"
#include "CNPCStateMachine.h"
#include "CStateMachine.h"
#include "CTransform.h"

CNPCController::CNPCController(DEVICE graphicDev)
    : CComponent(graphicDev),
      m_StateMachine(nullptr),
      m_AnimatorCom(nullptr),
      m_CombatStatCom(nullptr),
      m_Work(NPC_WORK::NONE),
      m_TargetType(NPC_TARGET_TYPE::NONE),
      m_ComponentsCached(false),
      m_TargetObject(nullptr),
      m_TargetPos(0.f, 0.f, 0.f),
      m_PrayTargetPos(18.f, 1.f, 21.5f),
      m_FoodSearchCooldown(0.f)
{ }

CNPCController::CNPCController(const CNPCController& rhs)
    : CComponent(rhs),
      m_StateMachine(rhs.m_StateMachine),
      m_AnimatorCom(nullptr),
      m_CombatStatCom(nullptr),
      m_Work(rhs.m_Work),
      m_TargetType(rhs.m_TargetType),
      m_ComponentsCached(false),
      m_TargetObject(rhs.m_TargetObject),
      m_TargetPos(rhs.m_TargetPos),
      m_PrayTargetPos(rhs.m_PrayTargetPos),
      m_FoodSearchCooldown(rhs.m_FoodSearchCooldown)
{
    if (m_StateMachine)
        m_StateMachine->AddRef();
}

CNPCController::~CNPCController()
{ }

HRESULT CNPCController::Ready_Controller()
{
    return S_OK;
}

void CNPCController::Initialize_Component()
{
    // StateMachine 생성 및 설정
    // NPC는 개별적으로 StateMachine을 가질 수 있으므로 여기서 생성
    CNPCStateMachine* stateMachine = CNPCStateMachine::Create(dynamic_cast<CNPCBase*>(m_Owner));

    if (stateMachine)
    {
        Set_StateMachine(stateMachine);
        Safe_Release(stateMachine); // Set_StateMachine에서 AddRef 함
    }
}

_int CNPCController::Update_Component(const _float& timeDelta)
{
    Cache_Components();

    if (m_StateMachine)
        m_StateMachine->Update_StateMachine(timeDelta);

    if (m_FoodSearchCooldown > 0.f)
        m_FoodSearchCooldown -= timeDelta;

    return 0;
}

CTransform* CNPCController::Get_OwnerTransform()
{
    if (!m_ComponentsCached)
        Cache_Components();

    return m_TransformCom;
}

CAnimator* CNPCController::Get_OwnerAnimator()
{
    if (!m_ComponentsCached)
        Cache_Components();

    return m_AnimatorCom;
}

CCombatStat* CNPCController::Get_OwnerCombatStat()
{
    if (!m_ComponentsCached)
        Cache_Components();

    return m_CombatStatCom;
}

void CNPCController::Set_StateMachine(CStateMachine* stateMachine)
{
    if (m_StateMachine != stateMachine)
    {
        Safe_Release(m_StateMachine);
        m_StateMachine = stateMachine;
        if (m_StateMachine)
            m_StateMachine->AddRef();
    }
}

CNPCStateMachine::NPC_STATE CNPCController::Get_CurNPCState()
{
    return static_cast<CNPCStateMachine::NPC_STATE>(Get_CurrentState());
}

void CNPCController::Cache_Components()
{
    if (!m_Owner || m_ComponentsCached)
        return;

    m_TransformCom = dynamic_cast<Engine::CTransform*>(
        m_Owner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));

    m_AnimatorCom = dynamic_cast<Engine::CAnimator*>(
        m_Owner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::ANIMATOR));

    m_CombatStatCom = dynamic_cast<Engine::CCombatStat*>(
        m_Owner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::COMBATSTAT));

    m_ComponentsCached = true;
}

_uint CNPCController::Get_CurrentState()
{
    if (m_StateMachine)
        return m_StateMachine->Get_CurStateNum();

    return -1;
}

CNPCController* CNPCController::Create(DEVICE graphicDev)
{
    auto instance = new CNPCController(graphicDev);

    if (FAILED(instance->Ready_Controller()))
    {
        Safe_Release(instance);
        return nullptr;
    }

    return instance;
}

CComponent* CNPCController::Clone()
{
    return new CNPCController(*this);
}

void CNPCController::Free()
{
    Safe_Release(m_StateMachine);

    CComponent::Free();
}
