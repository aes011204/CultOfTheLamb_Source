#include "pch.h"
#include "CMenuExtra.h"

#include "CCreateHelper.h"
#include "CProtoMgr.h"
#include "CRcTex.h"
#include "CRenderer.h"
#include "CState.h"
#include "CTexture.h"
#include "CTransform.h"

CMenuExtra::CMenuExtra(DEVICE graphicDev, int textureIndex) : CRenderObject(graphicDev), m_textureIndex(textureIndex)
{
}

CMenuExtra::CMenuExtra(const CMenuExtra& rhs) : CRenderObject(rhs)
{
}

CMenuExtra::~CMenuExtra()
{
}

HRESULT CMenuExtra::Ready_GameObject()
{
    if (FAILED(CRenderObject::Ready_GameObject()))
        return E_FAIL;

    return S_OK;
}

_int CMenuExtra::Update_GameObject(const _float& timeDelta)
{
    _int exit = CRenderObject::Update_GameObject(timeDelta);



    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

    return exit;
}

void CMenuExtra::LateUpdate_GameObject(const _float& timeDelta)
{
    CRenderObject::LateUpdate_GameObject(timeDelta);
}

void CMenuExtra::Render_GameObject()
{
    //m_GraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

    m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());
    Render_Setting();

    m_TextureCom->Set_Texture(m_textureIndex);
    m_BufferCom->Render_Buffer();

    Render_Reset();

    //m_GraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
}

HRESULT CMenuExtra::Add_Component()
{
    if (FAILED(CRenderObject::Add_Component()))
        return E_FAIL;


    // texture
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_MAINMENU);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);

    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_MENU_CHAR, m_TextureCom });

    //bufferoffset1
    m_BufferComOffset1 = CreateProtoComponent<CRcTex>(this, COMPONENTTYPE::RC_TEX);
    NULL_CHECK_RETURN(m_BufferComOffset1, E_FAIL);


    //bufferoffset1
    m_BufferComOffset2 = CreateProtoComponent<CRcTex>(this, COMPONENTTYPE::RC_TEX);
    NULL_CHECK_RETURN(m_BufferComOffset2, E_FAIL);



    return S_OK;
}

CMenuExtra* CMenuExtra::Create(DEVICE graphicDev, int textureIndex)
{
    auto MenuExtra = new CMenuExtra(graphicDev, textureIndex);

    if (FAILED(MenuExtra->Ready_GameObject()))
    {
        MSG_BOX("MenuExtra Create Failed");
        Safe_Release(MenuExtra);
        return nullptr;
    }

    return MenuExtra;
}

void CMenuExtra::Free()
{
    Safe_Release(m_BufferComOffset1);
    Safe_Release(m_BufferComOffset2);
    CRenderObject::Free();
}
