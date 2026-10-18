#include "pch.h"
#include "CBossPhase2.h"
#include "CCreateHelper.h"
#include "CAnimator.h"
#include "CTexture.h"
#include "CTransform.h"
#include "CBossController.h"
#include "CBoss2_StateMachine.h"
#include "CBoss2_SummonFollower.h"
#include "CCombatStat.h"
#include "CBoxCollider.h"
#include "CEyeball.h"
#include "CUI_BossHp.h"

CBossPhase2::CBossPhase2(DEVICE graphicDev)
    : CMonsterBase(graphicDev), m_BossController(nullptr)
{
}

CBossPhase2::CBossPhase2(const CBossPhase2& rhs)
    : CMonsterBase(rhs), m_BossController(rhs.m_BossController)
{
}

CBossPhase2::~CBossPhase2()
{
}

HRESULT CBossPhase2::Ready_GameObject()
{
    if (FAILED(CMonsterBase::Ready_GameObject()))
        return E_FAIL;

    m_CombatStatCom->Set_MaxHp(1000.f);
    m_CombatStatCom->Set_HitState(false); // Hit State로 변경할건지

    m_TransformCom->Set_Scale(_vec3(13.7f, 6.3f, 3.f));
    m_TransformCom->Set_RotationAxis(ROT_X, D3DXToRadian(25.f));

    m_BoxColCom->Set_Size(_vec3(1.2f, 1.6f, 1.f));
    m_BoxColCom->Set_Offset(_vec3(0.f, -2.1f, 0.f));

    return S_OK;
}

void CBossPhase2::Initialize_GameObject()
{
    CMonsterBase::Initialize_GameObject();

    // State Machine
    auto stateMachine = CBoss2_StateMachine::Create(this);
    m_BossController->Set_StateMachine(stateMachine); 
    Safe_Release(stateMachine);

    
}

_int CBossPhase2::Update_GameObject(const _float& timeDelta)
{
    _int exit = CMonsterBase::Update_GameObject(timeDelta);

    // 사망
    if (m_CombatStatCom && m_CombatStatCom->Is_Dead())
    {
        if (m_BossController)
        {
            auto stateMachine = m_BossController->Get_StateMachine();
            if (stateMachine)
            {
                _int curState = stateMachine->Get_CurStateNum();

                if (curState != PATTERN::PHASE2_DEAD && curState != PATTERN::PHASE2_FOLLOWER)
                {
                    stateMachine->Change_State(PATTERN::PHASE2_DEAD);
                }
            }
        }
    }

    return exit;
}

void CBossPhase2::LateUpdate_GameObject(const _float& timeDelta)
{
    CMonsterBase::LateUpdate_GameObject(timeDelta);

}

void CBossPhase2::Render_GameObject()
{
    CMonsterBase::Render_GameObject();


}

void CBossPhase2::Render_Editor()
{
    CMonsterBase::Render_Editor();

    ImGui::SeparatorText("=== Boss Phase2 ===");

    if (m_BossController && m_BossController->Get_StateMachine())
    {
        auto stateMachine = m_BossController->Get_StateMachine();
        _int curState = stateMachine->Get_CurStateNum();

        ImGui::Text("Current State : %s", Get_Boss2_StateName(curState));

        ImGui::Separator();

        for (int i = 0; i < PATTERN::PHASE2_END; i++)
        {
            if (ImGui::Button(Get_Boss2_StateName(i)))
            {
                stateMachine->Change_State(i);
            }
        }
    }

}

const char* CBossPhase2::Get_Boss2_StateName(_int stateNum)
{
    // 상태 추가될때마다 추가하기
    switch (stateNum)
    {
    case PHASE2_SUMMON:      return "PHASE2_Summon";
    case PHASE2_START:      return "PHASE2_Start";
    case PHASE2_IDLE:       return "PHASE2_Idle";
    case PHASE2_ROAR:       return "PHASE2_Roar";
    case PHASE2_EYEBALL:    return "PHASE2_EYEBALL";
    case PHASE2_CHAIN:      return "PHASE2_CHAIN";
    case PHASE2_BULLET1:    return "PHASE2_BULLET";
    case PHASE2_BULLET2:    return "PHASE_BULLET2";
    case PHASE2_GROGGY:     return "PHASE_GROGGY";
    case PHASE2_DEAD:       return "PHASE2_DEAD";
    case PHASE2_FOLLOWER:   return "PHASE2_FOLLOWER";
    case PHASE2_END:        return "PHASE2_END";
    }
    return "UNKNOWN";
}

void CBossPhase2::Add_Eyeball(CEyeball* eyeball)
{
    if (eyeball)
        m_Eyeballs.emplace_back(eyeball);
}

void CBossPhase2::Remove_Eyeball(CEyeball* eyeball)
{
    auto iter = find(m_Eyeballs.begin(), m_Eyeballs.end(), eyeball);

    if (iter != m_Eyeballs.end())
    {
        m_Eyeballs.erase(iter);
    }
}

void CBossPhase2::Command_Ground()
{
    for (auto eyeball : m_Eyeballs)
    {
        if (!eyeball) continue;

        // 이미 죽었거나, 펜딩킬 예정이면 스킵
        if (eyeball->Is_PendingDestroy())
            continue;

        auto controller = eyeball->Get_Controller();
        if (controller)
        {
            auto stateMachine = controller->Get_StateMachine();
            if (stateMachine)
            {
                _int curState = stateMachine->Get_CurStateNum();

                // 공격 진행중이면 스킵 Eyeball Attack End 에서 알아서 처리
                if (curState == CEyeball::PATTERN::EYEBALL_ATTACK)
                    continue;

                stateMachine->Change_State(CEyeball::PATTERN::EYEBALL_GROUND_MOVE);
            }
        }
    }
}

_bool CBossPhase2::Is_InSpecialPattern()
{
    if (!m_BossController)
        return false;

    auto stateMachine = m_BossController->Get_StateMachine();
    if (!stateMachine)
        return false;

    _int curState = stateMachine->Get_CurStateNum();

    return (curState == PATTERN::PHASE2_BULLET1 ||
                curState == PATTERN::PHASE2_BULLET2 ||
                curState == PATTERN::PHASE2_GROGGY);
}

void CBossPhase2::Trigger_OnSummonRoarEnd()
{
    if (m_OnSummonRoarEnd)
    {
        m_OnSummonRoarEnd();
    }
}

void CBossPhase2::Resume_Summon()
{
    auto stateMachine = m_BossController->Get_StateMachine();
    if (stateMachine && stateMachine->Get_CurStateNum() == PHASE2_SUMMON)
    {
        auto summonState = dynamic_cast<CBoss2_SummonFollower*>(stateMachine->Get_CurState());

        if (summonState)
            summonState->Resume_Summon();
    }

}

CBossPhase2* CBossPhase2::Create(DEVICE graphicDev)
{
    CBossPhase2* boss = new CBossPhase2(graphicDev);

    if (FAILED(boss->Ready_GameObject()))
    {
        Safe_Release(boss);
        MSG_BOX("Boss2 Create Failed");
        return nullptr;
    }

    return boss;
}

void CBossPhase2::Animation_Setting()
{
    m_AnimatorCom->Create_Animation(L"Boss2_Summon", 134, 0.05f, _vec2(1.01f, 1.05f), _vec2(-0.015f, 0.01f));

    m_AnimatorCom->Create_Animation(L"Boss2_Start", 85, 0.05f, _vec2(1.f, 1.f), _vec2(0.005f, 0.05f));
    m_AnimatorCom->Create_Animation(L"Boss2_Idle", 68, 0.05f, _vec2(1.f, 1.f), _vec2(0.005f, 0.05f));
    m_AnimatorCom->Create_Animation(L"Boss2_Idle_Alive", 68, 0.05f, _vec2(1.f, 1.f), _vec2(0.005f, 0.05f));

    m_AnimatorCom->Create_Animation(L"Boss2_Roar", 70, 0.04f, _vec2(1.f, 1.f), _vec2(0.f, -0.08f));
    m_AnimatorCom->Create_Animation(L"Boss2_Roar_Alive", 70, 0.04f, _vec2(1.1f, 1.f), _vec2(-0.005f, -0.025f));

    m_AnimatorCom->Create_Animation(L"Boss2_HandSlam", 68, 0.04f, _vec2(1.f, 1.12f), _vec2(0.f, -0.19f));
    m_AnimatorCom->Create_Animation(L"Boss2_Dead", 117, 0.04f, _vec2(1.09f, 1.25), _vec2(0.f, 0.2f));

    m_AnimatorCom->Create_Animation(L"Boss2_DownStart", 64, 0.04f, _vec2(1.09f, 1.1f), _vec2(0.f, -0.1f));
    m_AnimatorCom->Create_Animation(L"Boss2_DownIdle", 67, 0.04f, _vec2(0.93f, 0.79f), _vec2(0.f, -0.14f));

    m_AnimatorCom->Create_Animation(L"Boss2_Sweep", 82, 0.04f, _vec2(1.16f, 1.25f), _vec2(0.f, -0.18f));
    m_AnimatorCom->Create_Animation(L"Boss2_Follower_Transform", 62, 0.04f, _vec2(0.115f, 0.5f), _vec2(0.f, -0.52f));
    m_AnimatorCom->Create_Animation(L"Boss2_Follower_UnConverted", 50, 0.05f, _vec2(0.046f, 0.14f), _vec2(0.f, -0.75f));
}

HRESULT CBossPhase2::Add_Component()
{
    if(FAILED(CMonsterBase::Add_Component()))
        return E_FAIL;

    COMPONENTTYPE type;

    type = COMPONENTTYPE::TEX_BOSS2_PHASE2;
    m_TextureCom = CreateProtoComponent<CTexture>(this, type);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);
    m_Components[ID_STATIC].insert({ type, m_TextureCom });

    m_AnimatorCom->Set_TextureType(type);

    // Controller
    type = COMPONENTTYPE::BOSS2_CONTROLLER;
    m_BossController = CreateProtoComponent<CBossController>(this, type);
    NULL_CHECK_RETURN(m_BossController, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ type, m_BossController });


    return S_OK;
}

void CBossPhase2::Free()
{
    CMonsterBase::Free();
}
