#pragma once

#include "CMapObjectBase.h"
#include "CMapObject_Grass.h"
#include "CMapObject_Rubble.h"
#include "CMapObject_Deco.h"
#include "CMapObject_DungeonDeco.h"
#include "CMapObject_ETC.h"

static CMapObjectBase* CreateMapObjectType(DEVICE graphicDev, MAPOBJTYPE type, _int textureIndex)
{
    CMapObjectBase* obj = nullptr;

    switch (type)
    {
    case MAPOBJTYPE::GRASS:
        obj = CMapObject_Grass::Create(graphicDev);
        break;
    case MAPOBJTYPE::RUBBLE:
        obj = CMapObject_Rubble::Create(graphicDev);
        break;
    case MAPOBJTYPE::DECO:
        obj = CMapObject_Deco::Create(graphicDev);
        break;
    case MAPOBJTYPE::DUNGEON_DECO:
        obj = CMapObject_DungeonDeco::Create(graphicDev);
        break;
    case MAPOBJTYPE::ETC:
        obj = CMapObject_ETC::Create(graphicDev);
        break;
    }

    if (obj)
        obj->Set_TextureIndex(textureIndex);

    return obj;
}
