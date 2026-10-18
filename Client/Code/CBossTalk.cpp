#include "pch.h"
#include "CBossTalk.h"
#include "CMonsterState.h"
#include "CBossPhase1.h"
#include "CBossStateMachine.h"
#include "CTransform.h"
#include "CAnimator.h"
#include "CSpriteAnimation.h"
#include "CDInputMgr.h"

CBossTalk::CBossTalk(CBossPhase1* pOwner, CBossStateMachine* pStateMachine)
{
    m_pOwner = pOwner;
    m_pStateMachine = pStateMachine;
}

CBossTalk::~CBossTalk()
{
}

void CBossTalk::Enter()
{
    CMonsterState::Enter();
    m_pOwnerAnimatorCom->Play_Animation(L"Before_Idle", ANIMSTATE::LOOP);
    m_ePhase = PHASE::IDLE;
}

void CBossTalk::Exit()
{
    CMonsterState::Exit();
}

HRESULT CBossTalk::Ready_State()
{

    m_fMinStateTime = 0.f;

    m_pOwnerTransformCom = static_cast<CTransform*>(m_pOwner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));
    m_pOwnerAnimatorCom = static_cast<CAnimator*>(m_pOwner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::ANIMATOR));

    return S_OK;
}

_int CBossTalk::Update_State(const _float& timeDelta)
{
    CMonsterState::Update_State(timeDelta);

    if (m_pOwnerAnimatorCom->Get_CurAnimation() == nullptr)
        return CBossPhase1::BOSSPHASE1::TALK;

    switch (m_ePhase)
    {
    case PHASE::IDLE:
        if (CDInputMgr::GetInstance()->Get_DIKeyState(DIK_0))
            m_ePhase = PHASE::TALK;
        break;

    case PHASE::TALK://움직임 + idle
        if (m_pOwnerAnimatorCom->Get_CurFrame() == m_pOwnerAnimatorCom->Get_MaxFrame() - 1)
        {

        }

        if (CDInputMgr::GetInstance()->Get_DIKeyState(DIK_9))
            m_ePhase = PHASE::CAHANGE;
      
        break;
    case PHASE::CAHANGE:

        m_pOwnerAnimatorCom->Play_Animation(L"Escape", ANIMSTATE::LOOP);
        if (m_pOwnerAnimatorCom->Get_CurFrame() == m_pOwnerAnimatorCom->Get_MaxFrame() - 1)
        {
            return::CBossPhase1::BOSSAWAKE;
        }


        break;
    }



    return CBossPhase1::BOSSPHASE1::TALK;
}

void CBossTalk::LateUpdate_State(const _float& timeDelta)
{
}

void CBossTalk::Render_State()
{
}

CBossTalk* CBossTalk::Create(CBossPhase1* pOwner, CBossStateMachine* pStateMachine)
{
    auto BossTalk = new CBossTalk(pOwner, pStateMachine);

    if (FAILED(BossTalk->Ready_State()))
    {
        MSG_BOX("BossTalk Create Failed");
        Safe_Release(BossTalk);
        return nullptr;
    }

    return BossTalk;
}

void CBossTalk::Free()
{
}
