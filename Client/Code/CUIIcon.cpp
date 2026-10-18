#include "pch.h"
#include "CUIIcon.h"
#include "CCreateHelper.h"
#include "CRcTex.h"
#include "CRenderer.h"
#include "CTexture.h"
#include "CTransform.h"

int CUIIcon::m_nextID = 0;

CUIIcon::CUIIcon(DEVICE pGraphicDev)
    : CUI(pGraphicDev),
    m_Register(true)
{
    m_ID = ++m_nextID;
}

CUIIcon::CUIIcon(const CUIIcon& rhs)
    : CUI(rhs),
    m_Register(true)
{
    m_ID = ++m_nextID;
}

CUIIcon::~CUIIcon()
{}

HRESULT CUIIcon::Ready_GameObject(COMPONENTTYPE texProtoTag)
{
    if (FAILED(Add_Component(texProtoTag)))
        return E_FAIL;

    m_TransformCom->Set_Pos(_vec3(0.f, 0.f, 1.f));
    //m_TransformCom->Set_Scale(_vec3(100.f, 100.f, 1.f));

    return S_OK;
}

bool CUIIcon::SetCloneTexAspectScale(_vec3 vec) // 텍스쳐 등록후
{
    float x, y;
    m_TextureCom->Get_TextureSize(0, x, y);
    if (false == SetTexAspectInit(m_TextureCom, m_TransformCom, 0, nullptr, true, x))
        return false;

    SetAspectScale(m_TransformCom, vec);

    return true;
}

_int CUIIcon::Update_GameObject(const _float& timeDelta)
{
    _int exit = CUI::Update_GameObject(timeDelta);
    if (m_Register)
    {
        CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);
    }

    return exit;
}

void CUIIcon::LateUpdate_GameObject(const _float& timeDelta)
{
    CUI::LateUpdate_GameObject(timeDelta);
}

void CUIIcon::Render_Setting()
{
    //m_GraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    //
    //m_GraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    //m_GraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    //m_GraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

}


void CUIIcon::Render_Reset()
{
    //m_GraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    //m_GraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    //m_GraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CUIIcon::Render_GameObject()
{
    CUI::Render_GameObject();

    Render_Setting();
    m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());
    m_TextureCom->Set_Texture(m_textureIndex);
    m_BufferCom->Render_Buffer();
    Render_Reset();
}

HRESULT CUIIcon::Add_Component(COMPONENTTYPE texProtoTag)
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
    m_TextureCom = CreateProtoComponent<CTexture>(this, texProtoTag);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);

    m_Components[ID_STATIC].insert({ texProtoTag, m_TextureCom });

    return S_OK;
}

CUIIcon* CUIIcon::Create(DEVICE graphicDev, COMPONENTTYPE texProtoTag)
{
    auto Icon = new CUIIcon(graphicDev);
    if (FAILED(Icon->Ready_GameObject(texProtoTag)))
    {
        Safe_Release(Icon);
        MSG_BOX("Icon Created Failed");
        return nullptr;
    }
    return Icon;
}

void CUIIcon::Free()
{
    CUI::Free();
}
