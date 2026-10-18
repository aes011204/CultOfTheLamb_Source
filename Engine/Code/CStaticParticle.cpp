#include "CStaticParticle.h"
#include "CCreateHelper.h"
#include "CTexture.h"
#include "CRcTex.h"
#include "CTransform.h"
#include "CRenderer.h"
#include "CManagement.h"
#include "CLayer.h"

CStaticParticle::CStaticParticle(DEVICE graphicDev)
    : CRenderObject(graphicDev)
{
}

CStaticParticle::CStaticParticle(const CStaticParticle& rhs)
    : CRenderObject(rhs)
{
}

CStaticParticle::~CStaticParticle()
{
}

HRESULT CStaticParticle::Ready_GameObject()
{
    if (FAILED(CRenderObject::Ready_GameObject()))
        return E_FAIL;

    m_TransformCom->Set_RotationAxis(ROT_X, D3DXToRadian(25.f));

    return S_OK;
}

_int CStaticParticle::Update_GameObject(const _float& timeDelta)
{
    _int exit = CRenderObject::Update_GameObject(timeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_PARTICLE, this);

    if (!m_TransformCom)
        return exit;
    _vec3 pos = m_TransformCom->Get_Pos();

    if (!m_IsOnFloor)
    {
        m_Velocity.y -= m_Gravity * timeDelta;
        pos += m_Velocity * timeDelta;

        // 바닥 충돌 체크
        if (pos.y <= m_FloorY)
        {
            pos.y = m_FloorY;
            m_Velocity = _vec3(0.f, 0.f, 0.f);
            m_IsOnFloor = true;
            m_ElapsedTime = 0.f; 
        }

        m_TransformCom->Set_Pos(pos);
    }
    else
    {   
        m_ElapsedTime += timeDelta;
        // 바닥에 닿고나서부터 LifeTime 시작 후, 사라지도록
        if (m_ElapsedTime >= m_LifeTime)
        {
            _float fadeRatio = (m_ElapsedTime - m_LifeTime) / m_FadeTime;
            m_Alpha = 1.f - min(fadeRatio, 1.f);

            if (m_Alpha <= 0.f)
            {
                Mark_As_PendingKill();
            }
        }
    }

    return exit;
}

void CStaticParticle::Render_GameObject()
{
    if (!m_TransformCom || !m_TextureCom || !m_BufferCom)
        return;

    m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());

    Render_Setting();

    // 알파 적용
    m_GraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB((_int)(m_Alpha * 255), 255, 255, 255));
    m_GraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);

    m_TextureCom->Set_Texture(m_TextureIndex);
    m_BufferCom->Render_Buffer();

    // 알파 복원
    m_GraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);

    Render_Reset();
}

HRESULT CStaticParticle::Add_Component()
{
    if (FAILED(CRenderObject::Add_Component()))
        return E_FAIL;

    // Texture
    m_TextureCom = CreateProtoComponent<CTexture>(this, m_TextureType);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);
    m_Components[ID_STATIC].insert({ m_TextureType, m_TextureCom });

    return S_OK;
}

CStaticParticle* CStaticParticle::Create(DEVICE graphicDev, const _vec3& position, COMPONENTTYPE textureType)
{
    auto particle = new CStaticParticle(graphicDev);

    particle->m_TextureType = textureType;

    if (FAILED(particle->Ready_GameObject()))
    {
        Safe_Release(particle);
        MSG_BOX("StaticParticle Create Failed");
        return nullptr;
    }

    particle->m_TransformCom->Set_Pos(position);

    return particle;
}

void CStaticParticle::Free()
{
    CRenderObject::Free();


}
