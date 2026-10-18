#include "pch.h"
#include "CBossAwake.h"
#include "CMonsterState.h"
#include "CBossPhase1.h"
#include "CBossStateMachine.h"
#include "CTransform.h"
#include "CAnimator.h"
#include "CSpriteAnimation.h"

CBossAwake::CBossAwake(CBossPhase1* pOwner, CBossStateMachine* pStateMachine)
{
    m_pOwner = pOwner;
    m_pStateMachine = pStateMachine;
}

CBossAwake::~CBossAwake()
{
}

void CBossAwake::Enter()
{
    CMonsterState::Enter();
    m_pOwnerAnimatorCom->Play_Animation(L"BossAwake", ANIMSTATE::ONCE);
    m_ePhase = PHASE::AWAKE;
}

void CBossAwake::Exit()
{
    CMonsterState::Exit();

}

HRESULT CBossAwake::Ready_State()
{
    //m_CurStateKey = CBoss::BOSS_STATE::AWAKE;

    m_fMinStateTime = 0.f;

    m_pOwnerTransformCom = static_cast<CTransform*>(m_pOwner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));
    m_pOwnerAnimatorCom = static_cast<CAnimator*>(m_pOwner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::ANIMATOR));

    return S_OK;
}

_int CBossAwake::Update_State(const _float& timeDelta)
{
    CMonsterState::Update_State(timeDelta);

    if (m_pOwnerAnimatorCom->Get_CurAnimation() == nullptr)
        return CBossPhase1::BOSSPHASE1::BOSSAWAKE;

    switch (m_ePhase)
    {
    case PHASE::AWAKE:
        //m_pOwnerAnimatorCom->Play_Animation(L"BossAwake", ANIMSTATE::ONCE);

        if (m_pOwnerAnimatorCom->Get_CurFrame() == m_pOwnerAnimatorCom->Get_MaxFrame() - 1)
            m_ePhase = PHASE::MOVE;
        break;

    case PHASE::MOVE://움직임 + idle

        
        m_pOwnerAnimatorCom->Play_Animation(L"BossIdle", ANIMSTATE::LOOP);
        if (m_pOwnerAnimatorCom->Get_CurFrame() == m_pOwnerAnimatorCom->Get_MaxFrame() - 1)
        {
     
        }
            _vec3 tmpdir = (m_pOwner->Get_Center() - m_pOwnerTransformCom->Get_Pos());
            D3DXVec3Normalize(&dir, &(tmpdir));
         //   tmpAniDone = true;

        float distance = D3DXVec3Length(&tmpdir);

       // if (tmpAniDone == true)
        {
            m_pOwnerTransformCom->Set_Pos(m_pOwnerTransformCom->Get_Pos() + dir * m_Speed * timeDelta);

            if (distance <= 0.1f)
            {
                tmpAniDone = false;
                m_pOwnerTransformCom->Set_Pos((m_pOwner->Get_Center()));
                return CBossPhase1::BOSSPHASE1::BOSSIDLE;// 이거 아님 공격상태전환 아이들로 

            }
        }
            break;
    }



    return CBossPhase1::BOSSPHASE1::BOSSAWAKE;
}

void CBossAwake::LateUpdate_State(const _float& timeDelta)
{
}

void CBossAwake::Render_State()
{

   
}

CBossAwake* CBossAwake::Create(CBossPhase1* pOwner, CBossStateMachine* pStateMachine)
{
    auto BossAwake = new CBossAwake(pOwner, pStateMachine);

    if (FAILED(BossAwake->Ready_State()))
    {
        MSG_BOX("BossAwake Create Failed");
        Safe_Release(BossAwake);
        return nullptr;
    }

    return BossAwake;
}

void CBossAwake::Free()
{

}
