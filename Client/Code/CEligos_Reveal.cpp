#include "pch.h"
#include "CEligos_Reveal.h"
#include "CMonsterState.h"
#include "CEligos.h"
#include "CEligos_StateMachine.h"
#include "CTransform.h"
#include "CAnimator.h"
#include "CSceneMgr.h"
#include "CSpriteAnimation.h"


CEligos_Reveal::CEligos_Reveal(CEligos* pOwner, CEligos_StateMachine* pStateMachine)
{
    m_pOwner = pOwner;
    m_pStateMachine = pStateMachine;
}

CEligos_Reveal::~CEligos_Reveal()
{
}

void CEligos_Reveal::Enter()
{
    CMonsterState::Enter();
    // 일단 지금은 
    m_pOwnerAnimatorCom->Play_Animation(L"Eligos_roar", ANIMSTATE::ONCE);


}

void CEligos_Reveal::Exit()
{
    CMonsterState::Exit();
}

HRESULT CEligos_Reveal::Ready_State()
{
    m_fMinStateTime = 0.f;

    m_pOwnerTransformCom = static_cast<CTransform*>(m_pOwner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));
    m_pOwnerAnimatorCom = static_cast<CAnimator*>(m_pOwner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::ANIMATOR));

    return S_OK;
}

_int CEligos_Reveal::Update_State(const _float& timeDelta)
{
    CMonsterState::Update_State(timeDelta);

    if (m_pOwnerAnimatorCom->Get_CurAnimation() == nullptr)
        return CEligos::ELIGOSSTATE::BOSSREVEAL;

    if (m_pOwnerAnimatorCom->Get_CurFrame() == m_pOwnerAnimatorCom->Get_MaxFrame() - 1)
    {
        return CEligos::ELIGOSSTATE::BOSSIDLE;
    }


    return  CEligos::ELIGOSSTATE::BOSSREVEAL;
}

void CEligos_Reveal::LateUpdate_State(const _float& timeDelta)
{
}

void CEligos_Reveal::Render_State()
{
}

CEligos_Reveal* CEligos_Reveal::Create(CEligos* pOwner, CEligos_StateMachine* pStateMachine)
{
    auto BossDead = new CEligos_Reveal(pOwner, pStateMachine);

    if (FAILED(BossDead->Ready_State()))
    {
        MSG_BOX("BossDead Create Failed");
        Safe_Release(BossDead);
        return nullptr;
    }

    return BossDead;
}

void CEligos_Reveal::Free()
{
}
