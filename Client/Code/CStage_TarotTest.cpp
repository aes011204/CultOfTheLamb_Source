#include "pch.h"
#include "CStage_TarotTest.h"

#include "CCameraController.h"
#include "CCamSetupBuilder.h"
#include "CLayer.h"
#include "CMainCamera.h"
#include "CMainEditorMgr.h"
#include "CMainMenu.h"
#include "CMonster.h"
#include "CPersistentObjectHelper.h"
#include "CPlayer.h"
#include "CState.h"
#include "CTarotStageController.h"
#include "CTerrain.h"
#include "CTransform.h"
#include "CWall.h"

CStage_TarotTest::CStage_TarotTest(DEVICE graphicDev)
    : Engine::CScene(graphicDev)
{

}

CStage_TarotTest::~CStage_TarotTest()
{

}

HRESULT CStage_TarotTest::Ready_Scene()
{
    if (FAILED(Ready_Environment_Layer(LAYERTYPE::ENVIRONMENT)))
        return E_FAIL;

    if (FAILED(Ready_GameLogic_Layer(LAYERTYPE::GAMELOGIC)))
        return E_FAIL;

    if (FAILED(Ready_UI_Layer(LAYERTYPE::UI)))
        return E_FAIL;

    return S_OK;
}

_int CStage_TarotTest::Update_Scene(const _float& timeDelta)
{
    _int exit = Engine::CScene::Update_Scene(timeDelta);

    return exit;
}

void CStage_TarotTest::LateUpdate_Scene(const _float& timeDelta)
{
    Engine::CScene::LateUpdate_Scene(timeDelta);
}

void CStage_TarotTest::Render_Scene()
{
    // debug 용
}

HRESULT CStage_TarotTest::Ready_Environment_Layer(LAYERTYPE layerType)
{
    Engine::CLayer* layer = Engine::CLayer::Create();

    if (nullptr == layer)
        return E_FAIL;

    CGameObject* gameObject = nullptr;
    // -----------------------------
    // Tile
    // -----------------------------
    gameObject = CTerrain::Create(m_GraphicDev);

    NULL_CHECK_RETURN_MSG(
        gameObject,
        E_FAIL,
        L"CStage_TarotTest::Ready_GameLogic_Layer() failed: CTerrain::Create() returned null");

    FAILED_CHECK_MSG(
        layer->Add_GameObject(OBJTYPE::TERRAIN, gameObject),
        L"CStage_TarotTest::Ready_GameLogic_Layer() failed: CLayer::Add_GameObject(Tile) failed");

    // Wall
    CWall* wall = CWall::Create(m_GraphicDev);
    NULL_CHECK_RETURN(wall, E_FAIL);

    layer->Add_GameObject(OBJTYPE::WALL, wall);

    m_Layers.insert({ layerType, layer });          // scene -> map

    return S_OK;
}

HRESULT CStage_TarotTest::Ready_GameLogic_Layer(LAYERTYPE layerType)
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

    // 플레이어의 Transform 가져오기
    const auto playerTransform =
        dynamic_cast<CTransform*>(gameObject->Get_Component(
            ID_DYNAMIC,
            COMPONENTTYPE::TRANSFORM));

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
            //.PivotOffset({ 0.f, 0.f, 0.f })
            //.Distance(12.f)
            //.RotX(45.f)
            //.RotY(0.f)
            //.LagSpeed(10.f)
            //.ZoomSpeed(5.f)
            .Snap()
            .Apply();

        // 에디터에 카메라 설정
        CMainEditorMgr::GetInstance()->Set_MainCamera(camController);
    }

  
    m_Layers.insert({ layerType, layer });

    return S_OK;

}

HRESULT CStage_TarotTest::Ready_UI_Layer(LAYERTYPE layerType)
{
    Engine::CLayer* layer = Engine::CLayer::Create();

    if (nullptr == layer)
        return E_FAIL;

    Engine::CGameObject* pGameObject = nullptr;

    // CTransform* tarotCardTransform = dynamic_cast<CTransform*>(pGameObject->Get_Component(COMPONENTID::ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));
   //  tarotCardTransform->Set_Pos(-100.f, 0.f, 0.f);
   //  tarotCardTransform->Set_Scale(100.f, 100.f, 0.f);

     //NULL_CHECK_RETURN_MSG(
     //    pGameObject,
     //    E_FAIL,
     //    L"CStage_TarotTest::Ready_Ui_Layer() failed: CUITarotCard::Create() returned null");
     //
     //FAILED_CHECK_MSG(
     //    layer->Add_GameObject(OBJTYPE::TERRAIN, pGameObject),
     //    L"CStage_TarotTest::Ready_Ui_Layer() failed: CLayer::Add_GameObject(CUITarotCard) failed");

    m_Layers.insert({ layerType, layer });

    return S_OK;
}

HRESULT CStage_TarotTest::Ready_Prototype()
{
    // Move Loading Scene

    return S_OK;
}

CStage_TarotTest* CStage_TarotTest::Create(DEVICE graphicDev)
{
    auto stage = new CStage_TarotTest(graphicDev);

    if (FAILED(stage->Ready_Scene()))
    {
        MSG_BOX("Stage Taro Create Failed");
        Safe_Release(stage);
        return nullptr;
    }

    return stage;
}

void CStage_TarotTest::Free()
{
    Engine::CScene::Free();
}
