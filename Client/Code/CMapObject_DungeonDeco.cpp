#include "pch.h"
#include "CMapObject_DungeonDeco.h"
#include "CTransform.h"

CMapObject_DungeonDeco::CMapObject_DungeonDeco(DEVICE graphicDev)
    : CMapObjectBase(graphicDev)
{ }

CMapObject_DungeonDeco::CMapObject_DungeonDeco(const CMapObjectBase& rhs)
    : CMapObjectBase(rhs)
{ }

CMapObject_DungeonDeco::~CMapObject_DungeonDeco()
{ }

HRESULT CMapObject_DungeonDeco::Ready_GameObject()
{
    if (FAILED(Add_BaseComponent(COMPONENTTYPE::TEX_DUNGEON_DECO)))
        return E_FAIL;

    m_TransformCom->Set_Scale(_vec3(1.f, 1.f, 1.f));

    return S_OK;
}

_int CMapObject_DungeonDeco::Update_GameObject(const _float& timeDelta)
{
    _int exit = CMapObjectBase::Update_GameObject(timeDelta);

    return exit;
}

void CMapObject_DungeonDeco::LateUpdate_GameObject(const _float& timeDelta)
{
    CMapObjectBase::LateUpdate_GameObject(timeDelta);
}

void CMapObject_DungeonDeco::Render_GameObject()
{
    CMapObjectBase::Render_GameObject();
}

MAPOBJTYPE CMapObject_DungeonDeco::Get_Type()
{
    return MAPOBJTYPE::DUNGEON_DECO;
}

CMapObject_DungeonDeco* CMapObject_DungeonDeco::Create(DEVICE graphicDev)
{
    CMapObject_DungeonDeco* deco = new CMapObject_DungeonDeco(graphicDev);

    if (FAILED(deco->Ready_GameObject()))
    {
        Safe_Release(deco);
        MSG_BOX("DungeonDeco Create Failed");
        return nullptr;
    }

    return deco;
}

void CMapObject_DungeonDeco::Free()
{
    CMapObjectBase::Free();
}
