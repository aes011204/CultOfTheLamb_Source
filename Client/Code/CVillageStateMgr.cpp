#include "pch.h"
#include "CVillageStateMgr.h"

IMPLEMENT_SINGLETON(CVillageStateMgr)

CVillageStateMgr::CVillageStateMgr()
{}

CVillageStateMgr::~CVillageStateMgr()
{
    CVillageStateMgr::Free();
}

void CVillageStateMgr::Save_Building(const SavedBuilding& buildingInfo)
{
    m_VillageState.savedBuildings.push_back(buildingInfo);
}

void CVillageStateMgr::Update_Building(const _vec3& pos, _bool built)
{
    auto& buildings = m_VillageState.savedBuildings;
    for (auto& building : buildings)
    {
        _vec3 diff = building.pos - pos;
        if (D3DXVec3Length(&diff) < 0.1f) // 위치가 같으면 (오차범위 내)
        {
            building.isBuilt = built;
            return;
        }
    }
}

void CVillageStateMgr::Save_Interactive(const SavedInteractive& interactiveInfo)
{
    auto& interactives = m_VillageState.savedInteractives;

    // 이미 있는지 확인하고 있으면 업데이트
    for (auto& item : interactives)
    {
        _vec3 diff = item.pos - interactiveInfo.pos;
        if (D3DXVec3Length(&diff) < 0.1f)
        {
            item = interactiveInfo;
            return;
        }
    }

    // 없으면 추가
    interactives.push_back(interactiveInfo);
}

SavedInteractive* CVillageStateMgr::Get_InteractiveState(const _vec3& pos)
{
    auto& interactives = m_VillageState.savedInteractives;
    for (auto& item : interactives)
    {
        _vec3 diff = item.pos - pos;
        if (D3DXVec3Length(&diff) < 0.1f)
            return &item;
    }

    return nullptr;
}

void CVillageStateMgr::Save_NPC(const wstring& name, const _bool canSpawn, const _bool firstSpawn)
{
    SavedNpc npcInfo;
    npcInfo.name                 = name;
    npcInfo.isActive             = canSpawn;
    npcInfo.firstSpawnAnimPlayed = firstSpawn;

    m_VillageState.savedNPCs[name] = npcInfo;
}

SavedNpc* CVillageStateMgr::Get_NPCState(const wstring& name)
{
    auto& npcs = m_VillageState.savedNPCs;
    auto  iter = npcs.find(name);

    if (iter != npcs.end())
        return &iter->second;

    // 없으면 생성 (Get or Create)
    SavedNpc newNpc;
    newNpc.name                 = name;
    newNpc.isActive             = false; // 기본값
    newNpc.firstSpawnAnimPlayed = false; // 기본값

    npcs.insert({ name, newNpc });

    return &npcs[name];
}

void CVillageStateMgr::Free()
{
    // SavedSceneState 내부 멤버(vector/map)는 자동 소멸됨
    m_VillageState.savedBuildings.clear();
    m_VillageState.savedInteractives.clear();
    m_VillageState.savedNPCs.clear();
}
