#pragma once
#include "CBase.h"

BEGIN(Engine)
class CTexture;
END

class CResourceMgr : public CBase
{
    DECLARE_SINGLETON(CResourceMgr)

private:
    explicit CResourceMgr();
    ~CResourceMgr() override;

public:
    HRESULT Ready_Resource(DEVICE graphicDev);
    HRESULT Ready_BossResource(DEVICE graphicDev);
    HRESULT Ready_BossResource2(DEVICE graphicDev);
    HRESULT Ready_Buffer(DEVICE graphicDev);
    HRESULT Ready_Player(DEVICE graphicDev);
    HRESULT Ready_Monster(DEVICE graphicDev);
    HRESULT Ready_UI(DEVICE graphicDev);
    HRESULT Ready_Item(DEVICE graphicDev);
    HRESULT Ready_Common(DEVICE graphicDev);
    HRESULT Ready_Terrain(DEVICE graphicDev);
    HRESULT Ready_Decoration(DEVICE graphicDev);
    HRESULT Ready_Particle(DEVICE graphicDev);
    HRESULT Ready_Building(DEVICE graphicDev);
    HRESULT Ready_BossP1(DEVICE graphicDev);

public:
    HRESULT Ready_BossP2(DEVICE graphicDev);
    HRESULT Ready_BossP2_Eyeball(DEVICE graphicDev);
    HRESULT Ready_BossP2_Follower(DEVICE graphicDev);

    HRESULT Ready_Follower(DEVICE graphicDev);

    HRESULT Ready_MiniBoss(DEVICE graphicDev);


public:
    void Free() override;
};


