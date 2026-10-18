#include "CLightMgr.h"

#include "CLight.h"

IMPLEMENT_SINGLETON(CLightMgr)

CLightMgr::CLightMgr()
{ }

CLightMgr::~CLightMgr()
{
    CLightMgr::Free();
}

HRESULT CLightMgr::Ready_Light(DEVICE           graphicDev,
                               const D3DLIGHT9* lightInfo,
                               const _uint&     index)
{
    auto light = CLight::Create(graphicDev, lightInfo, index);
    if (nullptr == light)
        return E_FAIL;

    m_LightList.push_back(light);

    return S_OK;
}

void CLightMgr::Clear_Lights()
{
    for_each(m_LightList.begin(), m_LightList.end(), CDeleteObj());
    m_LightList.clear();
}

void CLightMgr::Free()
{
    for_each(m_LightList.begin(), m_LightList.end(), CDeleteObj());
    m_LightList.clear();
}
