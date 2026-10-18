#include "pch.h"
#include "CEligos_Attack_Fly.h"
#include "CMonsterState.h"
#include "CEligos.h"
#include "CEligos_StateMachine.h"
#include "CCreateHelper.h"
#include "CTransform.h"
#include "CAnimator.h"
#include "CSceneMgr.h"
#include "CSpriteAnimation.h"
#include "CEligos_Flies.h"
#include "CRandomMgr.h"

CEligos_Attack_Fly::CEligos_Attack_Fly(CEligos* pOwner, CEligos_StateMachine* pStateMachine)
{
    m_pOwner = pOwner;
    m_pStateMachine = pStateMachine;
}

CEligos_Attack_Fly::~CEligos_Attack_Fly()
{

}

void CEligos_Attack_Fly::Enter()
{
    CMonsterState::Enter();
    // 일단 지금은 
    m_pOwnerAnimatorCom->Play_Animation(L"Eligos_roar", ANIMSTATE::ONCE);

    m_ePhase = PHASE::WAIT;
}

void CEligos_Attack_Fly::Exit()
{
    
    CMonsterState::Exit();
}

HRESULT CEligos_Attack_Fly::Ready_State()
{
    m_fMinStateTime = 0.f;

    m_pOwnerTransformCom = static_cast<CTransform*>(m_pOwner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));
    m_pOwnerAnimatorCom = static_cast<CAnimator*>(m_pOwner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::ANIMATOR));

    return S_OK;
}

_int CEligos_Attack_Fly::Update_State(const _float& timeDelta)
{
    CMonsterState::Update_State(timeDelta);

    if (m_pOwnerAnimatorCom->Get_CurAnimation() == nullptr)
        return CEligos::ELIGOSSTATE::ATTACK_FLY;



    switch (m_ePhase)
    {
    case PHASE::WAIT:

        if (m_pOwnerAnimatorCom->Get_CurFrame() == m_pOwnerAnimatorCom->Get_MaxFrame() - 1)
        {
            m_ePhase = PHASE::READY;
            m_StartFlag = false;

        }
        break;


    case PHASE::READY:
        if (m_StartFlag == false)
        {
        m_pOwnerAnimatorCom->Play_Animation(L"Eligos_burpcharge", ANIMSTATE::ONCE);
  
            m_StartFlag = true;

        }
        if (m_pOwnerAnimatorCom->Get_CurFrame() == m_pOwnerAnimatorCom->Get_MaxFrame() - 1)
        {
            m_StartFlag = false;

            m_ePhase = PHASE::SHOOT;

        }
        break;

    case PHASE::SHOOT: //움직이면서 쏘는거

        if (m_StartFlag == false)
        {
            m_pOwnerAnimatorCom->Play_Animation(L"Eligos_burp", ANIMSTATE::ONCE);
            SpawnFlies(10,0.3,0.3, 1);
            m_StartFlag = true;

        }


        if (m_pOwnerAnimatorCom->Get_CurFrame() == m_pOwnerAnimatorCom->Get_MaxFrame() - 1)
        {

            m_StartFlag = false;
            return CEligos::ELIGOSSTATE::BOSSIDLE;
        }
    }

    return CEligos::ELIGOSSTATE::ATTACK_FLY;
}

void CEligos_Attack_Fly::LateUpdate_State(const _float& timeDelta)
{
}
void CEligos_Attack_Fly::SpawnFlies(int fliesNum, float noiseAngle, float noiseDist, float baseDist)
{
    Find_Player();
    for (int i = 0; i < fliesNum; i++)
    {
    int angle = (360/fliesNum)*i;
    float dist = baseDist;
        angle += (CRandomMgr::GetInstance()->Get_Int_Random(int(-noiseAngle * 1000), + int(noiseAngle * 1000))) / 1000.f;
        dist += (CRandomMgr::GetInstance()->Get_Int_Random(int( - noiseDist * 1000), + int (noiseDist * 1000))) / 1000.f;
    

        CEligos_Flies* fly = CEligos_Flies::Create(m_pOwner->Get_GraphicDev(),m_pOwner, (CGameObject*)m_pPlayer);
        fly->Initialize_GameObject();
        _vec3 pos = { cos(D3DXToRadian(angle) * dist), 0.f, sin(D3DXToRadian(angle) * dist) };

        fly->Get_Transform()->Set_Pos(m_pOwnerTransformCom->Get_Pos() + pos);
        SpawnObjectOnLayer(fly, OBJTYPE::MONSTER);

    }

}
void CEligos_Attack_Fly::Render_State()
{
}

CEligos_Attack_Fly* CEligos_Attack_Fly::Create(CEligos* pOwner, CEligos_StateMachine* pStateMachine)
{
    auto BossDead = new CEligos_Attack_Fly(pOwner, pStateMachine);

    if (FAILED(BossDead->Ready_State()))
    {
        MSG_BOX("BossDead Create Failed");
        Safe_Release(BossDead);
        return nullptr;
    }

    return BossDead;
}

void CEligos_Attack_Fly::Free()
{

}
