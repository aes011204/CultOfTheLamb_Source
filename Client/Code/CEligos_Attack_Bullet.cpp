#include "pch.h"
#include "CEligos_Attack_Bullet.h"
#include "CMonsterState.h"
#include "CEligos.h"
#include "CEligos_StateMachine.h"
#include "CTransform.h"
#include "CAnimator.h"
#include "CSceneMgr.h"
#include "CSpriteAnimation.h"

CEligos_Attack_Bullet::CEligos_Attack_Bullet(CEligos* pOwner, CEligos_StateMachine* pStateMachine)
{
    m_pOwner = pOwner;
    m_pStateMachine = pStateMachine;
}

CEligos_Attack_Bullet::~CEligos_Attack_Bullet()
{

}

void CEligos_Attack_Bullet::Enter()
{
    CMonsterState::Enter();
    // 일단 지금은 
    m_pOwnerAnimatorCom->Play_Animation(L"Eligos_fly", ANIMSTATE::LOOP);

    m_ePhase = PHASE::MOVE;
}

void CEligos_Attack_Bullet::Exit()
{
    CMonsterState::Exit();
}

HRESULT CEligos_Attack_Bullet::Ready_State()
{
    m_fMinStateTime = 0.f;

    m_pOwnerTransformCom = static_cast<CTransform*>(m_pOwner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));
    m_pOwnerAnimatorCom = static_cast<CAnimator*>(m_pOwner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::ANIMATOR));

    return S_OK;
}

_int CEligos_Attack_Bullet::Update_State(const _float& timeDelta)
{
    CMonsterState::Update_State(timeDelta);

    if (m_pOwnerAnimatorCom->Get_CurAnimation() == nullptr)
        return CEligos::ELIGOSSTATE::ATTACK_BULLET;

    switch (m_ePhase)
    {
    case PHASE::MOVE:

        if (m_pOwnerAnimatorCom->Get_CurFrame() == m_pOwnerAnimatorCom->Get_MaxFrame() - 1)
        {
            // 이거 말고 제 위치에 도달 하면 레디로 가자
            m_ePhase = PHASE::READY;

            //if (m_OnTalkStart)
            //    m_OnTalkStart();
        }

        break;

    case PHASE::READY:
        m_pOwnerAnimatorCom->Play_Animation(L"Eligos_attackcharge_bullet", ANIMSTATE::ONCE);

        if (m_pOwnerAnimatorCom->Get_CurFrame() == m_pOwnerAnimatorCom->Get_MaxFrame() - 1)
        {
            m_ePhase = PHASE::SHOOT;

        }
        break;

    case PHASE::SHOOT: //움직이면서 쏘는거
        m_pOwnerAnimatorCom->Play_Animation(L"Eligos_attack_bullet", ANIMSTATE::ONCE);

        if (m_pOwnerAnimatorCom->Get_CurFrame() == m_pOwnerAnimatorCom->Get_MaxFrame() - 1)
        {
            

            return CEligos::ELIGOSSTATE::BOSSIDLE;   
        }
    }

    return CEligos::ELIGOSSTATE::ATTACK_BULLET;
}

void CEligos_Attack_Bullet::LateUpdate_State(const _float& timeDelta)
{
}

void CEligos_Attack_Bullet::Render_State()
{
}

CEligos_Attack_Bullet* CEligos_Attack_Bullet::Create(CEligos* pOwner, CEligos_StateMachine* pStateMachine)
{
    auto BossDead = new CEligos_Attack_Bullet(pOwner, pStateMachine);

    if (FAILED(BossDead->Ready_State()))
    {
        MSG_BOX("BossDead Create Failed");
        Safe_Release(BossDead);
        return nullptr;
    }

    return BossDead;
}

void CEligos_Attack_Bullet::Free()
{

}
