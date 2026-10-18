#pragma once
#include "CScene.h"

class CStage_TarotTest : public CScene
{
private:
    explicit CStage_TarotTest(DEVICE graphicDev);
    ~CStage_TarotTest() override;

public:
    HRESULT Ready_Scene() override;
    _int    Update_Scene(const _float& timeDelta) override;
    void    LateUpdate_Scene(const _float& timeDelta) override;
    void    Render_Scene() override;

private:
    HRESULT Ready_Environment_Layer(LAYERTYPE layerType);
    HRESULT Ready_GameLogic_Layer(LAYERTYPE layerType);
    HRESULT Ready_UI_Layer(LAYERTYPE layerType);

    HRESULT Ready_Prototype();

public:
    static CStage_TarotTest* Create(DEVICE graphicDev);

private:
    void Free() override;
};
