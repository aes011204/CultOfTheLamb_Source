#pragma once
#include "CBase.h"
#include "Engine_Define.h"

BEGIN(Engine)
class ENGINE_DLL CLight : public CBase
{
private:
    explicit CLight(DEVICE graphicDev);
    ~CLight() override;

public:
    HRESULT Ready_Light(const D3DLIGHT9* lightInfo, const _uint& index);

private:
    DEVICE    m_GraphicDev;
    D3DLIGHT9 m_Light;
    _uint     m_Index;

public:
    static CLight* Create(DEVICE           graphicDev,
                          const D3DLIGHT9* lightInfo,
                          const _uint&     index);
private:
    void Free() override;
};

END
