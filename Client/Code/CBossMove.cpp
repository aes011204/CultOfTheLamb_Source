#include "pch.h"
#include "CBossMove.h"
#include "CMonsterState.h"
#include "CBossPhase1.h"
#include "CBossStateMachine.h"
#include "CTransform.h"
#include "CAnimator.h"
#include "CSpriteAnimation.h"
#include "CRandomMgr.h"
#include "CCollider.h"

CBossMove::CBossMove(CBossPhase1* pOwner, CBossStateMachine* pStateMachine)
{
    m_pOwner = pOwner;
    m_pStateMachine = pStateMachine;
}

CBossMove::~CBossMove()
{
}

void CBossMove::Enter()
{
    CMonsterState::Enter();
    m_pOwnerAnimatorCom->Play_Animation(L"BossMove_Down", ANIMSTATE::ONCE);
    m_ePhase = PHASE::DOWN;
}

void CBossMove::Exit()
{
    CMonsterState::Exit();

}

HRESULT CBossMove::Ready_State()
{
    m_fMinStateTime = 0.f;

    m_pOwnerTransformCom = static_cast<CTransform*>(m_pOwner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));
    m_pOwnerAnimatorCom = static_cast<CAnimator*>(m_pOwner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::ANIMATOR));

    return S_OK;
}

_int CBossMove::Update_State(const _float& timeDelta)
{
    CMonsterState::Update_State(timeDelta);

    if (m_pOwnerAnimatorCom->Get_CurAnimation() == nullptr)
        return CBossPhase1::BOSSPHASE1::BOSSMOVE;

    switch (m_ePhase)
    {
    case PHASE::DOWN:

        if (m_pOwnerAnimatorCom->Get_CurFrame() == m_pOwnerAnimatorCom->Get_MaxFrame() - 1)
        {
            m_ePhase = PHASE::WAIT;
            dynamic_cast<CCollider*>(m_pOwner->Get_Component(COMPONENTID::ID_DYNAMIC, COMPONENTTYPE::BOX_COLL))->Set_CollisionActive(false);
        }
        break;

    case PHASE::WAIT: //움직임


        m_fCurStateElapsedTime += timeDelta;

        if (m_fCurStateElapsedTime >= 3.0f)
        {
            _vec3 BaseCenter = m_pOwner->Get_Center();
            int BaseLength = m_pOwner->Get_Length();
            float interval = 1.f;
            // 일단 나중에 밖으로 뺴서 체인이랑 합치기
            RandomMove(timeDelta, BaseCenter, BaseLength, interval);

            dynamic_cast<CCollider*>(m_pOwner->Get_Component(COMPONENTID::ID_DYNAMIC, COMPONENTTYPE::BOX_COLL))->Set_CollisionActive(true);


            m_fCurStateElapsedTime = 0;
            m_ePhase = PHASE::UP;
        }
        break;

    case PHASE::UP://움직임 + idle

        m_pOwnerAnimatorCom->Play_Animation(L"BossMove_up", ANIMSTATE::ONCE);
        if (m_pOwnerAnimatorCom->Get_CurFrame() == m_pOwnerAnimatorCom->Get_MaxFrame() - 1)
            return CBossPhase1::BOSSPHASE1::BOSSIDLE;// 이거 아님 공격상태전환 아이들로 
        break;
    }

    return CBossPhase1::BOSSPHASE1::BOSSMOVE;
}

void CBossMove::RandomMove(const _float& timeDelta, _vec3 BaseCenter, int BaseLenght, float interval)
{
    _vec3 LT = { BaseCenter.x - BaseLenght * 0.5f, 0.f, BaseCenter.z + BaseLenght * 0.5f };
    _vec3 RB = { BaseCenter.x + BaseLenght * 0.5f,0.f,BaseCenter.z - BaseLenght * 0.5f };

    int offset = 5;// TOOD:: 나중에 맴버 변수로 오프셋 조정 (사이의 거리)

    int xCell = BaseLenght / offset;
    int zCell = BaseLenght / offset;


    int tmpx = CRandomMgr::GetInstance()->Get_Int_Random(0, xCell); // x 몇번쨰 칸인지는 랜덤
    int tmpz = CRandomMgr::GetInstance()->Get_Int_Random(0, zCell); // x 몇번쨰 칸인지는 랜덤

    float xPos = tmpx * (offset);
    float zPos = tmpz * (offset);

    m_pOwnerTransformCom->Set_Pos(LT.x + xPos, m_pOwnerTransformCom->Get_Pos().y, LT.z - zPos);

}

void CBossMove::LateUpdate_State(const _float& timeDelta)
{
}

CBossMove* CBossMove::Create(CBossPhase1* pOwner, CBossStateMachine* pStateMachine)
{
    auto BossMove = new CBossMove(pOwner, pStateMachine);

    if (FAILED(BossMove->Ready_State()))
    {
        MSG_BOX("BossMove Create Failed");
        Safe_Release(BossMove);
        return nullptr;
    }

    return BossMove;
}

void CBossMove::Free()
{
}
