#include "CRenderObject.h"
#include "CTransform.h"
#include "CRcTex.h"
#include "CTexture.h"
#include "CCreateHelper.h"

CRenderObject::CRenderObject(DEVICE graphicDev)
    : CGameObject(graphicDev)
    , m_TransformCom(nullptr)
    , m_BufferCom(nullptr)
    , m_TextureCom(nullptr)
{ }

CRenderObject::CRenderObject(const CRenderObject& rhs)
    : CGameObject(rhs)
    , m_TransformCom(nullptr)
    , m_BufferCom(nullptr)
    , m_TextureCom(nullptr)
{ }

CRenderObject::~CRenderObject()
{ }

HRESULT CRenderObject::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    return S_OK;
}

void CRenderObject::Initialize_GameObject()
{
    CGameObject::Initialize_GameObject();


}

_int CRenderObject::Update_GameObject(const _float& timeDelta)
{
    return CGameObject::Update_GameObject(timeDelta);
}

void CRenderObject::LateUpdate_GameObject(const _float& timeDelta)
{
    CGameObject::LateUpdate_GameObject(timeDelta);
}

void CRenderObject::Render_GameObject()
{
    if (m_TransformCom)
        m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());

    // 렌더 설정
    Render_Setting();

    // 버퍼 렌더링
    if (m_BufferCom)
        m_BufferCom->Render_Buffer();

    // 렌더 리셋
    Render_Reset();
}

HRESULT CRenderObject::Add_Component()
{
    m_TransformCom = CreateProtoComponent<CTransform>(this, COMPONENTTYPE::TRANSFORM);
    NULL_CHECK_RETURN(m_TransformCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::TRANSFORM, m_TransformCom });

    m_BufferCom = CreateProtoComponent<CRcTex>(this, COMPONENTTYPE::RC_TEX);
    NULL_CHECK_RETURN(m_BufferCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::RC_TEX, m_BufferCom });

    return S_OK;
}

void CRenderObject::Render_Setting()
{
    m_GraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_GraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_GraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_GraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    m_GraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    m_GraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
    m_GraphicDev->SetRenderState(D3DRS_ALPHAREF, 0);

    m_GraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE); // 일단 FALSE, 조명 먹을 애들만 TRUE

    Set_Material();
}

void CRenderObject::Render_Reset()
{
    m_GraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    m_GraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    m_GraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

    m_GraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
}

void CRenderObject::Set_Material()
{
    D3DMATERIAL9 material;

    ZeroMemory(&material, sizeof(D3DMATERIAL9));

    material.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
    material.Ambient = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);  
    material.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
    material.Emissive = D3DXCOLOR(0.f, 0.f, 0.f, 1.f);
    material.Power = 0.f;

    m_GraphicDev->SetMaterial(&material);

}

void CRenderObject::Free()
{
    CGameObject::Free();
}
