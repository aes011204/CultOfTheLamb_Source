#pragma once
#include "CScene.h"

class CEdit : public CScene
{
private:
    explicit CEdit(DEVICE graphicDev);
    ~CEdit() override;

public:
    HRESULT Ready_Scene() override;
    _int    Update_Scene(const _float& timeDelta) override;
    void    LateUpdate_Scene(const _float& timeDelta) override;
    void    Render_Scene() override;
    HRESULT Ready_Light() override;

private:
    HRESULT Ready_Environment_Layer(LAYERTYPE layerType);
    HRESULT Ready_GameLogic_Layer(LAYERTYPE layerType);
    HRESULT Ready_UI_Layer(LAYERTYPE layerType);

public:
    static CEdit* Create(DEVICE graphicDev);

private:
    void Free() override;
};
