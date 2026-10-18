#include "pch.h"
#include "CComponent.h"
#include "CPlayerConstruction.h"

#include <CSoundMgr.h>
#include "CUI_ProgressBar.h"
#include "CCollider.h"
#include "CConstructCircle.h"
#include "CSceneMgr.h"

CPlayerConstruction::CPlayerConstruction(DEVICE graphicDev)
    : CComponent(graphicDev)
{ }

CPlayerConstruction::CPlayerConstruction(const CPlayerConstruction& rhs)
    : CComponent(rhs)
{ }

CPlayerConstruction::~CPlayerConstruction()
{ }

HRESULT CPlayerConstruction::Ready_PlayerConstruction()
{
    return S_OK;
}

void CPlayerConstruction::Initialize_Component()
{ }

_int CPlayerConstruction::Update_Component(const _float& timeDelta)
{
    if (CSceneMgr::GetInstance()->Is_Changing())
        m_pConstructZone = nullptr;

    if (m_pConstructZone && m_pConstructZone->Is_PendingDestroy())
        m_pConstructZone = nullptr;

    return 0;
}

void CPlayerConstruction::LateUpdate_Component()
{ }

void CPlayerConstruction::Action_Construction(const _float& timeDelta)
{
    if (m_pConstructZone)
    {
        if (m_ProgressBar)
        {
            _float progress = m_pConstructZone->Get_Progress();
            m_ProgressBar->Set_Progress(progress / 100.f);
            m_ProgressBar->Set_IsActive(true);
        }

#pragma region 건축 효과음. 0.565초마다 재생
        g_Timers[TIMER_SOUND_PL_CONSTRUCT] += timeDelta;

        if (g_Timers[TIMER_SOUND_PL_CONSTRUCT] >= 0.565f)
        {
            CSoundMgr::GetInstance()->Play_Once(L"Construction/Hammering_2.wav", SOUND_EFFECT, 0.5f);
            g_Timers[TIMER_SOUND_PL_CONSTRUCT] = 0.f;
        }
#pragma endregion
    }
}

void CPlayerConstruction::OnOverlapEnter(CCollider* self, CCollider* other)
{
    if (other->Get_CollisionLayer() == COLLISION_LAYERTYPE::CONSTRUCTION_SITE ||
        other->Get_CollisionLayer() == COLLISION_LAYERTYPE::TRIGGER)
    {
        if (auto circle = dynamic_cast<CConstructCircle*>(other->Get_Owner()))
            m_pConstructZone = circle;
    }
}

void CPlayerConstruction::OnOverlapExit(CCollider* self, CCollider* other)
{
    if (other->Get_CollisionLayer() == COLLISION_LAYERTYPE::CONSTRUCTION_SITE ||
        other->Get_CollisionLayer() == COLLISION_LAYERTYPE::TRIGGER)
    {
        if (m_pConstructZone == dynamic_cast<CConstructCircle*>(other->Get_Owner()))
            m_pConstructZone = nullptr;
    }
}

void CPlayerConstruction::BuildingCompleted() // 초기화
{
    if (m_ProgressBar)
        m_ProgressBar->Set_IsActive(false);
    m_pConstructZone = nullptr;
}

CPlayerConstruction* CPlayerConstruction::Create(DEVICE graphicDev)
{
    auto playerConstruction = new CPlayerConstruction(graphicDev);

    if (FAILED(playerConstruction->Ready_PlayerConstruction()))
    {
        Safe_Release(playerConstruction);
        MSG_BOX("PlayerConstruction Create Failed");
        return nullptr;
    }

    return playerConstruction;
}

CComponent* CPlayerConstruction::Clone()
{
    return new CPlayerConstruction(*this);
}

void CPlayerConstruction::Free()
{
    m_pConstructZone = nullptr;

    CComponent::Free();
}
