#pragma once
#include "CScene.h"

class CLoading;

class CLogo_Dungeon : public CScene
{
private:
    explicit CLogo_Dungeon(DEVICE graphicDev);
    ~CLogo_Dungeon() override;

public:
    HRESULT Ready_Scene() override;
    void    Initialize_Scene() override;
    _int    Update_Scene(const _float& timeDelta) override;
    void    LateUpdate_Scene(const _float& timeDelta) override;
    void    Render_Scene() override;

private:
    HRESULT Ready_Environment_Layer(LAYERTYPE layerType);
    HRESULT Ready_GameLogic_Layer(LAYERTYPE layerType) { return S_OK; }
    HRESULT Ready_UI_Layer(LAYERTYPE layerType);

    HRESULT Ready_Prototype();

private:
    CLoading* m_Loading;

public:
    static CLogo_Dungeon* Create(DEVICE graphicDev);

private:
    void Free() override;

    _bool   m_FadeInStarted = false;
};
