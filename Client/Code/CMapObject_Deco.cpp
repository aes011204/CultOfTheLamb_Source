#include "pch.h"
#include "CMapObject_Deco.h"

#include "CTransform.h"

CMapObject_Deco::CMapObject_Deco(DEVICE graphicDev)
    : CMapObjectBase(graphicDev)
{ }

CMapObject_Deco::CMapObject_Deco(const CMapObjectBase& rhs)
    : CMapObjectBase(rhs)
{ }

CMapObject_Deco::~CMapObject_Deco()
{ }

HRESULT CMapObject_Deco::Ready_GameObject()
{
    if (FAILED(Add_BaseComponent(COMPONENTTYPE::TEX_DECO)))
        return E_FAIL;

    m_TransformCom->Set_Scale(_vec3(1.f, 1.f, 1.f));

    return S_OK;
}

_int CMapObject_Deco::Update_GameObject(const _float& timeDelta)
{
    _int exit = CMapObjectBase::Update_GameObject(timeDelta);

    return exit;
}

void CMapObject_Deco::LateUpdate_GameObject(const _float& timeDelta)
{
    CMapObjectBase::LateUpdate_GameObject(timeDelta);
}

void CMapObject_Deco::Render_GameObject()
{
    CMapObjectBase::Render_GameObject();
}

MAPOBJTYPE CMapObject_Deco::Get_Type()
{
    return MAPOBJTYPE::DECO;
}

CMapObject_Deco* CMapObject_Deco::Create(DEVICE graphicDev)
{
    CMapObject_Deco* deco = new CMapObject_Deco(graphicDev);

    if (FAILED(deco->Ready_GameObject()))
    {
        Safe_Release(deco);
        MSG_BOX("Decoration Create Failed");
        return nullptr;
    }

    return deco;
}

void CMapObject_Deco::Free()
{
    CMapObjectBase::Free();
}
