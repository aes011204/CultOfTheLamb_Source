#include "pch.h"
#include "CStage_Boss1.h"
#include "CState.h"
#include "CBossPhase1.h"
#include "CCameraController.h"
#include "CCamSetupBuilder.h"
#include "CRigidbody.h"
#include "CDropSystem.h"
#include "CLayer.h"
#include "CMainCamera.h"
#include "CMainEditorMgr.h"
#include "CCombatStat.h"
#include "CMainMenu.h"
#include "CPersistentObjectHelper.h"
#include "CPlayer.h"
#include "CTerrain.h"
#include "CTransform.h"
#include "CTrigger_Dungeon.h"
#include "CUICircle.h"
#include "CUIIcon.h"
#include "CUIMp.h"
#include "CUIPlayerPanel.h"
#include "CUIWeapon.h"
#include "CUI_BossHp.h"
#include "CUI_Dialogue.h"
#include "CWall.h"
#include "CAnimator.h"
#include "CBossDead.h"
#include "CBossStateMachine.h"
#include "CBossTalk.h"
#include "CFog.h"
#include "CPlayerController.h"
#include "CWeaponEquip.h"

CStage_Boss1::CStage_Boss1(DEVICE graphicDev)
    : Engine::CScene(graphicDev), m_Terrain(nullptr), m_Boss(nullptr), m_Player(nullptr), m_Dialogue(nullptr),
      m_CamController(nullptr),
      m_PlayerTransformCom(nullptr),
      m_PlayerCombatStatCom(nullptr), m_PlayerInventoryCom(nullptr),
      m_WeaponEquipCom(nullptr),
      m_PlayerSkillCom(nullptr), m_BossHpbar(nullptr)
{
}

CStage_Boss1::~CStage_Boss1()
{ }

HRESULT CStage_Boss1::Ready_Scene()
{
    if (FAILED(Ready_Environment_Layer(LAYERTYPE::ENVIRONMENT)))
        return E_FAIL;

    if (FAILED(Ready_GameLogic_Layer(LAYERTYPE::GAMELOGIC)))
        return E_FAIL;

    if (FAILED(Ready_UI_Layer(LAYERTYPE::UI)))
        return E_FAIL;

    if (FAILED(Ready_Light()))
        return E_FAIL;

    //아이템 드롭 할거면 씬 셋팅 필수
    CDropSystem::GetInstance()->SetCurrentScene(this);

    return S_OK;
}

void CStage_Boss1::Initialize_Scene()
{
    CScene::Initialize_Scene();

    m_Terrain->LoadMapOnStart("../Bin//Resource/Data/Terrain/Terrain_Boss1.map");

    Setup_TriggerCallbacks();
    Setup_DialogueCallbacks();
    Setup_BossCallbacks();
    Setup_PlayerCallbacks();

    // 처음에는 거리비례 카메라 Off
    auto camMovement = m_CamController->Get_OwnerCamMovement();
    camMovement->Disable_DynamicDistance();
}

_int CStage_Boss1::Update_Scene(const _float& timeDelta)
{
    _int exit = Engine::CScene::Update_Scene(timeDelta);

    Move_PlayerUI(timeDelta);
    Move_BossUI(timeDelta);

    if (m_WaitingForDialogue)
    {
        m_DialogueDelayTimer += timeDelta;

        if (m_DialogueDelayTimer >= 1.1f)
        {
            m_Dialogue->Start_Dialogue("Boss_Start", _vec2(0.f, 200.f), _vec2(300.f, 80.f));

            m_WaitingForDialogue = false;
        }
    }


    return exit;
}

void CStage_Boss1::LateUpdate_Scene(const _float& timeDelta)
{
    Engine::CScene::LateUpdate_Scene(timeDelta);
}

void CStage_Boss1::Render_Scene()
{
    
}

HRESULT CStage_Boss1::Ready_Light()
{
    return CScene::Ready_Light();
}

void CStage_Boss1::Create_Trigger(_vec3 pos, _vec3 size, OnTriggerCallback callback, _bool autoDestroy)
{
    auto gameLayer = m_Layers[LAYERTYPE::GAMELOGIC];
    NULL_CHECK(gameLayer);

    auto trigger = CTrigger_Dungeon::Create(m_GraphicDev);
    NULL_CHECK(trigger);

    trigger->Set_Bounds(pos, size);
    trigger->Set_TriggerOnce(false); // 한 번만 실행, 테스트 용으로 일단 fasle

    trigger->Set_Callback([callback, trigger, autoDestroy]()
        {
            callback();

            if (autoDestroy)
                trigger->Mark_As_PendingKill();
        });

    gameLayer->Add_GameObject(OBJTYPE::TRIGGER, trigger);
}

HRESULT CStage_Boss1::Ready_Environment_Layer(LAYERTYPE layerType)
{
    Engine::CLayer* layer = Engine::CLayer::Create();

    if (nullptr == layer)
        return E_FAIL;

    CGameObject* gameObject = nullptr;
    // -----------------------------
    // Tile
    // -----------------------------
    m_Terrain = CTerrain::Create(m_GraphicDev);
    
    NULL_CHECK_RETURN_MSG(
        m_Terrain,
        E_FAIL,
        L"CStage_Boss1::Ready_GameLogic_Layer() failed: CTerrain::Create() returned null");
    
    FAILED_CHECK_MSG(
        layer->Add_GameObject(OBJTYPE::TERRAIN, m_Terrain),
        L"CStage_TarotTest::Ready_GameLogic_Layer() failed: CLayer::Add_GameObject(Tile) failed");
    
    // Wall
    CWall* wall = CWall::Create(m_GraphicDev);
    NULL_CHECK_RETURN(wall, E_FAIL);
    layer->Add_GameObject(OBJTYPE::WALL, wall);

    // Fog
    m_Fog = CFog::Create(m_GraphicDev);
    NULL_CHECK_RETURN(m_Fog, E_FAIL);

    m_Fog->Set_OriginalPos(m_FogPos);
    m_Fog->Set_SwaySpeed(m_FogSwaySpeed);
    m_Fog->Set_SwayAmount(m_FogSwayAmount);
    m_Fog->Set_Alpha(m_FogAlpha);
    layer->Add_GameObject(OBJTYPE::FOG, m_Fog);

    for (_int i = 0; i < 10; i++)
    {
        CFog* fog2 = CFog::Create(m_GraphicDev);
        fog2->Set_OriginalPos(_vec3(48.5f, 8.f, 11.f));
        fog2->Set_SwaySpeed(0.7f);  // 약간 다른 속도
        layer->Add_GameObject(OBJTYPE::FOG, fog2);
    }


    m_Layers.insert({ layerType, layer });          // scene -> map

    return S_OK;
}

HRESULT CStage_Boss1::Ready_GameLogic_Layer(LAYERTYPE layerType)
{
    Engine::CLayer* layer = Engine::CLayer::Create();

    NULL_CHECK_RETURN_MSG(
        layer, E_FAIL,
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
            &m_Player),
        L"Persistent object setup failed");

    // 플레이어의 Transform 가져오기
    m_PlayerTransformCom = m_Player->Get_Transform();
    m_PlayerTransformCom->Set_Pos({ 47.f,1.f,10.f });

    m_PlayerCombatStatCom = dynamic_cast<CCombatStat*>(
        m_Player->Get_Component(ID_DYNAMIC, COMPONENTTYPE::COMBATSTAT));

    // -----------------------------
    // Camera
    // -----------------------------
    FAILED_CHECK_MSG(
        Engine::AcquirePersistentObject<CMainCamera>(
            OBJTYPE::CAMERA, L"MainCamera", m_GraphicDev, layer, &gameObject),
        L"Persistent object setup failed");

    m_CamController = dynamic_cast<CCameraController*>(
        gameObject->Get_Component(ID_DYNAMIC, COMPONENTTYPE::CAM_CONTROLLER));

    // 카메라 설정값 초기화
    if (m_CamController != nullptr)
    {
        // 씬에서 카메라 생성 시 값 초기화
        // : CCameraSetup을 카메라 설정을 중도에 바꿀 때 사용 가능하고,
        //   그럴 때는 필요한 것만 사용해 .Apply() 하면 된다.
        CCamSetupBuilder(m_CamController)
            .SetTarget(m_PlayerTransformCom)
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

    // -----------------------------
    // BossPhase1
    // -----------------------------

    // 보스 생성
    m_Boss = CBossPhase1::Create(m_GraphicDev);
    NULL_CHECK_RETURN_MSG(m_Boss, E_FAIL, L"CStage::Ready_GameLogic_Layer() failed: CBossPhase2::Create() returned null")

    FAILED_CHECK_MSG(
        layer->Add_GameObject(OBJTYPE::BOSS1, m_Boss),
        L"CStage::Ready_GameLogic_Layer() failed: CLayer::Add_GameObject(Boss) failed");


    m_Layers.insert({ layerType, layer });

    return S_OK;
}

HRESULT CStage_Boss1::Ready_UI_Layer(LAYERTYPE layerType)
{
    Engine::CLayer* layer = Engine::CLayer::Create();
    NULL_CHECK_RETURN(layer, E_FAIL);

    Engine::CGameObject* gameObject = nullptr;

    // ==== 플레이어 UI ====

    // Panel
    FAILED_CHECK_MSG(
        Engine::AcquirePersistentObject<CUIPlayerPanel>(
            OBJTYPE::UI, L"UI_PlayerPanel", m_GraphicDev, layer, &gameObject), L"Persistent object setup failed: UI_PlayerPanel");

    CUIPlayerPanel* playerPanel = dynamic_cast<CUIPlayerPanel*>(gameObject);
    NULL_CHECK_RETURN(playerPanel, E_FAIL);

    playerPanel->Set_PlayerCombatStat(m_PlayerCombatStatCom);
    playerPanel->Set_Resister(true);
    m_Player_UI.push_back(playerPanel);

    // Mp
    FAILED_CHECK_MSG(
        Engine::AcquirePersistentObject<CUIMp>(
            OBJTYPE::UI, L"UI_Mp", m_GraphicDev, layer, &gameObject), L"Persistent object setup failed: UI_Mp");

    dynamic_cast<CUIMp*>(gameObject)->Set_PlayerCombatStat(m_PlayerCombatStatCom);
    m_Player_UI.push_back(dynamic_cast<CUI*>(gameObject));

    // CUI_Circle
    FAILED_CHECK_MSG(
        Engine::AcquirePersistentObject<CUICircle>(
            OBJTYPE::UI, L"UI_Circle", m_GraphicDev, layer, &gameObject), L"Persistent object setup failed: UI_Circle");

    dynamic_cast<CUICircle*>(gameObject)->Set_PlayerCombatStat(m_PlayerCombatStatCom);
    m_Player_UI.push_back(dynamic_cast<CUI*>(gameObject));

    // CUI_Weapon
    FAILED_CHECK_MSG(
        Engine::AcquirePersistentObject<CUIWeapon>(
            OBJTYPE::UI, L"UI_Weapon", m_GraphicDev, layer, &gameObject), L"Persistent object setup failed: UI_Weapon");

    m_Player_UI.push_back(dynamic_cast<CUI*>(gameObject));

    //왼쪽 상단 스킬 아이콘 UI
    CUIIcon* uiIcon = CUIIcon::Create(m_GraphicDev, COMPONENTTYPE::TEX_UI_ICON);
    uiIcon->Set_TextureIndex(1);
    uiIcon->Get_Transform()->Set_Pos(-524.f, 260.f, 0.f);
    uiIcon->Get_Transform()->Set_Scale(12.5f, 15.f, 0.f);

    FAILED_CHECK_MSG(
        layer->Add_GameObject(OBJTYPE::UI, uiIcon),
        L"CStage::Ready_UI_Layer() failed: CLayer::Add_GameObject(CUIIcon) failed");

    m_Player_UI.push_back(uiIcon);

    // ===== 보스 UI =====
    auto combat = dynamic_cast<CCombatStat*>(
        m_Boss->Get_Component(ID_DYNAMIC, COMPONENTTYPE::COMBATSTAT));

    NULL_CHECK_RETURN(combat, E_FAIL);

    m_BossHpbar = CUI_BossHp::Create(m_GraphicDev, combat);
    m_BossHpbar->Get_Transform()->Set_Pos(_vec3(0.f, -400.f, 0.f));

    layer->Add_GameObject(OBJTYPE::UI, m_BossHpbar);

    // ==== Dialogue ==== 
    m_Dialogue = CUI_Dialogue::Create(m_GraphicDev);
    layer->Add_GameObject(OBJTYPE::UI, m_Dialogue);

    m_Layers.insert({ layerType, layer });

    return S_OK;
}

void CStage_Boss1::OnDialogueEnd(const string& id)
{
    if (id == "Boss_Start")
    {
        On_BossStartEnd();
    }
    else if (id == "Boss_Sacrifice")
    {
        On_SacrificeEnd();
    }
    else if (id == "Boss_Reject")
    {
        On_RejectEnd();
    }
    else if (id == "Boss1_Dead")
    {
        On_DeadEnd();
    }

}

void CStage_Boss1::Begin_PlayerDecide()
{
    auto rigidbody = dynamic_cast<CRigidbody*>(
        m_Player->Get_Component(ID_DYNAMIC, COMPONENTTYPE::RIGIDBODY));

    if (rigidbody)
        rigidbody->Set_Velocity({ 0.f, 0.f, 0.f });

    auto state = dynamic_cast<CState*>(m_Player->Get_Component(
        ID_DYNAMIC, COMPONENTTYPE::STATE));

    NULL_CHECK(state);

    state->Change_State(ACTORSTATE::PLAYER_DECIDE);

    m_Dialogue->Start_Dialogue("Boss_Sacrifice", _vec2(0.f, 200.f), _vec2(300.f, 80.f));

    CCamSetupBuilder(m_CamController)
        .SetTarget(m_PlayerTransformCom)
        .Distance(8.f)
        .LagSpeed(3.f)
        .ZoomSpeed(1.f)
        .Apply();

}

void CStage_Boss1::Hide_PlayerUI()
{
    Setup_UIAnimation(220.f);

    CCamSetupBuilder(m_CamController)
        .SetTarget(m_Boss->Get_Transform())
        .Distance(11.f)
        .LagSpeed(3.f)
        .ZoomSpeed(0.5f)
        .Apply();

    m_WaitingForDialogue = true;
    m_DialogueDelayTimer = 0.f;
}

void CStage_Boss1::Show_PlayerUI()
{
    Setup_UIAnimation(-220.f);
}

void CStage_Boss1::Setup_UIAnimation(_float offsetY)
{
    m_IsUIMoving = true;
    m_UIAnimTime = 0.f;

    m_OriginalUIPositions.clear();
    m_TargetUIPositions.clear();

    for (const auto& ui : m_Player_UI)
    {
        _vec3 pos = ui->Get_Transform()->Get_Pos();

        m_OriginalUIPositions.emplace_back(pos);
        m_TargetUIPositions.emplace_back(_vec3(pos.x, pos.y + offsetY, pos.z));
    }
}

void CStage_Boss1::OnRefusedEnd()
{
    // 첫 번째 연출 끝났을 때 콜백
    m_CamController->Get_OwnerCamMovement()->Set_OnTransitionEnd([this]()
        {
            // Refuse 카메라 연출 끝, 이제 보스한테 줌
            CCamSetupBuilder(m_CamController)
                .SetTarget(m_Boss->Get_Transform())
                .Distance(11.f)
                .LagSpeed(3.f)
                .ZoomSpeed(3.f)
                .Apply();

            // 두 번쨰 연출 끝났을 때 콜백
            m_CamController->Get_OwnerCamMovement()->Set_OnTransitionEnd([this]()
                {
                    // 대사 시작
                    m_Dialogue->Start_Dialogue("Boss_Reject", _vec2(0.f, 210.f), _vec2(300.f, 80.f));
                });
        });


    // 카메라 플레이어한테 원래대로 돌리기
    CCamSetupBuilder(m_CamController)
        .SetTarget(m_PlayerTransformCom)
        .Distance(12.f)
        .LagSpeed(3.f)
        .ZoomSpeed(3.f)
        .Apply();

}

void CStage_Boss1::On_BossStartEnd()
{
    Reset_Cam();
}

void CStage_Boss1::On_SacrificeEnd()
{
    auto weapon = dynamic_cast<CWeaponEquip*>(
        m_Player->Get_Component(ID_STATIC, COMPONENTTYPE::WEAPON_EQUIP));

    NULL_CHECK(weapon);

    auto animator = dynamic_cast<CAnimator*>(
        m_Player->Get_Component(ID_DYNAMIC, COMPONENTTYPE::ANIMATOR));

    NULL_CHECK(animator);

    int weaponID = weapon->Get_CurrentWeaponID();

    // 3001 : 검, 3002 : 건틀릿
    if (weapon->Get_CurrentWeaponID() == 3001)
    {
        animator->Play_Animation(L"Refuse_Sword", ANIMSTATE::ONCE);
    }

    else if (weapon->Get_CurrentWeaponID() == 3002)
    {
        animator->Play_Animation(L"Refuse_Gauntlet", ANIMSTATE::ONCE);
    }
    // 디버깅 할 때 무기 안들대 기본값
    else
        animator->Play_Animation(L"Refuse_Sword", ANIMSTATE::ONCE);

    auto state = dynamic_cast<CState*>(m_Player->Get_Component(
        ID_DYNAMIC, COMPONENTTYPE::STATE));

    NULL_CHECK(state);

    state->Change_State(ACTORSTATE::PLAYER_REFUSE);
}

void CStage_Boss1::On_RejectEnd()
{
    // 대사 끝, 전투 시작
    auto boss = dynamic_cast<CBossPhase1*>(m_Boss);
    NULL_CHECK(boss);
    
    auto stateMachine = boss->Get_StateMachine();

    auto talkState = dynamic_cast<CBossTalk*>(
        stateMachine->Get_CurState());

    if (talkState)
        talkState->Set_Phase(CBossTalk::PHASE::CAHANGE);

    Reset_Cam();        // Cam 원래대로 돌리고,
    Setup_DynamicCam(); // Dynamic Cam 시작

    Show_PlayerUI();    // // UI 원래대로, 이후 보스 체력바도 보이도록

    if (m_BossHpbar)
    {
        m_BossHpbar->Set_Visible(true);
        m_IsBossUIMoving = true;
    }
        
}

void CStage_Boss1::On_DeadEnd()
{
    auto boss = dynamic_cast<CBossPhase1*>(m_Boss);

    auto deadState = dynamic_cast<CBossDead*>(
        boss->Get_StateMachine()->Get_CurState());

    if (deadState)
    {
        // 여기서 보스 Disappear로 전환하면서 애니메이션 끝나면 씬전환
        deadState->Set_DialogueFinished();
    }
}

void CStage_Boss1::Reset_Cam()
{
    CCamSetupBuilder(m_CamController)
        .SetTarget(m_PlayerTransformCom)
        .Distance(18.f) // 게임이랑 비슷하게할거면 Distance좀 더 조절해야 할듯
        .LagSpeed(3.f)
        .ZoomSpeed(0.5f)
        .Apply();
}

void CStage_Boss1::Setup_DynamicCam()
{
    auto camMovement = m_CamController->Get_OwnerCamMovement();

    camMovement->Set_SecondTarget(m_Boss->Get_Transform());
    camMovement->Set_DynamicDistanceParams(14.f, 23.f, 7.f, 15.f);

    camMovement->Enable_DynamicDistance(true);
}

void CStage_Boss1::Setup_TriggerCallbacks()
{
    // 트리거 생성
    const _vec3 firstTriggerPos = _vec3(47.3f, 1.f, 30.f);
    const _vec3 secondTriggerPos = _vec3(47.8f, 1.f, 49.f);

    Create_Trigger(firstTriggerPos, _vec3(10.f, 5.f, 5.f), [this]()
        {
            Hide_PlayerUI();
        }, true);

    Create_Trigger(secondTriggerPos, _vec3(5.f, 5.f, 5.f), [this]()
        {
            Begin_PlayerDecide();
        }, true);
}

void CStage_Boss1::Setup_DialogueCallbacks()
{
    NULL_CHECK(m_Dialogue);

    // 대화 종료
    m_Dialogue->Set_OnDialogueEnd([this](const string& id)
        {
            OnDialogueEnd(id);
        });
}

void CStage_Boss1::Setup_BossCallbacks()
{
    auto boss = dynamic_cast<CBossPhase1*>(m_Boss);

    boss->Set_OnDeadTalkStart([this]()
        {
            m_CamController->Get_OwnerCamMovement()->Disable_DynamicDistance();

            CCamSetupBuilder(m_CamController)
                .SetTarget(m_Boss->Get_Transform())
                .Distance(10.f)
                .PivotOffset(_vec3(0.f, 0.f, -3.f))
                .LagSpeed(4.f)
                .ZoomSpeed(3.5f)
                .Apply();

            m_CamController->Get_OwnerCamMovement()->Set_OnTransitionEnd([this]()
                {
                    m_Dialogue->Start_Dialogue("Boss1_Dead", _vec2(0, 150));
                });
        });
}

void CStage_Boss1::Setup_PlayerCallbacks()
{
    auto controller = dynamic_cast<CPlayerController*>(m_Player->Get_Component(
        ID_DYNAMIC, COMPONENTTYPE::PL_CONTROLLER));

    NULL_CHECK(controller);

    controller->Set_OnRefuseEnd([this]()
        {
            OnRefusedEnd();
        });
}

void CStage_Boss1::Move_PlayerUI(const _float timeDelta)
{
    if (!m_IsUIMoving)
        return;

    m_UIAnimTime += timeDelta;
    _float ratio = clamp(m_UIAnimTime / m_UIAnimDuration, 0.f, 1.f);

    for (size_t i = 0; i < m_Player_UI.size(); ++i)
    {
        _vec3 current = Engine::Lerp_EaseIn(
            m_OriginalUIPositions[i],
            m_TargetUIPositions[i],
            ratio,
            3.3f  
        );

        m_Player_UI[i]->Get_Transform()->Set_Pos(current);
    }

    if (ratio >= 1.f)
        m_IsUIMoving = false;
}

void CStage_Boss1::Move_BossUI(const _float timeDelta)
{
    if (!m_IsBossUIMoving || !m_BossHpbar)
        return;

    const _vec3 startPos  = { 0.f, -400.f, 0.f };
    const _vec3 targetPos = { 0.f, -250.f, 0.f };

    _float ratio = clamp(m_UIAnimTime / m_UIAnimDuration, 0.f, 1.f);

    _vec3 current = Engine::Lerp_EaseIn(
        startPos,
        targetPos,
        ratio,
        3.3f
    );

    m_BossHpbar->Get_Transform()->Set_Pos(current);

    if (ratio >= 1.f)
        m_IsBossUIMoving = false;
   
}

CStage_Boss1* CStage_Boss1::Create(DEVICE graphicDev)
{
    auto stage = new CStage_Boss1(graphicDev);

    if (FAILED(stage->Ready_Scene()))
    {
        MSG_BOX("Stage Boss1 Create Failed");
        Safe_Release(stage);
        return nullptr;
    }

    return stage;
}

void CStage_Boss1::Free()
{
    Engine::CScene::Free();
}
