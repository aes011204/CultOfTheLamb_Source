#pragma once

#include "CBase.h"

BEGIN(Engine)
class CScene;
END

class CFog;

class CFogMgr : public CBase
{
    DECLARE_SINGLETON(CFogMgr)

private:
    CFogMgr();
    virtual ~CFogMgr();

public:
    void Ready_FogMgr(DEVICE graphicDev);
    void Update_FogMgr(const _float& timeDelta);
    void Render_FogWindow();
    void Render_Preview();

    void Start_Place();
    void Stop_Place();
    void Place_Fog();         // 현재 위치에 안개 생성
    void Remove_LastFog();    // 마지막 안개 삭제

public:
    HRESULT Save_ToStream(std::ofstream& ofs);
    HRESULT Load_FromStream(std::ifstream& ifs, _vec3 offset);
    void    Clear_AllFog();
    bool    IsPlacing() const { return m_IsPlacing; }

private:
    void Free() override;

private:
    DEVICE              m_GraphicDev = nullptr;
    vector<CFog*>       m_PlacedFogs;
    CFog*               m_PreviewFog = nullptr;
    bool                m_IsPlacing = false;

    _vec3               m_FogScale = { 10.f, 8.f, 1.f };
    _float              m_SwaySpeed = 0.5f;
    _float              m_SwayAmount = 2.f;
    _float              m_Alpha = 0.5f;
    _float              m_YOffset = 2.f;

    _vec3               m_FogRotation = { 0.f, 0.f, 0.f };
    _float              m_FogColor[3] = { 1.f, 1.f, 1.f };

};
