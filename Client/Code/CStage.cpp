#include "pch.h"
#include "CStage.h"

#include "CSoundMgr.h"
#include "CCameraController.h"
#include "CCamSetupBuilder.h"
#include "CChest.h"
#include "CCombatStat.h"
#include "CConstructCircle.h"
#include "CConstructionMgr.h"
#include "CCookingController.h"
#include "CCreateHelper.h"
#include "CDInputMgr.h"
#include "CDroppedItem.h"
#include "CDropSystem.h"
#include "CFloaterMonster.h"
#include "CFollowerCow.h"
#include "CFollowerElephant.h"
#include "CFollowerPig.h"
#include "CHarvestableHerb.h"
#include "CHumanMonster.h"
#include "CInventory.h"
#include "CItemDB.h"
#include "CKitchen.h"
#include "CLayer.h"
#include "CMainCamera.h"
#include "CMainEditorMgr.h"
#include "CParticleHelper.h"
#include "CPersistentObjectHelper.h"
#include "CPlayer.h"
#include "CPlayerSkill.h"
#include "CRock.h"
#include "CSceneMgr.h"
#include "CTemple.h"
#include "CTerrain.h"
#include "CTransform.h"
#include "CTree.h"
#include "CTrigger_Dungeon.h"
#include "CUIBuildMenu.h"
#include "CUICircle.h"
#include "CUICookingGame.h"
#include "CUICookingMenu.h"
#include "CUIIcon.h"
#include "CUIItemGetManager.h"
#include "CUIMp.h"
#include "CUIPlayerPanel.h"
#include "CUITabMenu.h"
#include "CUIWeapon.h"
#include "CUI_Dialogue.h"
#include "CUI_FollowerCommand.h"
#include "CUI_Skill.h"
#include "CVillageStateMgr.h"
#include "CWorkBench.h"
#include "ItemInstance.h"
#include "CUI_SkillPanel.h"
#include "CPlayerConstruction.h"
#include "CPlayerGathering.h"
#include "CUI_ProgressBar.h"
#include "CWeaponEquip.h"
#include "CFontMgr.h"
#include "CUI_Text.h"
#include "CRenderer.h"

class CUIBuildMenu;

CStage::CStage(DEVICE graphicDev)
    : Engine::CScene(graphicDev),
      m_PlayerCombatStatCom(nullptr),
      m_PlayerInventoryCom(nullptr),
      m_WeaponEquipCom(nullptr),
      m_WorkBench(nullptr),
      m_PlayerSkillCom(nullptr),
      m_Terrain(nullptr),
      m_CookingMenu(nullptr),
      m_FollowerCommand(nullptr),
      m_PlayerGatherCom(nullptr),
      m_PlayerConstructionCom(nullptr)
{ }

CStage::~CStage()
{ }

HRESULT CStage::Ready_Scene()
{
    if (FAILED(Ready_Environment_Layer(LAYERTYPE::ENVIRONMENT)))
        return E_FAIL;

    if (FAILED(Ready_GameLogic_Layer(LAYERTYPE::GAMELOGIC)))
        return E_FAIL;

    if (FAILED(Ready_UI_Layer(LAYERTYPE::UI)))
        return E_FAIL;

    if (FAILED(Ready_Light()))
        return E_FAIL;

    // 이건 드롭 시스템 쓸려면 각 씬 마다 셋팅 해야함
    CDropSystem::GetInstance()->SetCurrentScene(this);

    // 안은수 test
    // 이젠 테스트 끝내야죠 

    struct SpawnInfo
    {
        int   id;
        _vec3 pos;
    };

    vector<SpawnInfo> spawnList = {
        { 1001, { 11.f, 1.f, 10.f } },      // 동전
        { 1001, { 12.f, 1.f, 10.f } },
        { 1001, { 13.f, 1.f, 10.f } },
        { 1001, { 14.f, 1.f, 10.f } },
        { 1001, { 15.f, 1.f, 10.f } },
        { 3001, { 20.f, 1.f, 10.f } },      // 검
        { 2001, { 25.f, 1.f, 10.f } },      // 풀죽
        { 2001, { 27.f, 1.f, 10.f } },
        { 2003, { 30.f, 1.f, 10.f } },      // 베리 그릇
        { 2003, { 32.f, 1.f, 10.f } }
    };

    for (const auto& info : spawnList)
    {
        ItemInstance inst = { info.id, -1, 0 };
        CDropSystem::GetInstance()->SpawnDrop(m_GraphicDev, inst, info.pos);
    }

    //for (int i = 0; i < CItemDB::GetInstance()->GetVector().size(); i++)
    //{
    //    int          id   = CItemDB::GetInstance()->GetVector()[i].id;
    //    ItemInstance inst = { id, -1, 0 };
    //    CDropSystem::GetInstance()->SpawnDrop(m_GraphicDev, inst, { 5.f, 1.f, 10.f + (i * 3.f) });
    //}
    //
    //int size = CItemDB::GetInstance()->GetVector().size();
    //for (int i = 0; i < size; i++)
    //{
    //    int reverseIndex = size - 1 - i;
    //    int id           = CItemDB::GetInstance()->GetVector()[reverseIndex].id;
    //
    //    ItemInstance inst = { id, -1, 0 };
    //    CDropSystem::GetInstance()->SpawnDrop(m_GraphicDev, inst, { 10.f, 1.f, 10.f + (i * 3.f) });
    //}
    //
    //size = CItemDB::GetInstance()->GetVector().size();
    //for (int i = 0; i < size; i++)
    //{
    //    int reverseIndex = size - 1 - i;
    //    int id           = CItemDB::GetInstance()->GetVector()[reverseIndex].id;
    //
    //    ItemInstance inst = { id, -1, 0 };
    //    CDropSystem::GetInstance()->SpawnDrop(m_GraphicDev, inst, { 15.f, 1.f, 10.f + (i * 3.f) });
    //}

    // Construction Manager 의 Placer초기화
    CConstructionMgr::GetInstance()->Create_Placer(m_GraphicDev);

    return S_OK;
}

void CStage::Initialize_Scene()
{
    CScene::Initialize_Scene();

    // 초기 맵 불러오기 <- 이거 Initialize에서 진행해야 맵 오브젝트가 잘 불러진다.
    m_Terrain->LoadMapOnStart("../Bin//Resource/Data/Terrain/Terrain_Village2.map");
    

    CSoundMgr::GetInstance()->Play_BGM(L"BGM/BGM_Village.wav", 0.3f, true, 1.f);

   // m_UIOpenTime = 4.f;

    m_VillageEnterUI = CUI_Text::Create(m_GraphicDev);
    m_VillageEnterUI->Set_Font(L"NanumSquare_Max");
    m_VillageEnterUI->Set_Text(L"어린 양의 교단");
    m_VillageEnterUI->Set_Color({ 1.f, 1.f, 1.f, 0.5f });

    CManagement::GetInstance()->Get_Scene()->Get_Layer(LAYERTYPE::UI)->Add_GameObject(OBJTYPE::UI, m_VillageEnterUI);
    
    // 던전 이동 트리거 만들기
    Create_Trigger();

    // -----------------------------
    // 저장된 건물 불러오기 기능
    // -----------------------------
    auto layer = m_Layers[LAYERTYPE::GAMELOGIC];
    if (layer && CVillageStateMgr::GetInstance()->Get_State())
    {
        SavedSceneState* savedSceneState = CVillageStateMgr::GetInstance()->Get_State();
        for (const auto& building : savedSceneState->savedBuildings)
        {
            if (building.isBuilt)
            {
                CGameObject* obj      = nullptr;
                _vec3        spawnPos = building.pos;

                switch (building.type)
                {
                case BUILDING_TYPE::TEMPLE:
                {
                    obj = CTemple::Create(m_GraphicDev);
                    spawnPos.y += 2.f;
                }
                break;
                case BUILDING_TYPE::KITCHEN:
                {
                    obj = CKitchen::Create(m_GraphicDev);
                    spawnPos.y += 0.6f;

                    // Kitchen은 CookingMenu 연결 필요
                    if (auto kitchen = dynamic_cast<CKitchen*>(obj))
                        kitchen->Set_CookingMenu(m_CookingMenu);
                }
                break;
                default: break;
                }

                if (obj != nullptr)
                {
                    obj->Initialize_GameObject();
                    auto transform = dynamic_cast<CTransform*>(obj->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));
                    if (transform) transform->Set_Pos(spawnPos);

                    layer->Add_GameObject(OBJTYPE::BUILDING, obj);
                }
            }
            else
            {
                _vec3 scale = { 1.f, 1.f, 1.f };
                if (building.type == BUILDING_TYPE::TEMPLE) scale = _vec3(4.f, 2.8f, 1.f);
                else if (building.type == BUILDING_TYPE::KITCHEN) scale = _vec3(3.5f, 2.5f, 1.f);

                CConstructCircle* circle = CConstructCircle::Create(m_GraphicDev, scale);
                if (circle != nullptr)
                {
                    circle->Setup_Construction(building.type);
                    circle->Initialize_GameObject();

                    auto transform = dynamic_cast<CTransform*>(circle->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));
                    if (transform) transform->Set_Pos(building.pos);

                    layer->Add_GameObject(OBJTYPE::CONSTRUCTION_SITE, circle);
                }
            }
        }
    }

    // Follower Creation
    m_FollowerSpawnTimer = 7.f;
    m_FollowerSpawnStep  = 0;
}

_int CStage::Update_Scene(const _float& timeDelta)
{
    _int exit = Engine::CScene::Update_Scene(timeDelta);

    if (m_UIOpenTime>=0.f)
    {
         m_UIOpenTime -= timeDelta;

        _vec2 pos = { 50.f,50.f };

        m_VillageEnterUI->Set_IsActive(true);

        if (m_UIOpenTime <= 0.f)
        {
            m_VillageEnterUI->Set_IsActive(false);
            m_UIOpenTime = 0.f;
        }
    }


    auto* playerSkill = dynamic_cast<CPlayerSkill*>(
        CManagement::GetInstance()->Get_Component(ID_STATIC, LAYERTYPE::GAMELOGIC, OBJTYPE::PLAYER, COMPONENTTYPE::PL_SKILL));

    if (playerSkill)
    {
        playerSkill->Set_SkillUnlocked(false);
        playerSkill->Set_CurrentCharges(0);

    }
   // m_WeaponEquipCom = dynamic_cast<CWeaponEquip*>(
   //     CManagement::GetInstance()->Get_Component(ID_STATIC, LAYERTYPE::GAMELOGIC, OBJTYPE::PLAYER, COMPONENTTYPE::WEAPON_EQUIP));
   // if (m_WeaponEquipCom && m_WeaponEquipCom->Get_WeaponEquipped())
   //     m_WeaponEquipCom->Unequip_Weapon();

    m_FollowerSpawnTimer += timeDelta;

    // 던전에서 구출한 이후, 순차적으로 소환
    if (m_FollowerSpawnTimer >= 7.f)
    {
        switch (m_FollowerSpawnStep)
        {
        case 0:
            Spawn_FollowerCow();
            m_FollowerSpawnTimer = 0.f;
            m_FollowerSpawnStep++;
            break;
        case 1:
            Spawn_FollowerElephant();
            m_FollowerSpawnTimer = 0.f;
            m_FollowerSpawnStep++;
            break;
        case 2:
            Spawn_FollowerPig();
            m_FollowerSpawnTimer = 0.f;
            m_FollowerSpawnStep++;
            break;
        default: break;
        }
    }

    CConstructionMgr::GetInstance()->Update_ConstructionMgr(timeDelta);

    if (CDInputMgr::GetInstance()->KeyDown(DIK_F))
        CSceneMgr::GetInstance()->Change_SceneMgr(SCENETYPE::SC_LOGO_BOSS1);

    if (CDInputMgr::GetInstance()->KeyDown(DIK_G))
        CSceneMgr::GetInstance()->Change_SceneMgr(SCENETYPE::SC_LOGO_BOSS2);

    // 수동 소환 테스트 : FollowerCow
    if (CDInputMgr::GetInstance()->KeyDown(DIK_R))
    {
        // Force activate and spawn
        SavedNpc* cowState = CVillageStateMgr::GetInstance()->Get_NPCState(L"FollowerCow");
        if (cowState) cowState->isActive = true;
        Spawn_FollowerCow();
    }

    // 수동 소환 테스트 : FollowerElephant
    if (CDInputMgr::GetInstance()->KeyDown(DIK_T))
    {
        // Force activate and spawn
        SavedNpc* elephantState = CVillageStateMgr::GetInstance()->Get_NPCState(L"FollowerElephant");
        if (elephantState) elephantState->isActive = true;
        Spawn_FollowerElephant();
    }

    // 수동 소환 테스트 : FollowerPig
    if (CDInputMgr::GetInstance()->KeyDown(DIK_Y))
    {
        // Force activate and spawn
        SavedNpc* pigState = CVillageStateMgr::GetInstance()->Get_NPCState(L"FollowerPig");
        if (pigState) pigState->isActive = true;
        Spawn_FollowerPig();
    }

    // 건축 모드 테스트 : Temple
    if (CDInputMgr::GetInstance()->KeyDown(DIK_B))
    {
        if (CConstructionMgr::GetInstance()->IsBuilding())
            CConstructionMgr::GetInstance()->End_BuildMode();
        else
            CConstructionMgr::GetInstance()->Start_BuildMode(BUILDING_TYPE::TEMPLE);
    }

    // 건축 모드 테스트 : Kitchen
    if (CDInputMgr::GetInstance()->KeyDown(DIK_N))
    {
        if (CConstructionMgr::GetInstance()->IsBuilding())
            CConstructionMgr::GetInstance()->End_BuildMode();
        else
            CConstructionMgr::GetInstance()->Start_BuildMode(BUILDING_TYPE::KITCHEN);
    }

    // xptmxm dksdmstn
    if (CDInputMgr::GetInstance()->KeyDown(DIK_U))
    {
        /*CHarvestableHerb*  tmpObject = CHarvestableHerb::Create(m_GraphicDev, CHarvestableHerb::BERRY::SMALLBERRY);
        NULL_CHECK_RETURN_MSG(tmpObject, E_FAIL, L"CStage::Ready_GameLogic_Layer() failed: CWorldInteractive::Create() returned null")
 
            SpawnObjectOnLayer(tmpObject, OBJTYPE::INTERACTIVE);
 
         tmpObject->Get_Transform()->Set_Pos(20.f, 0.6f, 20.f);
 
 
         RecipeInst r{ 4001 ,3,2001 };
 
         CUICookingGame* cook = CUICookingGame::Create(m_GraphicDev, tmpObject, r);
 
         SpawnUIOnLayer(cook, OBJTYPE::UI);
 
         cook->Initialize_GameObject();*/

        CChest* tmp = CChest::Create(m_GraphicDev, { 23, 2, 23 });
        tmp->AddItem(1002, 4);
        tmp->Initialize_GameObject();
        SpawnObjectOnLayer(tmp, OBJTYPE::INTERACTIVE);
    }

    return exit;
}

void CStage::LateUpdate_Scene(const _float& timeDelta)
{
    Engine::CScene::LateUpdate_Scene(timeDelta);
}

void CStage::Render_Scene()
{
    CConstructionMgr::GetInstance()->Render_ConstructionMgr();
}

HRESULT CStage::Ready_Environment_Layer(LAYERTYPE layerType)
{
    Engine::CLayer* layer = Engine::CLayer::Create();

    if (nullptr == layer)
        return E_FAIL;

    m_Terrain = CTerrain::Create(m_GraphicDev);
    NULL_CHECK_RETURN(m_Terrain, E_FAIL);

    layer->Add_GameObject(OBJTYPE::TERRAIN, m_Terrain);

    m_Layers.insert({ layerType, layer });

    return S_OK;
}

HRESULT CStage::Ready_GameLogic_Layer(LAYERTYPE layerType)
{
    Engine::CLayer* layer = Engine::CLayer::Create();

    NULL_CHECK_RETURN_MSG(
        layer,
        E_FAIL,
        L"CStage::Ready_GameLogic_Layer() failed: CLayer::Create() returned null");

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
            &gameObject),
        L"Persistent object setup failed");

    NULL_CHECK_RETURN(gameObject, E_FAIL);

    // 플레이어의 Transform 가져오기
    const auto playerTransform =
        dynamic_cast<CTransform*>(gameObject->Get_Component(
            ID_DYNAMIC,
            COMPONENTTYPE::TRANSFORM));

    NULL_CHECK_RETURN(playerTransform, E_FAIL);

    m_PlayerInventoryCom =
        dynamic_cast<CInventory*>(gameObject->Get_Component(
            ID_STATIC,
            COMPONENTTYPE::INVENTORY));

    NULL_CHECK_RETURN(m_PlayerInventoryCom, E_FAIL);

    m_PlayerCombatStatCom =
        dynamic_cast<CCombatStat*>(gameObject->Get_Component(
            ID_DYNAMIC,
            COMPONENTTYPE::COMBATSTAT));

    NULL_CHECK_RETURN(m_PlayerCombatStatCom, E_FAIL);

    m_PlayerSkillCom = dynamic_cast<CPlayerSkill*>(gameObject->Get_Component(ID_STATIC, COMPONENTTYPE::PL_SKILL));
    NULL_CHECK_RETURN(m_PlayerSkillCom, E_FAIL);

    m_PlayerConstructionCom =
        dynamic_cast<CPlayerConstruction*>(gameObject->Get_Component(
            ID_STATIC,
            COMPONENTTYPE::CONSTRUCTION));

    m_PlayerGatherCom = dynamic_cast<CPlayerGathering*>(gameObject->Get_Component(
        ID_STATIC,
        COMPONENTTYPE::GATHERING));

    m_WeaponEquipCom = dynamic_cast<CWeaponEquip*>(gameObject->Get_Component(
        ID_STATIC,
        COMPONENTTYPE::WEAPON_EQUIP));

    if (m_WeaponEquipCom && m_WeaponEquipCom->Get_WeaponEquipped())
        m_WeaponEquipCom->Unequip_Weapon(false);

    // -----------------------------
    // Camera
    // -----------------------------

    // 카메라 생성
    FAILED_CHECK_MSG(
        Engine::AcquirePersistentObject<CMainCamera>(
            OBJTYPE::CAMERA, L"MainCamera", m_GraphicDev, layer, &gameObject),
        L"Persistent object setup failed"
    );

    auto camController = dynamic_cast<CCameraController*>(
        gameObject->Get_Component(ID_DYNAMIC, COMPONENTTYPE::CAM_CONTROLLER));

    // 카메라 설정값 초기화
    if (camController != nullptr)
    {
        // 씬에서 카메라 생성 시 값 초기화
        // : CCameraSetup을 카메라 설정을 중도에 바꿀 때 사용 가능하고,
        //   그럴 때는 필요한 것만 사용해 .Apply() 하면 된다.
        CCamSetupBuilder(camController)
            .SetTarget(playerTransform)
            .PivotOffset({ 0.f, 0.f, 0.f })
            .Distance(12.f)
            .RotX(45.f)
            .RotY(0.f)
            .LagSpeed(10.f)
            .ZoomSpeed(5.f)
            .Snap()
            .Apply();

        // 에디터에 카메라 설정
        CMainEditorMgr::GetInstance()->Set_MainCamera(camController);
    }

    // -----------------------------
    // 상호작용 오브젝트
    // -----------------------------

    CHarvestableHerb* tmpHerb;
    tmpHerb = CHarvestableHerb::Create(m_GraphicDev, CHarvestableHerb::BERRY::BIGBERRY);
    NULL_CHECK_RETURN_MSG(tmpHerb, E_FAIL, L"CStage::Ready_GameLogic_Layer() failed: CWorldInteractive::Create() returned null")
    FAILED_CHECK_MSG(
        layer->Add_GameObject(OBJTYPE::INTERACTIVE, tmpHerb),
        L"CStage::Ready_GameLogic_Layer() failed: CLayer::Add_GameObject(CWorldInteractive) failed");
    tmpHerb->Get_Transform()->Set_Pos(10.f, 0.5f, 30.f);
    tmpHerb->Check_WorldState();

    ///////////////////////////////////////////////////////////////////

    //tmpHerb = CHarvestableHerb::Create(m_GraphicDev, CHarvestableHerb::BERRY::SMALLBERRY);
    //NULL_CHECK_RETURN_MSG(tmpHerb, E_FAIL, L"CStage::Ready_GameLogic_Layer() failed: CWorldInteractive::Create() returned null")
    //FAILED_CHECK_MSG(
    //    layer->Add_GameObject(OBJTYPE::INTERACTIVE, tmpHerb),
    //    L"CStage::Ready_GameLogic_Layer() failed: CLayer::Add_GameObject(CWorldInteractive) failed");
    //tmpHerb->Get_Transform()->Set_Pos(20.f, 0.6f, 20.f);
    //tmpHerb->Check_WorldState();

    ///////////////////////////////////////////////////////////////////

    CRock* tmpRock;
    tmpRock = CRock::Create(m_GraphicDev, CRock::ROCK::SMALLROCK);
    NULL_CHECK_RETURN_MSG(tmpRock, E_FAIL, L"CStage::Ready_GameLogic_Layer() failed: CWorldInteractive::Create() returned null")
    FAILED_CHECK_MSG(
        layer->Add_GameObject(OBJTYPE::INTERACTIVE, tmpRock),
        L"CStage::Ready_GameLogic_Layer() failed: CLayer::Add_GameObject(CWorldInteractive) failed");
    tmpRock->Get_Transform()->Set_Pos(25.f, 1.f, 25.f);
    tmpRock->Check_WorldState();

    ///////////////////////////////////////////////////////////////////

    //tmpRock = CRock::Create(m_GraphicDev, CRock::ROCK::BIGROCK);
    //NULL_CHECK_RETURN_MSG(tmpRock, E_FAIL, L"CStage::Ready_GameLogic_Layer() failed: CWorldInteractive::Create() returned null")
    //FAILED_CHECK_MSG(
    //    layer->Add_GameObject(OBJTYPE::INTERACTIVE, tmpRock),
    //    L"CStage::Ready_GameLogic_Layer() failed: CLayer::Add_GameObject(CWorldInteractive) failed");
    //tmpRock->Get_Transform()->Set_Pos(25.f, 1.f, 20.f);
    //tmpRock->Check_WorldState();

    ///////////////////////////////////////////////////////////////////

    //tmpRock = CRock::Create(m_GraphicDev, CRock::ROCK::MIDDLEROCK);
    //NULL_CHECK_RETURN_MSG(tmpRock, E_FAIL, L"CStage::Ready_GameLogic_Layer() failed: CWorldInteractive::Create() returned null")
    //FAILED_CHECK_MSG(
    //    layer->Add_GameObject(OBJTYPE::INTERACTIVE, tmpRock),
    //    L"CStage::Ready_GameLogic_Layer() failed: CLayer::Add_GameObject(CWorldInteractive) failed");
    //tmpRock->Get_Transform()->Set_Pos(25.f, 1.f, 30.f);
    //tmpRock->Check_WorldState();

    ///////////////////////////////////////////////////////////////////

    CTree* tmpTree;

    tmpTree = CTree::Create(m_GraphicDev, CTree::CTree::RUBBLE1);
    NULL_CHECK_RETURN_MSG(tmpTree, E_FAIL, L"CStage::Ready_GameLogic_Layer() failed: CWorldInteractive::Create() returned null")
    FAILED_CHECK_MSG(
        layer->Add_GameObject(OBJTYPE::INTERACTIVE, tmpTree),
        L"CStage::Ready_GameLogic_Layer() failed: CLayer::Add_GameObject(CWorldInteractive) failed");
    tmpTree->Get_Transform()->Set_Pos(20.f, 1.f, 30.f);
    tmpTree->Check_WorldState();

    ///////////////////////////////////////////////////////////////////

    //tmpTree = CTree::Create(m_GraphicDev, CTree::CTree::RUBBLE2);
    //NULL_CHECK_RETURN_MSG(tmpTree, E_FAIL, L"CStage::Ready_GameLogic_Layer() failed: CWorldInteractive::Create() returned null")
    //FAILED_CHECK_MSG(
    //    layer->Add_GameObject(OBJTYPE::INTERACTIVE, tmpTree),
    //    L"CStage::Ready_GameLogic_Layer() failed: CLayer::Add_GameObject(CWorldInteractive) failed");
    //tmpTree->Get_Transform()->Set_Pos(10.f, 1.f, 25.f);
    //tmpTree->Check_WorldState();

    ///////////////////////////////////////////////////////////////////

    //tmpTree = CTree::Create(m_GraphicDev, CTree::CTree::RUBBLE3);
    //NULL_CHECK_RETURN_MSG(tmpTree, E_FAIL, L"CStage::Ready_GameLogic_Layer() failed: CWorldInteractive::Create() returned null")
    //FAILED_CHECK_MSG(
    //    layer->Add_GameObject(OBJTYPE::INTERACTIVE, tmpTree),
    //    L"CStage::Ready_GameLogic_Layer() failed: CLayer::Add_GameObject(CWorldInteractive) failed");
    //tmpTree->Get_Transform()->Set_Pos(10.f, 1.f, 20.f);
    //tmpTree->Check_WorldState();

    ///////////////////////////////////////////////////////////////////

    //tmpTree = CTree::Create(m_GraphicDev, CTree::CTree::RUBBLE4);
    //NULL_CHECK_RETURN_MSG(tmpTree, E_FAIL, L"CStage::Ready_GameLogic_Layer() failed: CWorldInteractive::Create() returned null")
    //FAILED_CHECK_MSG(
    //    layer->Add_GameObject(OBJTYPE::INTERACTIVE, tmpTree),
    //    L"CStage::Ready_GameLogic_Layer() failed: CLayer::Add_GameObject(CWorldInteractive) failed");
    //tmpTree->Get_Transform()->Set_Pos(10.f, 1.f, 15.f);
    //tmpTree->Check_WorldState();

    ///////////////////////////////////////////////////////////////////

    //tmpTree = CTree::Create(m_GraphicDev, CTree::CTree::REALSMALLTREE);
    //NULL_CHECK_RETURN_MSG(tmpTree, E_FAIL, L"CStage::Ready_GameLogic_Layer() failed: CWorldInteractive::Create() returned null")
    //
    //FAILED_CHECK_MSG(
    //    layer->Add_GameObject(OBJTYPE::INTERACTIVE, tmpTree),
    //    L"CStage::Ready_GameLogic_Layer() failed: CLayer::Add_GameObject(CWorldInteractive) failed");
    //tmpTree->Get_Transform()->Set_Pos(15.f, 2.f, 30.f);
    //tmpTree->Check_WorldState();

    ///////////////////////////////////////////////////////////////////

    tmpTree = CTree::Create(m_GraphicDev, CTree::CTree::REALBIGTREE);
    NULL_CHECK_RETURN_MSG(tmpTree, E_FAIL, L"CStage::Ready_GameLogic_Layer() failed: CWorldInteractive::Create() returned null")

    FAILED_CHECK_MSG(
        layer->Add_GameObject(OBJTYPE::BUILDING, tmpTree),
        L"CStage::Ready_GameLogic_Layer() failed: CLayer::Add_GameObject(CWorldInteractive) failed");
    tmpTree->Get_Transform()->Set_Pos(10.f, 2.5f, 30.f);
    tmpTree->Check_WorldState();

    ///////////////////////////////////////////////////////////////////

    // -----------------------------
    // 건물 오브젝트
    // -----------------------------

    auto workBench = CWorkBench::Create(m_GraphicDev);
    m_WorkBench    = workBench;

    FAILED_CHECK_MSG(
        layer->Add_GameObject(OBJTYPE::BUILDING, workBench),
        L"Add_GameObject(CWorkBench) failed");

    m_Layers.insert({ layerType, layer });

    return S_OK;
}

HRESULT CStage::Ready_UI_Layer(LAYERTYPE layerType)
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
        L"CStage::Ready_UI_Layer() failed: CLayer::Add_GameObject(CUIIcon) failed");

    //CUITabMenu
    CUITabMenu* uiTabMenu = CUITabMenu::Create(m_GraphicDev);
    uiTabMenu->SetInventory(m_PlayerInventoryCom);
    uiTabMenu->Set_PlayerCombatStat(m_PlayerCombatStatCom);
    uiTabMenu->Initialize();

    FAILED_CHECK_MSG(layer->Add_GameObject(OBJTYPE::UI, uiTabMenu),
                     L"CStage::Ready_UI_Layer() failed: CLayer::Add_GameObject(UITabMenu) failed");

    FAILED_CHECK_MSG(Engine::AcquirePersistentObject<CUIItemGetManager>
                     (OBJTYPE::UI, L"UI_ItemGetManager",
                         m_GraphicDev, layer,
                         &gameObject),
                     L"Persistent object setup failed: UI_ItemGetManager")
    dynamic_cast<CUIItemGetManager*>(gameObject)->SetInventory(m_PlayerInventoryCom);
    dynamic_cast<CUIItemGetManager*>(gameObject)->Initialize();

    CUIBuildMenu* uiBuildMenu = CUIBuildMenu::Create(m_GraphicDev);
    FAILED_CHECK_MSG(layer->Add_GameObject(OBJTYPE::UI, uiBuildMenu),
                     L"CStage::Ready_UI_Layer() failed: CLayer::Add_GameObject(UI_BuildMenu) failed");
    m_WorkBench->Set_BuildMenu(uiBuildMenu);
    CCookingController* cookingController = dynamic_cast<CCookingController*>(m_WorkBench->Get_Component(
        COMPONENTID::ID_STATIC,
        COMPONENTTYPE::COOKCTRL));
    uiBuildMenu->Set_Inventory(m_PlayerInventoryCom);
    /*uiBuildMenu->SetCellEvent([]()
    {
        return dynamic_cast<CCookingController*>(m_WorkBench->Get_Component(COMPONENTID::ID_STATIC,COMPONENTTYPE::COOKCTRL))->CanCooK(uiBuildMenu->,m_PlayerInventoryCom)})*/

    CUICookingMenu* uiCookingMenu = CUICookingMenu::Create(m_GraphicDev);
    FAILED_CHECK_MSG(layer->Add_GameObject(OBJTYPE::UI, uiCookingMenu),
                     L"CStage::Ready_UI_Layer() failed: CLayer::Add_GameObject(UI_CookingMenu) failed");

    if (uiCookingMenu)
        m_CookingMenu = uiCookingMenu;

    CUI_FollowerCommand* followerCommand = CUI_FollowerCommand::Create(m_GraphicDev);
    FAILED_CHECK_MSG(layer->Add_GameObject(OBJTYPE::UI, followerCommand),
                     L"CStage::Ready_UI_Layer() failed: CLayer::Add_GameObject(CUI_FollowerCommand) failed");

    if (followerCommand)
        m_FollowerCommand = followerCommand;

    FAILED_CHECK_MSG(
        Engine::AcquirePersistentObject<CUI_SkillPanel>(
            OBJTYPE::UI,
            L"UI_SkillPanel",
            m_GraphicDev,
            layer,
            &gameObject),
        L"Persistent object setup failed: UI_SkillPanel");

    CUI_ProgressBar* uiProgressBar = CUI_ProgressBar::Create(m_GraphicDev);
    FAILED_CHECK_MSG(layer->Add_GameObject(OBJTYPE::UI, uiProgressBar),
                     L"CStage::Ready_UI_Layer() failed: CLayer::Add_GameObject(UI_ProgressBar) failed");

    m_PlayerGatherCom->Set_ProgressBar(uiProgressBar);
    m_PlayerConstructionCom->Set_ProgressBar(uiProgressBar);

    m_Layers.insert({ layerType, layer });

    return S_OK;
}

HRESULT CStage::Ready_Prototype()
{
    return S_OK;
}

HRESULT CStage::Ready_Light()
{
    //return CScene::Ready_Light();
    return S_OK;
}

void CStage::Create_Trigger()
{
    auto gameLayer = m_Layers[LAYERTYPE::GAMELOGIC];
    NULL_CHECK(gameLayer);

    auto trigger = CTrigger_Dungeon::Create(m_GraphicDev);

    // Terrain 위치에 맞춰서 스폰
    _vec3 triggerPos = m_Terrain->Get_Transform()->Get_Pos();
    _vec3 offset     = _vec3(18.f, 0.f, 46.f);

    // 트리거 위치, 크기 세팅
    trigger->Set_Bounds(triggerPos + offset, _vec3(10.f, 5.f, 5.f));
    trigger->Set_TriggerOnce(false);

    trigger->Set_Callback([this]
    {
        CSceneMgr::GetInstance()->Change_SceneMgr(
            SCENETYPE::SC_DUNGEON);
    });

    gameLayer->Add_GameObject(OBJTYPE::TRIGGER, trigger);
}

void CStage::Spawn_FollowerCow()
{
    auto layer = m_Layers[LAYERTYPE::GAMELOGIC];
    NULL_CHECK(layer);

    SavedNpc* cowState       = CVillageStateMgr::GetInstance()->Get_NPCState(L"FollowerCow");
    _bool     shouldSpawnCow = false; // 기본값: 데이터 없으면 스폰 안함

    if (cowState != nullptr)
        shouldSpawnCow = cowState->isActive;

    if (shouldSpawnCow)
    {
        CFollowerCow* followerCow = CFollowerCow::Create(m_GraphicDev);

        layer->Add_GameObject(OBJTYPE::NPC, followerCow);

        followerCow->Initialize_GameObject();
    }
}

void CStage::Spawn_FollowerElephant()
{
    auto layer = m_Layers[LAYERTYPE::GAMELOGIC];
    NULL_CHECK(layer);

    SavedNpc* cowState            = CVillageStateMgr::GetInstance()->Get_NPCState(L"FollowerElephant");
    _bool     shouldSpawnElephant = false; // 기본값: 데이터 없으면 스폰 안함

    if (cowState != nullptr)
        shouldSpawnElephant = cowState->isActive;

    if (shouldSpawnElephant)
    {
        CFollowerElephant* followerElephant = CFollowerElephant::Create(m_GraphicDev);

        layer->Add_GameObject(OBJTYPE::NPC, followerElephant);

        followerElephant->Initialize_GameObject();
    }
}

void CStage::Spawn_FollowerPig()
{
    auto layer = m_Layers[LAYERTYPE::GAMELOGIC];
    NULL_CHECK(layer);

    SavedNpc* cowState       = CVillageStateMgr::GetInstance()->Get_NPCState(L"FollowerPig");
    _bool     shouldSpawnPig = false; // 기본값: 데이터 없으면 스폰 안함

    if (cowState != nullptr)
        shouldSpawnPig = cowState->isActive;

    if (shouldSpawnPig)
    {
        CFollowerPig* followerPig = CFollowerPig::Create(m_GraphicDev);

        layer->Add_GameObject(OBJTYPE::NPC, followerPig);

        followerPig->Initialize_GameObject();
    }
}

CStage* CStage::Create(DEVICE graphicDev)
{
    auto stage = new CStage(graphicDev);

    if (FAILED(stage->Ready_Scene()))
    {
        MSG_BOX("Stage Create Failed");
        Safe_Release(stage);
        return nullptr;
    }

    return stage;
}

void CStage::Free()
{
    CDropSystem::GetInstance()->Clear_Item();
    CDroppedItem::Reset_WeaponExplainUI();
    CConstructionMgr::GetInstance()->Release_Placer();
   
    Engine::CScene::Free();
}
