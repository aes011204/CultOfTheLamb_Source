#include "CLight.h"

CLight::CLight(DEVICE graphicDev)
    : m_GraphicDev(graphicDev), m_Light{}, m_Index(0)
{
    m_GraphicDev->AddRef();
}

CLight::~CLight()
{ }

HRESULT CLight::Ready_Light(const D3DLIGHT9* lightInfo, const _uint& index)
{
    memcpy(&m_Light, lightInfo, sizeof(D3DLIGHT9));

    m_Index = index;

    m_GraphicDev->SetLight(index, lightInfo);
    m_GraphicDev->LightEnable(index, TRUE);

    return S_OK;
}

CLight* CLight::Create(DEVICE graphicDev, const D3DLIGHT9* lightInfo, const _uint& index)
{
    auto light = new CLight(graphicDev);

    if (FAILED(light->Ready_Light(lightInfo, index)))
    {
        Safe_Release(light);
        MSG_BOX("Light Create Failed");
        return nullptr;
    }

    return light;
}

void CLight::Free()
{
    m_GraphicDev->LightEnable(m_Index, FALSE);

    Safe_Release(m_GraphicDev);
}
