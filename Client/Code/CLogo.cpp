#include "pch.h"
#include "CLogo.h"
#include "CDInputMgr.h"
#include "CEdit.h"
#include "CFontMgr.h"
#include "CLayer.h"
#include "CLoading.h"
#include "CMonster.h"
#include "CProtoMgr.h"
#include "CRcTex.h"
#include "CTexture.h"
#include "CSceneMgr.h"
#include "CUI_LoadingCircle.h"
#include "CTransform.h"
#include "CAnimator.h"

CLogo::CLogo(DEVICE graphicDev)
	: Engine::CScene(graphicDev),
	m_Loading(nullptr)
{
}

CLogo::~CLogo()
{
}

HRESULT CLogo::Ready_Scene()
{
	if (FAILED(Ready_Prototype()))
		return E_FAIL;

	if (FAILED(Ready_Environment_Layer(LAYERTYPE::ENVIRONMENT)))
		return E_FAIL;

	if (FAILED(Ready_UI_Layer(LAYERTYPE::UI)))
		return E_FAIL;

	m_Loading = CLoading::Create(m_GraphicDev, CLoading::LOADING_STAGE);

	if (nullptr == m_Loading)
		return E_FAIL;

	return S_OK;
}

_int CLogo::Update_Scene(const _float& timeDelta)
{
	_int exit = Engine::CScene::Update_Scene(timeDelta);

    static bool bLoadingComplete = false;
    if (!bLoadingComplete && m_Loading->Get_Finish())
    {
        bLoadingComplete = true;
        CSceneMgr::GetInstance()->Change_SceneMgr(SCENETYPE::SC_MAINMENU);
    }

	return exit;
}

void CLogo::LateUpdate_Scene(const _float& timeDelta)
{
	Engine::CScene::LateUpdate_Scene(timeDelta);
}

void CLogo::Render_Scene()
{
	// debug 용
	_vec2 pos{ 281.8f, 602.8f };

	CFontMgr::GetInstance()->Render_Font(L"NanumSquare_Regular",
		m_Loading->Get_String(),
		&pos,
		D3DXCOLOR(1.f, 0.f, 0.f, 1.f));
}

HRESULT CLogo::Ready_Environment_Layer(LAYERTYPE layerType)
{
	auto layer = Engine::CLayer::Create();
	NULL_CHECK_RETURN(layer, E_FAIL)

	Engine::CGameObject* gameObject = nullptr;

	m_Layers.insert({ layerType, layer });

	return S_OK;
}

HRESULT CLogo::Ready_UI_Layer(LAYERTYPE layerType)
{
	auto layer = Engine::CLayer::Create();
    NULL_CHECK_RETURN(layer, E_FAIL);

    CUI_LoadingCircle* loadingCircle = CUI_LoadingCircle::Create(m_GraphicDev);
    NULL_CHECK_RETURN(loadingCircle, E_FAIL);

    layer->Add_GameObject(OBJTYPE::UI, loadingCircle);

	m_Layers.insert({ layerType, layer });

	return S_OK;
}

HRESULT CLogo::Ready_Prototype()
{
	// 로고 내 사용하는 프로토타입은 여기서만 추가

	// Buffer : Rect UV
	if (FAILED(CProtoMgr::GetInstance()
		->Ready_Prototype(COMPONENTTYPE::RC_TEX, Engine::CRcTex::Create(m_GraphicDev))))
		return E_FAIL;

    // Transform
    if (FAILED(CProtoMgr::GetInstance()
        ->Ready_Prototype(
        COMPONENTTYPE::TRANSFORM, Engine::CTransform::Create(m_GraphicDev))))
        return E_FAIL;

    // Animator
    if (FAILED(CProtoMgr::GetInstance()
        ->Ready_Prototype(
        COMPONENTTYPE::ANIMATOR, CAnimator::Create(m_GraphicDev))))
        return E_FAIL;

    CTexture* loadingCircle = CTexture::Create(m_GraphicDev, TEX_NORMAL, L"", 0);
    NULL_CHECK_RETURN(loadingCircle, E_FAIL);

    if (FAILED(loadingCircle->Add_Texture(L"Loading_Circle", TEX_NORMAL,
        L"../Bin/Resource/Texture/Logo/Loading/Loading%d.png", 58)))
        return E_FAIL;

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(COMPONENTTYPE::TEX_LOADING_CIRCLE, loadingCircle)))
        return E_FAIL;

	return S_OK;
}

CLogo* CLogo::Create(DEVICE graphicDev)
{
	auto logo = new CLogo(graphicDev);

	if (FAILED(logo->Ready_Scene()))
	{
		MSG_BOX("Logo Create Failed");
		Safe_Release(logo);
		return nullptr;
	}

	return logo;
}

void CLogo::Free()
{
	Safe_Release(m_Loading);

	Engine::CScene::Free();
}
