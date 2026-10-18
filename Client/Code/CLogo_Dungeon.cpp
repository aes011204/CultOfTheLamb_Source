#include "pch.h"
#include "CLogo_Dungeon.h"
#include "CDInputMgr.h"
#include "CEdit.h"
#include "CFontMgr.h"
#include "CLayer.h"
#include "CLoading.h"
#include "CSceneMgr.h"
#include "CUI_LoadingCircle.h"
#include "CRenderer.h"

CLogo_Dungeon::CLogo_Dungeon(DEVICE graphicDev)
    : Engine::CScene(graphicDev),
      m_Loading(nullptr)
{ }

CLogo_Dungeon::~CLogo_Dungeon()
{ }

HRESULT CLogo_Dungeon::Ready_Scene()
{
    if (FAILED(Ready_Prototype()))
        return E_FAIL;

    if (FAILED(Ready_Environment_Layer(LAYERTYPE::ENVIRONMENT)))
        return E_FAIL;

    if (FAILED(Ready_UI_Layer(LAYERTYPE::UI)))
        return E_FAIL;

    CRenderer::GetInstance()->SetFog(false);

    m_Loading = CLoading::Create(m_GraphicDev, CLoading::LOADING_BOSS2);

    if (nullptr == m_Loading)
        return E_FAIL;

    return S_OK;
}

void CLogo_Dungeon::Initialize_Scene()
{
    CScene::Initialize_Scene();

}

_int CLogo_Dungeon::Update_Scene(const _float& timeDelta)
{
    _int exit = Engine::CScene::Update_Scene(timeDelta);

    if (true == m_Loading->Get_Finish())
    {
        CSceneMgr::GetInstance()->Change_SceneMgr(SCENETYPE::SC_BOSS_STAGE2);
    }

    return exit;
}

void CLogo_Dungeon::LateUpdate_Scene(const _float& timeDelta)
{
    Engine::CScene::LateUpdate_Scene(timeDelta);
}

void CLogo_Dungeon::Render_Scene()
{
    _vec2 pos{ 281.8f, 602.8f };

    CFontMgr::GetInstance()->Render_Font(L"NanumSquare_Regular",
        m_Loading->Get_String(),
        &pos,
        D3DXCOLOR(1.f, 0.f, 0.f, 1.f));
}

HRESULT CLogo_Dungeon::Ready_Environment_Layer(LAYERTYPE layerType)
{
    auto layer = Engine::CLayer::Create();

    NULL_CHECK_RETURN(layer, E_FAIL)

    Engine::CGameObject* gameObject = nullptr;

    m_Layers.insert({ layerType, layer });

    return S_OK;
}

HRESULT CLogo_Dungeon::Ready_UI_Layer(LAYERTYPE layerType)
{
    auto layer = Engine::CLayer::Create();
    NULL_CHECK_RETURN(layer, E_FAIL);

    CUI_LoadingCircle* loadingCircle = CUI_LoadingCircle::Create(m_GraphicDev);
    NULL_CHECK_RETURN(loadingCircle, E_FAIL);

    layer->Add_GameObject(OBJTYPE::UI, loadingCircle);

    m_Layers.insert({ layerType, layer });

    return S_OK;
}

HRESULT CLogo_Dungeon::Ready_Prototype()
{
    

    return S_OK;
}

CLogo_Dungeon* CLogo_Dungeon::Create(DEVICE graphicDev)
{
    auto logo = new CLogo_Dungeon(graphicDev);

    if (FAILED(logo->Ready_Scene()))
    {
        MSG_BOX("Logo Create Failed");
        Safe_Release(logo);
        return nullptr;
    }

    return logo;
}

void CLogo_Dungeon::Free()
{
    Safe_Release(m_Loading);

    Engine::CScene::Free();
}
