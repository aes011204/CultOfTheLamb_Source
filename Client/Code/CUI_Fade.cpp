#include "pch.h"
#include "CUI_Fade.h"
#include "CRenderer.h"
#include "CRcTex.h"
#include "CTransform.h"
#include "CCreateHelper.h"

CUI_Fade::CUI_Fade(DEVICE graphicDev)
    : CUI(graphicDev),
    m_BufferCom(nullptr),
    m_TransformCom(nullptr),
    m_Alpha(0.0f),
    m_FadeSpeed(1.0f),
    m_IsFadingIn(false),
    m_IsActive(false),
    m_UseRenderer(true)
{
}

CUI_Fade::~CUI_Fade()
{
}

HRESULT CUI_Fade::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    m_Alpha = 0.0f;
    m_IsActive = false;

    return S_OK;
}

void CUI_Fade::Initialize_GameObject()
{
    CUI::Initialize_GameObject();

}

_int CUI_Fade::Update_GameObject(const _float& timeDelta)
{
    _int exit = CUI::Update_GameObject(timeDelta);

    if (m_UseRenderer)
    {
        CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);
    }

    if (!m_IsActive) // 진행중이면 ㄴㄴ
        return exit;

    // Fade In: 불투명 -> 투명
    if (m_IsFadingIn)
    {
        m_Alpha -= m_FadeSpeed * timeDelta;

        if (m_Alpha <= 0.f)
        {
            m_Alpha = 0.f;
            m_IsActive = false;  
        }
    }
    // Fade Out: 투명 -> 불투명
    else
    {
        m_Alpha += m_FadeSpeed * timeDelta;

        if (m_Alpha >= 1.f)
        {
            m_Alpha = 1.f;
            m_IsActive = false; 
        }
    }

    return exit;
}

void CUI_Fade::LateUpdate_GameObject(const _float& timeDelta)
{
    CUI::LateUpdate_GameObject(timeDelta);
}

void CUI_Fade::Render_GameObject()
{
    CUI::Render_GameObject();

    if (!m_IsActive)
        return;

    m_TransformCom->Set_Pos(0.f, 0.f, 0.f);
    m_TransformCom->Set_Scale(WINCX, WINCY, 1.f);

    m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());

    DWORD alpha = (_ubyte)(m_Alpha * 255.f);
    DWORD color = D3DCOLOR_ARGB(alpha, 0, 0, 0);  

    m_GraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, color);

    // 알파 블렌딩
    m_GraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_GraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_GraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    m_GraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR); 
    m_GraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR); 

    // 텍스처 없이
    m_GraphicDev->SetTexture(0, NULL);
    m_BufferCom->Render_Buffer();

    // 복원
    m_GraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);

    m_GraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}

void CUI_Fade::Set_Alpha(_float alpha)
{
    m_Alpha = alpha;

    if (alpha <= 0.f)
        m_IsActive = false;
}

void CUI_Fade::Start_FadeIn(_float speed)
{
    m_Alpha = 1.0f;        
    m_FadeSpeed = speed;   
    m_IsFadingIn = true;   
    m_IsActive = true;     
}

void CUI_Fade::Start_FadeOut(_float speed)
{
    m_Alpha = 0.0f;        
    m_FadeSpeed = speed;   
    m_IsFadingIn = false;  
    m_IsActive = true;     
}

HRESULT CUI_Fade::Add_Component()
{
    // Buffer
    m_BufferCom = CreateProtoComponent<CRcTex>(this, COMPONENTTYPE::RC_TEX);
    NULL_CHECK_RETURN(m_BufferCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::RC_TEX, m_BufferCom });

    // Transform
    m_TransformCom = CreateProtoComponent<CTransform>(this, COMPONENTTYPE::TRANSFORM);
    NULL_CHECK_RETURN(m_TransformCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::TRANSFORM, m_TransformCom });

    return S_OK;
}

CUI_Fade* CUI_Fade::Create(DEVICE graphicDev)
{
    auto fade = new CUI_Fade(graphicDev);

    if (FAILED(fade->Ready_GameObject()))
    {
        Safe_Release(fade);
        MSG_BOX("Fade UI Create Failed");
        return nullptr;
    }

    return fade;
}

void CUI_Fade::Free()
{
    CUI::Free();
}
