#include "pch.h"
#include "CEligos_Attack_Melee.h"
#include "CMonsterState.h"
#include "CEligos.h"
#include "CEligos_StateMachine.h"
#include "CTransform.h"
#include "CAnimator.h"
#include "CSceneMgr.h"
#include "CSpriteAnimation.h"

CEligos_Attack_Melee::CEligos_Attack_Melee(CEligos* pOwner, CEligos_StateMachine* pStateMachine)
{
    m_pOwner = pOwner;
    m_pStateMachine = pStateMachine;
}

CEligos_Attack_Melee::~CEligos_Attack_Melee()
{

}

void CEligos_Attack_Melee::Enter()
{
    CMonsterState::Enter();
    // 일단 지금은 
    m_pOwnerAnimatorCom->Play_Animation(L"Eligos_attackcharge", ANIMSTATE::ONCE);

    m_ePhase = PHASE::READY;
}

void CEligos_Attack_Melee::Exit()
{
    CMonsterState::Exit();
}

HRESULT CEligos_Attack_Melee::Ready_State()
{
    m_fMinStateTime = 0.f;

    m_pOwnerTransformCom = static_cast<CTransform*>(m_pOwner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));
    m_pOwnerAnimatorCom = static_cast<CAnimator*>(m_pOwner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::ANIMATOR));

    return S_OK;
}

_int CEligos_Attack_Melee::Update_State(const _float& timeDelta)
{
    CMonsterState::Update_State(timeDelta);

    if (m_pOwnerAnimatorCom->Get_CurAnimation() == nullptr)
        return CEligos::ELIGOSSTATE::ATTACK_MELEE;

    switch (m_ePhase)
    {


    case PHASE::READY:

        if (m_pOwnerAnimatorCom->Get_CurFrame() == m_pOwnerAnimatorCom->Get_MaxFrame() - 1)
        {
            m_ePhase = PHASE::SHOOT;

        }
        break;

    case PHASE::SHOOT: //움직이면서 쏘는거
        m_pOwnerAnimatorCom->Play_Animation(L"Eligos_attack", ANIMSTATE::ONCE);

        if (m_pOwnerAnimatorCom->Get_CurFrame() == m_pOwnerAnimatorCom->Get_MaxFrame() - 1)
        {


            return CEligos::ELIGOSSTATE::BOSSIDLE;
        }
    }

    return CEligos::ELIGOSSTATE::ATTACK_MELEE;
}

void CEligos_Attack_Melee::LateUpdate_State(const _float& timeDelta)
{
}

void CEligos_Attack_Melee::Render_State()
{
}

CEligos_Attack_Melee* CEligos_Attack_Melee::Create(CEligos* pOwner, CEligos_StateMachine* pStateMachine)
{
    auto BossDead = new CEligos_Attack_Melee(pOwner, pStateMachine);

    if (FAILED(BossDead->Ready_State()))
    {
        MSG_BOX("BossDead Create Failed");
        Safe_Release(BossDead);
        return nullptr;
    }

    return BossDead;
}

void CEligos_Attack_Melee::Free()
{

}
