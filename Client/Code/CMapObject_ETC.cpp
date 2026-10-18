#include "pch.h"
#include "CMapObject_ETC.h"

#include "CTransform.h"

CMapObject_ETC::CMapObject_ETC(DEVICE graphicDev)
    : CMapObjectBase(graphicDev)
{ }

CMapObject_ETC::CMapObject_ETC(const CMapObjectBase& rhs)
    : CMapObjectBase(rhs)
{ }

CMapObject_ETC::~CMapObject_ETC()
{ }

HRESULT CMapObject_ETC::Ready_GameObject()
{
    if (FAILED(Add_BaseComponent(COMPONENTTYPE::TEX_ETC)))
        return E_FAIL;

    m_TransformCom->Set_Scale(_vec3(1.f, 1.f, 1.f));

    return S_OK;
}

_int CMapObject_ETC::Update_GameObject(const _float& timeDelta)
{
    _int exit = CMapObjectBase::Update_GameObject(timeDelta);

    return exit;
}

void CMapObject_ETC::LateUpdate_GameObject(const _float& timeDelta)
{
    CMapObjectBase::LateUpdate_GameObject(timeDelta);
}

void CMapObject_ETC::Render_GameObject()
{
    CMapObjectBase::Render_GameObject();
}

MAPOBJTYPE CMapObject_ETC::Get_Type()
{
    return MAPOBJTYPE::ETC;
}

CMapObject_ETC* CMapObject_ETC::Create(DEVICE graphicDev)
{
    CMapObject_ETC* etc = new CMapObject_ETC(graphicDev);

    if (FAILED(etc->Ready_GameObject()))
    {
        Safe_Release(etc);
        MSG_BOX("ETC Create Failed");
        return nullptr;
    }

    return etc;
}

void CMapObject_ETC::Free()
{
    CMapObjectBase::Free();
}
