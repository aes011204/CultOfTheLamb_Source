#include "pch.h"
#include "CEligos_Idle.h"
#include "CMonsterState.h"
#include "CEligos.h"
#include "CEligos_StateMachine.h"
#include "CTransform.h"
#include "CAnimator.h"
#include "CSceneMgr.h"
#include "CSpriteAnimation.h"
#include "CCombatStat.h"

int CEligos_Idle::ChangeAPattern = 0;

CEligos_Idle::CEligos_Idle(CEligos* pOwner, CEligos_StateMachine* pStateMachine)
{
    m_pOwner = pOwner;
    m_pStateMachine = pStateMachine;
}

CEligos_Idle::~CEligos_Idle()
{
}

void CEligos_Idle::Enter()
{
    CMonsterState::Enter();
    // 일단 지금은 
    m_pOwnerAnimatorCom->Play_Animation(L"Eligos_fly", ANIMSTATE::LOOP);

}

void CEligos_Idle::Exit()
{
    CMonsterState::Exit();
}

HRESULT CEligos_Idle::Ready_State()
{
    m_fMinStateTime = 0.f;

    m_pOwnerTransformCom = static_cast<CTransform*>(m_pOwner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));
    m_pOwnerAnimatorCom = static_cast<CAnimator*>(m_pOwner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::ANIMATOR));
    m_pOwnerStateCom = static_cast<CCombatStat*>(m_pOwner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::COMBATSTAT));

    return S_OK;
}

_int CEligos_Idle::Update_State(const _float& timeDelta)
{
    CMonsterState::Update_State(timeDelta);

    if (m_pOwnerAnimatorCom->Get_CurAnimation() == nullptr)
        return CEligos::ELIGOSSTATE::BOSSIDLE;

    // 여기는 애니 없이 바로

    if (m_pOwnerStateCom->Get_Hp() <= 0)
    {
        return  CEligos::ELIGOSSTATE::BOSSDEAD;
    }

    Find_Player();
        _vec3 tmpOwnerPos = { m_pOwnerTransformCom->Get_Pos().x,m_pPlayerTransformCom->Get_Pos().y,m_pOwnerTransformCom->Get_Pos().z };
        _vec3 vec = m_pPlayerTransformCom->Get_Pos() - tmpOwnerPos;
        float length = D3DXVec3Length(&vec);



    // 어웨이크 -> 어택
    if (m_pStateMachine->Get_PrevStateKey() == (int)CEligos::ELIGOSSTATE::BOSSREVEAL)
    {
        // 플레이어가 일정범위 안이면
        if (length <= 5.f)
        {
            return  CEligos::ELIGOSSTATE::BOSSDEAD;
        }
        else
        {
        return  CEligos::ELIGOSSTATE::ATTACK_BULLET;

        }
    }

    ////어택->이동
    //if (m_pStateMachine->Get_PrevStateKey() == (int)CEligos::ELIGOSSTATE::ATTACK_BULLET ||
    //    m_pStateMachine->Get_PrevStateKey() == (int)CEligos::ELIGOSSTATE::ATTACK_FLY ||
    //    m_pStateMachine->Get_PrevStateKey() == (int)CEligos::ELIGOSSTATE::ATTACK_MELEE)
    //{

    //    // 플레이어가 일정범위 안이면
    //    if (length <= 5.f)
    //    {
    //        cout << "melee\n";

    //        return  CEligos::ELIGOSSTATE::ATTACK_MELEE;
    //    }
    //    else
    //    {
    //    ChangeAPattern %= 2;
    //    int tmp = ChangeAPattern;
    //    ChangeAPattern++;

    //    //if ((CRandomMgr::GetInstance()->Get_Int_Random(0, 1)) == 1) // 랜덤
    //    if (tmp == 1)
    //    {

    //        cout << "fly\n";
    //        return CEligos::ELIGOSSTATE::ATTACK_FLY;
    //    }
    //    else
    //    {
    //        cout << "bullet\n";

    //        return CEligos::ELIGOSSTATE::ATTACK_BULLET;

    //    }

    //    }
    //}


    return  CEligos::ELIGOSSTATE::BOSSIDLE;
}

void CEligos_Idle::LateUpdate_State(const _float& timeDelta)
{
}

void CEligos_Idle::Render_State()
{
}

CEligos_Idle* CEligos_Idle::Create(CEligos* pOwner, CEligos_StateMachine* pStateMachine)
{
    auto BossDead = new CEligos_Idle(pOwner, pStateMachine);

    if (FAILED(BossDead->Ready_State()))
    {
        MSG_BOX("BossDead Create Failed");
        Safe_Release(BossDead);
        return nullptr;
    }

    return BossDead;
}

void CEligos_Idle::Free()
{
}
