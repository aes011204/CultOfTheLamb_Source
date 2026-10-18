#pragma once

#include "CBase.h"
#include "CMapObjectBase.h"

class CMapEditorPlacer;

struct MapObjectInfo
{
    const char* name;
    MAPOBJTYPE  type;
    bool*       showFlag;
};

BEGIN(Engine)

class CMapObjectMgr :public CBase
{
    DECLARE_SINGLETON(CMapObjectMgr)

private:
    explicit CMapObjectMgr();
    virtual  ~CMapObjectMgr();

public:
    void Ready_MapObjectMgr(DEVICE graphicDev);
    void Update_MapObjectMgr(const _float& timeDelta);

    // Map Object 배치
    void    Render_MapObjectWindow();

    void    Render_GrassWindow();
    void    Render_RubbleWindow();
    void    Render_DecoWindow();
    void    Render_DungeonDeco();
    void    Render_ETC();

    HRESULT SaveMapObject(const char* filePath);
    HRESULT LoadMapObject(const char* filePath);

    HRESULT Save_ToStream(ofstream& ofs);
    HRESULT Load_FromStream(ifstream& ifs, _vec3 terrainOffset = _vec3(0.f, 0.f, 0.f));

    void    Clear_PlacedObjects();  // 로드 전 기존 오브젝트 정리
    void    Refresh_MapObjectList();

    void    Undo_LastPlacement();   // 오브젝트 되돌리기
    void    Add_Placement(CGameObject* object);

    void    Remove_Placement(CGameObject* object); // 오브젝트 제거

    CMapEditorPlacer* Get_MapPlacer() const { return m_MapPlacer; }
    const vector<CGameObject*>& Get_PlacedObjects()
    {
        return m_PlacedObjects;
    }

private:
    inline void Free() override;

private:
    DEVICE                  m_GraphicDev = nullptr;
    CMapEditorPlacer*       m_MapPlacer = nullptr;

    vector<string>          m_MapObjList{};
    vector<CGameObject*>    m_PlacedObjects{}; // 배치된 오브젝트

    _int                    m_SelectMapObjIndex = 0;

    _int                    m_SelectGrassIndex = 0;
    _int                    m_SelectRubbleIndex = 0;
    _int                    m_SelectDecoIndex = 0;
    _int                    m_SelectDungeonDecoIndex = 0;
    _int                    m_SelectETCIndex = 0;

    _bool                   m_ShowGrass = false;
    _bool                   m_ShowRubble = false;
    _bool                   m_ShowDeco = false;
    _bool                   m_ShowDungeonDeco = false;
    _bool                   m_ShowETC = false;

    char                    m_MapObjFileName[128] = "MapObject_";


};

END
