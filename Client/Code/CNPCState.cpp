#include "pch.h"
#include "CNPCState.h"

#include "CLayer.h"
#include "CManagement.h"
#include "CNPCBase.h"
#include "CNPCController.h"
#include "CNPCStateMachine.h"
#include "CPlayer.h"
#include "CScene.h"

CNPCState::CNPCState(CNPCBase* owner, CNPCStateMachine* stateMachine)
    : m_Owner(owner),
      m_StateMachine(stateMachine)
{
    if (m_Owner)
    {
        m_Controller     = dynamic_cast<CNPCController*>(m_Owner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::NPC_CONTROLLER));
        m_OwnerTransform = m_Owner->Get_Transform();

        // Animator는 Controller에 있을 수도 있고 Owner에 있을 수도 있음.
        if (m_Controller)
            m_OwnerAnimator = m_Controller->Get_OwnerAnimator();

        m_OwnerRigidbody = m_Owner->Get_Rigidbody();
    }
}

CNPCState::~CNPCState()
{ }

void CNPCState::Enter()
{
    m_CurStateElapsedTime = 0.f;
    Find_Player();
}

_int CNPCState::Update_State(const _float& timeDelta)
{
    m_CurStateElapsedTime += timeDelta;
    return 0;
}

void CNPCState::Exit()
{
    m_CurStateElapsedTime = 0.f;
}

void CNPCState::Find_Player()
{
    if (m_Player)
        return;

    CGameObject* object = nullptr;

    CScene* scene = CManagement::GetInstance()->Get_Scene();
    if (scene)
    {
        CLayer* layer = scene->Get_Layer(LAYERTYPE::GAMELOGIC);
        if (layer)
        {
            auto& objects = layer->Get_Objects();
            auto  iter    = objects.find(OBJTYPE::PLAYER);
            if (iter != objects.end() && !iter->second.empty())
                object = iter->second.front();
        }
    }

    if (object)
    {
        m_Player = dynamic_cast<CPlayer*>(object);
        if (m_Player)
            m_PlayerTransform = m_Player->Get_Transform();
    }
}

void CNPCState::Free()
{ }
