#include "pch.h"
#include "CEdit.h"

#include "CBoxCollider.h"
#include "CCameraController.h"
#include "CCamSetupBuilder.h"
#include "CDInputMgr.h"
#include "CLayer.h"
#include "CMainCamera.h"
#include "CMainEditorMgr.h"
#include "CMainMenu.h"
#include "CMonster.h"
#include "CPersistentObjectHelper.h"
#include "CPlayer.h"
#include "CSceneMgr.h"
#include "CSphereCollider.h"
#include "CState.h"
#include "CTerrain.h"
#include "CTransform.h"
#include "CWall.h"
#include "CWallRenderer.h"

CEdit::CEdit(DEVICE graphicDev)
    : Engine::CScene(graphicDev)
{ }

CEdit::~CEdit()
{ }

HRESULT CEdit::Ready_Scene()
{
    if (FAILED(Ready_Environment_Layer(LAYERTYPE::ENVIRONMENT)))
        return E_FAIL;

    if (FAILED(Ready_GameLogic_Layer(LAYERTYPE::GAMELOGIC)))
        return E_FAIL;

    if (FAILED(Ready_UI_Layer(LAYERTYPE::UI)))
        return E_FAIL;

    if (FAILED(Ready_Light()))
        return E_FAIL;

    return S_OK;
}

_int CEdit::Update_Scene(const _float& timeDelta)
{
    _int exit = Engine::CScene::Update_Scene(timeDelta);

    return exit;
}

void CEdit::LateUpdate_Scene(const _float& timeDelta)
{
    Engine::CScene::LateUpdate_Scene(timeDelta);
}

void CEdit::Render_Scene()
{
    // debug 용
}

HRESULT CEdit::Ready_Light()
{
    return CScene::Ready_Light();

}

HRESULT CEdit::Ready_Environment_Layer(LAYERTYPE layerType)
{
    Engine::CLayer* layer = Engine::CLayer::Create();

    if (nullptr == layer)
        return E_FAIL;

    CGameObject* gameObject = nullptr;
    // -----------------------------
    // Tile
    // -----------------------------
    CTerrain* terrain = CTerrain::Create(m_GraphicDev);

    NULL_CHECK_RETURN_MSG(terrain, E_FAIL, L"Terrain create failed");
    //terrain->LoadMapOnStart("../Bin//Resource/Data/Terrain/Terrain_WallTest1.map");

    layer->Add_GameObject(OBJTYPE::TERRAIN, terrain);

    m_Layers.insert({ layerType, layer });

    return S_OK;
}

HRESULT CEdit::Ready_GameLogic_Layer(LAYERTYPE layerType)
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
    const auto playerTransform = dynamic_cast<CTransform*>(
        gameObject->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));

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
            .PivotOffset({ 0.f, 4.7f, -5.3f })
            .Distance(12.f)
            .RotX(45.f)
            //.RotY(0.f)
            //.LagSpeed(10.f)
            //.ZoomSpeed(5.f)
            .Snap()
            .Apply();
    }

    auto mainCam = dynamic_cast<CMainCamera*>(gameObject);
    if (mainCam)
    {
        auto camController = dynamic_cast<CCameraController*>(
            mainCam->Get_Component(ID_DYNAMIC, COMPONENTTYPE::CAM_CONTROLLER));

        camController->Set_CamTarget(playerTransform);

        if (camController)
            CMainEditorMgr::GetInstance()->Set_MainCamera(camController); // 에디터에 카메라 설정
    }

    m_Layers.insert({ layerType, layer });

    return S_OK;
}

HRESULT CEdit::Ready_UI_Layer(LAYERTYPE layerType)
{
    Engine::CLayer* layer = Engine::CLayer::Create();

    if (nullptr == layer)
        return E_FAIL;

    Engine::CGameObject* pGameObject = nullptr;

    m_Layers.insert({ layerType, layer });

    return S_OK;
}

CEdit* CEdit::Create(DEVICE graphicDev)
{
    auto stage = new CEdit(graphicDev);

    if (FAILED(stage->Ready_Scene()))
    {
        MSG_BOX("Stage Create Failed");
        Safe_Release(stage);
        return nullptr;
    }

    return stage;
}

void CEdit::Free()
{
    Engine::CScene::Free();
}
