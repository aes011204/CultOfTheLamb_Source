#include "pch.h"
#include "CMapObject_Rubble.h"

#include "CTransform.h"

CMapObject_Rubble::CMapObject_Rubble(DEVICE graphicDev)
    : CMapObjectBase(graphicDev)
{ }

CMapObject_Rubble::CMapObject_Rubble(const CMapObjectBase& rhs)
    : CMapObjectBase(rhs)
{ }

CMapObject_Rubble::~CMapObject_Rubble()
{ }

HRESULT CMapObject_Rubble::Ready_GameObject()
{
    if (FAILED(Add_BaseComponent(COMPONENTTYPE::TEX_RUBBLE)))
        return E_FAIL;

    m_TransformCom->Set_Scale(_vec3(1.f, 1.f, 1.f));

    return S_OK;
}

_int CMapObject_Rubble::Update_GameObject(const _float& timeDelta)
{
    _int exit = CMapObjectBase::Update_GameObject(timeDelta);

    return exit;
}

void CMapObject_Rubble::LateUpdate_GameObject(const _float& timeDelta)
{
    CMapObjectBase::LateUpdate_GameObject(timeDelta);
}

void CMapObject_Rubble::Render_GameObject()
{
    CMapObjectBase::Render_GameObject();
}

MAPOBJTYPE CMapObject_Rubble::Get_Type()
{
    return MAPOBJTYPE::RUBBLE;
}

CMapObject_Rubble* CMapObject_Rubble::Create(DEVICE graphicDev)
{
    CMapObject_Rubble* rubble = new CMapObject_Rubble(graphicDev);

    if (FAILED(rubble->Ready_GameObject()))
    {
        Safe_Release(rubble);
        MSG_BOX("Rubble Create Failed");
        return nullptr;
    }

    return rubble;
}

void CMapObject_Rubble::Free()
{
    CMapObjectBase::Free();
}
