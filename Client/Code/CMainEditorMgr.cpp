#include "pch.h"
#include "CMainEditorMgr.h"
#include "CFogMgr.h"
#include "CAnimator.h"
#include "CBoxCollider.h"
#include "CCameraCalc.h"
#include "CTerrain.h"
#include "CCameraController.h"
#include "CCollisionManager.h"
#include "CCombatStat.h"
#include "CComInspectors.h"
#include "CComponent.h"
#include "CDInputMgr.h"
#include "CEditContext.h"
#include "CEnumHelper.h"
#include "CGameObject.h"
#include "CInventory.h"
#include "CLayer.h"
#include "CManagement.h"
#include "CMapEditorPlacer.h"
#include "CMapObjectMgr.h"
#include "CPickingHelper.h"
#include "CPlayerController.h"
#include "CRigidbody.h"
#include "CScene.h"
#include "CSceneMgr.h"
#include "CState.h"
#include "CTerrainRenderer.h"
#include "CTimerMgr.h"
#include "CTransform.h"
#include "CWallRenderer.h"
#include "CWeaponEquip.h"

IMPLEMENT_SINGLETON(CMainEditorMgr)

CMainEditorMgr::~CMainEditorMgr()
{
    CMainEditorMgr::Free();
}

void CMainEditorMgr::Ready_MainEditor(DEVICE graphicDev, CEditContext* ctx)
{
    m_GraphicDev = graphicDev;
    m_GraphicDev->AddRef();

    Set_EditContext(ctx);
    CMapObjectMgr::GetInstance()->Ready_MapObjectMgr(graphicDev);
    CFogMgr::GetInstance()->Ready_FogMgr(graphicDev);

    ImGuiStyle& style = ImGui::GetStyle();
    style.Alpha = 0.95f;

    style.WindowBorderSize  = 1.0f;
    style.WindowRounding    = 4.0f;
    style.FrameRounding     = 2.0f;
}

void CMainEditorMgr::Update_MainEditor(const _float& timeDelta)
{
    auto inputMgr = CDInputMgr::GetInstance();
    if (inputMgr->KeyDown(DIK_F1))
    {
        Toggle_ImGui();
    }

    if (m_MainCameraController) 
    {
        if (!m_IsPlaying) // Pause 상태에서도 카메라는 움직일 수 있도록 Delta값 세팅
        {
            _float timeDelta = CTimerMgr::GetInstance()->Get_TimeDelta();

            CGameObject* camera = m_MainCameraController->Get_Owner();
            if (camera)
                camera->Update_GameObject(timeDelta);
        }
    }

    CMapObjectMgr::GetInstance()->Update_MapObjectMgr(timeDelta);
    CFogMgr::GetInstance()->Update_FogMgr(timeDelta);
}

void CMainEditorMgr::Set_EditContext(CEditContext* ctx)
{
    Safe_Release(m_Context);

    m_Context = ctx;
}

void CMainEditorMgr::Render_MainEditor()
{
    if (!m_Context)
        return;

    if (!m_ShowImGui)
        return;

    CScene* scene = CManagement::GetInstance()->Get_Scene();
    if (scene)
        m_Context->Set_ActiveScene(scene);

    else
        MSG_BOX("Scene Is Where ? ");

    // 프리뷰 렌더
    {
        if (CMapObjectMgr::GetInstance()->Get_MapPlacer())
            CMapObjectMgr::GetInstance()->Get_MapPlacer()->Render_Privew();

        if (CFogMgr::GetInstance()->IsPlacing())
            CFogMgr::GetInstance()->Render_Preview();
    }

    ImGui::Begin("MainEditor [ Toggle F1 ] [ Toggle F8 -> Free Mode]");

    if (ImGui::BeginTabBar("MainEditorTabs"))
    {
        if (ImGui::BeginTabItem("Scene"))
        {
            Render_SceneTab();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("MapTool"))
        {
            Render_MapToolTab();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Scene Change"))
        {
            Render_SceneChangeTab();
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::End();

    CMapObjectMgr::GetInstance()->Render_GrassWindow();
    CMapObjectMgr::GetInstance()->Render_RubbleWindow();
    CMapObjectMgr::GetInstance()->Render_DecoWindow();
    CMapObjectMgr::GetInstance()->Render_DungeonDeco();
    CMapObjectMgr::GetInstance()->Render_ETC();

}

void CMainEditorMgr::Render_SceneTab()
{
    ImGui::BeginChild("[ Tool bar ]", ImVec2(0, 70), true);
    Render_ToolBar();
    ImGui::EndChild();

    ImGui::Separator();

    // 왼쪽 Hierarchy
    ImGui::BeginChild("[ Hierarchy ]", ImVec2(170, 0), true);
    Render_Hierarchy();
    ImGui::EndChild();

    ImGui::SameLine();

    // 오른쪽 Inspector
    ImGui::BeginChild("[ Inspector ]", ImVec2(0, 0), true);
    Render_Inspector();
    ImGui::EndChild();
}

void CMainEditorMgr::Render_MapToolTab()
{
    // 공용 툴바
    ImGui::BeginChild("[ MapTool_Toolbar ]", ImVec2(0, 40), true);
    Render_ToolBar();
    ImGui::EndChild();

    ImGui::Separator();

    // =======================
    // 왼쪽 : 메뉴 (선택만 담당)
    // =======================
    ImGui::BeginChild("[ MapTool_Menu ]", ImVec2(140, 0), true);

    ImGui::Text("Map Tools");
    ImGui::Separator(); 

    _bool selTerrain   = (m_MapToolView == MAPTOOL_VIEW::TERRAIN);
    _bool selMapObject = (m_MapToolView == MAPTOOL_VIEW::MAP_OBJECT);
    _bool selWall      = (m_MapToolView == MAPTOOL_VIEW::WALL);
    _bool selMonster   = (m_MapToolView == MAPTOOL_VIEW::MONSTER_SPAWNER);
    _bool selFog = (m_MapToolView == MAPTOOL_VIEW::FOG_RENDERER);


    if (ImGui::Selectable("Terrain Renderer", selTerrain))
        m_MapToolView = MAPTOOL_VIEW::TERRAIN;

    if (ImGui::Selectable("Map Object", selMapObject))
        m_MapToolView = MAPTOOL_VIEW::MAP_OBJECT;

    if (ImGui::Selectable("Wall", selWall))
        m_MapToolView = MAPTOOL_VIEW::WALL;

    if (ImGui::Selectable("Monster Spawner", selMonster))
        m_MapToolView = MAPTOOL_VIEW::MONSTER_SPAWNER;

    if (ImGui::Selectable("Fog", selFog))
        m_MapToolView = MAPTOOL_VIEW::FOG_RENDERER;

    ImGui::EndChild();

    // ==========
    // 오른쪽 세팅
    // ==========
    ImGui::SameLine();

    ImGui::BeginChild("[ MapTool_Content ]", ImVec2(0, 0), true);

    switch (m_MapToolView)
    {
    case MAPTOOL_VIEW::TERRAIN:
    {
        CTerrainRenderer* terrain = Find_TerrainRenderer();

        if (terrain)
        {
            ImGui::Text("Terrain Renderer");
            ImGui::Separator();
            Terrain_Inspector(terrain);
        }
        else
            ImGui::Text(u8"TerrainRenderer 를 찾을 수 없음");
        break;
    }

    case MAPTOOL_VIEW::MAP_OBJECT:
    {
        CMapObjectMgr::GetInstance()->Render_MapObjectWindow();
        break;
    }

    case MAPTOOL_VIEW::WALL:
    {
        CWallRenderer* wallRenderer = Find_WallRenderer();

        if (wallRenderer)
        {
            WallRenderer_Inspector(wallRenderer);
        }

        break;
    }

    case MAPTOOL_VIEW::MONSTER_SPAWNER:
    {
        CTerrain* terrain = Find_Terrain();

        if (terrain)
        {
            CObjectSpawner* spawner = dynamic_cast<CObjectSpawner*>(
                terrain->Get_Component(ID_DYNAMIC, COMPONENTTYPE::OBJECT_SPAWNER));

            if (spawner)
            {
                ImGui::Text("Monster Spawner");
                ImGui::Separator();

                MonsterSpawner_Inspector(spawner);
            }
            else
                ImGui::Text(u8"MonsterSpawner이 없어요");
        }

        break;
    }

    case MAPTOOL_VIEW::FOG_RENDERER:
    {
        CFogMgr::GetInstance()->Render_FogWindow();
        break;
    }

    }

    ImGui::EndChild();
}

void CMainEditorMgr::Render_SceneChangeTab()
{
    ImGui::PushID("Scene_ChangeTab");
    {
        const char* sceneChange[] = { "Main", "Stage", "Edit", "Stage_Boss1", "Stage_Boss2", "Stage_TaroTest", "Stage_Dungeon"};

        SCENETYPE sceneType = CSceneMgr::GetInstance()->Get_SceneType();

        static _int currentSceneIndex = 0;

        ImGui::Text(u8"현재 스테이지 : %s", ToString(sceneType));
        ImGui::Separator();

        if (ImGui::Combo(u8"Scene Change", &currentSceneIndex, sceneChange, IM_ARRAYSIZE(sceneChange)))
        {
            SCENETYPE newSceneType = SCENETYPE::SC_MAINMENU;  

            switch (currentSceneIndex)
            {
            case 0: newSceneType = SCENETYPE::SC_MAINMENU;     break;
            case 1: newSceneType = SCENETYPE::SC_STAGE;        break;
            case 2: newSceneType = SCENETYPE::SC_EDIT;         break;
            case 3: newSceneType = SCENETYPE::SC_BOSS_STAGE1;  break;
            case 4: newSceneType = SCENETYPE::SC_BOSS_STAGE2;  break;
            case 5: newSceneType = SCENETYPE::SC_TAROT_TEST;   break;
            case 6: newSceneType = SCENETYPE::SC_DUNGEON;      break;

            default: break;
            }

            CSceneMgr::GetInstance()->Change_SceneMgr(newSceneType);
        }
  
    }
    ImGui::PopID();
}

void CMainEditorMgr::Render_ToolBar()
{
    float delta = CTimerMgr::GetInstance()->Get_TimeDelta(L"Timer_FPS60");
    float fps   = (delta > 0.f) ? 1.f / delta : 0.f;

    //ImGui::Columns(3, "ToolbarCols", false);

    // ============
    // Play / FPS
    // ============

    if (CDInputMgr::GetInstance()->KeyDown(DIK_F7))
    {
        m_IsPlaying = !m_IsPlaying;
    }

    {
        ImGui::AlignTextToFramePadding();

        if (ImGui::Button(m_IsPlaying ? "Pause" : "Play", ImVec2(60.f, 0.f)))
            m_IsPlaying = !m_IsPlaying;

        ImGui::SameLine();

        ImGui::Text("FPS : %.1f", fps);
    }

    //ImGui::NextColumn();
    ImGui::SameLine();

    // =======
    // Camera
    // =======
    {
        ImGui::AlignTextToFramePadding();

        Menu_CameraInspector(m_MainCameraController);
    }

    ImGui::SameLine();

    // =======
    // TimeScale
    // =======
    {
        ImGui::AlignTextToFramePadding();
        ImGui::Text("| Speed:");
        ImGui::SameLine();

        if (ImGui::Button("-", ImVec2(20.f, 0.f)))
            m_TimeScale = max(0.5f, m_TimeScale - 0.5f);  
        ImGui::SameLine();

        ImGui::Text("%.1fx", m_TimeScale);

        ImGui::SameLine();

        if (ImGui::Button("+", ImVec2(20.f, 0.f)))
            m_TimeScale = min(3.0f, m_TimeScale + 0.5f);  
        
    }

    // ============
    // Debug Check
    // ============
    {
        static _bool debugCheck = true;
        ImGui::AlignTextToFramePadding();
        ImGui::Checkbox("Debugging Mode", &debugCheck);

        CCollisionManager::GetInstance()->Render_Debug(debugCheck);
    }

    ImGui::SameLine();

    // ============
    //  Mouse Pos
    // ============

    POINT screenPos, clientPos;
    GetCursorPos(&screenPos);
    clientPos = screenPos;
    ScreenToClient(g_hWnd, &clientPos);

    _vec3 worldMousePos;
    if (Engine::PickPlaneY(m_GraphicDev, g_hWnd, 0.f, worldMousePos))
    {
        ImGui::Text("| Mouse Pos : (%.2f, %.2f, %.2f)", worldMousePos.x, worldMousePos.y, worldMousePos.z);
    }

    //ImGui::Columns(1);
}

void CMainEditorMgr::Render_Hierarchy()
{
    CScene* scene = m_Context->Get_ActiveScene();
    if (scene == nullptr)
    {
        ImGui::Text("No Active Scene");
        return;
    }

    if (ImGui::TreeNode("Scene"))
    {
        const auto& layers = scene->Get_Layers();

        for (const auto& layerPair : layers)
        {
            LAYERTYPE layerType = layerPair.first;
            CLayer*   layer     = layerPair.second;

            if (layer == nullptr) continue;

            string layerName = ToString(layerType);

            if (ImGui::TreeNode(layerName.c_str()))
            {
                const auto& objMap = layer->Get_Objects();

                for (const auto& objPair : objMap)
                {
                    OBJTYPE                     objType = objPair.first;
                    const vector<CGameObject*>& objList = objPair.second;

                    for (CGameObject* obj : objList)
                    {
                        if (!obj) continue;

                        bool isSelected = (obj == m_Context->Get_SelectedObject());

                        const wstring& wName = obj->Get_Name();
                        string         name(wName.begin(), wName.end()); // wstring -> string

                        if (ImGui::Selectable(name.c_str(), isSelected))
                            m_Context->Select_Object(obj);
                    }
                }

                ImGui::TreePop(); // Layer
            }
        }

        ImGui::TreePop(); // Scene
    }
}

void CMainEditorMgr::Render_Inspector()
{
    CGameObject* obj = m_Context->Get_SelectedObject();

    if (obj == nullptr || obj->Is_PendingDestroy())
    {
        if (obj && obj->Is_PendingDestroy())
        {
            m_Context->Select_Object(nullptr);  // 선택 해제
            m_Context->Select_Component(nullptr);
        }

        ImGui::Text("Not Object Selected");
        return;
    }

    // 기본값 Transform으로 시작
    if (m_Context->Get_SelectedComponent() == nullptr)
    {
        CComponent* transformCom = obj->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM);

        if (transformCom)
            m_Context->Select_Component(transformCom);
    }

    // Object 이름 세팅
    {
        if (obj && obj->Is_PendingDestroy() == false)
        {
            wstring wName = obj->Get_Name();
            string  name(wName.begin(), wName.end());
            ImGui::Text("Object : %s", name.c_str());
        }
    }

    ImGui::Separator();
    ImGui::Text("[ Components ]");

    Render_DynamicComponents(obj);
    Render_StaticComponents(obj);

    ImGui::Separator();
    obj->Render_Editor();

    Render_SelectComponent();
}

void CMainEditorMgr::Render_DynamicComponents(CGameObject* obj)
{
    if (!obj || obj->Is_PendingDestroy())
        return;

    // Dynamic Component
    const auto& dynamicComponents = obj->Get_DynamicComponents();

    for (const auto& typePair : dynamicComponents)
    {
        COMPONENTTYPE type = typePair.first;
        CComponent*   com  = typePair.second;

        if (!com) continue;

        wstring wTypeName = com->Get_Name();
        string  typeName(wTypeName.begin(), wTypeName.end());

        bool isSelected = (com == m_Context->Get_SelectedComponent());

        if (ImGui::Selectable(typeName.c_str(), isSelected))
            m_Context->Select_Component(com);
    }
}

void CMainEditorMgr::Render_StaticComponents(CGameObject* obj)
{
    if (!obj || obj->Is_PendingDestroy())
        return;

    // Static Component
    auto inven = obj->Get_Component(ID_STATIC, COMPONENTTYPE::INVENTORY);
    if (inven)
    {
        wstring wTypeName = inven->Get_Name();
        string  typeName(wTypeName.begin(), wTypeName.end());

        bool isSelected = (inven == m_Context->Get_SelectedComponent());
        if (ImGui::Selectable(typeName.c_str(), isSelected))
            m_Context->Select_Component(inven);
    }

    auto weapon = obj->Get_Component(ID_STATIC, COMPONENTTYPE::WEAPON_EQUIP);
    if (weapon)
    {
        wstring wTypeName = weapon->Get_Name();
        string  typeName(wTypeName.begin(), wTypeName.end());

        bool isSelected = (weapon == m_Context->Get_SelectedComponent());
        if (ImGui::Selectable(typeName.c_str(), isSelected))
            m_Context->Select_Component(weapon);
    }
}

void CMainEditorMgr::Render_SelectComponent()
{
    // 선택된 컴포넌트의 인스펙터
    CComponent* selectComponent = m_Context->Get_SelectedComponent();

    if (selectComponent)
    {
        CGameObject* owner = selectComponent->Get_Owner();

        if (!owner || owner->Is_PendingDestroy())
        {
            m_Context->Select_Component(nullptr);
            return;
        }
    }

    if (selectComponent)
    {
        ImGui::Separator();

        string componentName = WStringToUtf8(selectComponent->Get_Name());

        switch (selectComponent->Get_Type())
        {
        // Dynamic
        case COMPONENTTYPE::TRANSFORM:
        {
            auto transform = dynamic_cast<CTransform*>(selectComponent);
            Transform_Inspector(transform);
            break;
        }
        case COMPONENTTYPE::STATE:
        {
            auto state = dynamic_cast<CState*>(selectComponent);
            State_Inspector(state);
            break;
        }
        case COMPONENTTYPE::COMBATSTAT:
        {
            auto stat = dynamic_cast<CCombatStat*>(selectComponent);
            CombatStat_Inspector(stat);
            break;
        }

        case COMPONENTTYPE::TERRAIN_RENDER:
        {
            auto terrain = dynamic_cast<CTerrainRenderer*>(selectComponent);
            Terrain_Inspector(terrain);
            break;
        }

        case COMPONENTTYPE::BOX_COLL:
        {
            auto collider = dynamic_cast<CBoxCollider*>(selectComponent);
            BoxColl_Inspector(collider);
            break;
        }

        // Static
        case COMPONENTTYPE::INVENTORY:
        {
            auto inven = dynamic_cast<CInventory*>(selectComponent);
            Inventory_Inspector(inven);
            break;
        }

        case COMPONENTTYPE::WEAPON_EQUIP:
        {
            auto weapon = dynamic_cast<CWeaponEquip*>(selectComponent);
            Weapon_Inspector(weapon);
            break;
        }

        case COMPONENTTYPE::PL_CONTROLLER:
        {
            auto controller = dynamic_cast<CPlayerController*>(selectComponent);
            PlayerController_Inspector(controller);
        }
        break;

        case COMPONENTTYPE::RIGIDBODY:
        {
            auto rigidbody = dynamic_cast<CRigidbody*>(selectComponent);
            RigidBody_Inspector(rigidbody);
        }
        break;

        case COMPONENTTYPE::ANIMATOR:
        {
            auto animator = dynamic_cast<CAnimator*>(selectComponent);
            Animator_Inspector(animator);
        }
        break;

        case COMPONENTTYPE::CAM_CONTROLLER:
        {
            auto controller = dynamic_cast<CCameraController*>(selectComponent);
            CamController_Inspector(controller);
        }
        break;

        case COMPONENTTYPE::OBJECT_SPAWNER:
        {
            auto spawner = dynamic_cast<CObjectSpawner*>(selectComponent);
            MonsterSpawner_Inspector(spawner);
        }
        break;

        default: break;
        // 다른 컴포넌트 타입 ImGui에 보여줄거 있으면 추가하기
        }
    }
}

CTerrainRenderer* CMainEditorMgr::Find_TerrainRenderer()
{
    if (!m_Context)
        return nullptr;

    CScene* scene = m_Context->Get_ActiveScene();
    if (!scene)
        return nullptr;

    const auto& layers = scene->Get_Layers();

    for (const auto& layerPair : layers)
    {
        CLayer* layer = layerPair.second;
        if (!layer) continue;

        const auto& objMap = layer->Get_Objects();

        for (const auto& objPair : objMap)
        {
            const vector<CGameObject*>& objList = objPair.second;

            for (CGameObject* obj : objList)
            {
                if (!obj) continue;

                CComponent* com =
                    obj->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TERRAIN_RENDER);
                if (!com)
                    continue;

                auto terrain = dynamic_cast<CTerrainRenderer*>(com);
                if (terrain)
                    return terrain;
            }
        }
    }

    return nullptr;
}

CWallRenderer* CMainEditorMgr::Find_WallRenderer()
{
    auto texRenderer = Find_TerrainRenderer();

    NULL_CHECK_RETURN(texRenderer, nullptr);

    CGameObject* owner = texRenderer->Get_Owner();
    NULL_CHECK_RETURN(owner, nullptr);

    auto* wallCom = dynamic_cast<CWallRenderer*>(
        owner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::WALL_RENDERER));

    NULL_CHECK_RETURN(wallCom, nullptr);

    return wallCom;
}

CTerrain* CMainEditorMgr::Find_Terrain()
{
    auto terrainRenderer = Find_TerrainRenderer();
    if (!terrainRenderer)
        return nullptr;

    return dynamic_cast<CTerrain*>(terrainRenderer->Get_Owner());
}

void CMainEditorMgr::Free()
{
    CMapObjectMgr::DestroyInstance();
    CFogMgr::DestroyInstance();

    Safe_Release(m_Context);
    Safe_Release(m_GraphicDev);
}
