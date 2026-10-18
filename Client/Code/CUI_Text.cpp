#include "pch.h"
#include "CUI_Text.h"
#include "CRenderer.h"
#include "CCreateHelper.h"
#include "CTransform.h"
#include "CFontMgr.h"

CUI_Text::CUI_Text(DEVICE graphicDev) : CUI(graphicDev)
{
}

CUI_Text::CUI_Text(const CUI_Text& rhs) :CUI(rhs)
{
}

CUI_Text::~CUI_Text()
{
}


HRESULT CUI_Text::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    m_Active = true;

    return S_OK;
}


_int CUI_Text::Update_GameObject(const _float& timeDelta)
{
    if (!m_Active)
        return 0;
        CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);


    return 0;
}

void CUI_Text::LateUpdate_GameObject(const _float& timeDelta)
{
}

void CUI_Text::Render_GameObject()
{
    if (!m_Active)
        return;

    m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());

   _vec2 m_TextOffset = { m_TransformCom->Get_Pos().x,m_TransformCom->Get_Pos().y };
    _vec2 screen = { m_TextOffset.x + WINCX * 0.5f ,WINCY * 0.5f - m_TextOffset.y };

    CFontMgr::GetInstance()->Render_Font(m_Font,
        m_Text,
        &screen,
        m_D3dxColor);

}

HRESULT CUI_Text::Add_Component()
{
    // transform
    m_TransformCom = CreateProtoComponent<CTransform>(this, COMPONENTTYPE::TRANSFORM);
    NULL_CHECK_RETURN(m_TransformCom, E_FAIL);

    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::TRANSFORM, m_TransformCom });

    return S_OK;

}
void CUI_Text::Free()
{
    CUI::Free();
}
CUI_Text* CUI_Text::Create(DEVICE graphicDev)
{
    auto textui = new CUI_Text(graphicDev);
    if (FAILED(textui->Ready_GameObject()))
    {
        Safe_Release(textui);
        MSG_BOX("CUI_Text Created Failed");
        return nullptr;
    }
    return textui;
}
