#include "pch.h"
#include "CUI_LoadingCircle.h"
#include "CAnimator.h"
#include "CCreateHelper.h"
#include "CRcTex.h"
#include "CRenderer.h"
#include "CTexture.h"
#include "CTransform.h"

CUI_LoadingCircle::CUI_LoadingCircle(DEVICE graphicDev)
    : CUI(graphicDev)
{
}

CUI_LoadingCircle::CUI_LoadingCircle(const CUI_LoadingCircle& rhs)
    : CUI(rhs)
{
}

CUI_LoadingCircle::~CUI_LoadingCircle()
{
}

HRESULT CUI_LoadingCircle::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    m_Position = { -550.f, -300.f };  
    m_Scale = { 80.f, 80.f };

    m_TransformCom->Set_Pos(m_Position.x, m_Position.y, 0.f);
    m_TransformCom->Set_Scale(m_Scale.x, m_Scale.y, 1.f);

    return S_OK;
}

void CUI_LoadingCircle::Initialize_GameObject()
{
    if (m_AnimatorCom->Find_Animation(L"Loading_Circle"))
        return;

    CUI::Initialize_GameObject();

    m_AnimatorCom->Set_TextureType(COMPONENTTYPE::TEX_LOADING_CIRCLE);

    m_AnimatorCom->Create_Animation(L"Loading_Circle",
        58,
        0.05f,
        _vec2(1.65f, 1.f),
        _vec2(-0.5f, 0.8f));

    m_AnimatorCom->Play_Animation(L"Loading_Circle", ANIMSTATE::LOOP);

}

_int CUI_LoadingCircle::Update_GameObject(const _float& timeDelta)
{
    _int exit = CUI::Update_GameObject(timeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

    return exit;
}

void CUI_LoadingCircle::LateUpdate_GameObject(const _float& timeDelta)
{
    CUI::LateUpdate_GameObject(timeDelta);
}

void CUI_LoadingCircle::Render_GameObject()
{
    CUI::Render_GameObject();

    m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());

    m_GraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_GraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_GraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    if (m_TextureCom && m_AnimatorCom)
    {
        const wstring& key = m_AnimatorCom->Get_CurKey();
        _int frame = m_AnimatorCom->Get_CurFrame();
        m_TextureCom->Set_Texture(key, frame);
    }

    if (m_BufferCom)
        m_BufferCom->Render_Buffer();

    m_GraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}

HRESULT CUI_LoadingCircle::Add_Component()
{
    // Buffer
    m_BufferCom = CreateProtoComponent<CRcTex>(this, COMPONENTTYPE::RC_TEX);
    NULL_CHECK_RETURN(m_BufferCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::RC_TEX, m_BufferCom });

    // Transform
    m_TransformCom = CreateProtoComponent<CTransform>(this, COMPONENTTYPE::TRANSFORM);
    NULL_CHECK_RETURN(m_TransformCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::TRANSFORM, m_TransformCom });

    // Texture
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_LOADING_CIRCLE);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_LOADING_CIRCLE, m_TextureCom });

    // Animator
    m_AnimatorCom = CreateProtoComponent<CAnimator>(this, COMPONENTTYPE::ANIMATOR);
    NULL_CHECK_RETURN(m_AnimatorCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::ANIMATOR, m_AnimatorCom });


    return S_OK;
}

CUI_LoadingCircle* CUI_LoadingCircle::Create(DEVICE graphicDev)
{
    CUI_LoadingCircle* loadingCircle = new CUI_LoadingCircle(graphicDev);

    if (FAILED(loadingCircle->Ready_GameObject()))
    {
        Safe_Release(loadingCircle);
        MSG_BOX("Loading Circle Crate Failed");
        return nullptr;
    }

    loadingCircle->Initialize_GameObject();

    return loadingCircle;
}

void CUI_LoadingCircle::Free()
{
    CUI::Free();
}
