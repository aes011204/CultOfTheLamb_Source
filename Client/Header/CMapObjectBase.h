#pragma once

#include "CGameObject.h"
#include "CRenderObject.h"

class CTerrain;

BEGIN(Engine)
class CRcTex;
class CTransform;
class CTexture;
END

enum class MAPOBJTYPE
{
    /* Object 종류 추가하기 */
    GRASS, RUBBLE, DECO, DUNGEON_DECO, ETC, MAPOBJ_END,
};

typedef struct tagMapData
{
    MAPOBJTYPE type;
    _int textureIndex;
    _vec3 pos;
    _vec3 scale;
    _vec3 rotation;

} MapObjectData;

class CMapObjectBase : public CRenderObject
{
protected:
    explicit CMapObjectBase(DEVICE graphicDev);
    explicit CMapObjectBase(const CMapObjectBase& rhs);
    ~CMapObjectBase() override;

public:
    HRESULT Ready_GameObject() override;
    void    Initialize_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;

    virtual MAPOBJTYPE Get_Type() abstract;

    void Set_Material() override;

public:
    // 에디터 배치용 함수들
    void Set_WorldPos(const _vec3& pos);
    void Set_AttachToTerrain(bool attach) { m_AttachTerrain = attach; }

    void Set_Cell(_int x, _int z)
    {
        m_CellX = x;
        m_CellZ = z;
    }

    void    Set_Terrain(CTerrain* terrain) { m_Terrain = terrain; }

    void    Set_TextureIndex(_int index) { m_TextureIndex = index; }
    _int    Get_TextureIndex() const { return m_TextureIndex; }

    void    Set_Preview(_bool isPreivew) { m_IsPreview = isPreivew; }

protected:
    HRESULT Add_BaseComponent(COMPONENTTYPE InType);
    void    Render_Setting() override;
    void    Render_Reset() override;

    

protected:
    void Free() override;

protected:
    CTexture*   m_TextureCom;

    _bool       m_AttachTerrain;
    _bool       m_IsPreview;

    _int        m_CellX = -1;
    _int        m_CellZ = -1;

    CTerrain*   m_Terrain;

    _int        m_TextureIndex = 0;

    // 오브젝트 반투명 세팅
    _float      m_Alpha = 1.f;
    _float      m_TargetAlpha = 1.f;
    CTransform* m_PlayerTransformCom = nullptr;
    
};
