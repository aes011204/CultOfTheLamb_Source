#pragma once
#include "CBase.h"

class CConstructionPlacer;

class CConstructionMgr : public CBase
{
    DECLARE_SINGLETON(CConstructionMgr)

private:
    explicit CConstructionMgr();
    ~CConstructionMgr() override;

public:
    void Ready_ConstructionMgr(DEVICE graphicDev);
    void Update_ConstructionMgr(const _float& fTimeDelta);
    void Render_ConstructionMgr();

    void Create_Placer(DEVICE graphicDev);
    void Release_Placer();

    // 건설 모드 제어
    void Start_BuildMode(BUILDING_TYPE type);
    void End_BuildMode();

    _bool IsBuilding() const { return m_bIsBuilding; }

private:
    void Free() override;

private:
    DEVICE               m_GraphicDev = nullptr;
    CConstructionPlacer* m_pPlacer    = nullptr;

    _bool m_bIsBuilding     = false;
    _bool m_bWaitForRelease = false;
};
