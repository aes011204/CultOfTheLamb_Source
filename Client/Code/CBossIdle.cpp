#include "pch.h"
#include "CBossIdle.h"
#include "CMonsterState.h"
#include "CBossPhase1.h"
#include "CBossStateMachine.h"
#include "CAnimator.h"
#include "CSpriteAnimation.h"
#include "CTransform.h"
#include "CCombatStat.h"
#include "CRandomMgr.h"

int CBossIdle::ChangeAPattern = 0;

CBossIdle::CBossIdle(CBossPhase1* pOwner, CBossStateMachine* pStateMachine)
{
    m_pOwner = pOwner;
    m_pStateMachine = pStateMachine;
}

CBossIdle::~CBossIdle()
{
}

void CBossIdle::Enter()
{
    CMonsterState::Enter();
    m_pOwnerAnimatorCom->Play_Animation(L"BossIdle", ANIMSTATE::LOOP);

}

void CBossIdle::Exit()
{
    CMonsterState::Exit();
}

HRESULT CBossIdle::Ready_State()
{
    m_fMinStateTime = 0.f;

    m_pOwnerTransformCom = static_cast<CTransform*>(m_pOwner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));
    m_pOwnerAnimatorCom = static_cast<CAnimator*>(m_pOwner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::ANIMATOR));
    m_pOwnerStateCom = static_cast<CCombatStat*>(m_pOwner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::COMBATSTAT));

    return S_OK;
}

_int CBossIdle::Update_State(const _float& timeDelta)
{
    CMonsterState::Update_State(timeDelta);

    if (m_pOwnerAnimatorCom->Get_CurAnimation() == nullptr)
        return CBossPhase1::BOSSPHASE1::BOSSIDLE;

    // 여기는 애니 없이 바로

    if (m_pOwnerStateCom->Get_Hp() <= 0)
    {
        return CBossPhase1::BOSSPHASE1::BOSSDEAD;
    }


    // 어웨이크 -> 어택
    if (m_pStateMachine->Get_PrevStateKey() == (int)CBossPhase1::BOSSPHASE1::BOSSAWAKE)
    {
        return CBossPhase1::BOSSPHASE1::BULLETATTACK;
    }
    
    //어택->이동
    if (m_pStateMachine->Get_PrevStateKey() == (int)CBossPhase1::BOSSPHASE1::BULLETATTACK ||
        m_pStateMachine->Get_PrevStateKey() == (int)CBossPhase1::BOSSPHASE1::CHAINATTACK)
    {
        return CBossPhase1::BOSSPHASE1::BOSSMOVE;
    }
    

    // 여기서는 애니는 이미 Idle LOOP 중이라고 가정
    // (Enter에서 재생)
    
        // 이동 ->공격
    if (m_pStateMachine->Get_PrevStateKey() == (int)CBossPhase1::BOSSPHASE1::BOSSMOVE)
    {
        // 전이 이동이 였으면 idle 한번 돌고 공격으로 넘어감
        m_pOwnerAnimatorCom->Play_Animation(L"BossIdle", ANIMSTATE::ONCE);
        if (m_pOwnerAnimatorCom->Get_CurFrame() == m_pOwnerAnimatorCom->Get_MaxFrame()-1)
        {
            ChangeAPattern %= 2;
            int tmp = ChangeAPattern;
            ChangeAPattern++;
           
            //if ((CRandomMgr::GetInstance()->Get_Int_Random(0, 1)) == 1) // 랜덤
            if(tmp ==1)
                return CBossPhase1::BOSSPHASE1::BULLETATTACK;
            else
                return CBossPhase1::BOSSPHASE1::CHAINATTACK;
    
        }
    }

    return CBossPhase1::BOSSPHASE1::BOSSIDLE;


}

void CBossIdle::LateUpdate_State(const _float& timeDelta)
{
}

CBossIdle* CBossIdle::Create(CBossPhase1* pOwner, CBossStateMachine* pStateMachine)
{
    auto BossIdle = new CBossIdle(pOwner, pStateMachine);

    if (FAILED(BossIdle->Ready_State()))
    {
        MSG_BOX("BossIdle Create Failed");
        Safe_Release(BossIdle);
        return nullptr;
    }

    return BossIdle;
}

void CBossIdle::Free()
{
}
