#include "pch.h"
#include "CBoss2_ChainAttack.h"
#include "CBossController.h"
#include "CBossPhase2.h"
#include "CAnimator.h"
#include "CWarningChain.h"
#include "CTransform.h"
#include "CCreateHelper.h"
#include "CTimerMgr.h"

CBoss2_ChainAttack::CBoss2_ChainAttack(CGameObject* owner)
    : m_Owner(owner), m_Pattern_Cooldown(0)
{
}

CBoss2_ChainAttack::~CBoss2_ChainAttack()
{
}

void CBoss2_ChainAttack::Enter()
{
    CMonsterState::Enter();

    auto boss = static_cast<CBossPhase2*>(m_Owner);
    m_Controller = boss->Get_Controller();

    if (m_Controller)
    {
        auto animator = m_Controller->Get_OwnerAnimator();

        if (animator)
            animator->Play_Animation(L"Boss2_Sweep", ANIMSTATE::ONCE);
    }

    m_fCurStateElapsedTime = 0.f;
    m_SlamCount = 0;

    Find_Player();

    m_IsFinished = false;

    m_ChainSpawnTimer = 0.f;
    m_ChainCount = 0;
    m_HasSpawnedWarning = false;
}

_int CBoss2_ChainAttack::Update_State(const _float& timeDelta)
{
    m_fCurStateElapsedTime += timeDelta;

    if (m_IsFinished)
    {
        return CBossPhase2::PATTERN::PHASE2_IDLE;
    }

    PlaySweep();

    return CBossPhase2::PATTERN::PHASE2_CHAIN;
}

void CBoss2_ChainAttack::Exit()
{
    CMonsterState::Exit();

    // 상태 끝날 때
}

HRESULT CBoss2_ChainAttack::Ready_State()
{
    m_fMinStateTime = 0.5f;
    m_Pattern_Cooldown = 5.f;

    m_IsFinished = false;

    m_SlamCount = 0;

    return S_OK;
}

void CBoss2_ChainAttack::PlaySweep()
{
    auto animator = m_Controller->Get_OwnerAnimator();

    if (animator->Get_CurFrame() >= 20 && m_ChainCount < m_MaxChainCount)
    {
        if (!m_HasSpawnedWarning)
        {
            _vec3 playerPos = m_pPlayerTransformCom->Get_Pos();
            m_ChainStartPos = playerPos;

            _vec3 bossPos = m_Controller->Get_OwnerTransform()->Get_Pos();
            _vec3 direction = playerPos - bossPos;
            direction.y = 0.f;
            D3DXVec3Normalize(&m_ChainDirection, &direction);

            m_HasSpawnedWarning = true;
        }

        m_ChainSpawnTimer += CTimerMgr::GetInstance()->Get_TimeDelta();

        if (m_ChainSpawnTimer >= 0.1f)
        {
            auto boss2 = dynamic_cast<CBossPhase2*>(m_Owner);
            if (boss2)
            {
                CWarningChain* warning = CWarningChain::Create(boss2->Get_GraphicDev(), m_Owner);

                CTransform* warningTrans = dynamic_cast<CTransform*>(
                    warning->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));

                if (warningTrans)
                {
                    _vec3 warningPos = m_ChainStartPos + (m_ChainDirection * m_ChainSpacing * m_ChainCount);
                    warningPos.y = 0.f;

                    warningTrans->Set_Pos(warningPos);
                }

                SpawnObjectOnLayer(warning);
            }

            m_ChainCount++;
            m_ChainSpawnTimer = 0.f;
        }
    }

    if (animator->Is_AnimationFinished())
    {
        m_IsFinished = true;
    }
}

void CBoss2_ChainAttack::SpawnChain()
{

}

CBoss2_ChainAttack* CBoss2_ChainAttack::Create(CGameObject* owner)
{
    auto state = new CBoss2_ChainAttack(owner);

    if (FAILED(state->Ready_State()))
    {
        Safe_Release(state);
        MSG_BOX("Boss Eyeball Create Failed");
        return nullptr;
    }

    return state;
}

void CBoss2_ChainAttack::Free()
{
    
}
