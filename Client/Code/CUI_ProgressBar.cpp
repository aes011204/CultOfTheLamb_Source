#include "pch.h"
#include "CUI_ProgressBar.h"
#include "CRcTex.h"
#include "CTexture.h"
#include "CTransform.h"
#include "CCreateHelper.h"
#include "CManagement.h"
#include "CRenderer.h"

CUI_ProgressBar::CUI_ProgressBar(DEVICE graphicDev): CUI(graphicDev) {}
CUI_ProgressBar::CUI_ProgressBar(const CUI_ProgressBar& rhs): CUI(rhs) {}
CUI_ProgressBar::~CUI_ProgressBar() {}

HRESULT CUI_ProgressBar::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    return S_OK;
}

void CUI_ProgressBar::Initialize_GameObject()
{
    CUI::Initialize_GameObject();

    if (m_TransformCom)
    {
        m_PlayerTransform = dynamic_cast<CTransform*>(
            CManagement::GetInstance()->Get_Component(
                ID_DYNAMIC,
                LAYERTYPE::GAMELOGIC,
                OBJTYPE::PLAYER,
                COMPONENTTYPE::TRANSFORM));
        m_TransformCom->Set_Scale(0.5f, 0.2f, 1.f);
    }
    m_Active = false;
}

_int CUI_ProgressBar::Update_GameObject(const _float& timeDelta)
{
    if (!m_Active)
        return 0;

    _int exit = CUI::Update_GameObject(timeDelta);

    if (m_PlayerTransform)
    {
        _matrix view,      proj, world;
        _vec3   screenPos, ownerPos;

        ownerPos = m_PlayerTransform->Get_Pos();
        ownerPos.y += 2.2f;

        m_GraphicDev->GetTransform(D3DTS_VIEW, &view);
        m_GraphicDev->GetTransform(D3DTS_PROJECTION, &proj);
        D3DXMatrixIdentity(&world);

        D3DVIEWPORT9 vp;
        m_GraphicDev->GetViewport(&vp);

        D3DXVec3Project(&screenPos, &ownerPos, &vp, &proj, &view, &world);

        _float uiX = screenPos.x - WINCX * 0.5f;
        _float uiY = -(screenPos.y - WINCY * 0.5f);

        m_TransformCom->Set_Pos(uiX, uiY, 0.f);
    }

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

    return exit;
}

void CUI_ProgressBar::LateUpdate_GameObject(const _float& timeDelta)
{
    CUI::LateUpdate_GameObject(timeDelta);
}

void CUI_ProgressBar::Render_GameObject()
{
    if (!m_Active)
        return;

    m_GraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
    //m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());
    CUI::Render_GameObject();

    const _float HALF_WIDTH  = 100.f;
    const _float HALF_HEIGHT = 50.f;

    m_BufferCom->Set_Scale(HALF_WIDTH, HALF_HEIGHT);
    m_BufferCom->Set_Offset(0.f, 0.f);

    m_GraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_GraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_GraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_GraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    m_GraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    m_GraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
    m_GraphicDev->SetRenderState(D3DRS_ALPHAREF, 0);
    m_TransformCom->Get_Pos();
    m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());

    m_TextureCom->Set_Texture(0);
    m_BufferCom->Render_Buffer();

    const float UV_LEFT  = 590 / 598.f;
    const float UV_RIGHT = 1 / 598.f;
    const float UV_RANGE = UV_RIGHT - UV_LEFT;

    float hw      = HALF_WIDTH * m_Progress;
    float offsetX = HALF_WIDTH * (1.f - m_Progress);

    m_ProgressBar->Set_Scale(hw, HALF_HEIGHT);
    m_ProgressBar->Set_Offset(offsetX, 0.f);

    float u_end = UV_LEFT + UV_RANGE * m_Progress;

    m_ProgressBar->Set_UV(UV_LEFT, 0.f, u_end, 1.f);
    m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());
    m_TextureCom->Set_Texture(3);
    m_ProgressBar->Render_Buffer();

    m_GraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    m_GraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    m_GraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

    m_GraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
}

HRESULT CUI_ProgressBar::Add_Component()
{
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_UI_COOK);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_UI_COOK, m_TextureCom });

    m_TransformCom = CreateProtoComponent<CTransform>(this, COMPONENTTYPE::TRANSFORM);
    NULL_CHECK_RETURN(m_TransformCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::TRANSFORM, m_TransformCom });

    m_BufferCom = CreateProtoComponent<CRcTex>(this, COMPONENTTYPE::RC_TEX);
    NULL_CHECK_RETURN(m_BufferCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::RC_TEX, m_BufferCom });

    m_ProgressBar = CreateProtoComponent<CRcTex>(this, COMPONENTTYPE::RC_TEX);
    NULL_CHECK_RETURN(m_ProgressBar, E_FAIL);

    return S_OK;
}

void CUI_ProgressBar::Free()
{
    Safe_Release(m_ProgressBar);
    CUI::Free();
}

CUI_ProgressBar* CUI_ProgressBar::Create(DEVICE graphicDev)
{
    CUI_ProgressBar* bar = new CUI_ProgressBar(graphicDev);

    if (FAILED(bar->Ready_GameObject()))
    {
        Safe_Release(bar);
        MSG_BOX("CUI_ProgressBar Create Failed");
        return nullptr;
    }
    return bar;
}
