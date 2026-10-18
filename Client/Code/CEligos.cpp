#include "pch.h"
#include "CEligos.h"
#include "CAnimator.h"
#include "CRenderer.h"
#include "CTransform.h"
#include "CTexture.h"
#include "CAnimator.h"
#include "CBoxCollider.h"
#include "CCreateHelper.h"
#include "CCombatStat.h"
#include "CMonsterState.h"
#include "CUI_BossHp.h"
#include "CRigidbody.h"

#include "CEligos_StateMachine.h"



CEligos::CEligos(DEVICE graphicDev) : CMonsterBase(graphicDev), m_StateMachine(nullptr)
{
}

CEligos::CEligos(const CEligos& rhs) : CMonsterBase(rhs), m_StateMachine(nullptr)
{
}

CEligos::~CEligos()
{
}

HRESULT CEligos::Ready_GameObject()
{
    CMonsterBase::Ready_GameObject();

    //if (FAILED(Add_Component()))
    //    return E_FAIL;

    m_StateMachine = CEligos_StateMachine::Create(this);

    m_TransformCom->Set_Scale({ 5.0f,5.0f ,1.f });

    m_BoxColCom->Set_Size({ m_TransformCom->Get_Scale().x * .2f,m_TransformCom->Get_Scale().y ,2.f });

    //!! Rigidbody에서 중력만 사용 안함
    m_RigidbodyCom->Set_UseAccel(true);
    m_RigidbodyCom->Set_UseFriction(true);
    m_RigidbodyCom->Set_UseGravity(false);

    m_RigidbodyCom->Set_Friction(0.3f);
    //

    m_TransformCom->Set_Pos(0.f, 0.f, 0.f);
    m_TransformCom->Set_RotationAxis(ROT_X, D3DXToRadian(25.f));

    m_CombatStatCom->Set_MaxHp(500.f);
    m_CombatStatCom->Set_Attack(10.f);
    m_CombatStatCom->Set_Mp(5.f);
    m_CombatStatCom->Set_Speed(5.f);
    m_CombatStatCom->Set_HitState(false);


    if (m_BoxColCom)
        m_BoxColCom->Set_CollisionLayer(COLLISION_LAYERTYPE::ENEMY);

    //Animation_Setting();

    return S_OK;
}

void CEligos::Initialize_GameObject()
{
    CMonsterBase::Initialize_GameObject();

}

_int CEligos::Update_GameObject(const _float& timeDelta)
{
    int exit = CMonsterBase::Update_GameObject(timeDelta);

    _int result = m_StateMachine->Update_StateMachine(timeDelta);

 

    return exit;
}

void CEligos::LateUpdate_GameObject(const _float& timeDelta)
{
    CMonsterBase::LateUpdate_GameObject(timeDelta);

    m_StateMachine->LateUpdate_StateMachine(timeDelta);

}

void CEligos::Render_GameObject()
{
    m_StateMachine->Get_CurState()->Render_State();

    CMonsterBase::Render_GameObject();


}

void CEligos::Render_Editor()
{
    CMonsterBase::Render_Editor();

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


void CEligos::OnBeginOverlap(CCollider* self, CCollider* other)
{
    CMonsterBase::OnBeginOverlap(self, other);
    m_bHurt = true;
}

void CEligos::OnEndOverlap(CCollider* self, CCollider* other)
{
    CMonsterBase::OnEndOverlap(self, other);

}

HRESULT CEligos::Add_Component()
{

    // 텍스쳐는 해야함 무조건


    // 부모쪽에 Transform, Buffer, Animator, State, CombatStat, BoxCollider 있음
    if (FAILED(CCharacterBase::Add_Component()))
        return E_FAIL;

    // Texture
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_MINIBOSS);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_MINIBOSS, m_TextureCom });

    m_AnimatorCom->Set_TextureType(COMPONENTTYPE::TEX_MINIBOSS);

    // CRigidbody
    m_RigidbodyCom = CreateProtoComponent<CRigidbody>(this, COMPONENTTYPE::RIGIDBODY);
    NULL_CHECK_RETURN(m_RigidbodyCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::RIGIDBODY, m_RigidbodyCom });


    return S_OK;
}

void CEligos::Animation_Setting()
{

    m_AnimatorCom->Create_Animation(L"Eligos_roar", 140, 0.02f, _vec2(1.f, 1.f));
    m_AnimatorCom->Create_Animation(L"Eligos_fly", 42, 0.02f, _vec2(1.f, 1.f));
    m_AnimatorCom->Create_Animation(L"Eligos_burpcharge", 50, 0.02f, _vec2(1.0f, 1.0f));
    m_AnimatorCom->Create_Animation(L"Eligos_burp", 42, 0.02f, _vec2(1.0f, 1.0f));
    m_AnimatorCom->Create_Animation(L"Eligos_attackcharge", 59, 0.02f, _vec2(1.0f, 1.0f));
    m_AnimatorCom->Create_Animation(L"Eligos_attack", 32, 0.02f, _vec2(1.0f, 1.0f));

    m_AnimatorCom->Create_Animation(L"Eligos_attackcharge_bullet", 59, 0.02f, _vec2(1.0f, 1.0f));
    m_AnimatorCom->Create_Animation(L"Eligos_attack_bullet", 32, 0.06f, _vec2(1.0f, 1.0f));
   
}

const char* CEligos::GetBossStateName(_int StateNum)
{
    switch (StateNum)
    {
    case BOSSREVEAL:         return "BOSSREVEAL";
    case ATTACK_BULLET:      return "ATTACK_BULLET";
    case ATTACK_FLY:         return "ATTACK_FLY";
    case ATTACK_MELEE:       return "ATTACK_MELEE";
    case BOSSDEAD:           return "BOSSDEAD";
    case BOSSIDLE:           return "BOSSIDLE";
    case BOSSMOVE:           return "BOSSMOVE";

    case BOSS_END:           return "BOSS_END";
    }
    return "UNKNOWN";
}

CEligos* CEligos::Create(DEVICE graphicDev)
{
    auto MiniBoss = new CEligos(graphicDev);

    if (FAILED(MiniBoss->Ready_GameObject()))
    {
        MSG_BOX("MiniBoss Create Failed");
        Safe_Release(MiniBoss);
        return nullptr;
    }

    return MiniBoss;
}

void CEligos::Free()
{
    Safe_Release(m_StateMachine);

    CMonsterBase::Free();
}
