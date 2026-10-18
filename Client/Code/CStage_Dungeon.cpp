#include "pch.h"
#include "CStage_Dungeon.h"

#include "CCameraController.h"
#include "CCamSetupBuilder.h"
#include "CCombatStat.h"
#include "CDInputMgr.h"
#include "CDroppedItem.h"
#include "CDropSystem.h"
#include "CEligos.h"
#include "CFloaterMonster.h"
#include "CFollowerCow.h"
#include "CFollowerElephant.h"
#include "CFollowerPig.h"
#include "CInventory.h"
#include "CItemDB.h"
#include "CLayer.h"
#include "CMainCamera.h"
#include "CMainEditorMgr.h"
#include "CPersistentObjectHelper.h"
#include "CPlayer.h"
#include "CSceneMgr.h"
#include "CSoundMgr.h"
#include "CTarotNPC.h"
#include "CTarotStageController.h"
#include "CTerrain.h"
#include "CTransform.h"
#include "CTrigger_Dungeon.h"
#include "CUICircle.h"
#include "CUIIcon.h"
#include "CUIItemGetManager.h"
#include "CUIMp.h"
#include "CUIPlayerPanel.h"
#include "CUITabMenu.h"
#include "CUIWeapon.h"
#include "CUI_DungeonLootPanel.h"
#include "CUI_Fade.h"
#include "CUI_Skill.h"
#include "CUI_SkillPanel.h"
#include "ItemInstance.h"
#include "CVillageStateMgr.h"

class CUI_DungeonLootPanel;

CStage_Dungeon::CStage_Dungeon(DEVICE graphicDev)
    : CScene(graphicDev),
      m_PlayerCombatStatCom(nullptr),
      m_PlayerInventoryCom(nullptr),
      m_WeaponEquipCom(nullptr),
      m_CurrentRoom(Room::DUNGEON_END),
      m_Player(nullptr),
      m_PlayerTransform(nullptr),
      m_UI_Fade(nullptr),
      m_NextRoom(Room::DUNGEON_END),
      m_CamMovement(nullptr),
      m_Timer(0),
      m_TimerRunning(false),
      m_DungeonCleared(false)
{ }

CStage_Dungeon::~CStage_Dungeon()
{ }

HRESULT CStage_Dungeon::Ready_Scene()
{
    if (FAILED(Ready_Environment_Layer(LAYERTYPE::ENVIRONMENT)))
        return E_FAIL;

    if (FAILED(Ready_GameLogic_Layer(LAYERTYPE::GAMELOGIC)))
        return E_FAIL;

    if (FAILED(Ready_UI_Layer(LAYERTYPE::UI)))
        return E_FAIL;

    if (FAILED(Ready_Light()))
        return E_FAIL;

    EnterDungeon();

    return S_OK;
}

void CStage_Dungeon::Initialize_Scene()
{
    CScene::Initialize_Scene();

    m_RoomData[static_cast<_int>(Room::START)] =
    {
        { 10.f, 1.f, 10.f },
        "../Bin/Resource/Data/Terrain/Terrain_DungeonStart.map",
        { 0.f, 0.f, 0.f }
    };

    m_RoomData[static_cast<_int>(Room::MONSTER1)] =
    {
        { 55.f, 1.f, 25.f },
        "../Bin/Resource/Data/Terrain/Terrain_Mon1Test.map",
        { 50.f, 0.f, 0.f }
    };

    m_RoomData[static_cast<_int>(Room::MONSTER2)] =
    {
        { 140.f, 1.f, 10.f },
        "../Bin/Resource/Data/Terrain/Terrain_TarotMapFinal.map",
        { 150.f, 0.f, 0.f }
    };

    m_RoomData[static_cast<_int>(Room::TARO)] =
    {
        { 160.f, 1.f, 10.f },
        "../Bin/Resource/Data/Terrain/Terrain_TaroTest.map",
        { 250.f, 0.f, 0.f }
    };

    m_RoomData[static_cast<_int>(Room::BOSS)] =
    {
        { 210.f, 1.f, 10.f },
        "../Bin/Resource/Data/Terrain/Terrain_BossTest.map",
        { 200.f, 0.f, 0.f }
    };

    m_CurrentRoom = Room::START;

    for (auto& pair : m_Terrains)
    {
        Room      room    = pair.first;
        CTerrain* terrain = pair.second;

        if (terrain)
        {
            auto terrainTransform = terrain->Get_Transform();
            if (terrainTransform)
                terrainTransform->Set_Pos(m_RoomData[static_cast<_int>(room)].terrainOffset);

            terrain->LoadMapOnStart(m_RoomData[static_cast<_int>(room)].terrainFilePath.c_str());
        }
    }

    // 트리거 생성
    Create_RoomTriggers();

    // 무기 생성
    Spawn_Weapon();

    // Init에서 RoomData 세팅 이후 위치 세팅해줘야 함
    _vec3 startPos = m_RoomData[static_cast<_int>(Room::START)].playerSpawnPos;
    m_PlayerTransform->Set_Pos(_vec3(startPos.x, startPos.y, startPos.z));

    // 전투 BGM
    CSoundMgr::GetInstance()->Play_BGM(L"BGM/BGM_Battle2.wav", 0.3f, true, 1.f);

    m_DungeonCleared = false;
}

_int CStage_Dungeon::Update_Scene(const _float& timeDelta)
{
    if (m_TimerRunning)
        m_Timer += timeDelta;

    if (m_CurrentRoom == Room::MONSTER2 && CDInputMgr::GetInstance()->KeyDown(DIK_M))
    {
        m_IsClearMenuOpened = !m_IsClearMenuOpened;

        if (m_IsClearMenuOpened)
        {
            m_TimerRunning = false;   
            m_DungeonCleared = true;    
        }
        else
        {
            m_DungeonCleared = false; 
        }
    }
   
    if (m_RequestChangeScene)
    {
        CSceneMgr::GetInstance()->Change_SceneMgr(m_NextSceneType);
        m_RequestChangeScene = false;

        return 0;
    }

    _int exit = Engine::CScene::Update_Scene(timeDelta);

    // Fade
    Fade_Check(timeDelta);

    cout << m_KillCount << endl;

    return exit;
}

void CStage_Dungeon::LateUpdate_Scene(const _float& timeDelta)
{
    Engine::CScene::LateUpdate_Scene(timeDelta);

    if (CDInputMgr::GetInstance()->KeyDown(DIK_F))
        CSceneMgr::GetInstance()->Change_SceneMgr(SCENETYPE::SC_LOGO_BOSS1);

    if (CDInputMgr::GetInstance()->KeyDown(DIK_G))
        CSceneMgr::GetInstance()->Change_SceneMgr(SCENETYPE::SC_LOGO_BOSS2);

    if (CDInputMgr::GetInstance()->KeyDown(DIK_R))
    {
        Spawn_FollowerCow_Rescue();
        Spawn_FollowerElephant_Rescue();
        Spawn_FollowerPig_Rescue();
    }
}

void CStage_Dungeon::Render_Scene()
{ }

HRESULT CStage_Dungeon::Ready_Light()
{
    return CScene::Ready_Light();
}

void CStage_Dungeon::Spawn_Weapon()
{
    // 일단 무기 2개만 스폰
    CDropSystem::GetInstance()->SetCurrentScene(this);

    _int size = CItemDB::GetInstance()->GetVector().size();
    for (_int i = 0; i < 10; i++)
    {
        int id = CItemDB::GetInstance()->GetVector()[size - i - 1].id;

        ItemInstance inst = { id, -1, 0 };

        _vec3 roomPos = m_RoomData[static_cast<_int>(Room::START)].playerSpawnPos;

        auto drop = CDropSystem::GetInstance()->SpawnDrop(m_GraphicDev,
                                                          inst,
                                                          { roomPos.x + (i * 3.f) - 3.f, 1.f, roomPos.z - 2.5f },
                                                          _vec3(0.5f, 0.5f, 0.5f));

        drop->Initialize_GameObject();
    }

    for (_int i = 0; i < 10; i++)
    {
        int id = CItemDB::GetInstance()->GetVector()[size - i - 1].id;

        ItemInstance inst = { id, -1, 0 };

        _vec3 roomPos = m_RoomData[static_cast<_int>(Room::START)].playerSpawnPos;

        auto drop = CDropSystem::GetInstance()->SpawnDrop(m_GraphicDev,
                                                          inst,
                                                          { roomPos.x + (i * 3.f) - 3.f, 1.f, roomPos.z - 3.5f },
                                                          _vec3(0.5f, 0.5f, 0.5f));

        drop->Initialize_GameObject();
    }
}

void CStage_Dungeon::Spawn_FollowerCow_Rescue()
{
    auto layer = m_Layers[LAYERTYPE::GAMELOGIC];
    NULL_CHECK(layer);

    CFollowerCow* followerCow = CFollowerCow::Create(m_GraphicDev);
    NULL_CHECK(followerCow);

    if (FAILED(layer->Add_GameObject(OBJTYPE::NPC, followerCow)))
        return;

    followerCow->Initialize_GameObject();
    followerCow->Set_RescuedState(true);

    auto cowTransform = dynamic_cast<CTransform*>(
        followerCow->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));

    if (cowTransform)
    {
        _vec3 spawnPos = m_RoomData[static_cast<_int>(m_CurrentRoom)].terrainOffset;
       spawnPos += _vec3(6.f, 1.f, 8.f);

        cowTransform->Set_Pos(spawnPos);
    }

    SavedNpc* state = CVillageStateMgr::GetInstance()->Get_NPCState(L"FollowerCow");
    if (state) state->isActive = true;
}

void CStage_Dungeon::Spawn_FollowerElephant_Rescue()
{
    auto layer = m_Layers[LAYERTYPE::GAMELOGIC];
    NULL_CHECK(layer);

    CFollowerElephant* followerElephant = CFollowerElephant::Create(m_GraphicDev);
    NULL_CHECK(followerElephant);

    if (FAILED(layer->Add_GameObject(OBJTYPE::NPC, followerElephant)))
        return;

    followerElephant->Initialize_GameObject();
    followerElephant->Set_RescuedState(true);

    auto elephantTransform = dynamic_cast<CTransform*>(
        followerElephant->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));

    if (elephantTransform)
    {
        _vec3 spawnPos = m_RoomData[static_cast<_int>(m_CurrentRoom)].terrainOffset;
        spawnPos += _vec3(8.f, 1.f, 8.f);

        elephantTransform->Set_Pos(spawnPos);
    }

    SavedNpc* state = CVillageStateMgr::GetInstance()->Get_NPCState(L"FollowerElephant");
    if (state) state->isActive = true;
}

void CStage_Dungeon::Spawn_FollowerPig_Rescue()
{
    auto layer = m_Layers[LAYERTYPE::GAMELOGIC];
    NULL_CHECK(layer);

    CFollowerPig* followerPig = CFollowerPig::Create(m_GraphicDev);
    NULL_CHECK(followerPig);

    if (FAILED(layer->Add_GameObject(OBJTYPE::NPC, followerPig)))
        return;

    followerPig->Initialize_GameObject();
    followerPig->Set_RescuedState(true);

    auto pigTransform = dynamic_cast<CTransform*>(
        followerPig->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));

    if (pigTransform)
    {
        _vec3 spawnPos = m_RoomData[static_cast<_int>(m_CurrentRoom)].terrainOffset;
        spawnPos += _vec3(10.f, 1.f, 8.f);


        pigTransform->Set_Pos(spawnPos);
    }

    SavedNpc* state = CVillageStateMgr::GetInstance()->Get_NPCState(L"FollowerPig");
    if (state) state->isActive = true;
}

void CStage_Dungeon::Create_RoomTriggers()
{
    // START -> MONSTER1
    Create_Trigger_Between(
        Room::START,
        Room::MONSTER1,
        _vec3(9.3f, 1.f, 20.f), // 트리거 위치 오프셋 (방 위치 기준으로 소환돼서 적당히)
        _vec3(5.f, 5.f, 4.5f)); // 트리거 크기

    // MONSTER1 -> MONSTER2
    Create_Trigger_Between(
        Room::MONSTER1,
        Room::MONSTER2,
        _vec3(38.f, 1.f, 10.f),
        _vec3(5.f, 5.f, 4.5f));

    // MONSTER1 -> Boss
    //Create_Trigger_Between(
    //    Room::MONSTER1,
    //    _vec3(38.f, 1.f, 10.f),
    //    _vec3(4.5f, 5.f, 5.f),
    //    [this]()
    //    {
    //        m_RequestChangeScene = true;
    //        m_NextSceneType = SCENETYPE::SC_LOGO_BOSS1;
    //    }
    //);

    /*Create_Trigger_Between(
        Room::MONSTER2,
        Room::TARO,
        _vec3(18.f, 1.f, 26.f),     
        _vec3(4.5f, 5.f, 5.f)      
    );*/

    // MONSTER2 -> BOSS
    //Create_Trigger_Between(
    //    Room::MONSTER2,
    //    Room::BOSS,
    //    _vec3(18.f, 1.f, 0.f),
    //    _vec3(5.f, 5.f, 4.5f)
    //);
}

void CStage_Dungeon::Create_Trigger_Between(Room fromRoom, Room toRoom, _vec3 offsetFromRoom, _vec3 size)
{
    auto gameLayer = m_Layers[LAYERTYPE::GAMELOGIC];
    NULL_CHECK(gameLayer);

    auto trigger = CTrigger_Dungeon::Create(m_GraphicDev);

    _vec3 roomBasePos = m_RoomData[static_cast<_int>(fromRoom)].terrainOffset;
    _vec3 triggerPos  = roomBasePos + offsetFromRoom;

    trigger->Set_Bounds(triggerPos, size);
    trigger->Set_TriggerOnce(true);

    trigger->Set_Callback([this, toRoom]()
    {
        Change_Room(toRoom);
    });

    gameLayer->Add_GameObject(OBJTYPE::TRIGGER, trigger);
}

void CStage_Dungeon::Create_Trigger_Between(Room                  fromRoom,
                                            _vec3                 offsetFromRoom,
                                            _vec3                 size,
                                            std::function<void()> callback)
{
    auto gameLayer = m_Layers[LAYERTYPE::GAMELOGIC];
    NULL_CHECK(gameLayer);

    auto trigger = CTrigger_Dungeon::Create(m_GraphicDev);

    _vec3 roomBasePos = m_RoomData[static_cast<int>(fromRoom)].terrainOffset;
    _vec3 triggerPos  = roomBasePos + offsetFromRoom;

    trigger->Set_Bounds(triggerPos, size);
    trigger->Set_TriggerOnce(false);
    trigger->Set_Callback(callback);

    gameLayer->Add_GameObject(OBJTYPE::TRIGGER, trigger);
}

void CStage_Dungeon::EnterDungeon()
{
    m_Timer        = 0.f;
    m_TimerRunning = true;
}

void CStage_Dungeon::Change_Room(Room room)
{
    // 이미 페이딩 중이면 무시
    if (m_IsFading)
        return;

    if (m_UI_Fade)
    {
        m_NextRoom = room;
        m_IsFading = true;
        m_UI_Fade->Start_FadeOut(3.5f);
    }
}

void CStage_Dungeon::OnMonsterDeath(CGameObject* obj)
{
    CScene::OnMonsterDeath(obj);

    m_KillCount++;
}

HRESULT CStage_Dungeon::Ready_Environment_Layer(LAYERTYPE layerType)
{
    Engine::CLayer* layer = Engine::CLayer::Create();

    if (nullptr == layer)
        return E_FAIL;

    for (_int i = 0; i < static_cast<_int>(Room::DUNGEON_END); i++)
    {
        Room room = static_cast<Room>(i);

        m_Terrains[room] = CTerrain::Create(m_GraphicDev);
        NULL_CHECK_RETURN(m_Terrains[room], E_FAIL);

        layer->Add_GameObject(OBJTYPE::TERRAIN, m_Terrains[room]);
    }

    m_Layers.insert({ layerType, layer });

    return S_OK;
}

HRESULT CStage_Dungeon::Ready_GameLogic_Layer(LAYERTYPE layerType)
{
    Engine::CLayer* layer = Engine::CLayer::Create();

    NULL_CHECK_RETURN_MSG(
        layer,
        E_FAIL,
        L"CStage_Dungeon::Ready_GameLogic_Layer() failed: CLayer::Create() returned null");

    Engine::CGameObject* gameObject = nullptr;

    // -----------------------------
    // Player
    // -----------------------------

    // 플레이어 생성
    FAILED_CHECK_MSG(
        Engine::AcquirePersistentObject<CPlayer>(
            OBJTYPE::PLAYER,
            L"Player",
            m_GraphicDev,
            layer,
            &m_Player),
        L"Persistent object setup failed");

    NULL_CHECK_RETURN(m_Player, E_FAIL);

    // 플레이어 Transform
    m_PlayerTransform = dynamic_cast<CTransform*>(
        m_Player->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));

    NULL_CHECK_RETURN(m_PlayerTransform, E_FAIL);

    m_PlayerInventoryCom =
        dynamic_cast<CInventory*>(m_Player->Get_Component(
            ID_STATIC,
            COMPONENTTYPE::INVENTORY));

    NULL_CHECK_RETURN(m_PlayerInventoryCom, E_FAIL);

    m_PlayerCombatStatCom =
        dynamic_cast<CCombatStat*>(m_Player->Get_Component(
            ID_DYNAMIC,
            COMPONENTTYPE::COMBATSTAT));

    NULL_CHECK_RETURN(m_PlayerCombatStatCom, E_FAIL);

    // 카메라 생성
    FAILED_CHECK_MSG(
        Engine::AcquirePersistentObject<CMainCamera>(
            OBJTYPE::CAMERA, L"MainCamera", m_GraphicDev, layer, &gameObject),
        L"Persistent object setup failed");

    m_CamController = dynamic_cast<CCameraController*>(
        gameObject->Get_Component(ID_DYNAMIC, COMPONENTTYPE::CAM_CONTROLLER));

    m_CamMovement = dynamic_cast<CCameraMovement*>(
        gameObject->Get_Component(ID_DYNAMIC, COMPONENTTYPE::CAM_MOVEMENT));

    // 기존 줌 세팅 저장
    if (m_CamMovement)
        m_PrevZoomSpeed = m_CamMovement->Get_ZoomSpeed();

    // 카메라 설정값 초기화
    if (m_CamController != nullptr)
    {
        // 씬에서 카메라 생성 시 값 초기화
        // : CCameraSetup을 카메라 설정을 중도에 바꿀 때 사용 가능하고,
        //   그럴 때는 필요한 것만 사용해 .Apply() 하면 된다.
        CCamSetupBuilder(m_CamController)
            .SetTarget(m_PlayerTransform)
            //.PivotOffset({ 0.f, 0.f, 0.f })
            //.Distance(12.f)
            //.RotX(45.f)
            //.RotY(0.f)
            //.LagSpeed(10.f)
            //.ZoomSpeed(5.f)
            .Snap()
            .Apply();

        // 에디터에 카메라 설정
        CMainEditorMgr::GetInstance()->Set_MainCamera(m_CamController);
    }

    CGameObject* tarotController = CTarotStageController::Create(m_GraphicDev);

    NULL_CHECK_RETURN_MSG(
        tarotController,
        E_FAIL,
        L"CStage_TarotTest::Ready_GameLogic_Layer() failed: CTarotStageController::Create() returned null");

    FAILED_CHECK_MSG(
        layer->Add_GameObject(OBJTYPE::LOGIC, tarotController),
        L"CStage_TarotTest::Ready_GameLogic_Layer() failed: CLayer::Add_GameObject(CTarotStageController) failed");

    gameObject = CTarotNPC::Create(m_GraphicDev);
    FAILED_CHECK_MSG(layer->Add_GameObject(OBJTYPE::NPC, gameObject),
                     L"CStage::Ready_GameLogic_Layer() failed: CLayer::Add_GameObject(TarotNPC) failed");
    dynamic_cast<CTarotNPC*>(gameObject)->Set_TarotController(dynamic_cast<CTarotStageController*>(tarotController));

    gameObject = CFloaterMonster::Create(m_GraphicDev);
    FAILED_CHECK_MSG(layer->Add_GameObject(OBJTYPE::MONSTER, gameObject),
                     L"CStage::Ready_GameLogic_Layer() failed: CLayer::Add_GameObject(CFloaterMonster) failed");
    dynamic_cast<CFloaterMonster*>(gameObject)->Get_Transform()->Set_Pos(15.f, 1.f, 10.f);

    ///////////////////////////////////////////////////////////////////////////////
    //보스
    ///////////////////////////////////////////////////////////////////////////////

    // 보스 생성
    //CGameObject* m_Boss = CEligos::Create(m_GraphicDev);
    //NULL_CHECK_RETURN_MSG(m_Boss, E_FAIL, L"CStage::Ready_GameLogic_Layer() failed: CBossPhase2::Create() returned null")
    //
    //FAILED_CHECK_MSG(
    //    layer->Add_GameObject(OBJTYPE::BOSS1, m_Boss),
    //    L"CStage::Ready_GameLogic_Layer() failed: CLayer::Add_GameObject(Boss) failed");

    m_Layers.insert({ layerType, layer });

    return S_OK;
}

HRESULT CStage_Dungeon::Ready_UI_Layer(LAYERTYPE layerType)
{
    auto layer = Engine::CLayer::Create();
    NULL_CHECK_RETURN(layer, E_FAIL);

    CGameObject* gameObject = nullptr;

    FAILED_CHECK_MSG(
        Engine::AcquirePersistentObject<CUIPlayerPanel>(
            OBJTYPE::UI,
            L"UI_PlayerPanel",
            m_GraphicDev,
            layer,
            &gameObject),
        L"Persistent object setup failed: UI_PlayerPanel");

    CUIPlayerPanel* playerPanel = dynamic_cast<CUIPlayerPanel*>(gameObject);

    playerPanel->Set_PlayerCombatStat(m_PlayerCombatStatCom);
    playerPanel->Set_Resister(true);

    FAILED_CHECK_MSG(
        Engine::AcquirePersistentObject<CUIMp>(
            OBJTYPE::UI,
            L"UI_Mp",
            m_GraphicDev,
            layer,
            &gameObject),
        L"Persistent object setup failed: UI_Mp");

    CUIMp* uiMp = dynamic_cast<CUIMp*>(gameObject);
    uiMp->Set_PlayerCombatStat(m_PlayerCombatStatCom);

    FAILED_CHECK_MSG(
        Engine::AcquirePersistentObject<CUICircle>(
            OBJTYPE::UI,
            L"UI_Circle",
            m_GraphicDev,
            layer,
            &gameObject),
        L"Persistent object setup failed: UI_Circle");

    dynamic_cast<CUICircle*>(gameObject)->Set_PlayerCombatStat(m_PlayerCombatStatCom);

    FAILED_CHECK_MSG(
        Engine::AcquirePersistentObject<CUIWeapon>(
            OBJTYPE::UI,
            L"UI_Weapon",
            m_GraphicDev,
            layer,
            &gameObject),
        L"Persistent object setup failed: UI_Weapon");

    //왼쪽 상단 스킬 아이콘 UI
    CUIIcon* pgameObject = CUIIcon::Create(m_GraphicDev, COMPONENTTYPE::TEX_UI_ICON);
    pgameObject->Set_TextureIndex(1);
    pgameObject->Get_Transform()->Set_Pos(-524.f, 260.f, 0.f);
    pgameObject->Get_Transform()->Set_Scale(12.5f, 15.f, 0.f);

    FAILED_CHECK_MSG(
        layer->Add_GameObject(OBJTYPE::UI, pgameObject),
        L"CStage_Dungeon::Ready_UI_Layer() failed: CLayer::Add_GameObject(CUIIcon) failed");

    //CUI_TabMenu
    CUITabMenu* uiTabMenu = CUITabMenu::Create(m_GraphicDev);
    uiTabMenu->SetInventory(m_PlayerInventoryCom);
    uiTabMenu->Set_PlayerCombatStat(m_PlayerCombatStatCom);
    uiTabMenu->Initialize();

    FAILED_CHECK_MSG(layer->Add_GameObject(OBJTYPE::UI, uiTabMenu),
                     L"CStage_Dungeon::Ready_UI_Layer() failed: CLayer::Add_GameObject(UITabMenu) failed");

    FAILED_CHECK_MSG(Engine::AcquirePersistentObject<CUIItemGetManager>
                     (OBJTYPE::UI, L"UI_ItemGetManager",
                         m_GraphicDev, layer,
                         &gameObject),
                     L"Persistent object setup failed: UI_ItemGetManager")
    dynamic_cast<CUIItemGetManager*>(gameObject)->SetInventory(m_PlayerInventoryCom);
    dynamic_cast<CUIItemGetManager*>(gameObject)->Initialize();

    // Fade In - Fade Out
    m_UI_Fade = CUI_Fade::Create(m_GraphicDev);
    NULL_CHECK_RETURN(m_UI_Fade, E_FAIL);

    FAILED_CHECK_MSG(
        layer->Add_GameObject(OBJTYPE::UI, m_UI_Fade),
        L"CStage_Dungeon::Ready_UI_Layer() failed: CFade_UI Add failed");

    FAILED_CHECK_MSG(
        Engine::AcquirePersistentObject<CUI_SkillPanel>(
            OBJTYPE::UI,
            L"UI_SkillPanel",
            m_GraphicDev,
            layer,
            &gameObject),
        L"Persistent object setup failed: UI_SkillPanel");

    CUI_DungeonLootPanel* dungeonLootPanel = CUI_DungeonLootPanel::Create(m_GraphicDev);
    dungeonLootPanel->Set_RunLootLog(&m_RunLootLog);
    dungeonLootPanel->Initialize_GameObject();
    FAILED_CHECK_MSG(layer->Add_GameObject(OBJTYPE::UI, dungeonLootPanel),
        L"CStage_Dungeon::Ready_UI_Layer() failed: CLayer::Add_GameObject(CUI_DungeonLootPanel) failed");

    m_Layers.insert({ layerType, layer });

    return S_OK;
}

HRESULT CStage_Dungeon::Ready_Prototype()
{
    return S_OK;
}

CStage_Dungeon* CStage_Dungeon::Create(DEVICE graphicDev)
{
    auto stage = new CStage_Dungeon(graphicDev);

    if (FAILED(stage->Ready_Scene()))
    {
        MSG_BOX("Stage Create Failed");
        Safe_Release(stage);
        return nullptr;
    }

    return stage;
}

void CStage_Dungeon::Free()
{
    CDropSystem::GetInstance()->Clear_Item();
    CDroppedItem::Reset_WeaponExplainUI();

    m_CamMovement = nullptr;
    m_UI_Fade     = nullptr;

    Engine::CScene::Free();
}

void CStage_Dungeon::Fade_Check(const _float timeDelta)
{
    if (!m_IsFading || !m_UI_Fade)
        return;

    if (!m_UI_Fade->IsFading() && m_UI_Fade->Get_Alpha() >= 1.0f)
    {
        // 페이드 아웃 완료 후, 잠시 대기

        m_CurrentRoom = m_NextRoom;

        // TODO : 플레이어 소환 Offset 조절하기
        if (m_PlayerTransform)
            m_PlayerTransform->Set_Pos(m_RoomData[static_cast<_int>(m_NextRoom)].playerSpawnPos);

        m_UI_Fade->Start_FadeIn(0.5f);
        m_IsFading = false;
    }
}
