#include "pch.h"
#include "CConstructionMgr.h"

#include "CConstructionPlacer.h"
#include "CDInputMgr.h"

IMPLEMENT_SINGLETON(CConstructionMgr)

CConstructionMgr::CConstructionMgr()
{ }

CConstructionMgr::~CConstructionMgr()
{
    CConstructionMgr::Free();
}

void CConstructionMgr::Ready_ConstructionMgr(DEVICE graphicDev)
{
    m_GraphicDev = graphicDev;
    m_GraphicDev->AddRef();
}

void CConstructionMgr::Create_Placer(DEVICE graphicDev)
{
    Release_Placer();

    m_pPlacer = CConstructionPlacer::Create(graphicDev);
}

void CConstructionMgr::Release_Placer()
{
    if (m_pPlacer)
    {
        Safe_Release(m_pPlacer);
        m_pPlacer = nullptr;
    }
}

void CConstructionMgr::Update_ConstructionMgr(const _float& fTimeDelta)
{
    if (!m_bIsBuilding)
        return;

    if (m_pPlacer)
    {
        m_pPlacer->Update_Placer(fTimeDelta);

        // 마우스 클릭 체크하여 배치 시도
        if (m_bWaitForRelease)
        {
            if (!(CDInputMgr::GetInstance()->Get_DIMouseState(DIM_LB) & 0x80))
                m_bWaitForRelease = false;
        }
        else
        {
            if (CDInputMgr::GetInstance()->MouseDown(DIM_LB))
            {
                if (m_pPlacer->Try_PlaceObject())
                {
                    // 배치 성공 시 후처리
                    End_BuildMode();
                }
            }
        }

        // 취소 키 체크 (우클릭)
        if (CDInputMgr::GetInstance()->MouseDown(DIM_RB))
            End_BuildMode();
    }
}

void CConstructionMgr::Render_ConstructionMgr()
{
    if (!m_bIsBuilding)
        return;

    if (m_pPlacer)
        m_pPlacer->Render_Preview();
}

void CConstructionMgr::Start_BuildMode(BUILDING_TYPE type)
{
    if (!m_pPlacer)
        return;

    m_bIsBuilding     = true;
    m_bWaitForRelease = true;
    m_pPlacer->Set_Preview(type);
}

void CConstructionMgr::End_BuildMode()
{
    m_bIsBuilding = false;

    // Placer 내부 상태 초기화
    m_pPlacer->End_Preview();
}

void CConstructionMgr::Free()
{
    Safe_Release(m_pPlacer);
    Safe_Release(m_GraphicDev);
}
