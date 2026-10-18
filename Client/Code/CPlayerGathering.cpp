#include "pch.h"
#include "CPlayerGathering.h"
#include "CAnimator.h"
#include "CCollider.h"
#include "CCombatStat.h"
#include "CDInputMgr.h"
#include "CSceneMgr.h"
#include "CSoundMgr.h"
#include "CState.h"
#include "CTransform.h"
#include "CWorldInteractive.h"
#include "CUI_ProgressBar.h"

CPlayerGathering::CPlayerGathering(DEVICE graphicDev)
    : CComponent(graphicDev),
      m_OwnerAnimationCom(nullptr)
{
    ZeroMemory(&h, sizeof(h));
}

CPlayerGathering::CPlayerGathering(const CPlayerGathering& rhs)
    : CComponent(rhs),
      m_OwnerAnimationCom(nullptr)
{ }

CPlayerGathering::~CPlayerGathering()
{ }

HRESULT CPlayerGathering::Ready_PlayerGathering()
{
    return S_OK;
}

void CPlayerGathering::Initialize_Component()
{
    CComponent::Initialize_Component();
    m_OwnerStateCom     = dynamic_cast<CState*>(m_Owner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::STATE));
    m_OwnerAnimationCom = dynamic_cast<CAnimator*>(m_Owner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::ANIMATOR));
    h.attacker          = m_Owner;
    h.damage            = 2;
    h.dir               = { 0, 0, 0 };
    h.hitter            = nullptr;
    h.knockback         = 0;;
    h.knockbackTimer    = 0;
}

_int CPlayerGathering::Update_Component(const _float& timeDelta)
{
    _int exit = CComponent::Update_Component(timeDelta);

    if (CSceneMgr::GetInstance()->Is_Changing())
    {
        m_Target = nullptr;
        return exit;
    }

    // 채집 종료 조건 체크 (괄호로 우선순위 명확화)
    // 1. 타겟이 없음
    // 2. 좌클릭 뗌
    // 3. E키 뗌  
    // 4. 타겟이 더 이상 호버링되지 않음
    bool bShouldStop = (m_Target == nullptr) ||
                       CDInputMgr::GetInstance()->MouseUp(DIM_LB) ||
                       CDInputMgr::GetInstance()->KeyUp(DIK_E) ||
                       (m_Target && m_Target->Get_Hovering() == false);

    if (bShouldStop)
    {
        m_OwnerStateCom->Change_State(ACTORSTATE::IDLE);
        m_AttackTimer = 0;
        if (m_ProgressBar)  // nullptr 체크 추가
            m_ProgressBar->Set_IsActive(false);
        return exit;
    }


    if (m_Target && m_ProgressBar)
    {
        m_ProgressBar->Set_Progress(CalcProgress());
        m_ProgressBar->Set_IsActive(true);

        CCombatStat* combat = dynamic_cast<CCombatStat*>(m_Target->Get_Component(COMPONENTID::ID_DYNAMIC, COMPONENTTYPE::COMBATSTAT));

    }
    //애니메이션 방향 결정
    _vec3 vTargetDir = m_Target->Get_Transform()->Get_Pos();

    _vec3 dir = vTargetDir - m_Owner->Get_Transform()->Get_Pos();

    D3DXVec3Normalize(&dir, &dir);

    if (dir.x <= 0.f)
        m_OwnerStateCom->Change_Dir(ACTORDIR::LEFT);
    else
        m_OwnerStateCom->Change_Dir(ACTORDIR::RIGHT);

    m_AttackTimer += timeDelta;

    switch (m_Target->GetInteractiveType())
    {
    case CWorldInteractive::TYPE::BERRY:
    {
        if (m_AttackTimer >= 0.5)
        {
            // 공격 1 씩 주기
            CCombatStat* combat = dynamic_cast<CCombatStat*>(m_Target->Get_Component(COMPONENTID::ID_DYNAMIC, COMPONENTTYPE::COMBATSTAT));
            h.dir               = dir;
            h.hitter            = m_Target;

            combat->Apply_Damage(h);

            //TODO ACTION 사운드

            m_AttackTimer = 0;
        }

#pragma region 플레이어 베리 채집 효과음. 0.3초마다 재생
        g_Timers[TIMER_SOUND_PL_COLLECTBERRY] -= timeDelta;

        if (g_Timers[TIMER_SOUND_PL_COLLECTBERRY] < 0.f)
        {
            CSoundMgr::GetInstance()->Play_Once(L"Player/Player_Action.wav", SOUND_EFFECT, 0.5f);
            g_Timers[TIMER_SOUND_PL_COLLECTBERRY] = 0.3f;
        }
#pragma endregion
    }
    break;

    case CWorldInteractive::TYPE::WOOD:

        if (m_OwnerAnimationCom->Get_CurFrame() == 0)
            m_AttackFlag = false;
        if (m_OwnerAnimationCom->Get_CurFrame() == 25 && m_AttackFlag == false)
        {
            // 공격 1 씩 주기
            CCombatStat* combat = dynamic_cast<CCombatStat*>(m_Target->Get_Component(COMPONENTID::ID_DYNAMIC, COMPONENTTYPE::COMBATSTAT));
            h.dir               = dir;
            h.hitter            = m_Target;

            combat->Apply_Damage(h);
            m_AttackFlag = true;

            CSoundMgr::GetInstance()->Play_Once(L"Object/Wood/Wood_Chop_0.wav", SOUND_EFFECT, 0.5f);
        }
        break;

    case CWorldInteractive::TYPE::ROCK:
        if (m_OwnerAnimationCom->Get_CurFrame() == 0)
            m_AttackFlag = false;

        if (m_OwnerAnimationCom->Get_CurFrame() == 28 && m_AttackFlag == false)
        {
            // 공격 1 씩 주기
            CCombatStat* combat = dynamic_cast<CCombatStat*>(m_Target->Get_Component(COMPONENTID::ID_DYNAMIC, COMPONENTTYPE::COMBATSTAT));
            h.dir               = dir;
            h.hitter            = m_Target;

            combat->Apply_Damage(h);

            m_AttackTimer = 0;
            m_AttackFlag  = true;

            CSoundMgr::GetInstance()->Play_Once(L"Object/Rock/Stone_Impact_0.wav", SOUND_EFFECT, 0.5f);
        }
        break;
    }
    
    return exit;
}

void CPlayerGathering::LateUpdate_Component()
{
    CComponent::LateUpdate_Component();
}

void CPlayerGathering::OnBeginOverlap(CCollider* self, CCollider* other)
{
    auto interact = dynamic_cast<CWorldInteractive*>(other->Get_Owner());
    if (interact)
    {
        // 레퍼런스 올리고 내려야 할듯
        m_Target = interact;
    }
}

void CPlayerGathering::OnEndOverlap(CCollider* self, CCollider* other)
{
    if (other->Get_Owner() == m_Target)
        m_Target = nullptr;
}

_float CPlayerGathering::CalcProgress()
{
    CCombatStat* combat = dynamic_cast<CCombatStat*>(m_Target->Get_Component(COMPONENTID::ID_DYNAMIC, COMPONENTTYPE::COMBATSTAT));
    _float cur = (_float)combat->Get_Hp();
    _float max = (_float)combat->Get_MaxHp();

   m_Progress = (1.f - cur / max);

   return std::clamp(m_Progress, 0.f, 1.f);
}

CPlayerGathering* CPlayerGathering::Create(DEVICE graphicDev)
{
    CPlayerGathering* PlayerGathering = new CPlayerGathering(graphicDev);

    if (FAILED(PlayerGathering->Ready_PlayerGathering()))
    {
        Safe_Release(PlayerGathering);
        MSG_BOX("PlayerGathering Create Failed");
        return nullptr;
    }

    return PlayerGathering;
}

CComponent* CPlayerGathering::Clone()
{
    return new CPlayerGathering(*this);
}

void CPlayerGathering::Free()
{
    CComponent::Free();
}
