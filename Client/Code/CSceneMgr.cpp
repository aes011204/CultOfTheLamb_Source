#include "pch.h"
#include "CSceneMgr.h"
#include "CEdit.h"
#include "CLogo.h"
#include "CLogo_Boss.h"
#include "CLogo_Boss2.h"
#include "CMainMenu.h"
#include "CManagement.h"
#include "CStage.h"
#include "Engine_Enum.h"
#include "CStage_Boss1.h"
#include "CStage_Boss2.h"
#include "CStage_Dungeon.h"
#include "CStage_TarotTest.h"
#include "CUI_Fade.h"
#include "CLightMgr.h"

IMPLEMENT_SINGLETON(CSceneMgr)

CSceneMgr::CSceneMgr()
    : m_GraphicDev(nullptr),
      m_SceneType(SCENETYPE::SC_END),
      m_FadeUI(nullptr),
      m_PendingScene(SCENETYPE::SC_END), m_FadeSpeed(0),
      m_IsChanging(false),
      m_FadeOutComplete(false)
{
}

CSceneMgr::~CSceneMgr()
{
	CSceneMgr::Free();
}

HRESULT CSceneMgr::Ready_SceneMgr(DEVICE pGraphicDev)
{
	m_GraphicDev = pGraphicDev;
	m_GraphicDev->AddRef();

	Engine::CScene* logo = CLogo::Create(pGraphicDev);

	if (nullptr == logo)
		return E_FAIL;

	if (FAILED(CManagement::GetInstance()->Set_Scene(logo)))
	{
		MSG_BOX("Logo Setting Failed");
		return E_FAIL;
	}

	return S_OK;
}

void CSceneMgr::Change_SceneMgr(SCENETYPE typeInput)
{
    // 이미 전환 중이면 무시
    if (m_IsChanging)
        return;

    if (m_FadeUI == nullptr)
    {
        m_FadeUI = CUI_Fade::Create(m_GraphicDev);
    }

    // Fade UI 생성 실패 시 바로 전환
    if (m_FadeUI == nullptr)
    {

        m_SceneType = typeInput;

        switch (m_SceneType)
        {
        case SCENETYPE::SC_MAINMENU:
            CManagement::GetInstance()->Request_ChangeScene(CMainMenu::Create(m_GraphicDev));
            break;

        case SCENETYPE::SC_STAGE:
            CManagement::GetInstance()->Request_ChangeScene(CStage::Create(m_GraphicDev));
            break;

        case SCENETYPE::SC_EDIT:
            CManagement::GetInstance()->Request_ChangeScene(CEdit::Create(m_GraphicDev));
            break;

        case SCENETYPE::SC_DUNGEON:
            CManagement::GetInstance()->Request_ChangeScene(CStage_Dungeon::Create(m_GraphicDev));
            break;

        case SCENETYPE::SC_TAROT_TEST:
            CManagement::GetInstance()->Request_ChangeScene(CStage_TarotTest::Create(m_GraphicDev));
            break;

        case SCENETYPE::SC_BOSS_STAGE1:
            CManagement::GetInstance()->Request_ChangeScene(CStage_Boss1::Create(m_GraphicDev));
            break;

        case SCENETYPE::SC_BOSS_STAGE2:
            CManagement::GetInstance()->Request_ChangeScene(CStage_Boss2::Create(m_GraphicDev));
            break;

        case SCENETYPE::SC_LOGO_BOSS1:
            CManagement::GetInstance()->Request_ChangeScene(CLogo_Boss::Create(m_GraphicDev));
            break;

        case SCENETYPE::SC_LOGO_BOSS2:
            CManagement::GetInstance()->Request_ChangeScene(CLogo_Boss2::Create(m_GraphicDev));
            break;

        default:
            break;
        }
        return;
    }

    // Fade 있으면 예약만 하고 FadeOut 시작
    m_PendingScene = typeInput;
    m_IsChanging = true;
    m_FadeOutComplete = false;
    m_FadeUI->Start_FadeOut(2.0f);
}

void CSceneMgr::Update_SceneMgr(const _float& timeDelta)
{
    if (!m_FadeUI || !m_IsChanging)
        return;

    m_FadeUI->Update_GameObject(timeDelta);


    if (!m_FadeOutComplete &&
        m_FadeUI->IsFadeComplete() &&
        m_FadeUI->Get_Alpha() >= 1.0f)
    {
        m_FadeOutComplete = true;
        m_SceneType = m_PendingScene;

        // 실제 씬 전환
        switch (m_SceneType)
        {
        case SCENETYPE::SC_MAINMENU:
            CManagement::GetInstance()->Request_ChangeScene(CMainMenu::Create(m_GraphicDev));
            break;

        case SCENETYPE::SC_STAGE:
            CManagement::GetInstance()->Request_ChangeScene(CStage::Create(m_GraphicDev));
            break;

        case SCENETYPE::SC_EDIT:
            CManagement::GetInstance()->Request_ChangeScene(CEdit::Create(m_GraphicDev));
            break;

        case SCENETYPE::SC_DUNGEON:
            CManagement::GetInstance()->Request_ChangeScene(CStage_Dungeon::Create(m_GraphicDev));
            break;

        case SCENETYPE::SC_TAROT_TEST:
            CManagement::GetInstance()->Request_ChangeScene(CStage_TarotTest::Create(m_GraphicDev));
            break;

        case SCENETYPE::SC_BOSS_STAGE1:
            CManagement::GetInstance()->Request_ChangeScene(CStage_Boss1::Create(m_GraphicDev));
            break;

        case SCENETYPE::SC_BOSS_STAGE2:
            CManagement::GetInstance()->Request_ChangeScene(CStage_Boss2::Create(m_GraphicDev));
            break;

        case SCENETYPE::SC_LOGO_BOSS1:
            CManagement::GetInstance()->Request_ChangeScene(CLogo_Boss::Create(m_GraphicDev));
            break;

        case SCENETYPE::SC_LOGO_BOSS2:
            CManagement::GetInstance()->Request_ChangeScene(CLogo_Boss2::Create(m_GraphicDev));
            break;

        default:
            break;
        }

        // FadeIn 시작
        if (m_SceneType == SCENETYPE::SC_LOGO_BOSS1 ||
            m_SceneType == SCENETYPE::SC_LOGO_BOSS2)
        {
            m_FadeUI->Start_FadeIn(50.0f);
        }
        else
        {
            m_FadeUI->Start_FadeIn(2.0f);
        }
    }

    // FadeIn 완료 체크
    if (m_FadeOutComplete &&
        m_FadeUI->IsFadeComplete() &&
        m_FadeUI->Get_Alpha() <= 0.0f)
    {
        m_IsChanging = false;
        m_FadeOutComplete = false;
    }
}

void CSceneMgr::Render_SceneMgr()
{
    if (m_FadeUI && m_IsChanging)
    {
        m_FadeUI->Render_GameObject();
    }
}

void CSceneMgr::StartFadeIn(_float speed)
{
    if (!m_FadeUI)
        m_FadeUI = CUI_Fade::Create(m_GraphicDev);

    if (m_FadeUI)
    {
        m_IsChanging = true;
        m_FadeUI->Start_FadeIn(speed);
    }
}

void CSceneMgr::StartFadeOut(_float speed)
{
    if (!m_FadeUI)
        m_FadeUI = CUI_Fade::Create(m_GraphicDev);

    if (m_FadeUI)
    {
        m_IsChanging = true;
        m_FadeUI->Start_FadeOut(speed);
    }
}

void CSceneMgr::Free()
{
    Safe_Release(m_FadeUI);
	Safe_Release(m_GraphicDev);
}

