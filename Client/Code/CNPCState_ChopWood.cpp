#include "pch.h"
#include "CNPCState_ChopWood.h"

#include <CSoundMgr.h>
#include "CAnimator.h"
#include "CNPCBase.h"
#include "CNPCController.h"
#include "CNPCStateMachine.h"
#include "CRigidbody.h"
#include "CWorldInteractive.h"
#include "CCombatStat.h"
#include "CTransform.h"

CNPCState_ChopWood::CNPCState_ChopWood(CNPCBase* owner, CNPCStateMachine* stateMachine)
    : CNPCState(owner, stateMachine)
{ }

CNPCState_ChopWood::~CNPCState_ChopWood()
{ }

void CNPCState_ChopWood::Enter()
{
    CNPCState::Enter();

    if (m_OwnerAnimator && m_Controller)
    {
        wstring key = m_Controller->Get_AnimKeyPrefix() + L"ChopWood";
        m_OwnerAnimator->Play_Animation(key, ANIMSTATE::LOOP);
    }

    if (m_OwnerRigidbody)
        m_OwnerRigidbody->Set_Friction(0.3f);
}

_int CNPCState_ChopWood::Update_State(const _float& timeDelta)
{
    CNPCState::Update_State(timeDelta);

    if (m_OwnerAnimator && m_Controller)
    {
        CGameObject* target = m_Controller->Get_TargetObject();

        auto combatStat = dynamic_cast<CCombatStat*>(
            target->Get_Component(COMPONENTID::ID_DYNAMIC, COMPONENTTYPE::COMBATSTAT));

        if (combatStat && combatStat->Get_Hp() <= 0)
        {
            m_Controller->Set_TargetObject(nullptr);
            return static_cast<_int>(CNPCStateMachine::NPC_STATE::IDLE);
        }

        auto interactive = dynamic_cast<CWorldInteractive*>(target);
        if (interactive)
        {
            if (m_OwnerAnimator->Get_CurFrame() == 0 || m_OwnerAnimator->Get_CurFrame() == 34)
                m_bAttackFlag = false;

            if ((m_OwnerAnimator->Get_CurFrame() == 33 || m_OwnerAnimator->Get_CurFrame() == 60)
                && !m_bAttackFlag)
            {
                auto combat = dynamic_cast<CCombatStat*>(
                    target->Get_Component(COMPONENTID::ID_DYNAMIC, COMPONENTTYPE::COMBATSTAT));

                if (combat)
                {
                    HITDATA h;
                    ZeroMemory(&h, sizeof(h));
                    h.attacker = m_Owner;
                    h.damage   = 1.f;       // 나무 때리는 데미지 1
                    h.hitter   = target;

                    if (m_OwnerTransform && interactive->Get_Transform())
                    {
                        _vec3 dir = interactive->Get_Transform()->Get_Pos() - m_OwnerTransform->Get_Pos();
                        D3DXVec3Normalize(&h.dir, &dir);
                    }

                    combat->Apply_Damage(h);
                    m_bAttackFlag = true;

                    CSoundMgr::GetInstance()->Play_Once(L"Object/Wood/Wood_Chop_2.wav",
                                                        SOUND_EFFECT,
                                                        0.5f);
                }
            }
        }
    }

    return static_cast<_int>(CNPCStateMachine::NPC_STATE::CHOPWOOD);
}

void CNPCState_ChopWood::Exit()
{
    CNPCState::Exit();
}

HRESULT CNPCState_ChopWood::Ready_State()
{
    return S_OK;
}

CNPCState_ChopWood* CNPCState_ChopWood::Create(CNPCBase* owner, CNPCStateMachine* stateMachine)
{
    auto instance = new CNPCState_ChopWood(owner, stateMachine);

    if (FAILED(instance->Ready_State()))
    {
        Safe_Release(instance);
        return nullptr;
    }

    return instance;
}

void CNPCState_ChopWood::Free()
{
    CNPCState::Free();
}
