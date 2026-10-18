#include "pch.h"
#include "CMapObject_Grass.h"

#include "CTransform.h"

CMapObject_Grass::CMapObject_Grass(DEVICE graphicDev)
    : CMapObjectBase(graphicDev)
{ }

CMapObject_Grass::CMapObject_Grass(const CMapObjectBase& rhs)
    : CMapObjectBase(rhs)
{ }

CMapObject_Grass::~CMapObject_Grass()
{ }
 
HRESULT CMapObject_Grass::Ready_GameObject()
{
    if (FAILED(Add_BaseComponent(COMPONENTTYPE::TEX_GRASS)))
        return E_FAIL;

    // 기본 크기 세팅
    m_TransformCom->Set_Scale(_vec3(1.f, 1.f, 1.f));

    return S_OK;
}

_int CMapObject_Grass::Update_GameObject(const _float& timeDelta)
{
    _int exit = CMapObjectBase::Update_GameObject(timeDelta);

    return exit;
}

void CMapObject_Grass::LateUpdate_GameObject(const _float& timeDelta)
{
    CMapObjectBase::LateUpdate_GameObject(timeDelta);
}

void CMapObject_Grass::Render_GameObject()
{
    CMapObjectBase::Render_GameObject();
}

MAPOBJTYPE CMapObject_Grass::Get_Type()
{
    return MAPOBJTYPE::GRASS;
}

CMapObject_Grass* CMapObject_Grass::Create(DEVICE graphicDev)
{
    CMapObject_Grass* grass = new CMapObject_Grass(graphicDev);

    if (FAILED(grass->Ready_GameObject()))
    {
        Safe_Release(grass);
        MSG_BOX("Grass Create Failed");
        return nullptr;
    }

    return grass;
}

void CMapObject_Grass::Free()
{
    CMapObjectBase::Free();
}
