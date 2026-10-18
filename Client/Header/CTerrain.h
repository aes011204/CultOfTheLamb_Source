#pragma once

#include "CRenderObject.h"

BEGIN(Engine)
class CRcTex;
class CTerrainTex;
class CTransform;
class CTexture;
class CLayer;
END

class CTerrainRenderer;
class CWallRenderer;
class CObjectSpawner;

enum class EDIT_MODE
{
    TILE, OBJECT, EDIT_MODE_END
};

class CTerrain : public CRenderObject
{
private:
    explicit CTerrain(DEVICE GraphicDev);
    explicit CTerrain(const CTerrain& rhs);
    virtual ~CTerrain();

public:
    HRESULT Ready_GameObject() override;
    void    Initialize_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;

    void    Render_Editor() override;

    void    LoadMapOnStart(const char* filePath);
    void    Spawn_Walls(CLayer* layer);

public:
    // 벽만들기
    CWallRenderer*   Get_WallRenderer() const { return m_WallRendererCom; }
    _vec3            Get_CellCenterWorld(_int cellX, _int cellZ);
    _float           Get_CellSize();

public:
    // 오브젝트 소환
    void    Spawn_Objects(CLayer* layer);

private:
    HRESULT Add_Component() override;
    void    Set_Material() override;
    void    Render_Setting() override;

public:
    static CTerrain* Create(DEVICE GraphicDev);

protected:
    void Free() override;

private:    
    Engine::CTerrainTex*        m_BufferCom;

    Engine::CTexture*           m_TextureCom284;
    Engine::CTexture*           m_TextureCom1024;
    Engine::CTexture*           m_TextureComOutline;
    Engine::CTexture*           m_TextureComOutline2;

    CTerrainRenderer*           m_TerrainRendererCom;
    CWallRenderer*              m_WallRendererCom;
    CObjectSpawner*             m_ObjectSpawnerCom;

    _bool                       m_IsWallSpawn;
    _bool                       m_IsObjectSpawn;
};

