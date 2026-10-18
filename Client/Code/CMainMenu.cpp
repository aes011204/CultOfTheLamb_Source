#include "pch.h"
#include "CMainMenu.h"
#include "CSoundMgr.h"
#include "CCameraController.h"
#include "CCamSetupBuilder.h"
#include "CDInputMgr.h"
#include "CEdit.h"
#include "CFontMgr.h"
#include "CGameObject.h"
#include "CLayer.h"
#include "CMainBG.h"
#include "CMainCamera.h"
#include "CMenuChar.h"
#include "CPersistentObjectHelper.h"
#include "CSceneMgr.h"
#include "CStage.h"
#include "CTransform.h"
#include "CUIButton.h"
#include "CUIIcon.h"
#include "CMainEditorMgr.h"
#include "CMenuExtra.h"
#include "CRenderer.h"

CMainMenu::CMainMenu(DEVICE graphicDev)
    : Engine::CScene(graphicDev),

    bt(nullptr),
    bt1(nullptr),
    bt2(nullptr),
    bt3(nullptr)
{
}

CMainMenu::~CMainMenu()
{
}

HRESULT CMainMenu::Ready_Scene()
{
    if (FAILED(Ready_Environment_Layer(LAYERTYPE::ENVIRONMENT)))
        return E_FAIL;

    if (FAILED(Ready_GameLogic_Layer(LAYERTYPE::GAMELOGIC)))
        return E_FAIL;

    if (FAILED(Ready_UI_Layer(LAYERTYPE::UI)))
        return E_FAIL;

    if (FAILED(Ready_Light()))
        return E_FAIL;

    // 메인 화면 BGM
    CSoundMgr::GetInstance()->Play_BGM(L"BGM/BGM_Intro.wav", 0.4f, true, 1.f);
    CRenderer::GetInstance()->SetFog(true);

    return S_OK;
}

void CMainMenu::Initialize_Scene()
{
    CScene::Initialize_Scene();
}

_int CMainMenu::Update_Scene(const _float& timeDelta)
{
    _int exit = Engine::CScene::Update_Scene(timeDelta);

    if (CDInputMgr::GetInstance()->KeyDown(DIK_SPACE))
    {
        CAM_PRESET preset;
        preset.pivotOffset = { 0.f, 0.7f, 0.f };
        preset.dist = 8.6f;
        preset.pitch = 0.f;
        preset.yaw = 0.f;
        preset.lagSpeed = 15.f;

        m_CamController->Get_OwnerCamMovement()->Ease_ToPreset(preset, 2.5f, [](float x) {

            float c1 = 1.70158;

            float c3 = c1 + 1;

            return 1 + c3 * pow(x - 1, 3) + c1 * pow(x - 1, 2);});
    }


    return exit;
}

void CMainMenu::LateUpdate_Scene(const _float& timeDelta)
{
    Engine::CScene::LateUpdate_Scene(timeDelta);

}

void CMainMenu::Render_Scene()
{
    RenderFont(L"NanumSquare_Regular", L"플레이", bt);
    RenderFont(L"NanumSquare_Regular", L"맵툴", bt1);
    // RenderFont(L"견명조", L"설정", bt);
    RenderFont(L"NanumSquare_Regular", L"제작진", bt2);
    RenderFont(L"NanumSquare_Regular", L"종료", bt3);
}

HRESULT CMainMenu::Ready_Light()
{
    return CScene::Ready_Light();
}

void CMainMenu::RenderFont(const wstring& font, const wstring& text, CUIButton* btt)
{
    _vec2 pos{ btt->Get_Transform()->Get_Pos().x, btt->Get_Transform()->Get_Pos().y };

    _vec2 screen = { pos.x + WINCX * 0.5f,WINCY * 0.5f - pos.y };

    if (btt->Get_Hovered() == true)
    {
        CFontMgr::GetInstance()->Render_Font(font,
            text,
            &screen,
            D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
    }
    else
    {
        CFontMgr::GetInstance()->Render_Font(font,
            text,
            &screen,
            D3DXCOLOR(0.f, 0.f, 0.f, 1.f));
    }
}

HRESULT CMainMenu::Ready_Environment_Layer(LAYERTYPE layerType)
{
    Engine::CLayer* layer = Engine::CLayer::Create();

    if (nullptr == layer)
        return E_FAIL;

    Engine::CGameObject* pGameObject = nullptr;

    pGameObject = CMainBG::Create(m_GraphicDev);
    NULL_CHECK_RETURN(m_GraphicDev, E_FAIL)

        const auto bgTransform =
        dynamic_cast<CTransform*>(pGameObject->Get_Component(
            ID_DYNAMIC,
            COMPONENTTYPE::TRANSFORM));
    bgTransform->Set_Pos({ 0, 0, 1 });
    //playerTransform->Set_Scale({ 2.2f,1.2f,0.f });
    pGameObject->SetAspectScale(bgTransform, { 600.f, 600.f, 1.f });
    if (FAILED(layer->Add_GameObject(OBJTYPE::UI, pGameObject)))
        return E_FAIL;

    m_Layers.insert({ layerType, layer });          // scene -> map

    return S_OK;
}

HRESULT CMainMenu::Ready_GameLogic_Layer(LAYERTYPE layerType)
{
    Engine::CLayer* layer = Engine::CLayer::Create();

    Engine::CGameObject* gameObject = nullptr;

    gameObject = CMenuChar::Create(m_GraphicDev);
    NULL_CHECK_RETURN_MSG(gameObject, E_FAIL, L"CMainMenu::Ready_GameLogic_Layer() failed: CMenuChar::Create() returned null")

        FAILED_CHECK_MSG(
            layer->Add_GameObject(OBJTYPE::UI, gameObject),
            L"CMainMenu::Ready_GameLogic_Layer() failed: CLayer::Add_GameObject(CMenuChar) failed");

    const auto charTransform =
        dynamic_cast<CTransform*>(gameObject->Get_Component(
            ID_DYNAMIC,
            COMPONENTTYPE::TRANSFORM));
    //charTransform->Set_Scale({ 2.2f,1.2f,0.f });
   //gameObject->SetAspectScale(charTransform, { 3.0f, 3.0f, 1.f });


    charTransform->Set_Scale(_vec3(2.f, 2.9f, 1.f));

    // 카메라 생성
    FAILED_CHECK_MSG(
        Engine::AcquirePersistentObject<CMainCamera>(
            OBJTYPE::CAMERA, L"MainCamera", m_GraphicDev, layer, &gameObject),
        L"Persistent object setup failed"
    );

    m_CamController = dynamic_cast<CCameraController*>(
        gameObject->Get_Component(ID_DYNAMIC, COMPONENTTYPE::CAM_CONTROLLER));

    // 카메라 설정값 초기화
    if (m_CamController != nullptr)
    {
        // 씬에서 카메라 생성 시 값 초기화
        // : CCameraSetup을 카메라 설정을 중도에 바꿀 때 사용 가능하고,
        //   그럴 때는 필요한 것만 사용해 .Apply() 하면 된다.
        CCamSetupBuilder(m_CamController)
            .SetTarget(charTransform)
            .PivotOffset({ 0.f, 0.7f, 0.f })
            .Distance(31.f)
            .RotX(0.f)
            .RotY(0.f)
            .LagSpeed(15.f)
            .ZoomSpeed(5.f)
            .Snap()
            .Apply();

        // 에디터에 카메라 설정
        CMainEditorMgr::GetInstance()->Set_MainCamera(m_CamController);
    }
    

    if (nullptr == layer)
        return E_FAIL;

    /////////////////////////////////////////////////////////////////////
    //엑스트라 소환
    //////////////////////////////////////////////////////////////////////

    int index = 0;
    bool flag = false;
    for (int j = 1; j <= 4;j++)
    {
    int radius = 7*j;
    int num = 32*(j*1.0f);


        for (int i = 0; i < num;i++)
        {
            float angle = 360.f / (num); // 갯수 나누기 360 각도 구함

            float addangle = angle * i; // 몇번재 인지 정함

            int index = (int)(addangle / (360.f / 16.f));


            CMenuExtra* MenuExtra = CMenuExtra::Create(m_GraphicDev, index);

            CTransform* MenuExtraTransform =
                dynamic_cast<CTransform*>(MenuExtra->Get_Component(
                    ID_DYNAMIC,
                    COMPONENTTYPE::TRANSFORM));

            _vec3 ownerPos = charTransform->Get_Pos();


            _vec3 circle = { sin(D3DXToRadian(addangle)) * radius, 0, cos(D3DXToRadian(addangle)) * radius }; // 반지름 적용
            _vec3 TR = ownerPos + circle;
            MenuExtraTransform->Set_Pos({ TR.x, 0,TR.z });


            if (i > num * .5f)
            {
                _vec3 originScale = MenuExtraTransform->Get_Scale();
                MenuExtraTransform->Set_Scale({ originScale.x * -1,originScale.y,originScale.z });
            }


            FAILED_CHECK_MSG(
                layer->Add_GameObject(OBJTYPE::EXTRA, MenuExtra),
                L"CMainMenu::Ready_GameLogic_Layer() failed: CLayer::Add_GameObject(CMenuChar) failed");


        }
    }
    m_Layers.insert({ layerType, layer });          // scene -> map

    return S_OK;
}

HRESULT CMainMenu::Ready_UI_Layer(LAYERTYPE layerType)
{
    Engine::CLayer* layer = Engine::CLayer::Create();

    if (nullptr == layer)
        return E_FAIL;
    //////////////////////////////////////////////////////////////////////////////////
    //아이콘
    CUIIcon* pGameObject = CUIIcon::Create(m_GraphicDev, COMPONENTTYPE::TEX_UI_ICON);
    pGameObject->Set_TextureIndex(0);
    //pGameObject->Get_Transform()->Set_Scale(262.f / 1.3f,143.f / 1.3f,1.f);
    pGameObject->SetCloneTexAspectScale({ .2f, .2f, 1 });
    pGameObject->Get_Transform()->Set_Pos(-WINCX / 3, WINCY / 4.f, 1);
    // pGameObject = CUIIcon::Create(m_GraphicDev);

    FAILED_CHECK_MSG(
        layer->Add_GameObject(OBJTYPE::UI, pGameObject),
        L"CMainMenu::Ready_UI_Layer() failed: CLayer::Add_GameObject(CUIIcon) failed");
    ///////////////////////////////////////////////////////////////////////////////////
    float scaleX = 0.20f;
    float scaleY = 0.25f;
    // 버튼.
    bt = CUIButton::Create(m_GraphicDev, 1, 0);
    //bt->Set_TextureIndex(1,0);
    bt->SetCloneTexAspectScale({ scaleX, scaleY, 1 });
    bt->Get_Transform()->Set_Pos(-WINCX / 3, -0, .3);
    bt->SetOnClick([]() { CSceneMgr::GetInstance()->Change_SceneMgr(SCENETYPE::SC_STAGE); });
    FAILED_CHECK_MSG(
        layer->Add_GameObject(OBJTYPE::UI, bt),
        L"CMainMenu::Ready_UI_Layer() failed: CLayer::Add_GameObject(CUIIcon) failed");
    ///////////////////////////////////////////////////////////////////////////////////
    // 버튼
    bt1 = CUIButton::Create(m_GraphicDev, 1, 0);
    //bt1->Set_TextureIndex(1, 0);
    bt1->SetCloneTexAspectScale({ scaleX, scaleY, 1 });
    bt1->Get_Transform()->Set_Pos(-WINCX / 3, -WINCX / 18, .3);
    bt1->SetOnClick([]() { CSceneMgr::GetInstance()->Change_SceneMgr(SCENETYPE::SC_EDIT); });
    FAILED_CHECK_MSG(
        layer->Add_GameObject(OBJTYPE::UI, bt1),
        L"CMainMenu::Ready_UI_Layer() failed: CLayer::Add_GameObject(CUIIcon) failed");
    ///////////////////////////////////////////////////////////////////////////////////
    // 버튼
    bt2 = CUIButton::Create(m_GraphicDev, 1, 0);
    //bt2->Set_TextureIndex(1, 0);
    bt2->SetCloneTexAspectScale({ scaleX, scaleY, 1 });
    bt2->Get_Transform()->Set_Pos(-WINCX / 3, -WINCX / 9, .3);
    bt2->SetOnClick([]() { CSceneMgr::GetInstance()->Change_SceneMgr(SCENETYPE::SC_BOSS_STAGE1); });
    FAILED_CHECK_MSG(
        layer->Add_GameObject(OBJTYPE::UI, bt2),
        L"CMainMenu::Ready_UI_Layer() failed: CLayer::Add_GameObject(CUIIcon) failed");
    ///////////////////////////////////////////////////////////////////////////////////
    // 버튼
    bt3 = CUIButton::Create(m_GraphicDev, 1, 0);
    bt3->Set_TextureIndex(1, 0);
    bt3->SetCloneTexAspectScale({ scaleX, scaleY, 1 });
    bt3->Get_Transform()->Set_Pos(-WINCX / 3, -WINCX / 6, .3);
    bt3->SetOnClick([]() { DestroyWindow(g_hWnd); });
    FAILED_CHECK_MSG(
        layer->Add_GameObject(OBJTYPE::UI, bt3),
        L"CMainMenu::Ready_UI_Layer() failed: CLayer::Add_GameObject(CUIIcon) failed");

    m_Layers.insert({ layerType, layer });          // scene -> map

    return S_OK;
}

HRESULT CMainMenu::Ready_Prototype()
{
    return S_OK;
}

CMainMenu* CMainMenu::Create(DEVICE graphicDev)
{
    auto stage = new CMainMenu(graphicDev);

    if (FAILED(stage->Ready_Scene()))
    {
        MSG_BOX("Stage Create Failed");
        Safe_Release(stage);
        return nullptr;
    }

    return stage;
}

void CMainMenu::Free()
{
    CRenderer::GetInstance()->SetFog(false);

    CSoundMgr::GetInstance()->Stop_SoundChannel(SOUND_BGM);

    Engine::CScene::Free();
}
