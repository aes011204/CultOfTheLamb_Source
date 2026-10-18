#include "pch.h"
#include "CUI_Interactive.h"
#include "CRcTex.h"
#include "CTransform.h"
#include "CTexture.h"
#include "CCreateHelper.h"
#include "CRenderer.h"

CUI_Interactive::CUI_Interactive(DEVICE graphicDev):CUI(graphicDev)
{
}

CUI_Interactive::CUI_Interactive(const CUI_Interactive& rhs):CUI(rhs)
{
}

CUI_Interactive::~CUI_Interactive()
{
}

HRESULT CUI_Interactive::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    m_TransformCom->Set_Pos(_vec3(0.f, -250.f, 1.f));
    m_TransformCom->Set_Scale(_vec3(100.f, 30.f, 1.f));

    m_Active = false;
    m_textureIndex = 1;
    return S_OK;

}

_int CUI_Interactive::Update_GameObject(const _float& timeDelta)
{
    if (!m_Active)
        return 0;

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

    _int exit = CUI::Update_GameObject(timeDelta);
    //m_TransformCom->Set_Pos(_vec3(0.f, -250.f, 1.f));
    return exit;
}

void CUI_Interactive::LateUpdate_GameObject(const _float& timeDelta)
{
}

void CUI_Interactive::Render_GameObject()
{
    if (!m_Active)
        return;
    m_GraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    
    m_GraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_GraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_GraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);    m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());

    m_TextureCom->Set_Texture(m_textureIndex);
    m_BufferCom->Render_Buffer();

    m_GraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    m_GraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    m_GraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}
HRESULT CUI_Interactive::Add_Component()
{
    // buffer
    m_BufferCom = CreateProtoComponent<CRcTex>(this, COMPONENTTYPE::RC_TEX);
    NULL_CHECK_RETURN(m_BufferCom, E_FAIL);

    m_Components[ID_STATIC].insert({ COMPONENTTYPE::RC_TEX, m_BufferCom });

    // transform
    m_TransformCom = CreateProtoComponent<CTransform>(this, COMPONENTTYPE::TRANSFORM);
    NULL_CHECK_RETURN(m_TransformCom, E_FAIL);

    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::TRANSFORM, m_TransformCom });

    // texture
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_UI_INTERACTION);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);

    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_UI_INTERACTION, m_TextureCom });

    return S_OK;
}


void CUI_Interactive::Free()
{
    CUI::Free();
}

CUI_Interactive* CUI_Interactive::Create(DEVICE graphicDev)
{
    auto interactiveUI = new CUI_Interactive(graphicDev);

    if (FAILED(interactiveUI->Ready_GameObject()))
    {
        Safe_Release(interactiveUI);
        MSG_BOX("CUI_Interactive Create Failed");
        return nullptr;
    }

    return interactiveUI;
}
