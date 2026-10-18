#include "pch.h"
#include "CMonsterController.h"
#include "CStateMachine.h"
#include "CTransform.h"
#include "CAnimator.h"
#include "CCombatStat.h"
#include "CGameObject.h"

CMonsterController::CMonsterController(DEVICE graphicDev)
    : CComponent(graphicDev)
    , m_AnimatorCom(nullptr)
    , m_CombatStatCom(nullptr)
    , m_StateMachine(nullptr)
    , m_ComponentsCached(false)

{
}

CMonsterController::CMonsterController(const CMonsterController& rhs)
    : CComponent(rhs)
    , m_AnimatorCom(nullptr)
    , m_CombatStatCom(nullptr)
    , m_StateMachine(nullptr)
    , m_ComponentsCached(false)
{
}

CMonsterController::~CMonsterController()
{

}

HRESULT CMonsterController::Ready_Controller()
{
    if (Get_Owner())
    {
        Cache_Components();
    }

    return S_OK;
}

void CMonsterController::Initialize_Component()
{
    CComponent::Initialize_Component();

    if (!m_ComponentsCached && Get_Owner())
    {
        Cache_Components();
    }
}

_int CMonsterController::Update_Component(const _float& timeDelta)
{
    _int exit = CComponent::Update_Component(timeDelta);

    if (m_StateMachine)
    {
        m_StateMachine->Update_StateMachine(timeDelta);
    }


    return exit;
}

void CMonsterController::Set_StateMachine(CStateMachine* stateMachine)
{
    Safe_Release(m_StateMachine); // 기존 스테이트 머신 해제

    m_StateMachine = stateMachine;

    if (m_StateMachine)
    {
        m_StateMachine->AddRef();
    }
}

void CMonsterController::Change_State(_int stateKey)
{
    if (m_StateMachine)
    {
        m_StateMachine->Change_State(stateKey);
    }
}

_int CMonsterController::Get_CurrentState()
{
    if (m_StateMachine)
    {
        return m_StateMachine->Get_CurStateNum();
    }

    return -1;
}

void CMonsterController::Cache_Components()
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

CTransform* CMonsterController::Get_OwnerTransform()
{
    if (!m_ComponentsCached)
        Cache_Components();

    return m_TransformCom;
}

CAnimator* CMonsterController::Get_OwnerAnimator()
{
    if (!m_ComponentsCached)
        Cache_Components();

    return m_AnimatorCom;
}

CCombatStat* CMonsterController::Get_OwnerCombatStat()
{
    if (!m_ComponentsCached)
        Cache_Components();

    return m_CombatStatCom;
}

CMonsterController* CMonsterController::Create(DEVICE graphicDev)
{
    auto controller = new CMonsterController(graphicDev);

    if (FAILED(controller->Ready_Controller()))
    {
        Safe_Release(controller);
        MSG_BOX("Boss Controller Create Failed");
        return nullptr;
    }

    return controller;
}

CComponent* CMonsterController::Clone()
{
    return new CMonsterController(*this);
}

void CMonsterController::Free()
{
    Safe_Release(m_StateMachine);

    CComponent::Free();
}
