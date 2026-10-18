#include "CComponent.h"
#include "CEnumHelper.h"
#include "CGameObject.h"
#include "CTransform.h"

CComponent::CComponent()
    : m_ComponentType(COMPONENTTYPE::COMPONENT_END),
      m_GraphicDev(nullptr),
      m_Owner(nullptr),
      m_TransformCom(nullptr),
      m_IsClone(false),
      m_Active(true)
{ }

CComponent::CComponent(DEVICE graphicDev)
    : m_ComponentType(COMPONENTTYPE::COMPONENT_END),
      m_GraphicDev(graphicDev),
      m_Owner(nullptr),
      m_TransformCom(nullptr),
      m_IsClone(false),
      m_Active(true)
{
    m_GraphicDev->AddRef();
}

CComponent::CComponent(const CComponent& rhs)
    : m_ComponentType(COMPONENTTYPE::COMPONENT_END),
      m_GraphicDev(rhs.m_GraphicDev),
      m_Owner(nullptr),
      m_TransformCom(nullptr),
      m_IsClone(true),
      m_Active(true)
{
    if (m_GraphicDev != nullptr)
        m_GraphicDev->AddRef();
}

CComponent::~CComponent()
{ }

void CComponent::Set_Owner(CGameObject* owner)
{
    m_Owner = owner;

    if (m_Owner)
    {
        m_TransformCom = dynamic_cast<CTransform*>(
            m_Owner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));
    }
}

void CComponent::Free()
{
    Safe_Release(m_GraphicDev);
}
