#include "CGameObject.h"

#include "CComponent.h"
#include "CTexture.h"
#include "CTransform.h"

CGameObject::CGameObject(DEVICE graphicDev)
    : m_GraphicDev(graphicDev),
      m_Name(L"Default_GameObject"),
      m_Active(true),
      m_PendingKill(false)
{
    m_GraphicDev->AddRef();
}

CGameObject::CGameObject(const CGameObject& rhs)
    : m_GraphicDev(rhs.m_GraphicDev),
      m_Name(rhs.m_Name),
      m_Active(true),
      m_PendingKill(false)
{
    m_GraphicDev->AddRef();
}

CGameObject::~CGameObject()
{ }

CComponent* CGameObject::Get_Component(COMPONENTID componentID, COMPONENTTYPE componentType)
{
    CComponent* component = Find_Component(componentID, componentType);

    if (nullptr == component)
        return nullptr;

    return component;
}

HRESULT CGameObject::Ready_GameObject()
{
    return S_OK;
}

void CGameObject::Initialize_GameObject()
{
    for (auto& components : m_Components)
    {
        for (auto& component : components)
        {
            component.second->Initialize_Component();
        }
    }
}

_int CGameObject::Update_GameObject(const _float& timeDelta)
{
    for (auto& component : m_Components[ID_DYNAMIC])
    {
        component.second->Update_Component(timeDelta);
    }

    return 0;
}

void CGameObject::LateUpdate_GameObject(const _float& timeDelta)
{
    for (auto& component : m_Components[ID_DYNAMIC])
    {
        component.second->LateUpdate_Component();
    }
}

void CGameObject::Render_GameObject()
{ }

CTransform* CGameObject::Get_Transform()
{
    return dynamic_cast<CTransform*>(
        Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));
}

bool CGameObject::SetTexAspectInit(CTexture* texCom, CTransform* transCom, int index, wstring* key, bool is_width, float lengh)
{
    float w = 0, h = 0;

    if (texCom->Get_TextureSize(index, w, h, key) == false || texCom == nullptr)
        return false;
    float aspect;
    if (is_width == true)
    {
        aspect = h / w;

        m_BaseScaleAsp.x = (float)lengh;
        m_BaseScaleAsp.y = (float)lengh * aspect;
        m_BaseScaleAsp.z = 1.f;

        m_ScaleAsp = { 1.f, 1.f, 1.f };

        transCom->Set_Scale({
            m_BaseScaleAsp.x * m_ScaleAsp.x,
            m_BaseScaleAsp.y * m_ScaleAsp.y,
            1.f });
    }
    else
    {
        aspect = w / h;

        m_BaseScaleAsp.y = (float)lengh;
        m_BaseScaleAsp.x = (float)lengh * aspect;
        m_BaseScaleAsp.z = 1.f;

        m_ScaleAsp = { 1.f, 1.f, 1.f };

        transCom->Set_Scale({
            m_BaseScaleAsp.x * m_ScaleAsp.x,
            m_BaseScaleAsp.y * m_ScaleAsp.y,
            1.f });
    }

    return true;
}

void CGameObject::SetAspectScale(CTransform* transCom, _vec3 magnification)
{
    m_ScaleAsp = magnification;

    transCom->Set_Scale({
        m_BaseScaleAsp.x * m_ScaleAsp.x,
        m_BaseScaleAsp.y * m_ScaleAsp.y,
        1.f });
}

CComponent* CGameObject::Find_Component(COMPONENTID componentID, COMPONENTTYPE componentType)
{
    auto& componentMap = m_Components[componentID];

    auto iter = componentMap.find(componentType);

    if (iter == componentMap.end())
        return nullptr;

    return iter->second;
}

void CGameObject::Free()
{
    for (_uint i = 0; i < ID_END; ++i)
    {
        for_each(m_Components[i].begin(), m_Components[i].end(), CDeleteMap());
        m_Components[i].clear();
    }

    Safe_Release(m_GraphicDev);
}
