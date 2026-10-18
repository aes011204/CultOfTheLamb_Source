#include "pch.h"
#include "CEligos_StateMachine.h"
#include "CEligos.h"

#include "CEligos_Reveal.h"
#include "CEligos_Dead.h"
#include "CEligos_Move.h"
#include "CEligos_Idle.h"
#include "CEligos_Attack_Bullet.h"
#include "CEligos_Attack_Melee.h"
#include "CEligos_Attack_Fly.h"
#include "CEligos_Idle.h"

CEligos_StateMachine::CEligos_StateMachine(CMonsterBase* owner) : m_pOwner(owner)
{
}

CEligos_StateMachine::~CEligos_StateMachine()
{
}

HRESULT CEligos_StateMachine::Ready_StateMachine()
{
    Ready_BossStates();

    m_CurState = m_vecState[CEligos::ELIGOSSTATE::BOSSREVEAL];

    m_CurState->Enter();

    return S_OK;
}

_int CEligos_StateMachine::Update_StateMachine(const _float& timeDelta)
{
    // 씬메니져의 리퀘스트 씬 같은 느낌?
    if (m_CurState)
    {
        _uint nextState = m_CurState->Update_State(timeDelta);

        if (nextState == CEligos::ELIGOSSTATE::BOSS_END)
        {
            return nextState;
        }

        if (nextState < CEligos::ELIGOSSTATE::BOSS_END && nextState != m_CurStateKey)
        {
            Change_State(nextState);
        }
    }

    return (_int)m_CurStateKey;
}

void CEligos_StateMachine::LateUpdate_StateMachine(const _float& timeDelta)
{
    m_CurState->LateUpdate_State(timeDelta);
}

void CEligos_StateMachine::Change_State(_uint changeStateKey)
{
    // 체인지 씬이랑 비슷한 느낌

    if (m_CurStateKey == changeStateKey)
        return; // 이전이랑 같은 상황

    if (m_vecState[changeStateKey] == nullptr || changeStateKey >= CEligos::ELIGOSSTATE::BOSS_END)
        return; // 인덱스 범위 확인

    // 이전 상태 나가기
    if (m_CurState)
        m_CurState->Exit();

    // 이전 상태 등록
    //m_pPrevState = m_CurState;
    m_PrevStateKey = m_CurStateKey;
    m_CurStateKey = changeStateKey;

    // 새 상태로 가져오기
    m_CurState = m_vecState[changeStateKey];
    m_CurState->Enter();

}


CMonsterState* CEligos_StateMachine::Get_State(_uint stateKey)
{
    if (stateKey < m_vecState.size())
        return static_cast<CMonsterState*>(m_vecState[stateKey]);

    return nullptr;
}

_int CEligos_StateMachine::GetCurStateNum()
{
    { return m_CurState->Get_StateKey(); }
}

HRESULT CEligos_StateMachine::Ready_BossStates()
{
    m_vecState.resize(CEligos::ELIGOSSTATE::BOSS_END);
    //m_vecState.resize(2);

    CMonsterState* pState = nullptr;

    CEligos* Eligos = dynamic_cast<CEligos*>(m_pOwner);
    NULL_CHECK_RETURN(Eligos, E_FAIL);

    if (!(pState = CEligos_Reveal::Create(Eligos, this)))
        return E_FAIL;
    m_vecState[CEligos::ELIGOSSTATE::BOSSREVEAL] = pState;

    if (nullptr == (pState = CEligos_Attack_Bullet::Create(Eligos, this)))
        return E_FAIL;
    m_vecState[CEligos::ELIGOSSTATE::ATTACK_BULLET] = pState;

    if (nullptr == (pState = CEligos_Attack_Fly::Create(Eligos, this)))
        return E_FAIL;
    m_vecState[CEligos::ELIGOSSTATE::ATTACK_FLY] = pState;

    if (nullptr == (pState = CEligos_Attack_Melee::Create(Eligos, this)))
        return E_FAIL;
    m_vecState[CEligos::ELIGOSSTATE::ATTACK_MELEE] = pState;

    if (nullptr == (pState = CEligos_Dead::Create(Eligos, this)))
        return E_FAIL;
    m_vecState[CEligos::ELIGOSSTATE::BOSSDEAD] = pState;

    if (nullptr == (pState = CEligos_Idle::Create(Eligos, this)))
        return E_FAIL;
    m_vecState[CEligos::ELIGOSSTATE::BOSSIDLE] = pState;

    if (nullptr == (pState = CEligos_Move::Create(Eligos, this)))
        return E_FAIL;
    m_vecState[CEligos::ELIGOSSTATE::BOSSMOVE] = pState;

    //if (nullptr == (pState = CEligos_::Create(Eligos, this)))
    //    return E_FAIL;
    //m_vecState[CBossPhase1::BOSSPHASE1::TALK] = pState;

    return S_OK;
}

CEligos_StateMachine* CEligos_StateMachine::Create(CMonsterBase* owner)
{
    auto StateMachine = new CEligos_StateMachine(owner);
    if (FAILED(StateMachine->Ready_StateMachine()))
    {
        Safe_Release(StateMachine);
        MSG_BOX("StateMachine Created Failed");
        return nullptr;
    }

    return StateMachine;
}

void CEligos_StateMachine::Free()
{
    CStateMachine::Free();
}
