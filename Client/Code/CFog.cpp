#include "pch.h"
#include "CFog.h"
#include "CCreateHelper.h"
#include "CRcTex.h"
#include "CRenderer.h"
#include "CTexture.h"
#include "CTransform.h"

CFog::CFog(DEVICE graphicDev)
    : CRenderObject(graphicDev)
{
}

CFog::CFog(const CFog& rhs)
    : CRenderObject(rhs)
{
}

CFog::~CFog()
{
}

HRESULT CFog::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    // 일단 크게 설정해보기
    m_TransformCom->Set_Scale(_vec3(15.f, 10.f, 1.f));
    m_TransformCom->Set_RotationAxis(ROT_X, D3DXToRadian(25.f));

    return S_OK;
}

void CFog::Initialize_GameObject()
{
    CRenderObject::Initialize_GameObject();
}

_int CFog::Update_GameObject(const _float& timeDelta)
{
    _int exit  = CRenderObject::Update_GameObject(timeDelta);

    m_Time += timeDelta;

    // 좌우로 흔들림
    _float offsetX = sinf(m_Time * m_SwaySpeed) * m_SwayAmount;

    _vec3 movePos = m_OriginalPos;
    movePos.x += offsetX;

    m_TransformCom->Set_Pos(movePos);

    return exit;
}

void CFog::LateUpdate_GameObject(const _float& timeDelta)
{
    CRenderObject::LateUpdate_GameObject(timeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDERID::RENDER_ALPHA, this);
}

void CFog::Render_GameObject()
{
    if (!m_TransformCom || !m_BufferCom || !m_TextureCom)
        return;

    m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());

    Render_Setting();

    m_TextureCom->Set_Texture(0);
    m_BufferCom->Render_Buffer();

    Render_Reset();
}

void CFog::Set_OriginalPos(const _vec3& pos)
{
    m_OriginalPos = pos;
    m_TransformCom->Set_Pos(pos);
}

HRESULT CFog::Add_Component()
{
    if (FAILED(CRenderObject::Add_Component()))
        return E_FAIL;

    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_FOG);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_FOG, m_TextureCom });

    return S_OK;
}

void CFog::Render_Setting()
{
    CRenderObject::Render_Setting();

    // 조명 끄기 (안개는 조명 안 받음)
    m_GraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

    // 알파블렌딩 키기. 반투명
    m_GraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_GraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_GraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    m_GraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

    m_GraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR,
        D3DCOLOR_COLORVALUE(m_Color.r, m_Color.g, m_Color.b, m_Alpha));

    m_GraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);

    m_GraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);

    m_GraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_GraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
}

void CFog::Render_Reset()
{
    m_GraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);

    m_GraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
    m_GraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    m_GraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

    m_GraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);

	m_GraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, 0xFFFFFFFF); 
	m_GraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	m_GraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_GraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
}

CFog* CFog::Create(DEVICE graphicDev)
{
    CFog* fog = new CFog(graphicDev);

    if (FAILED(fog->Ready_GameObject()))
    {
        Safe_Release(fog);
        MSG_BOX("Fog Create Failed");
        return nullptr;

    }
    return fog;
}

void CFog::Free()
{
    CRenderObject::Free();
}
