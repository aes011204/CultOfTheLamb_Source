#include "pch.h"
#include "CMainBG.h"

#include "CCreateHelper.h"
#include "CRcTex.h"
#include "CRenderer.h"
#include "CTexture.h"
#include "CTransform.h"

CMainBG::CMainBG(DEVICE graphicDev)
    : CGameObject(graphicDev), m_BufferCom(nullptr), m_TextureCom(nullptr)
{ }

CMainBG::CMainBG(const CMainBG& rhs)
    : CGameObject(rhs)
{ }

CMainBG::~CMainBG()
{ }

HRESULT CMainBG::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    m_TextureCom->Set_Texture(0);

    if (false == SetTexAspectInit(m_TextureCom, m_TransformCom, 0))
        return E_FAIL;


    return S_OK;
}

_int CMainBG::Update_GameObject(const _float& timeDelta)
{
    _int exit = CGameObject::Update_GameObject(timeDelta);

    //m_TransformCom->Set_Scale(800.f, 600.f, 1.f);
    CRenderer::GetInstance()->Add_RenderGroup(RENDER_PRIORITY_UI, this);

    return exit;
}

void CMainBG::LateUpdate_GameObject(const _float& timeDelta)
{
    CGameObject::LateUpdate_GameObject(timeDelta);
}
void CMainBG::Render_GameObject()
{
    //m_GraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
    m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());
    //
    m_GraphicDev->SetRenderState(D3DRS_ZENABLE, FALSE);   // Z 버퍼 OFF
    //
    m_TextureCom->Set_Texture(0);

    m_BufferCom->Render_Buffer();
    //
    m_GraphicDev->SetRenderState(D3DRS_ZENABLE, TRUE);   // Z 버퍼 OFF
    //m_GraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
}

HRESULT CMainBG::Add_Component()
{
    // buffer
    m_BufferCom = CreateProtoComponent<CRcTex>(this, COMPONENTTYPE::RC_TEX);
    NULL_CHECK_RETURN(m_BufferCom, E_FAIL);

    m_Components[ID_STATIC].insert({ COMPONENTTYPE::RC_TEX, m_BufferCom });

    // texture
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_MAINBG);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);

    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_MAINBG, m_TextureCom });

    //transform
    m_TransformCom = CreateProtoComponent<CTransform>(this, COMPONENTTYPE::TRANSFORM);
    NULL_CHECK_RETURN(m_TransformCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::TRANSFORM, m_TransformCom });
   
    return S_OK;
}

CMainBG* CMainBG::Create(DEVICE pGraphicDev)
{
    auto backGround = new CMainBG(pGraphicDev);

    if (FAILED(backGround->Ready_GameObject()))
    {
        MSG_BOX("MainBG Create Failed");
        Safe_Release(backGround);
        return nullptr;
    }

    return backGround;
}

void CMainBG::Free()
{
    CGameObject::Free();
}
