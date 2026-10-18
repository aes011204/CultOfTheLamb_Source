#pragma once
#include "CBase.h"
#include "Engine_Define.h"
BEGIN(Engine)
class CGraphicDev;
class CManagement;
END

class CMainApp : public CBase
{
private:
    explicit CMainApp();
    ~CMainApp() override;

public:
    HRESULT Ready_MainApp();
    _int    Update_MainApp(const float& timeDelta);
    void    LateUpdate_MainApp(const float& timeDelta);
    void    Render_MainApp();

private:
    HRESULT Ready_DefaultSetting(DEVICE* graphicDev);
    HRESULT Ready_Scene(DEVICE graphicDev);

private:
    Engine::CManagement* m_ManagementClass;
    Engine::CGraphicDev* m_DeviceClass;

    DEVICE m_GraphicDev;

public:
    static CMainApp* Create();

private:
    void Free() override;
};
