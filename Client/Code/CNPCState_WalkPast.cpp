#include "pch.h"
#include "CNPCState_WalkPast.h"

#include "CAnimator.h"
#include "CCombatStat.h"
#include "CGameObject.h"
#include "CNPCBase.h"
#include "CNPCController.h"
#include "CNPCStateMachine.h"
#include "CRigidbody.h"
#include "CSoundMgr.h"
#include "CState.h"
#include "CTransform.h"

CNPCState_WalkPast::CNPCState_WalkPast(CNPCBase* owner, CNPCStateMachine* stateMachine)
    : CNPCState(owner, stateMachine)
{ }

CNPCState_WalkPast::~CNPCState_WalkPast()
{ }

void CNPCState_WalkPast::Enter()
{
    CNPCState::Enter();

    m_SoundPlayed = false;
    m_fSoundTimer = 0.f;

    if (m_OwnerAnimator && m_Controller)
    {
        wstring key = m_Controller->Get_AnimKeyPrefix() + L"WalkPast";
        m_OwnerAnimator->Play_Animation(key, ANIMSTATE::ONCE);
    }

    //if (m_OwnerRigidbody)
    //    m_OwnerRigidbody->Set_Friction(0.1f);
}

_int CNPCState_WalkPast::Update_State(const _float& timeDelta)
{
    CNPCState::Update_State(timeDelta);

    // Animation Finish Check
    if (m_OwnerAnimator && m_OwnerAnimator->Is_AnimationFinished())
        return static_cast<_int>(CNPCStateMachine::NPC_STATE::RUN);

#pragma region ㅎㅇ 효과음. 0.16초지나면 한 번 재생
    if (!m_SoundPlayed)
    {
        m_fSoundTimer += timeDelta;

        if (m_fSoundTimer >= 0.16f)
        {
            CSoundMgr::GetInstance()->Play_Once(L"Follower/wave4.wav", SOUND_EFFECT, 0.5f);

            m_fSoundTimer = 0.f;
            m_SoundPlayed = true;
        }
    }
#pragma endregion

    // 이동 로직
    if (m_Controller && m_OwnerTransform && m_OwnerRigidbody)
    {
        _vec3 targetPos = m_Controller->Get_TargetPos();
        targetPos.y     = 1.f;

        const _vec3  myPos        = m_OwnerTransform->Get_Pos();
        _vec3        vecToTarget  = targetPos - myPos;
        const _float distToTarget = D3DXVec3Length(&vecToTarget);

        // 도착
        if (distToTarget < 0.2f)
        {
            CGameObject* target = m_Controller->Get_TargetObject();

            // 타겟 오브젝트가 유효하거나, 혹은 특정 타겟 타입(PRAY_SITE 등)인 경우
            if ((target && !target->Is_PendingDestroy()) ||
                m_Controller->Get_TargetType() == CNPCController::NPC_TARGET_TYPE::PRAY_SITE)
            {
                switch (m_Controller->Get_TargetType())
                {
                case CNPCController::NPC_TARGET_TYPE::CONSTRUCTION:
                    return static_cast<_int>(CNPCStateMachine::NPC_STATE::BUILD);

                case CNPCController::NPC_TARGET_TYPE::FOOD:
                    return static_cast<_int>(CNPCStateMachine::NPC_STATE::EAT);

                case CNPCController::NPC_TARGET_TYPE::TREE:
                    return static_cast<_int>(CNPCStateMachine::NPC_STATE::CHOPWOOD);

                case CNPCController::NPC_TARGET_TYPE::ROCK:
                    return static_cast<_int>(CNPCStateMachine::NPC_STATE::MINING);

                case CNPCController::NPC_TARGET_TYPE::PRAY_SITE:
                    return static_cast<_int>(CNPCStateMachine::NPC_STATE::PRAY);

                default:
                    return static_cast<_int>(CNPCStateMachine::NPC_STATE::IDLE);
                }
            }

            return static_cast<_int>(CNPCStateMachine::NPC_STATE::IDLE);
        }

        D3DXVec3Normalize(&vecToTarget, &vecToTarget);

        // 애니메이션 방향
        if (m_Owner)
        {
            auto state = dynamic_cast<CState*>(m_Owner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::STATE));
            if (state)
            {
                if (vecToTarget.x > 0.1f) state->Change_Dir(ACTORDIR::RIGHT);
                else if (vecToTarget.x < -0.1f) state->Change_Dir(ACTORDIR::LEFT);
            }
        }

        _float speed = 0.0f;

        if (m_Controller)
        {
            auto stat = m_Controller->Get_OwnerCombatStat();
            if (stat) speed = stat->Get_Speed();
            speed *= 0.7f;
        }

        //m_OwnerRigidbody->Add_Force(vecToTarget * speed * 5.f);
        m_OwnerTransform->Move_Pos(vecToTarget, timeDelta, speed);
    }

    return static_cast<_int>(CNPCStateMachine::NPC_STATE::WALKPAST);
}

void CNPCState_WalkPast::Exit()
{
    CNPCState::Exit();

    //if (m_OwnerRigidbody)
    //    m_OwnerRigidbody->Set_Friction(0.3f);
}

HRESULT CNPCState_WalkPast::Ready_State()
{
    return S_OK;
}

CNPCState_WalkPast* CNPCState_WalkPast::Create(CNPCBase* owner, CNPCStateMachine* stateMachine)
{
    auto instance = new CNPCState_WalkPast(owner, stateMachine);

    if (FAILED(instance->Ready_State()))
    {
        Safe_Release(instance);
        return nullptr;
    }

    return instance;
}

void CNPCState_WalkPast::Free()
{
    CNPCState::Free();
}
