#include "pch.h"
#include "CPlayerMovement.h"

#include <CSoundMgr.h>
#include "CCombatStat.h"
#include "CGameObject.h"
#include "CRigidbody.h"
#include "CState.h"
#include "CTransform.h"

CPlayerMovement::CPlayerMovement(DEVICE graphicDev)
    : CComponent(graphicDev),

      m_OwnerStateCom(nullptr),
      m_OwnerCombatStatCom(nullptr),
      m_OwnerRigidbodyCom(nullptr),

      m_RollDir(0.f, 0.f, 0.f),
      m_Timer(0.f),
      m_RollDuration(0.4f),
      m_RollInvincibleTime(0.3f),
      m_IsInvincibleEnded(false)
{ }

CPlayerMovement::CPlayerMovement(const CPlayerMovement& rhs)
    : CComponent(rhs),

      m_OwnerStateCom(nullptr),
      m_OwnerCombatStatCom(nullptr),
      m_OwnerRigidbodyCom(nullptr),

      m_RollDir(rhs.m_RollDir),
      m_Timer(rhs.m_Timer),
      m_RollDuration(rhs.m_RollDuration),
      m_RollInvincibleTime(rhs.m_RollInvincibleTime),
      m_IsInvincibleEnded(rhs.m_IsInvincibleEnded)
{ }

CPlayerMovement::~CPlayerMovement()
{ }

HRESULT CPlayerMovement::Ready_PlayerMovement()
{
    return S_OK;
}

_int CPlayerMovement::Update_Component(const _float& timeDelta)
{
    // 매 프레임 작업 없음. 스태틱 컴포넌트로 사용할 것
    return 0;
}

void CPlayerMovement::LateUpdate_Component()
{ }

HRESULT CPlayerMovement::Init_OwnerComponent()
{
    HRESULT result = S_OK;

    m_OwnerStateCom = dynamic_cast<CState*>(
        m_Owner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::STATE));

    m_OwnerCombatStatCom = dynamic_cast<CCombatStat*>(
        m_Owner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::COMBATSTAT));

    m_OwnerRigidbodyCom = dynamic_cast<CRigidbody*>(
        m_Owner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::RIGIDBODY));

    if (nullptr == m_OwnerStateCom)
    {
        MessageBoxW(nullptr,
                    L"CPlayerMovement: Owner State Not Found",
                    L"System Message",
                    MB_ICONERROR);

        result = E_FAIL;
    }

    if (nullptr == m_OwnerCombatStatCom)
    {
        MessageBoxW(nullptr,
                    L"CPlayerMovement: Owner CombatStat Not Found",
                    L"System Message",
                    MB_ICONERROR);

        result = E_FAIL;
    }

    if (nullptr == m_OwnerRigidbodyCom)
    {
        MessageBoxW(nullptr,
                    L"CPlayerMovement: Owner Rigidbody Not Found",
                    L"System Message",
                    MB_ICONERROR);

        result = E_FAIL;
    }

    return result;
}

void CPlayerMovement::Run(const _vec3& moveDir, const _float& timeDelta)
{
    m_OwnerRigidbodyCom->Set_Friction(0.05f);

    ChangeActorDir(moveDir);

    _float speed = m_OwnerCombatStatCom->Get_Speed();
    m_OwnerRigidbodyCom->Add_Force(moveDir * speed * 20.f);
}

void CPlayerMovement::Stop()
{
    m_OwnerRigidbodyCom->Set_Friction(0.3f);
}

void CPlayerMovement::Start_Roll(const _vec3& moveDir)
{
    m_Timer             = 0.f;
    m_IsInvincibleEnded = false;

    if (D3DXVec3LengthSq(&moveDir) > 0.001f)
        m_RollDir = moveDir;
    else
        m_RollDir = ActorDirToVec(m_OwnerStateCom->Get_Dir());

    m_OwnerRigidbodyCom->Set_Friction(0.f);
    m_OwnerRigidbodyCom->Set_UseAccel(false);

    CSoundMgr::GetInstance()->Play_Once(L"Player/Player_Roll.wav",
                                        SOUND_EFFECT,
                                        0.5f);
}

_bool CPlayerMovement::Update_Roll(const _float& timeDelta)
{
    m_Timer += timeDelta;

    // 구르기 종료 체크
    if (m_Timer >= m_RollDuration)
    {
        m_OwnerRigidbodyCom->Set_UseAccel(true);
        m_OwnerRigidbodyCom->Set_Friction(0.2f);

        return false; // 구르기 종료
    }

    // 무적 시간 종료 체크
    if (!m_IsInvincibleEnded && m_Timer >= m_RollInvincibleTime)
    {
        m_IsInvincibleEnded = true;
        // TODO: 무적 상태 해제 로직
    }

    // 구르기 이동
    _float rollSpeed = m_OwnerCombatStatCom->Get_Speed() * 3.f;
    m_TransformCom->Move_Pos(m_RollDir, timeDelta, rollSpeed);

    return true; // 구르기 지속
}

void CPlayerMovement::End_Roll()
{
    m_OwnerRigidbodyCom->Set_UseAccel(true);
    m_OwnerRigidbodyCom->Set_Friction(0.2f);
    m_Timer = 0.f;
}

void CPlayerMovement::ImpulseMove(const _vec3 dir, _float force)
{
    m_OwnerRigidbodyCom->Set_Friction(0.1f);

    m_OwnerRigidbodyCom->Add_Force(dir * force);
}

void CPlayerMovement::ChangeActorDir(const _vec3& dir)
{
    if (D3DXVec3LengthSq(&dir) <= 0.001f) return;

    ACTORDIR newActorDir = m_OwnerStateCom->Get_Dir();

    if (dir.x > 0.1f)
    {
        if (dir.z > 0.1f) newActorDir = ACTORDIR::R_UP;
        else if (dir.z < -0.1f) newActorDir = ACTORDIR::R_DOWN;
        else newActorDir                    = ACTORDIR::RIGHT;
    }
    else if (dir.x < -0.1f)
    {
        if (dir.z > 0.1f) newActorDir = ACTORDIR::L_UP;
        else if (dir.z < -0.1f) newActorDir = ACTORDIR::L_DOWN;
        else newActorDir                    = ACTORDIR::LEFT;
    }
    else
    {
        if (dir.z > 0.1f) newActorDir = ACTORDIR::UP;
        else if (dir.z < -0.1f) newActorDir = ACTORDIR::DOWN;
    }

    m_OwnerStateCom->Change_Dir(newActorDir);
}

_vec3 CPlayerMovement::ActorDirToVec(ACTORDIR dir)
{
    _vec3 vDir;

    switch (dir)
    {
    case ACTORDIR::LEFT: vDir = { -1.f, 0.f, 0.f };
        break;
    case ACTORDIR::RIGHT: vDir = { 1.f, 0.f, 0.f };
        break;
    case ACTORDIR::UP: vDir = { 0.f, 0.f, 1.f };
        break;
    case ACTORDIR::DOWN: vDir = { 0.f, 0.f, -1.f };
        break;
    case ACTORDIR::L_UP: vDir = { -1.f, 0.f, 1.f };
        break;
    case ACTORDIR::R_UP: vDir = { 1.f, 0.f, 1.f };
        break;
    case ACTORDIR::L_DOWN: vDir = { -1.f, 0.f, -1.f };
        break;
    case ACTORDIR::R_DOWN: vDir = { 1.f, 0.f, -1.f };
        break;
    default: vDir = { 0.f, 0.f, 1.f };
        break;
    }

    D3DXVec3Normalize(&vDir, &vDir);
    return vDir;
}

CPlayerMovement* CPlayerMovement::Create(DEVICE graphicDev)
{
    auto playerMovement = new CPlayerMovement(graphicDev);

    if (FAILED(playerMovement->Ready_PlayerMovement()))
    {
        Safe_Release(playerMovement);
        MSG_BOX("PlayerMovement Create Failed");
        return nullptr;
    }

    return playerMovement;
}

CComponent* CPlayerMovement::Clone()
{
    return new CPlayerMovement(*this);
}

void CPlayerMovement::Free()
{
    m_OwnerStateCom      = nullptr;
    m_OwnerCombatStatCom = nullptr;
    m_OwnerRigidbodyCom  = nullptr;

    CComponent::Free();
}
