#include "pch.h"
#include "CLogo_Boss2.h"
#include "CDInputMgr.h"
#include "CEdit.h"
#include "CFontMgr.h"
#include "CLayer.h"
#include "CLoading.h"
#include "CSceneMgr.h"
#include "CUI_LoadingCircle.h"
#include "CRenderer.h"

CLogo_Boss2::CLogo_Boss2(DEVICE graphicDev)
    : Engine::CScene(graphicDev),
      m_Loading(nullptr)
{ }

CLogo_Boss2::~CLogo_Boss2()
{ }

HRESULT CLogo_Boss2::Ready_Scene()
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

void CLogo_Boss2::Initialize_Scene()
{
    CScene::Initialize_Scene();

}

_int CLogo_Boss2::Update_Scene(const _float& timeDelta)
{
    _int exit = Engine::CScene::Update_Scene(timeDelta);

    if (true == m_Loading->Get_Finish())
    {
        CSceneMgr::GetInstance()->Change_SceneMgr(SCENETYPE::SC_BOSS_STAGE2);
    }

    return exit;
}

void CLogo_Boss2::LateUpdate_Scene(const _float& timeDelta)
{
    Engine::CScene::LateUpdate_Scene(timeDelta);
}

void CLogo_Boss2::Render_Scene()
{
    _vec2 pos{ 281.8f, 602.8f };

    CFontMgr::GetInstance()->Render_Font(L"NanumSquare_Regular",
        m_Loading->Get_String(),
        &pos,
        D3DXCOLOR(1.f, 0.f, 0.f, 1.f));
}

HRESULT CLogo_Boss2::Ready_Environment_Layer(LAYERTYPE layerType)
{
    auto layer = Engine::CLayer::Create();

    NULL_CHECK_RETURN(layer, E_FAIL)

    Engine::CGameObject* gameObject = nullptr;

    m_Layers.insert({ layerType, layer });

    return S_OK;
}

HRESULT CLogo_Boss2::Ready_UI_Layer(LAYERTYPE layerType)
{
    auto layer = Engine::CLayer::Create();
    NULL_CHECK_RETURN(layer, E_FAIL);

    CUI_LoadingCircle* loadingCircle = CUI_LoadingCircle::Create(m_GraphicDev);
    NULL_CHECK_RETURN(loadingCircle, E_FAIL);

    layer->Add_GameObject(OBJTYPE::UI, loadingCircle);

    m_Layers.insert({ layerType, layer });

    return S_OK;
}

HRESULT CLogo_Boss2::Ready_Prototype()
{
    

    return S_OK;
}

CLogo_Boss2* CLogo_Boss2::Create(DEVICE graphicDev)
{
    auto logo = new CLogo_Boss2(graphicDev);

    if (FAILED(logo->Ready_Scene()))
    {
        MSG_BOX("Logo Create Failed");
        Safe_Release(logo);
        return nullptr;
    }

    return logo;
}

void CLogo_Boss2::Free()
{
    Safe_Release(m_Loading);

    Engine::CScene::Free();
}
