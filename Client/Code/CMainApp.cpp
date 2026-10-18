#include "pch.h"
#include "CMainApp.h"

#include "CCollisionManager.h"
#include "CConstructionMgr.h"
#include "CDialogueDB.h"
#include "CDInputMgr.h"
#include "CDropSystem.h"
#include "CEditContext.h"
#include "CFontMgr.h"
#include "CFrameMgr.h"
#include "CGraphicDev.h"
#include "CImGuiManager.h"
#include "CItemDB.h"
#include "CLightMgr.h"
#include "CMainEditorMgr.h"
#include "CManagement.h"
#include "CProtoMgr.h"
#include "CRandomMgr.h"
#include "CRecipeDB.h"
#include "CRenderer.h"
#include "CResourceMgr.h"
#include "CSceneMgr.h"
#include "CSoundMgr.h"
#include "CTexture.h"
#include "CTimerMgr.h"
#include "CVillageStateMgr.h"

CMainApp::CMainApp()
    : m_ManagementClass(CManagement::GetInstance()),
      m_DeviceClass(nullptr),
      m_GraphicDev(nullptr)
{ }

CMainApp::~CMainApp()
{ }

HRESULT CMainApp::Ready_MainApp()
{
    if (FAILED(Ready_DefaultSetting(&m_GraphicDev)))
        return E_FAIL;

#pragma region 데이터 파싱

    if (FAILED(CItemDB::GetInstance()->LoadFromJson("ItemData.txt")))
        return E_FAIL;

    if (FAILED(CRecipeDB::GetInstance()->LoadFromJson("RecipeData.txt")))
        return E_FAIL;

    if (FAILED(CDialogueDB::GetInstance()->LoadFromJson("Dialogue.json")))
        return E_FAIL;

#pragma endregion

    if (FAILED(Ready_Scene(m_GraphicDev)))
        return E_FAIL;

    // ImGui
    {
        CImGuiManager::GetInstance()->InitImGui(g_hWnd, m_GraphicDev);

        CEditContext* ctx = CEditContext::Create();
        CMainEditorMgr::GetInstance()->Ready_MainEditor(m_GraphicDev, ctx);
    }

    // SoundMgr Init
    CSoundMgr::GetInstance()->Ready_SoundMgr();

    // ConstructionMgr Init
    CConstructionMgr::GetInstance()->Ready_ConstructionMgr(m_GraphicDev);

    // 전역 타이머 초기화
    ZeroMemory(g_Timers, sizeof(g_Timers));

    return S_OK;
}

int CMainApp::Update_MainApp(const float& timeDelta)
{
    m_ManagementClass->Commit_ChangeScene();
    m_ManagementClass->Update_Scene(timeDelta);

    CDInputMgr::GetInstance()->Update_InputDev();
    CCollisionManager::GetInstance()->Update();
    CSoundMgr::GetInstance()->Update(timeDelta);
    CSceneMgr::GetInstance()->Update_SceneMgr(timeDelta);

    // Imgui
    {
        CImGuiManager::GetInstance()->Update();

        CMainEditorMgr::GetInstance()->Update_MainEditor(timeDelta);
    }

    return 0;
}

void CMainApp::LateUpdate_MainApp(const float& timeDelta)
{
    m_ManagementClass->LateUpdate_Scene(timeDelta);
}

void CMainApp::Render_MainApp()
{
    m_DeviceClass->Render_Begin(D3DXCOLOR(0.f, 0.f, 0.f, 1.f));

    m_ManagementClass->Render_Scene(m_GraphicDev);
    CMainEditorMgr::GetInstance()->Render_MainEditor();
    CSceneMgr::GetInstance()->Render_SceneMgr();

    // ImGui
    {
        CImGuiManager::GetInstance()->Render();
    }

    m_DeviceClass->Render_End();
}

HRESULT CMainApp::Ready_DefaultSetting(DEVICE* graphicDev)
{
    if (FAILED(CGraphicDev::GetInstance()
        ->Ready_GraphicDev(g_hWnd, MODE_WIN, WINCX, WINCY, &m_DeviceClass)))
        return E_FAIL;

    m_DeviceClass->AddRef();

    (*graphicDev) = m_DeviceClass->Get_GraphicDev();

    (*graphicDev)->AddRef();

    CTexture::InitTextureSystem();

    // 조명 연산 끄기 옵션. 나중에는 조명을 끄는 행위는 하지 말자 (온오프 자주 하면 부하↑)
    (*graphicDev)->SetRenderState(D3DRS_LIGHTING, FALSE);
    // D3DRS_ZENABLE : Z버퍼에 Z값을 기록, Z 정렬 수행 여부를 묻는 옵션 : On
    (*graphicDev)->SetRenderState(D3DRS_ZENABLE, TRUE);
    // D3DRS_ZWRITEENABLE : Z버퍼에 Z값을 기록할 지 말 지 여부를 묻는 옵션 : On
    (*graphicDev)->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
    // 텍스처가 너무 깨지지 않도록 선형 보간 필터링 적용
    (*graphicDev)->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    (*graphicDev)->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

    // Fonts
    if (FAILED(CFontMgr::GetInstance()->Ready_Font(
        *graphicDev, L"견명조", L"견명조", 20, 20, FW_HEAVY)))
        return E_FAIL;

    if (FAILED(CFontMgr::GetInstance()->Ready_Font(
        *graphicDev, L"궁서", L"궁서", 30, 30, FW_THIN)))
        return E_FAIL;

    //AddFontResourceEx(L"../Bin/Resource/Font/LaptureDisplay-Regular.otf", FR_PRIVATE, NULL);
    if (FAILED(CFontMgr::GetInstance()->Ready_Font(
        *graphicDev, L"LaptureDisplay-Regular", L"LaptureDisplay-Regular", 10, 24, FW_HEAVY)))
        return E_FAIL;


    if (FAILED(CFontMgr::GetInstance()->Ready_Font(
        *graphicDev, L"NanumSquare_Max", L"나눔스퀘어 네오 Heavy", 100, 110, FW_HEAVY)))
        return E_FAIL;


    //아이템 숫자 출력용 폰트

    if (FAILED(CFontMgr::GetInstance()->Ready_Font(
        *graphicDev, L"Ghana_Heavy", L"가나초콜릿", 33, 37, FW_HEAVY)))
        return E_FAIL;

    if (FAILED(CFontMgr::GetInstance()->Ready_Font(
        *graphicDev, L"NanumSquare_Heavy", L"나눔스퀘어 네오 Heavy", 33, 35, FW_HEAVY)))
        return E_FAIL;

    if (FAILED(CFontMgr::GetInstance()->Ready_Font(
        *graphicDev, L"NanumSquare_Heavy2", L"나눔스퀘어 네오 Heavy", 45, 48, FW_HEAVY)))
        return E_FAIL;

    if (FAILED(CFontMgr::GetInstance()->Ready_Font(
        *graphicDev, L"NanumSquare_Big", L"나눔스퀘어 네오 Regular", 30, 33, FW_MEDIUM)))
        return E_FAIL;

    if (FAILED(CFontMgr::GetInstance()->Ready_Font(
        *graphicDev, L"NanumSquare_Large", L"나눔스퀘어 네오 Regular", 25, 27, FW_MEDIUM)))

        return E_FAIL;
    //아이템 숫자 출력용 폰트
    if (FAILED(CFontMgr::GetInstance()->Ready_Font(
        *graphicDev, L"NanumSquare_Number", L"나눔스퀘어 네오 Heavy", 18, 20, FW_HEAVY)))
        return E_FAIL;

    //스킬 남은 횟수 출력용 폰트
    if (FAILED(CFontMgr::GetInstance()->Ready_Font(
        *graphicDev, L"NanumSquare_Skill", L"나눔스퀘어 네오 Heavy", 25, 25, FW_HEAVY)))
        return E_FAIL;

    //중간 크기 폰트
    if (FAILED(CFontMgr::GetInstance()->Ready_Font(
        *graphicDev, L"NanumSquare_Regular", L"나눔스퀘어 네오 Regular", 18, 21, FW_MEDIUM)))
        return E_FAIL;

    //작은 크기 폰트
    if (FAILED(CFontMgr::GetInstance()->Ready_Font(
        *graphicDev, L"NanumSquare_Small", L"나눔스퀘어 네오 Regular", 15, 18, FW_MEDIUM)))
        return E_FAIL;

    //아이템 카드 설명용 폰트
    if (FAILED(CFontMgr::GetInstance()->Ready_Font(
        *graphicDev, L"카드 설명", L"나눔스퀘어 네오 Regular", 16, 19, FW_MEDIUM)))
        return E_FAIL;

    //아이템 카드 부연 설명용 폰트
    if (FAILED(CFontMgr::GetInstance()->Ready_Font(
        *graphicDev, L"부연 설명", L"나눔스퀘어 네오 Regular", 12, 14, FW_MEDIUM)))
        return E_FAIL;

    //아이템 카드 이름 출력용 폰트
    if (FAILED(CFontMgr::GetInstance()->Ready_Font(
        *graphicDev, L"아이템 이름", L"나눔스퀘어 네오 Regular", 20, 24, FW_SEMIBOLD)))
        return E_FAIL;

    // DInput
    if (FAILED(CDInputMgr::GetInstance()->Ready_InputDev(g_hInst, g_hWnd)))
        return E_FAIL;

    return S_OK;
}

HRESULT CMainApp::Ready_Scene(DEVICE graphicDev)
{
    return CSceneMgr::GetInstance()->Ready_SceneMgr(graphicDev);
}

CMainApp* CMainApp::Create()
{
    auto mainApp = new CMainApp;

    if (FAILED(mainApp->Ready_MainApp()))
    {
        MSG_BOX("CMainApp Create Failed");
        Safe_Release(mainApp);
        return nullptr;
    }

    return mainApp;
}

void CMainApp::Free()
{
    { // 순서 중요
        CSceneMgr::DestroyInstance();
        CManagement::DestroyInstance();
        CRenderer::DestroyInstance();
    }

    CVillageStateMgr::DestroyInstance();
    CDialogueDB::DestroyInstance();
    CConstructionMgr::DestroyInstance();
    CDropSystem::DestroyInstance();
    CSoundMgr::DestroyInstance();
    CCollisionManager::DestroyInstance();
    CLightMgr::DestroyInstance();
    CDInputMgr::DestroyInstance();
    CRandomMgr::DestroyInstance();
    CItemDB::DestroyInstance();
    CRecipeDB::DestroyInstance();

    CMainEditorMgr::DestroyInstance();
    CImGuiManager::DestroyInstance();

    CProtoMgr::DestroyInstance();

    CResourceMgr::DestroyInstance();
    CFrameMgr::DestroyInstance();
    CTimerMgr::DestroyInstance();

    CTexture::CleanupTextureSystem();

    CFontMgr::DestroyInstance();

    CGraphicDev::DestroyInstance();

    Safe_Release(m_DeviceClass);
    Safe_Release(m_GraphicDev);
}
