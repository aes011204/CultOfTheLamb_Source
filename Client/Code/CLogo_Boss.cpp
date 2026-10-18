#include "pch.h"
#include "CLogo_Boss.h"
#include "CDInputMgr.h"
#include "CEdit.h"
#include "CFontMgr.h"
#include "CLayer.h"
#include "CLoading.h"
#include "CManagement.h"
#include "CMonster.h"
#include "CProtoMgr.h"
#include "CTexture.h"
#include "CMainMenu.h"
#include "CSceneMgr.h"
#include "CStage_Boss1.h"
#include "CUI_LoadingCircle.h"

CLogo_Boss::CLogo_Boss(DEVICE graphicDev)
    : Engine::CScene(graphicDev),
      m_Loading(nullptr)
{ }

CLogo_Boss::~CLogo_Boss()
{ }

HRESULT CLogo_Boss::Ready_Scene()
{
    if (FAILED(Ready_Prototype()))
        return E_FAIL;

    if (FAILED(Ready_Environment_Layer(LAYERTYPE::ENVIRONMENT)))
        return E_FAIL;

    if (FAILED(Ready_UI_Layer(LAYERTYPE::UI)))
        return E_FAIL;

    m_Loading = CLoading::Create(m_GraphicDev, CLoading::LOADING_BOSS1);

    if (nullptr == m_Loading)
        return E_FAIL;

    return S_OK;
}

_int CLogo_Boss::Update_Scene(const _float& timeDelta)
{
    _int exit = Engine::CScene::Update_Scene(timeDelta);

    if (true == m_Loading->Get_Finish())
    {
        CSceneMgr::GetInstance()->Change_SceneMgr(SCENETYPE::SC_BOSS_STAGE1);
    }

    return exit;
}

void CLogo_Boss::LateUpdate_Scene(const _float& timeDelta)
{
    Engine::CScene::LateUpdate_Scene(timeDelta);
}

void CLogo_Boss::Render_Scene()
{
    _vec2 pos{ 281.8f, 602.8f };

    CFontMgr::GetInstance()->Render_Font(L"NanumSquare_Regular",
        m_Loading->Get_String(),
        &pos,
        D3DXCOLOR(1.f, 0.f, 0.f, 1.f));
}

HRESULT CLogo_Boss::Ready_Environment_Layer(LAYERTYPE layerType)
{
    auto layer = Engine::CLayer::Create();

    NULL_CHECK_RETURN(layer, E_FAIL)

    Engine::CGameObject* gameObject = nullptr;

    m_Layers.insert({ layerType, layer });

    return S_OK;
}

HRESULT CLogo_Boss::Ready_UI_Layer(LAYERTYPE layerType)
{
    auto layer = Engine::CLayer::Create();
    NULL_CHECK_RETURN(layer, E_FAIL);

    CUI_LoadingCircle* loadingCircle = CUI_LoadingCircle::Create(m_GraphicDev);
    NULL_CHECK_RETURN(loadingCircle, E_FAIL);

    layer->Add_GameObject(OBJTYPE::UI, loadingCircle);

    m_Layers.insert({ layerType, layer });

    return S_OK;
}

HRESULT CLogo_Boss::Ready_Prototype()
{

    return S_OK;
}

CLogo_Boss* CLogo_Boss::Create(DEVICE graphicDev)
{
    auto logo = new CLogo_Boss(graphicDev);

    if (FAILED(logo->Ready_Scene()))
    {
        MSG_BOX("Logo Create Failed");
        Safe_Release(logo);
        return nullptr;
    }

    return logo;
}

void CLogo_Boss::Free()
{
    Safe_Release(m_Loading);

    Engine::CScene::Free();
}
