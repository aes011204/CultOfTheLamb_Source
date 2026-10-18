#include "pch.h"
#include "CRenderer.h"
#include "CBossPhase1.h"
#include "CBossStateMachine.h"
#include "CTransform.h"
#include "CTexture.h"
#include "CRcTex.h"
#include "CState.h"
#include "CAnimator.h"
#include "CBossDead.h"
#include "CBoxCollider.h"
#include "CCreateHelper.h"
#include "CCombatStat.h"
#include "CMonsterState.h"
#include "CUI_BossHp.h"


CBossPhase1::CBossPhase1(DEVICE graphicDev)
    : CMonsterBase(graphicDev)
    , m_StateMachine(nullptr)
{
}

CBossPhase1::CBossPhase1(const CBossPhase1& rhs)
    : CMonsterBase(rhs)
    , m_StateMachine(nullptr)
{
}

CBossPhase1::~CBossPhase1()
{
}

HRESULT CBossPhase1::Ready_GameObject()
{
    if (FAILED(CMonsterBase::Ready_GameObject()))
        return E_FAIL;

    m_StateMachine = CBossStateMachine::Create(this);

    m_TransformCom->Set_Scale({ 6.88f, 6.7f ,1.f });

    m_BoxColCom->Set_Size({ m_TransformCom->Get_Scale().x*.2f,m_TransformCom->Get_Scale().y ,2.f });

    BaseCenter.y = m_TransformCom->Get_Scale().y;

    m_TransformCom->Set_Pos(StartPos.x, m_TransformCom->Get_Scale().y, StartPos.z);
    m_TransformCom->Set_RotationAxis(ROT_X, D3DXToRadian(25.f));

    m_CombatStatCom->Set_MaxHp(500.f);
    m_CombatStatCom->Set_Attack(10.f);
    m_CombatStatCom->Set_Mp(5.f);
    m_CombatStatCom->Set_Speed(5.f);
    m_CombatStatCom->Set_HitState(false);


    if (m_BoxColCom)
        m_BoxColCom->Set_CollisionLayer(COLLISION_LAYERTYPE::ENEMY);

    return S_OK;
}

void CBossPhase1::Initialize_GameObject()
{
    BaseCenter = { 47 ,m_TransformCom->Get_Scale().y, 47 };
    m_TransformCom->Set_Pos(StartPos);

}

void CBossPhase1::OnBeginOverlap(CCollider* self, CCollider* other)
{
    CMonsterBase::OnBeginOverlap(self, other);

    m_bHurt = true;

}

void CBossPhase1::OnEndOverlap(CCollider* self, CCollider* other)
{
    CMonsterBase::OnEndOverlap(self, other);

}

_int CBossPhase1::Update_GameObject(const _float& timeDelta)
{
    _int exit = CMonsterBase::Update_GameObject(timeDelta);

    _int result = m_StateMachine->Update_StateMachine(timeDelta);

    if (result == BOSS_END && m_OnBossDead)
    {
        m_OnBossDead();
    }

    return exit;
}

void CBossPhase1::LateUpdate_GameObject(const _float& timeDelta)
{
    CMonsterBase::LateUpdate_GameObject(timeDelta);

    m_StateMachine->LateUpdate_StateMachine(timeDelta);
}

void CBossPhase1::Render_GameObject()
{
    m_StateMachine->Get_CurState()->Render_State();

    CMonsterBase::Render_GameObject();
}

void CBossPhase1::Render_Editor()
{
    ImGui::Text("=== BossP1 ===");

    _int num = m_StateMachine->GetCurStateNum();

    for (int i = 0; i < BOSS_END; i++)
    {
        if (ImGui::Button(GetBossStateName(i)))
        {
            m_StateMachine->Change_State(i);
        }
    }
}


HRESULT CBossPhase1::Add_Component()
{
    if (FAILED(CMonsterBase::Add_Component()))
        return E_FAIL;

    // texture
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_BOSSPHASE1 );
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_BOSSPHASE1, m_TextureCom });

    m_AnimatorCom->Set_TextureType(COMPONENTTYPE::TEX_BOSSPHASE1);

    return S_OK;
}

void CBossPhase1::Animation_Setting()
{
    m_AnimatorCom->Create_Animation(L"BossIdle", 100, 0.02f);

    m_AnimatorCom->Create_Animation(L"BossAwake", 105, 0.02f);
    m_AnimatorCom->Create_Animation(L"BossAttack", 97, 0.02f);
    m_AnimatorCom->Create_Animation(L"BossDead", 167, 0.02f, _vec2(1.f, 1.f), _vec2(0.f, -0.13f));
    m_AnimatorCom->Create_Animation(L"BossMove_up", 50, 0.02f);
    m_AnimatorCom->Create_Animation(L"BossMove_Down", 28, 0.02f);
    
    m_AnimatorCom->Create_Animation(L"BossGroggy_start", 115, 0.02f, _vec2(1.f, 1.f), _vec2(0.f, -0.13f));
    
    m_AnimatorCom->Create_Animation(L"BossGroggy_loop", 84, 0.02f);
    m_AnimatorCom->Create_Animation(L"BossGroggy_end", 67, 0.01f);

    m_AnimatorCom->Create_Animation(L"BossChainEff", 34, 0.02f);
    m_AnimatorCom->Create_Animation(L"ChainGroundEff", 1, 0.02f);

    m_AnimatorCom->Create_Animation(L"Before_Idle", 334, 0.03f, _vec2(1.26f, 1.f));
    m_AnimatorCom->Create_Animation(L"Escape",119, 0.02f, _vec2(1.26f, 1.f));

}

const char* CBossPhase1::GetBossStateName(_int StateNum)
{

    switch (StateNum)
    {
    case BOSSAWAKE:         return "BOSSAWAKE";
    case BULLETATTACK:      return "BULLETATTACK";
    case CHAINATTACK:       return "CHAINATTACK";
    case BOSSDEAD:          return "BOSSDEAD";
    case BOSSIDLE:          return "BOSSIDLE";
    case BOSSMOVE:          return "BOSSMOVE";
    case BOSSGROGGRY:       return "BOSSGROGGRY";
    case TALK:              return "TALK";
    case BOSS_END:          return "BOSS_END";
    }
    return "UNKNOWN";
}

CBossPhase1* CBossPhase1::Create(DEVICE graphicDev)
{
    auto Boss = new CBossPhase1(graphicDev);

    if (FAILED(Boss->Ready_GameObject()))
    {
        MSG_BOX("Boss Create Failed");
        Safe_Release(Boss);
        return nullptr;
    }

    return Boss;
}

void CBossPhase1::Free()
{
    //m_StateMachine->Release();
    Safe_Release(m_StateMachine);

    CMonsterBase::Free();

}

void CBossPhase1::Set_OnDeadTalkStart(OnDeadTalkStartCallback callback)
{
    auto deadState = dynamic_cast<CBossDead*>(
        m_StateMachine->Get_State(BOSSDEAD));

    if (deadState)
        deadState->Set_OnTalkStart(callback);
}
