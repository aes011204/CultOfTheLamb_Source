#include "pch.h"
#include "CTerrain.h"
#include "CCombatStat.h"
#include "CBoxCollider.h"
#include "CCreateHelper.h"
#include "CDInputMgr.h"
#include "CEnumHelper.h"
#include "CFloaterMonster.h"
#include "CHumanMonster.h"
#include "CLayer.h"
#include "CMonsterBase.h"
#include "CObjectSpawner.h"
#include "CRenderer.h"
#include "CTerrainRenderer.h"
#include "CTerrainTex.h"
#include "CTexture.h"
#include "CTransform.h"
#include "CUI_MonsterHp.h"
#include "CWall.h"
#include "CWallRenderer.h"
#include "CBoss_SlaveCow.h"

CTerrain::CTerrain(DEVICE GraphicDev)
    : CRenderObject(GraphicDev), m_BufferCom(nullptr), m_TextureCom284(nullptr),
      m_TextureCom1024(nullptr),
      m_TextureComOutline(nullptr), m_TextureComOutline2(nullptr),
      m_TerrainRendererCom(nullptr),
      m_WallRendererCom(nullptr), m_ObjectSpawnerCom(nullptr), m_IsWallSpawn(false), m_IsObjectSpawn(false)
{
}

CTerrain::CTerrain(const CTerrain& rhs)
    : CRenderObject(rhs), m_BufferCom(nullptr), m_TextureCom284(nullptr),
      m_TextureCom1024(nullptr),
      m_TextureComOutline(nullptr), m_TextureComOutline2(nullptr),
      m_TerrainRendererCom(nullptr),
      m_WallRendererCom(nullptr), m_ObjectSpawnerCom(nullptr), m_IsWallSpawn(false), m_IsObjectSpawn(false)
{
}

CTerrain::~CTerrain()
{
}

HRESULT CTerrain::Ready_GameObject()
{
    if (FAILED(CRenderObject::Ready_GameObject()))
        return E_FAIL;

    m_TransformCom->Set_Pos(_vec3(0.f, 0.f, 0.f));

    return S_OK;
}

void CTerrain::Initialize_GameObject()
{
    CRenderObject::Initialize_GameObject();

}

_int CTerrain::Update_GameObject(const _float& timeDelta)
{
    _int exit = CRenderObject::Update_GameObject(timeDelta);

    // 벽 소환, 배치
    {
        if (m_IsWallSpawn)
        {
            auto layer = CManagement::GetInstance()->Get_Scene()->Get_Layer(LAYERTYPE::ENVIRONMENT);
            if (layer)
            {
                Spawn_Walls(layer);
                m_IsWallSpawn = false;
            }
        }

        CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);

        if (m_WallRendererCom && m_WallRendererCom->Get_EditMode())
        {
            _int cellX, cellZ;

            if (m_TerrainRendererCom && m_TerrainRendererCom->PickCellMousePos(cellX, cellZ))
            {
                auto input = CDInputMgr::GetInstance();

                ImGuiIO& io = ImGui::GetIO();
                if (!io.WantCaptureMouse)
                {
                    if (input->MousePress(DIM_LB))
                    {
                        m_WallRendererCom->Set_WallPosition(cellX, cellZ, true);
                    }

                    else if (input->MousePress(DIM_RB))
                    {
                        m_WallRendererCom->Set_WallPosition(cellX, cellZ, false);
                    }
                }
            }
        }
    }
    
    // 몬스터 소환, 배치
    if (m_IsObjectSpawn)
    {
        auto layer = CManagement::GetInstance()->Get_Scene()->Get_Layer(LAYERTYPE::GAMELOGIC);
        if (layer)
        {
            Spawn_Objects(layer);
            m_IsObjectSpawn = false;
        }
    }
    // 몬스터 에디터 배치
    if (m_ObjectSpawnerCom && m_ObjectSpawnerCom->Get_EditMode())
    {
        _int cellX, cellZ;

        if (m_TerrainRendererCom && m_TerrainRendererCom->PickCellMousePos(cellX, cellZ))
        {
            auto input = CDInputMgr::GetInstance();

            ImGuiIO& io = ImGui::GetIO();
            if (!io.WantCaptureMouse)
            {
                if (input->MousePress(DIM_LB))
                {
                    SPAWN_OBJ_TYPE selectedType = m_ObjectSpawnerCom->Get_SelectedType();
                    m_ObjectSpawnerCom->Add_SpawnPoint(cellX, cellZ, selectedType);
                }
                else if (input->MousePress(DIM_RB))
                {
                    m_ObjectSpawnerCom->Remove_SpawnPoint(cellX, cellZ);
                }
            }
        }
    }


    return exit;
}

void CTerrain::LateUpdate_GameObject(const _float& timeDelta)
{
    CRenderObject::LateUpdate_GameObject(timeDelta);

}

void CTerrain::Render_GameObject()
{
    Render_Setting();

    if (m_TerrainRendererCom)
        m_TerrainRendererCom->Render_TerrainRenderer();

    Render_Reset();
}

HRESULT CTerrain::Add_Component()
{
    if (FAILED(CRenderObject::Add_Component()))
        return E_FAIL;

    m_BufferCom = CreateProtoComponent<CTerrainTex>(this, COMPONENTTYPE::TERRAIN_TEX);
    NULL_CHECK_RETURN(m_BufferCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TERRAIN_TEX, m_BufferCom });

    // Tile TextureComponent
    {
        m_TextureCom284 = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_TILE_284);
        NULL_CHECK_RETURN(m_TextureCom284, E_FAIL);
        m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_TILE_284, m_TextureCom284 });

        m_TextureCom1024 = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_TILE_1024);
        NULL_CHECK_RETURN(m_TextureCom1024, E_FAIL);
        m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_TILE_1024, m_TextureCom1024 });

        m_TextureComOutline = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_TILE_OUTLINE);
        NULL_CHECK_RETURN(m_TextureComOutline, E_FAIL);
        m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_TILE_OUTLINE, m_TextureComOutline });

        m_TextureComOutline2 = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_TILE_OUTLINE2);
        NULL_CHECK_RETURN(m_TextureComOutline2, E_FAIL);
        m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_TILE_OUTLINE2, m_TextureComOutline2 });
    }

    // Terrain Renderer
    m_TerrainRendererCom = CreateProtoComponent<CTerrainRenderer>(this, COMPONENTTYPE::TERRAIN_RENDER);
    NULL_CHECK_RETURN(m_TerrainRendererCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::TERRAIN_RENDER, m_TerrainRendererCom });

    FAILED_CHECK_RETURN(m_TerrainRendererCom->Ready_TerrainRenderer(), E_FAIL);

    // Wall Renderer
    m_WallRendererCom = CreateProtoComponent<CWallRenderer>(this, COMPONENTTYPE::WALL_RENDERER);
    NULL_CHECK_RETURN(m_WallRendererCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::WALL_RENDERER , m_WallRendererCom });

    m_WallRendererCom->Initialize(
        m_TerrainRendererCom->Get_SizeX(),
        m_TerrainRendererCom->Get_SizeZ(),
        m_TerrainRendererCom->Get_CellSize());

    // Monster Spawner -> Wall Renderer랑 비슷하게 해보기
    m_ObjectSpawnerCom = CreateProtoComponent<CObjectSpawner>(this, COMPONENTTYPE::OBJECT_SPAWNER);
    NULL_CHECK_RETURN(m_ObjectSpawnerCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::OBJECT_SPAWNER, m_ObjectSpawnerCom });

    m_ObjectSpawnerCom->Initialize(
        m_TerrainRendererCom->Get_SizeX(),
        m_TerrainRendererCom->Get_SizeZ(),
        m_TerrainRendererCom->Get_CellSize());

    return S_OK;
}

void CTerrain::Set_Material()
{
    D3DMATERIAL9 material;
    ZeroMemory(&material, sizeof(D3DMATERIAL9));

    material.Diffuse = COLOR(1.f, 1.f, 1.f, 1.f);       // 빛을 받았을 때 색
    material.Ambient = COLOR(0.2f, 0.2f, 0.2f, 1.f);    // 배경광 색
    material.Specular = COLOR(1.f, 1.f, 1.f, 1.f);      // 반사광 색
    material.Power = 0.f; // 반사광 세기

    m_GraphicDev->SetMaterial(&material);
}

void CTerrain::Render_Setting()
{
    CRenderObject::Render_Setting();

    m_GraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE); 
    Set_Material();
}

void CTerrain::Render_Editor()
{
    CRenderObject::Render_Editor();

    //ImGui::Text("=== Terrain ===");

}

void CTerrain::LoadMapOnStart(const char* filePath)
{
    if (m_TerrainRendererCom)
    {
        m_TerrainRendererCom->LoadMap(filePath);

        m_IsWallSpawn = true;         // 벽 생성
        m_IsObjectSpawn = true;      // 몬스터 생성
    }
}

void CTerrain::Spawn_Walls(CLayer* layer)
{
    if (!m_WallRendererCom || !m_TerrainRendererCom || !layer)
        return;

    const int sizeX = m_WallRendererCom->Get_GridSizeX();
    const int sizeZ = m_WallRendererCom->Get_GridSizeZ();
    _float cellSize = m_TerrainRendererCom->Get_CellSize();

    int wallCount = 0;

    for (_int z = 0; z < sizeZ - 1; z++)
    {
        for (_int x = 0; x < sizeX - 1; x++)
        {
            if (m_WallRendererCom->Has_Wall(x, z))
            {
                CWall* wall = CWall::Create(m_GraphicDev);

                _vec3 wallPos = m_TerrainRendererCom->GetCellCenterWorld(x, z);

                auto wallTransform = dynamic_cast<CTransform*>(
                    wall->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));

                if (wallTransform)
                    wallTransform->Set_Pos(wallPos);

                auto wallCollider = dynamic_cast<CBoxCollider*>(
                    wall->Get_Component(ID_DYNAMIC, COMPONENTTYPE::BOX_COLL));

                if (wallCollider)
                {
                    wallCollider->Set_Size(_vec3(cellSize, 2.f, cellSize));

                    layer->Add_GameObject(OBJTYPE::WALL, wall);

                    wall->Initialize_GameObject();

                    wallCount++;
                }
            }
        }
    }
}

_vec3 CTerrain::Get_CellCenterWorld(_int cellX, _int cellZ)
{
    if (!m_TerrainRendererCom)
        return _vec3(0.f, 0.f, 0.f);

    return m_TerrainRendererCom->GetCellCenterWorld(cellX, cellZ);
}

_float CTerrain::Get_CellSize()
{
    if (!m_TerrainRendererCom)
        return 1.f;

    return m_TerrainRendererCom->Get_CellSize();
}

void CTerrain::Spawn_Objects(CLayer* layer)
{
    if (!m_ObjectSpawnerCom || !m_TerrainRendererCom || !layer)
        return;

    const auto& spawnPoints = m_ObjectSpawnerCom->Get_SpawnPoints();

    for (const auto& spawn : spawnPoints)
    {
        // 타입에 따라 몬스터 생성
        CCharacterBase* object = nullptr;
        OBJTYPE objType;

        switch (spawn.objectType)
        {
        case SPAWN_OBJ_TYPE::HUMAN_MONSTER:
            object = CHumanMonster::Create(m_GraphicDev);
            objType = OBJTYPE::MONSTER;
            break;

        case SPAWN_OBJ_TYPE::FLOATER_MONSTER:
            object = CFloaterMonster::Create(m_GraphicDev);
            objType = OBJTYPE::MONSTER;
            break;

        case SPAWN_OBJ_TYPE::NPC_SLAVE:
            object = CBoss_SlaveCow::Create(m_GraphicDev);
            objType = OBJTYPE::NPC;
            break;

        case SPAWN_OBJ_TYPE::NPC_TARO:
            //object = CNPC_Taro::Create(m_GraphicDev);
            objType = OBJTYPE::NPC;
            break;

        default:
            continue;
        }
        if (!object)
            continue;

        // 위치 설정
        _vec3 spawnPos = m_TerrainRendererCom->GetCellCenterWorld(spawn.cellX, spawn.cellZ);

        auto transform = object->Get_Transform();
        if (transform)
            transform->Set_Pos(spawnPos);

        // Layer에 추가
        layer->Add_GameObject(objType, object);

        object->Initialize_GameObject();

        // HpBar 생성
        if (objType == OBJTYPE::MONSTER)  // ← 조건 추가
        {
            auto combat = dynamic_cast<CCombatStat*>(object->Get_Component(ID_DYNAMIC, COMPONENTTYPE::COMBATSTAT));
            if (combat)
            {
                CUI_MonsterHp* monsterHp = CUI_MonsterHp::Create(m_GraphicDev, combat);
                SpawnUIOnLayer(monsterHp, OBJTYPE::UI);

                monsterHp->Initialize_GameObject();
            }
        }
    }
}

CTerrain* CTerrain::Create(DEVICE GraphicDev)
{
    CTerrain* terrain = new CTerrain(GraphicDev);

    if (FAILED(terrain->Ready_GameObject()))
    {
        Safe_Release(terrain);
        MSG_BOX("Create Tile Failed");
        return nullptr;
    }

    return terrain;
}

void CTerrain::Free()
{
    Safe_Release(m_ObjectSpawnerCom); 
    Safe_Release(m_WallRendererCom);

    CRenderObject::Free();
}
