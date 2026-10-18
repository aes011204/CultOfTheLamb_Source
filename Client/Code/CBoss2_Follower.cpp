#include "pch.h"
#include "CBoss2_Follower.h"
#include "CBossController.h"
#include "CBossPhase2.h"
#include "CAnimator.h"
#include "CUI_Dialogue.h"
#include "CTransform.h"
#include "CBoxCollider.h"

CBoss2_Follower::CBoss2_Follower(CGameObject* owner)
    : m_Owner(owner)
{

}

CBoss2_Follower::~CBoss2_Follower()
{

}

void CBoss2_Follower::Enter()
{
    CMonsterState::Enter();

    auto boss = static_cast<CBossPhase2*>(m_Owner);
    m_Controller = boss->Get_Controller();

    if (m_Controller)
    {
        auto animator = m_Controller->Get_OwnerAnimator();
        if (animator)
            animator->Play_Animation(L"Boss2_Follower_Transform", ANIMSTATE::ONCE);
    }

    auto transform = m_Controller->Get_OwnerTransform();
    if (transform)
    {
        m_StartPos = transform->Get_Pos();
        m_TargetPos = m_StartPos;
        m_TargetPos.y -= 0.3f;
        m_TargetPos.z -= 10.f;
    }

    auto collider = dynamic_cast<CBoxCollider*>(m_Owner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::BOX_COLL));
    if (collider)
    {
        collider->Set_Size(_vec3(0.2f, 0.26f, 0.51f));
        collider->Set_Offset(_vec3(0.13f, -4.48f, -0.25f));
    }

    m_fCurStateElapsedTime = 0.f;
    m_DialogueStarted = false;
    m_IsFinished = false;
    m_Unconverted = false;
    m_MoveProgress = 0.f;
}

_int CBoss2_Follower::Update_State(const _float& timeDelta)
{
    m_fCurStateElapsedTime += timeDelta;

    auto animator = m_Controller->Get_OwnerAnimator();
    auto transform = m_Controller->Get_OwnerTransform();

    if (!m_Unconverted && transform)
    {
        const _float moveDuration = 1.0f; 
        m_MoveProgress += timeDelta / moveDuration;
        m_MoveProgress = min(m_MoveProgress, 1.f);

        _vec3 newPos;
        D3DXVec3Lerp(&newPos, &m_StartPos, &m_TargetPos, m_MoveProgress);
        transform->Set_Pos(newPos);
    }

    if (animator && animator->Is_AnimationFinished() && !m_Unconverted)
    {
        m_Unconverted = true;
        m_DialogueStarted = true;
        animator->Play_Animation(L"Boss2_Follower_UnConverted", ANIMSTATE::LOOP);

        if (transform)
            transform->Set_Pos(m_TargetPos);

        // 대사 시작
        Start_Dialogue();
        
    }

    return CBossPhase2::PATTERN::PHASE2_FOLLOWER;
}

void CBoss2_Follower::Exit()
{
    CMonsterState::Exit();

    // 상태 끝날 때
}

HRESULT CBoss2_Follower::Ready_State()
{
    m_fMinStateTime = 0.5f;
    m_StartPos = _vec3(0.f, 0.f, 0.f);
    m_TargetPos = _vec3(0.f, 0.f, 0.f);

    return S_OK;
}

void CBoss2_Follower::Start_Dialogue()
{
    auto boss = static_cast<CBossPhase2*>(m_Owner);

    boss->Trigger_OnFollowerStart();
}

CBoss2_Follower* CBoss2_Follower::Create(CGameObject* owner)
{
    auto state = new CBoss2_Follower(owner);

    if (FAILED(state->Ready_State()))
    {
        Safe_Release(state);
        MSG_BOX("Boss2 Follower Create Failed");
        return nullptr;
    }

    return state;
}

void CBoss2_Follower::Free()
{
    
}
