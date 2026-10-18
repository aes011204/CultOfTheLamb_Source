#include "pch.h"
#include "CBossGroggy.h"
#include "CMonsterState.h"
#include "CBossPhase1.h"
#include "CBossStateMachine.h"
#include "CTransform.h"
#include "CAnimator.h"
#include "CSpriteAnimation.h"

CBossGroggy::CBossGroggy(CBossPhase1* pOwner, CBossStateMachine* pStateMachine)
{
    m_pOwner = pOwner;
    m_pStateMachine = pStateMachine;
}

CBossGroggy::~CBossGroggy()
{

}

void CBossGroggy::Enter()
{
    CMonsterState::Enter();
    m_pOwnerAnimatorCom->Play_Animation(L"BossGroggy_start", ANIMSTATE::ONCE);
    m_ePhase = PHASE::DOWN;
}

void CBossGroggy::Exit()
{
    CMonsterState::Exit();

}

HRESULT CBossGroggy::Ready_State()
{
    m_fMinStateTime = 0.f;

    m_pOwnerTransformCom = static_cast<CTransform*>(m_pOwner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));
    m_pOwnerAnimatorCom = static_cast<CAnimator*>(m_pOwner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::ANIMATOR));

    return S_OK;
}

_int CBossGroggy::Update_State(const _float& timeDelta)
{
    CMonsterState::Update_State(timeDelta);

    if (m_pOwnerAnimatorCom->Get_CurAnimation() == nullptr)
        return CBossPhase1::BOSSPHASE1::BOSSGROGGRY;

    switch (m_ePhase)
    {
    case PHASE::DOWN:
        if (m_pOwnerAnimatorCom->Get_CurFrame() == m_pOwnerAnimatorCom->Get_MaxFrame() - 1)
            m_ePhase = PHASE::LOOP;
        break;

    case PHASE::LOOP:
        m_pOwnerAnimatorCom->Play_Animation(L"BossGroggy_loop", ANIMSTATE::ONCE);
        if (m_pOwnerAnimatorCom->Get_CurFrame() == m_pOwnerAnimatorCom->Get_MaxFrame() - 1)
        {
            m_ePhase = PHASE::UP;
        }
        break;

    case PHASE::UP:
        m_pOwnerAnimatorCom->Play_Animation(L"BossGroggy_end", ANIMSTATE::ONCE);
        if (m_pOwnerAnimatorCom->Get_CurFrame() == m_pOwnerAnimatorCom->Get_MaxFrame() - 1)
            return CBossPhase1::BOSSPHASE1::BOSSIDLE;// 보스 끝남 
        break;
    }
    return CBossPhase1::BOSSPHASE1::BOSSGROGGRY;

}

void CBossGroggy::LateUpdate_State(const _float& timeDelta)
{
}

void CBossGroggy::Render_State()
{
}

CBossGroggy* CBossGroggy::Create(CBossPhase1* pOwner, CBossStateMachine* pStateMachine)
{
    auto BossGroggy = new CBossGroggy(pOwner, pStateMachine);

    if (FAILED(BossGroggy->Ready_State()))
    {
        MSG_BOX("BossGroggy Create Failed");
        Safe_Release(BossGroggy);
        return nullptr;
    }

    return BossGroggy;
}

void CBossGroggy::Free()
{

}
