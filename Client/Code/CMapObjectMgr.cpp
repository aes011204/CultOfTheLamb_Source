#include "pch.h"
#include "CMapObjectMgr.h"

#include "CLayer.h"
#include "CManagement.h"
#include "CMapEditorPlacer.h"
#include "CMapObject_Helper.h"
#include "CProtoMgr.h"
#include "CScene.h"
#include "CTexture.h"
#include "CTransform.h"

IMPLEMENT_SINGLETON(CMapObjectMgr)

CMapObjectMgr::CMapObjectMgr()
{
  
}

CMapObjectMgr::~CMapObjectMgr()
{
    CMapObjectMgr::Free();
}

void CMapObjectMgr::Ready_MapObjectMgr(DEVICE graphicDev)
{
    m_GraphicDev = graphicDev;
    m_GraphicDev->AddRef();

    m_MapPlacer = CMapEditorPlacer::Create(graphicDev);

    Refresh_MapObjectList();
}

void CMapObjectMgr::Update_MapObjectMgr(const _float& timeDelta)
{
    if (m_MapPlacer)
    {
        m_MapPlacer->Update_MapPlacer(timeDelta);
    }

}

void CMapObjectMgr::Render_MapObjectWindow()
{
    bool bCheck = m_MapPlacer && m_MapPlacer->IsPlacing();

    ImGui::SeparatorText(u8"[ 맵 오브젝트 리스트 ]");

    auto Render_ObjectButton = [&](const char* name, MAPOBJTYPE type, bool& showFlag)
        {
            bool isSelected = (bCheck && m_MapPlacer->Get_CurType() == type);

            if (ImGui::Selectable(name, isSelected))
            {
                showFlag = !showFlag;
            }
        };

    Render_ObjectButton("Grass", MAPOBJTYPE::GRASS, m_ShowGrass);
    Render_ObjectButton("Rubble", MAPOBJTYPE::RUBBLE, m_ShowRubble);
    Render_ObjectButton("Deco", MAPOBJTYPE::DECO, m_ShowDeco);
    Render_ObjectButton("Dungeon Deco", MAPOBJTYPE::DUNGEON_DECO, m_ShowDungeonDeco);
    Render_ObjectButton("ETC", MAPOBJTYPE::ETC, m_ShowETC);

    ImGui::SeparatorText(u8"프리뷰 정보");

    ImGui::Text("Preview YOffset : %.2f", m_MapPlacer->Get_PreviewYOffset());

    //ImGui::Text("Preview Scale : %.2f", m_MapPlacer->Get_PreviewScaleX());
    if (m_MapPlacer->IsPlacing())
    {
        ImGui::Text("[ Preview Scale ]");

        _float scaleX = m_MapPlacer->Get_PreviewScaleX();
        _float scaleY = m_MapPlacer->Get_PreviewScaleY();

        if (ImGui::SliderFloat("Scale X", &scaleX, 0.1f, 30.f))
            m_MapPlacer->Set_PreviewScaleX(scaleX);

        if (ImGui::SliderFloat("Scale Y", &scaleY, 0.1f, 30.f))
            m_MapPlacer->Set_PreviewScaleY(scaleY);
    }

    ImGui::Text("[ Preview Rotation ]");
    ImGui::Text(u8"  X (Pitch): %.2f°", D3DXToDegree(m_MapPlacer->Get_PreviewRotation().x));
    ImGui::SameLine();
    ImGui::Text(u8"  Y (Yaw):   %.2f°", D3DXToDegree(m_MapPlacer->Get_PreviewRotation().y));
    ImGui::Text(u8"  Z (Roll):   %.2f°", D3DXToDegree(m_MapPlacer->Get_PreviewRotation().z));

    ImGui::NewLine();
    ImGui::Text(u8"되돌리기 "); ImGui::SameLine();
    if(ImGui::Button(u8"Return"))
    {
        Undo_LastPlacement();
    }

    ImGui::SameLine();

    static bool eraseMode = false;
    if (ImGui::Checkbox(u8"| 지우개 [우클릭]", &eraseMode))
    {
        if (m_MapPlacer)
        {
            m_MapPlacer->Set_EraseMode(eraseMode);
        }
    }

    ImGui::Separator();

#pragma region 맵 오브젝트 따로 저장
    //ImGui::SeparatorText(u8"[ 저장 파일명 ]");

    //ImGui::SetNextItemWidth(150.f);
    //ImGui::InputText("##MapObjectName", m_MapObjFileName, 128);

    //ImGui::SameLine();

    //if (ImGui::Button(u8"Save"))
    //{
    //    string path = "../Bin/Resource/Data/MapObject/";
    //    path += m_MapObjFileName;
    //    path += ".mapobj";

    //    if (SUCCEEDED(SaveMapObject(path.c_str())))
    //    {
    //        Refresh_MapObjectList(); // 저장하고, 새로고침하기
    //    }
    //}

    //ImGui::NewLine();

    //// 저장 파일 리스트
    //if (ImGui::CollapsingHeader(u8"저장된 맵오브젝트"))
    //{
    //    for (auto& fileName : m_MapObjList)
    //    {
    //        if (ImGui::Selectable(fileName.c_str()))
    //        {
    //            string fullPath = "../Bin/Resource/Data/MapObject/";
    //            fullPath += fileName;

    //            LoadMapObject(fullPath.c_str());
    //        }
    //    }
    //}
#pragma endregion

}

void CMapObjectMgr::Render_GrassWindow()
{
    if (m_ShowGrass == false)
        return;

    if (ImGui::Begin(u8"풀때기 배치", &m_ShowGrass))
    {
        CTexture* grassTexture =
            dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(COMPONENTTYPE::TEX_GRASS));

        if (grassTexture)
        {
            const _int TOTAL_GRASS = 56;
            const _int COLS = 5;
            const ImVec2 buttonSize(50.f, 50.f);

            for (_int i = 0; i < TOTAL_GRASS; i++)
            {
                IDirect3DBaseTexture9* baseTexture = grassTexture->Get_BaseTexture(i);

                if (baseTexture)
                {
                    ImTextureID texID = (ImTextureID)baseTexture;

                    ImGui::PushID(i);

                    _bool isSelected = (i == m_SelectGrassIndex);
                    if (isSelected)
                    {
                        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 1.f));
                    }

                    if (ImGui::ImageButton("Grass", texID, buttonSize))
                    {
                        if (m_MapPlacer)
                        {
                            m_SelectGrassIndex = i;
                            m_MapPlacer->Set_GrassIndex(i);
                            m_MapPlacer->Start_Place(MAPOBJTYPE::GRASS);
                        }

                    }

                    if (isSelected)
                    {
                        ImGui::PopStyleColor();
                    }

                    if (((i + 1) % COLS != 0))
                        ImGui::SameLine();

                    ImGui::PopID();
                }
            }

            Safe_Release(grassTexture);
        }
    }

    ImGui::End();
}

void CMapObjectMgr::Render_RubbleWindow()
{
    if (m_ShowRubble == false)
        return;

    if (ImGui::Begin(u8"잔해 배치", &m_ShowRubble))
    {
        CTexture* rubbleTexture =
            dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(COMPONENTTYPE::TEX_RUBBLE));

        if (rubbleTexture)
        {

            const _int TOTAL_RUBBLE = 94;
            const _int COLS = 4;
            const ImVec2 buttonSize(50.f, 50.f);

            for (_int i = 0; i < TOTAL_RUBBLE; i++)
            {
                IDirect3DBaseTexture9* baseTexture = rubbleTexture->Get_BaseTexture(i);

                if (baseTexture)
                {
                    ImTextureID texID = (ImTextureID)baseTexture;

                    ImGui::PushID(i);

                    _bool isSelected = (i == m_SelectRubbleIndex);
                    if (isSelected)
                    {
                        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 1.f));
                    }

                    if (ImGui::ImageButton("Rubble", texID, buttonSize))
                    {
                        if (m_MapPlacer)
                        {
                            m_SelectRubbleIndex = i;
                            m_MapPlacer->Set_RubbleIndex(i);
                            m_MapPlacer->Start_Place(MAPOBJTYPE::RUBBLE);
                        }
                    }

                    if (isSelected)
                    {
                        ImGui::PopStyleColor();
                    }

                    if (((i + 1) % COLS != 0))
                        ImGui::SameLine();

                    ImGui::PopID();
                }
            }

            Safe_Release(rubbleTexture);
        }
    }

    ImGui::End();
}

void CMapObjectMgr::Render_DecoWindow()
{
    if (m_ShowDeco == false)
        return;

    if (ImGui::Begin(u8"장식 배치", &m_ShowDeco))
    {
        CTexture* decoTexture =
            dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(COMPONENTTYPE::TEX_DECO));

        if (decoTexture)
        {
            const _int TOTAL_DECO = 83; 
            const _int COLS = 5;
            const ImVec2 buttonSize(50.f, 50.f);

            for (_int i = 0; i < TOTAL_DECO; i++)
            {
                IDirect3DBaseTexture9* baseTexture = decoTexture->Get_BaseTexture(i);

                if (baseTexture)
                {
                    ImTextureID texID = (ImTextureID)baseTexture;

                    ImGui::PushID(i);

                    _bool isSelected = (i == m_SelectDecoIndex);
                    if (isSelected)
                    {
                        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 1.f));
                    }

                    if (ImGui::ImageButton("Deco", texID, buttonSize))
                    {
                        if (m_MapPlacer)
                        {
                            m_SelectDecoIndex = i;
                            m_MapPlacer->Set_DecoIndex(i);
                            m_MapPlacer->Start_Place(MAPOBJTYPE::DECO);
                        }
                    }

                    if (isSelected)
                    {
                        ImGui::PopStyleColor();
                    }

                    if (((i + 1) % COLS != 0))
                        ImGui::SameLine();

                    ImGui::PopID();
                }
            }

            Safe_Release(decoTexture);
        }
    }

    ImGui::End();
}

void CMapObjectMgr::Render_DungeonDeco()
{
    if (m_ShowDungeonDeco == false)
        return;

    if (ImGui::Begin(u8"던전 장식 배치", &m_ShowDungeonDeco))
    {
        CTexture* decoTexture =
            dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(COMPONENTTYPE::TEX_DUNGEON_DECO));

        if (decoTexture)
        {
            const _int TOTAL_DECO = 165;
            const _int COLS = 5;
            const ImVec2 buttonSize(50.f, 50.f);

            for (_int i = 0; i < TOTAL_DECO; i++)
            {
                IDirect3DBaseTexture9* baseTexture = decoTexture->Get_BaseTexture(i);

                if (baseTexture)
                {
                    ImTextureID texID = (ImTextureID)baseTexture;

                    ImGui::PushID(i);

                    _bool isSelected = (i == m_SelectDungeonDecoIndex);
                    if (isSelected)
                    {
                        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 1.f));
                    }

                    if (ImGui::ImageButton("Dungeon Deco", texID, buttonSize))
                    {
                        if (m_MapPlacer)
                        {
                            m_SelectDungeonDecoIndex = i;
                            m_MapPlacer->Set_DungeonDecoIndex(i);
                            m_MapPlacer->Start_Place(MAPOBJTYPE::DUNGEON_DECO);
                        }
                    }

                    if (isSelected)
                    {
                        ImGui::PopStyleColor();
                    }

                    if (((i + 1) % COLS != 0))
                        ImGui::SameLine();

                    ImGui::PopID();
                }
            }

            Safe_Release(decoTexture);
        }
    }

    ImGui::End();
}

void CMapObjectMgr::Render_ETC()
{
    if (m_ShowETC == false)
        return;

    if (ImGui::Begin(u8"기타 장식 배치", &m_ShowETC))
    {
        CTexture* etc =
            dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(COMPONENTTYPE::TEX_ETC));

        if (etc)
        {
            const _int TOTAL_ETC = 56;
            const _int COLS = 5;
            const ImVec2 buttonSize(50.f, 50.f);

            for (_int i = 0; i < TOTAL_ETC; i++)
            {
                IDirect3DBaseTexture9* baseTexture = etc->Get_BaseTexture(i);

                if (baseTexture)
                {
                    ImTextureID texID = (ImTextureID)baseTexture;

                    ImGui::PushID(i);

                    _bool isSelected = (i == m_SelectETCIndex);
                    if (isSelected)
                    {
                        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 1.f));
                    }

                    if (ImGui::ImageButton("ETC", texID, buttonSize))
                    {
                        if (m_MapPlacer)
                        {
                            m_SelectETCIndex = i;
                            m_MapPlacer->Set_ETCIndex(i);
                            m_MapPlacer->Start_Place(MAPOBJTYPE::ETC);
                        }
                    }

                    if (isSelected)
                    {
                        ImGui::PopStyleColor();
                    }

                    if (((i + 1) % COLS != 0))
                        ImGui::SameLine();

                    ImGui::PopID();
                }
            }

            Safe_Release(etc);
        }
    }

    ImGui::End();
}

HRESULT CMapObjectMgr::SaveMapObject(const char* filePath)
{
    // 현재 Scene에서 Enviroment 레이어에 있는 오브젝트 들고오기
    CScene* scene = CManagement::GetInstance()->Get_Scene();
    CLayer* layer_Enviroment = scene->Get_Layer(LAYERTYPE::ENVIRONMENT);

    vector<MapObjectData> objects;

    for (auto& layerPair : layer_Enviroment->Get_Objects())
    {
        const auto& gameObjects = layerPair.second;

        for (auto& obj : gameObjects)
        {
            CMapObjectBase* mapObj = dynamic_cast<CMapObjectBase*>(obj);

            if (mapObj)
            {
                MapObjectData data;
                data.type = mapObj->Get_Type();
                data.textureIndex = mapObj->Get_TextureIndex();

                CTransform* objTransform = dynamic_cast<CTransform*>(mapObj->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));

                if (objTransform == nullptr)
                    continue;

                data.pos = objTransform->Get_Pos();
                data.scale = objTransform->Get_Scale();
                data.rotation = objTransform->Get_Euler();

                objects.push_back(data);
            }
        }
    }

    // 저장
    ofstream ofs(filePath, ios_base::binary);
    if (!ofs.is_open())
        return E_FAIL;

    uint32_t magic = 0x4D415042; // 'MAPB'
    uint32_t version = 2;
    uint32_t count = objects.size();

    ofs.write((char*)&magic, sizeof(magic));
    ofs.write((char*)&version, sizeof(version));
    ofs.write((char*)&count, sizeof(count));

    if (count > 0)
    {
        ofs.write((char*)objects.data(),
            sizeof(MapObjectData) * count);
    }

    ofs.close();
    return S_OK;
}

HRESULT CMapObjectMgr::LoadMapObject(const char* filePath)
{
    Clear_PlacedObjects();

    ifstream ifs(filePath, ios_base::binary);
    if (!ifs.is_open())
        return E_FAIL;

    uint32_t magic = 0;
    uint32_t version = 0;
    uint32_t count = 0;

    ifs.read((char*)&magic, sizeof(magic));
    ifs.read((char*)&version, sizeof(version));
    ifs.read((char*)&count, sizeof(count));

    if (magic != 0x4D415042) // 'MAPB'
    {
        ifs.close();
        return E_FAIL;
    }

    vector<MapObjectData> objectsData(count);
    if (count > 0)
    {
        ifs.read((char*)objectsData.data(),
            sizeof(MapObjectData) * count);
    }
    ifs.close();

    // 기존 오브젝트 정리까지 할 필요는 없을듯? Scene 넘어갈 때 진행하면 될거같다.
    // Clear_PlacedObjects(); 

    CScene* scene = CManagement::GetInstance()->Get_Scene();
    if (!scene) return E_FAIL;

    CLayer* layer = scene->Get_Layer(LAYERTYPE::ENVIRONMENT);
    if (!layer) return E_FAIL;

    for (const auto& data : objectsData)
    {
        CMapObjectBase* newObj = CreateMapObjectType(m_GraphicDev, data.type, data.textureIndex);
        if (newObj)
        {
            newObj->Set_WorldPos(data.pos);

            layer->Add_GameObject(OBJTYPE::PLACEMENT, newObj);

            Add_Placement(newObj);
            m_PlacedObjects.push_back(newObj);
        }
    }
  
    return S_OK;
}

HRESULT CMapObjectMgr::Save_ToStream(ofstream& ofs)
{
    CScene* scene = CManagement::GetInstance()->Get_Scene();
    CLayer* layer_Enviroment = scene->Get_Layer(LAYERTYPE::ENVIRONMENT);

    vector<MapObjectData> objects;

    for (auto& layerPair : layer_Enviroment->Get_Objects())
    {
        for (auto& obj : layerPair.second)
        {
            CMapObjectBase* mapObj = dynamic_cast<CMapObjectBase*>(obj);
            if (mapObj)
            {
                MapObjectData data;
                data.type = mapObj->Get_Type();
                data.textureIndex = mapObj->Get_TextureIndex();

                CTransform* objTransform = dynamic_cast<CTransform*>(
                    mapObj->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));

                if (objTransform)
                {
                    data.pos = objTransform->Get_Pos();
                    data.scale = objTransform->Get_Scale();        
                    data.rotation = objTransform->Get_Euler();
                    data.rotation.x = -data.rotation.x;
                    data.rotation.z = -data.rotation.z;

                    objects.push_back(data);
                }
            }
        }
    }

    // Stream에 쓰기
    uint32_t count = objects.size();
    ofs.write((char*)&count, sizeof(count));

    if (count > 0)
    {
        ofs.write((char*)objects.data(), sizeof(MapObjectData) * count);
    }

    return S_OK;
}

HRESULT CMapObjectMgr::Load_FromStream(ifstream& ifs, _vec3 terrainOffset)
{
    uint32_t count = 0;
    ifs.read((char*)&count, sizeof(count));

    vector<MapObjectData> objectsData(count);
    if (count > 0)
    {
        ifs.read((char*)objectsData.data(), sizeof(MapObjectData) * count);
    }

    CScene* scene = CManagement::GetInstance()->Get_Scene();
    if (!scene) return E_FAIL;

    CLayer* layer = scene->Get_Layer(LAYERTYPE::ENVIRONMENT);
    if (!layer) return E_FAIL;

    for (const auto& data : objectsData)
    {
        CMapObjectBase* newObj = CreateMapObjectType(m_GraphicDev, data.type, data.textureIndex);
        if (newObj)
        {
            CTransform* transform = dynamic_cast<CTransform*>(
                newObj->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));

            if (transform)
            {
                _vec3 worldPos = data.pos + terrainOffset;

                transform->Set_Pos(worldPos);

                transform->Set_Scale(data.scale);

                _vec3 loadedRotation = data.rotation;
                loadedRotation.x = -loadedRotation.x;
                //loadedRotation.y = -loadedRotation.y;
                loadedRotation.z = -loadedRotation.z;

                transform->Set_Rotation(loadedRotation);
            }

            layer->Add_GameObject(OBJTYPE::PLACEMENT, newObj);

            newObj->Initialize_GameObject();
        }
    }

    return S_OK;
}

void CMapObjectMgr::Clear_PlacedObjects()
{
    // TODO : 기능 확장해야함

    m_PlacedObjects.clear();
}

void CMapObjectMgr::Refresh_MapObjectList()
{
    m_MapObjList.clear(); // 맵 리스트 비우고, 새로고침 느낌나게

    const char* mapObjDir = "../Bin/Resource/Data/MapObject";

    filesystem::path dirPath(mapObjDir);

    // 폴더 없으면 생성하기
    if (!filesystem::exists(dirPath))
    {
        filesystem::create_directories(dirPath);
        return;
    }

    if (!filesystem::is_directory(dirPath))
        return;

    // 폴더에서 .mapobj 파일들 찾기
    for (const auto& entry : filesystem::directory_iterator(dirPath))
    {
        if (!entry.is_regular_file())
            continue;

        const filesystem::path& p = entry.path();
        if (p.extension() == ".mapobj")
        {
            // 파일명만 저장
            m_MapObjList.push_back(p.filename().string());
        }
    }

    // 정렬
    sort(m_MapObjList.begin(), m_MapObjList.end());

    // 인덱스 조정
    if (m_MapObjList.empty())
        m_SelectMapObjIndex = -1;

    else if (m_SelectMapObjIndex >= (_int)m_MapObjList.size())
    {
        m_SelectMapObjIndex = (_int)m_MapObjList.size() - 1;
    }
}

void CMapObjectMgr::Undo_LastPlacement()
{
    if (m_PlacedObjects.empty())
        return;

    // 마지막 오브젝트 가져오기
    CGameObject* lastObj = m_PlacedObjects.back();
    m_PlacedObjects.pop_back();

    if (lastObj)
    {
        lastObj->Mark_As_PendingKill();
    }

}

void CMapObjectMgr::Add_Placement(CGameObject* object)
{
    if (object)
    {
        m_PlacedObjects.push_back(object);
    }
}

void CMapObjectMgr::Remove_Placement(CGameObject* object)
{
    if (!object)
        return;

    auto iter = find(m_PlacedObjects.begin(), m_PlacedObjects.end(), object);
    if (iter != m_PlacedObjects.end())
    {
        m_PlacedObjects.erase(iter);
    }

}

void CMapObjectMgr::Free()
{
    m_MapObjList.clear();

    Safe_Release(m_MapPlacer);
    Safe_Release(m_GraphicDev);
}
