#include "CUIPanel.h"
#include "CUI.h"

CUIPanel::CUIPanel(DEVICE graphicDev)
    : CUI(graphicDev)
{ }

CUIPanel::CUIPanel(const CUIPanel& rhs)
    : CUI(rhs)
{ }

CUIPanel::~CUIPanel()
{ }

HRESULT CUIPanel::Ready_GameObject()
{
    CUI::Ready_GameObject();

    return S_OK;
}

_int CUIPanel::Update_GameObject(const _float& timeDelta)
{
    _int exit = CUI::Update_GameObject(timeDelta);

    for (auto& child : m_Children)
    {
        child->Update_GameObject(timeDelta);
    }

    return exit;
}

void CUIPanel::LateUpdate_GameObject(const _float& timeDelta)
{
    CUI::LateUpdate_GameObject(timeDelta);

    for (auto& child : m_Children)
    {
        child->LateUpdate_GameObject(timeDelta);
    }
}

void CUIPanel::AddChild(CUI* ui)
{
    if (ui == nullptr)
        return;

    m_Children.push_back(ui);
}

void CUIPanel::Render_GameObject()
{
    for (auto& child : m_Children)
    {
        child->Render_GameObject();
    }
}

void CUIPanel::Free()
{
    for (auto& child : m_Children)
    {
        Safe_Release(child);
    }

    m_Children.clear();

    CGameObject::Free();
}
