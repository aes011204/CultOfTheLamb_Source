#include "pch.h"
#include "CObjectSpawner.h"
#include "CCreateHelper.h"
#include "CTerrain.h"

CObjectSpawner::CObjectSpawner(DEVICE graphicDev)
    : CComponent(graphicDev)
    , m_GridSizeX(0)
    , m_GridSizeZ(0)
    , m_CellSize(1.f)
    , m_EditMode(false)
    , m_SelectedType(SPAWN_OBJ_TYPE::HUMAN_MONSTER)
{
}

CObjectSpawner::CObjectSpawner(const CObjectSpawner& rhs)
    : CComponent(rhs)
    , m_SpawnPoints(rhs.m_SpawnPoints)
    , m_GridSizeX(rhs.m_GridSizeX)
    , m_GridSizeZ(rhs.m_GridSizeZ)
    , m_CellSize(rhs.m_CellSize)
    , m_EditMode(false)
    , m_SelectedType(SPAWN_OBJ_TYPE::HUMAN_MONSTER)
{
}

CObjectSpawner::~CObjectSpawner()
{
}

HRESULT CObjectSpawner::Ready_MonsterSpawner(DEVICE graphicDev)
{
    return S_OK;
}

_int CObjectSpawner::Update_Component(const _float& timeDelta)
{
    _int exit = CComponent::Update_Component(timeDelta);



    return exit;
}

void CObjectSpawner::LateUpdate_Component()
{
    CComponent::LateUpdate_Component();
}

void CObjectSpawner::Render_Editor()
{
    CComponent::Render_Editor();

    ImGui::PushID("MonsterSpawner");
    {
        ImGui::SeparatorText("Monster Spawner");
        
        ImGui::Checkbox("Edit Mode", &m_EditMode);

        if (m_EditMode)
        {
            // 몬스터 타입 선택
            const char* items[] = {
                "Human Monster",
                "Floater Monster",
                "NPC - Taro",
                "NPC - Slave"};

            _int itemCount = static_cast<int>(SPAWN_OBJ_TYPE::SPAWN_TYPE_END);
            _int currentIdx = static_cast<int>(m_SelectedType);

            if (ImGui::Combo("Object Type", &currentIdx, items, itemCount))
            {
                m_SelectedType = static_cast<SPAWN_OBJ_TYPE>(currentIdx);
            }
            ImGui::TextColored(ImVec4(1, 1, 0, 1), "LMB: Place | RMB: Remove");
        }

        ImGui::Separator();

        _int objectCount = Get_ObjectCount();

        ImGui::Text("Total Objectss : %d", objectCount);
        if (ImGui::Button("Remove All"))
        {
            Clear_All();
        }
        
        if (m_Owner)
        {
            CTerrain* terrain = dynamic_cast<CTerrain*>(m_Owner);
            if (terrain)
            {
                for (const auto& spawn : m_SpawnPoints)
                {
                    _vec3 spawnPos = terrain->Get_CellCenterWorld(spawn.cellX, spawn.cellZ);

                    DWORD color;
                    switch (spawn.objectType)
                    {
                    case SPAWN_OBJ_TYPE::HUMAN_MONSTER:
                        color = D3DCOLOR_ARGB(255, 255, 0, 0);    // 빨강
                        break;

                    case SPAWN_OBJ_TYPE::FLOATER_MONSTER:
                        color = D3DCOLOR_ARGB(255, 0, 255, 255);  // 시안
                        break;

                    case SPAWN_OBJ_TYPE::NPC_TARO:
                        color = D3DCOLOR_ARGB(255, 0, 255, 0);    // 초록
                        break;

                    case SPAWN_OBJ_TYPE::NPC_SLAVE:
                        color = D3DCOLOR_ARGB(200, 122, 122, 0);    
                        break;

                    default:
                        color = D3DCOLOR_ARGB(255, 255, 255, 255); // 흰색
                        break;
                    }

                    Engine::DrawDebugSphere(m_GraphicDev, spawnPos, 0.5f, color);
                }
            }
        }
    }
    ImGui::PopID();
}

void CObjectSpawner::Initialize(_int sizeX, _int sizeZ, _float cellSize)
{
    m_GridSizeX = sizeX;
    m_GridSizeZ = sizeZ;
    m_CellSize = cellSize;

    m_SpawnPoints.clear();
}

void CObjectSpawner::Add_SpawnPoint(_int x, _int z, SPAWN_OBJ_TYPE type)
{
    const _int cellCntX = m_GridSizeX - 1;
    const _int cellCntZ = m_GridSizeZ - 1;

    if (x < 0 || x >= cellCntX || z < 0 || z >= cellCntZ)
        return;

    // 이미 있는지 확인
    for (auto& spawn : m_SpawnPoints)
    {
        if (spawn.cellX == x && spawn.cellZ == z)
        {
            // 같은 위치면 타입만 교체
            spawn.objectType = type;
            return;
        }
    }

    // 새로 추가
    SpawnData data;
    data.cellX = x;
    data.cellZ = z;
    data.objectType = type;

    m_SpawnPoints.push_back(data);
}

void CObjectSpawner::Remove_SpawnPoint(_int x, _int z)
{
    for (auto it = m_SpawnPoints.begin(); it != m_SpawnPoints.end(); ++it)
    {
        if (it->cellX == x && it->cellZ == z)
        {
            m_SpawnPoints.erase(it);
            return;
        }
    }
}

void CObjectSpawner::Clear_All()
{
    m_SpawnPoints.clear();
}

_bool CObjectSpawner::Has_ObjPoint(_int x, _int z)
{
    for (const auto& spawn : m_SpawnPoints)
    {
        if (spawn.cellX == x && spawn.cellZ == z)
            return true;
    }

    return false;
}

HRESULT CObjectSpawner::Save_Objects(ofstream& ofs) const
{
    if (!ofs.is_open())
        return E_FAIL;

    // 개수 저장
    uint32_t count = static_cast<uint32_t>(m_SpawnPoints.size());
    ofs.write(reinterpret_cast<const char*>(&count), sizeof(uint32_t));
    // 각 스폰 포인트 저장
    for (const auto& spawn : m_SpawnPoints)
    {
        ofs.write(reinterpret_cast<const char*>(&spawn.cellX), sizeof(_int));
        ofs.write(reinterpret_cast<const char*>(&spawn.cellZ), sizeof(_int));
        ofs.write(reinterpret_cast<const char*>(&spawn.objectType), sizeof(SPAWN_OBJ_TYPE));
    }
    return S_OK;
}

HRESULT CObjectSpawner::Load_Objects(ifstream& ifs)
{
    if (!ifs.is_open())
        return E_FAIL;
    m_SpawnPoints.clear();

    uint32_t count = 0;
    ifs.read(reinterpret_cast<char*>(&count), sizeof(uint32_t));

    for (uint32_t i = 0; i < count; ++i)
    {
        SpawnData spawn;
        ifs.read(reinterpret_cast<char*>(&spawn.cellX), sizeof(_int));
        ifs.read(reinterpret_cast<char*>(&spawn.cellZ), sizeof(_int));
        ifs.read(reinterpret_cast<char*>(&spawn.objectType), sizeof(SPAWN_OBJ_TYPE));

        m_SpawnPoints.push_back(spawn);
    }

    return S_OK;
}

CObjectSpawner* CObjectSpawner::Create(DEVICE graphicDev)
{
    CObjectSpawner* spawner = new CObjectSpawner(graphicDev);

    if (FAILED(spawner->Ready_MonsterSpawner(graphicDev)))
    {
        Safe_Release(spawner);
        MSG_BOX("Create MonsterSpawner Failed");
        return nullptr;
    }

    return spawner;
}

CComponent* CObjectSpawner::Clone()
{
    return new CObjectSpawner(*this);
}

void CObjectSpawner::Free()
{
    CComponent::Free();
}
