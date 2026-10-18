#include "pch.h"
#include "CMapObjectBase.h"

#include "CCreateHelper.h"
#include "CRcTex.h"
#include "CRenderer.h"
#include "CTerrain.h"
#include "CTexture.h"
#include "CTransform.h"

CMapObjectBase::CMapObjectBase(DEVICE graphicDev)
    : CRenderObject(graphicDev),
    m_TextureCom(nullptr),
    m_AttachTerrain(false), m_IsPreview(false),
    m_Terrain(nullptr)
{
}

CMapObjectBase::CMapObjectBase(const CMapObjectBase& rhs)
    : CRenderObject(rhs),
    m_TextureCom(nullptr),
    m_AttachTerrain(rhs.m_AttachTerrain), m_IsPreview(false),
    m_CellX(rhs.m_CellX),
    m_CellZ(rhs.m_CellZ),
    m_Terrain(rhs.m_Terrain)
{
}

CMapObjectBase::~CMapObjectBase()
{
}

HRESULT CMapObjectBase::Ready_GameObject()
{
    // 여기에 Light?
    return S_OK;
}

void CMapObjectBase::Initialize_GameObject()
{
    CRenderObject::Initialize_GameObject();

}

_int CMapObjectBase::Update_GameObject(const _float& timeDelta)
{
    _int exit = CRenderObject::Update_GameObject(timeDelta);


    return exit;
}

void CMapObjectBase::LateUpdate_GameObject(const _float& timeDelta)
{
    CRenderObject::LateUpdate_GameObject(timeDelta);

    if (!m_IsPreview)
        CRenderer::GetInstance()->Add_RenderGroup(RENDERID::RENDER_ALPHA, this);
}

void CMapObjectBase::Render_GameObject()
{
    if (!m_TransformCom || !m_BufferCom || !m_TextureCom)
        return;

    m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());

    Render_Setting();

    m_TextureCom->Set_Texture(m_TextureIndex);
    m_BufferCom->Render_Buffer();

    Render_Reset();

}

void CMapObjectBase::Set_Material()
{
    m_GraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);

    D3DMATERIAL9 material;

    ZeroMemory(&material, sizeof(D3DMATERIAL9));

    material.Diffuse = COLOR(1.f, 1.f, 1.f, 1.f);
    material.Ambient = COLOR(1.f, 1.f, 1.f, 1.f);
    material.Specular = COLOR(1.f, 1.f, 1.f, 1.f);
    material.Emissive = COLOR(0.f, 0.f, 0.f, 1.f);
    material.Power = 0.f;

    m_GraphicDev->SetMaterial(&material);
}

void CMapObjectBase::Set_WorldPos(const _vec3& pos)
{
    if (m_TransformCom == nullptr)
    {
        CComponent* com = Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM);
        m_TransformCom = dynamic_cast<CTransform*>(com);

        if (m_TransformCom == nullptr)
            return;
    }

    m_TransformCom->Set_Pos(pos);
}

HRESULT CMapObjectBase::Add_BaseComponent(COMPONENTTYPE InType)
{
    if (FAILED(CRenderObject::Add_Component()))
        return E_FAIL;

    // 자식에서 Add_BaseComponent에 타입 넣어주기
    m_TextureCom = CreateProtoComponent<CTexture>(this, InType);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);

    m_Components[ID_STATIC].insert({ InType, m_TextureCom });

    return S_OK;
}

void CMapObjectBase::Render_Setting()
{
    CRenderObject::Render_Setting();

    m_GraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
    Set_Material();
}

void CMapObjectBase::Render_Reset()
{
    CRenderObject::Render_Reset();
}



void CMapObjectBase::Free()
{
    CRenderObject::Free();
}
