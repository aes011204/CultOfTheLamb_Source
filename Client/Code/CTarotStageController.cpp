#include "pch.h"
#include "CTarotStageController.h"
#include "CUI_Dialogue.h"
#include <CManagement.h>
#include "CUITarotCard.h"
#include "CRenderer.h"
#include "CTransform.h"
#include "CUITarotDescCard.h"
#include "CPlayerSkill.h"
#include "CScene.h"
#include "CLayer.h"
#include "CSoundMgr.h"

class CPlayerSkill;

CTarotStageController::CTarotStageController(DEVICE graphicDev) : CGameObject(graphicDev)
{
}

CTarotStageController::CTarotStageController(const CTarotStageController& rhs) : CGameObject(rhs)
{
}

CTarotStageController::~CTarotStageController()
{
}

void CTarotStageController::Render_Editor()
{
    CGameObject::Render_Editor();
}
HRESULT CTarotStageController::Ready_GameObject()
{
    m_CurrentState = STAGESTATE::NONE;

    Create_Cards();

    m_Active = false;

    return S_OK;
}

void CTarotStageController::Initialize_GameObject()
{
    CGameObject::Initialize_GameObject();

    m_Dialogue = CUI_Dialogue::Create(m_GraphicDev);
    auto layer = CManagement::GetInstance()->Get_Scene()->Get_Layer(LAYERTYPE::UI);
    layer->Add_GameObject(OBJTYPE::UI, m_Dialogue);

    m_Dialogue->Set_OnDialogueEnd([this](const string& id)
        {
            if (id == "Tarot_Start")
            {
                m_CurrentState = STAGESTATE::CARD;
            }

            if (id == "Tarot_End")
            {
                m_Active = false;
                m_CurrentState = STAGESTATE::ENDING;
        
            }
        });
}

_int CTarotStageController::Update_GameObject(const _float& timeDelta)
{
    if (!m_Active)
        return 0;

    int exit = CGameObject::Update_GameObject(timeDelta);

    switch (m_CurrentState)
    {
    case STAGESTATE::NONE:
        m_CurrentState = STAGESTATE::DIALOGUE;
        break;
    case STAGESTATE::DIALOGUE:
        if (!m_DialogueStarted)
        {
            m_Dialogue->Start_Dialogue("Tarot_Start", _vec2(0.f, 200.f), _vec2(300.f, 80.f));

            m_DialogueStarted = true;
        }

        break;
    case STAGESTATE::CARD:
        if (!m_SoundEnded)
        {
            CSoundMgr::GetInstance()->Play_Once(L"ui/UI_TarotCardPull.wav",
                SOUNDTYPE::SOUND_UI,
                0.5f);
            m_SoundEnded = true;
        }
            m_ExplainCard1->Update_GameObject(timeDelta);
            m_ExplainCard2->Update_GameObject(timeDelta);
            m_TarotCard1->Update_GameObject(timeDelta);
            m_TarotCard2->Update_GameObject(timeDelta);
       
        //시작 애니메이션이 끝났는지 확인하고 끝났으면 다음 상태로 연결
        if (m_TarotCard1->Get_IsFinished() && m_TarotCard2->Get_IsFinished() && m_TarotCard1->Get_TarotState() == TAROTSTATE::START)
        {
            m_TarotCard1->Set_CurrentState(TAROTSTATE::CHOICE);
            m_TarotCard1->Set_PrevState(TAROTSTATE::START);
            m_TarotCard2->Set_CurrentState(TAROTSTATE::CHOICE);
            m_TarotCard2->Set_PrevState(TAROTSTATE::START);
            m_TarotCard1->Set_IsFinished(false);
            m_TarotCard2->Set_IsFinished(false);
            m_ExplainCard1->Set_Slide(true);
            m_ExplainCard2->Set_Slide(true);
            m_SoundEnded = false;
        }

        if (m_TarotCard1->Get_Selected() || m_TarotCard2->Get_Selected())
        {
         
            m_TarotCard1->Set_CurrentState(TAROTSTATE::RESULT);
            m_TarotCard1->Set_PrevState(TAROTSTATE::CHOICE);
            m_TarotCard2->Set_CurrentState(TAROTSTATE::RESULT);
            m_TarotCard2->Set_PrevState(TAROTSTATE::CHOICE);

            m_ExplainCard1->Set_RenderOff(true);
            m_ExplainCard2->Set_RenderOff(true);
            m_CurrentState = STAGESTATE::ENDING;
        }

        break;
    case STAGESTATE::ENDING:
        if (!m_DialogueEnded)
        {
        m_Dialogue->Start_Dialogue("Tarot_End", _vec2(0.f, 200.f), _vec2(300.f, 80.f));
        m_DialogueEnded = true;
        }
        break;


    }



    return exit;
}

void CTarotStageController::LateUpdate_GameObject(const _float& timeDelta)
{
    if (!m_Active)
        return;

    CGameObject::LateUpdate_GameObject(timeDelta);

    switch (m_CurrentState)
    {
    case STAGESTATE::CARD:

        m_ExplainCard1->LateUpdate_GameObject(timeDelta);
        m_ExplainCard2->LateUpdate_GameObject(timeDelta);
        m_TarotCard1->LateUpdate_GameObject(timeDelta);
        m_TarotCard2->LateUpdate_GameObject(timeDelta);
        break;

    case STAGESTATE::DIALOGUE:
        break;

    }
}


void CTarotStageController::Render_GameObject()
{
    //각 오브젝트 직접 렌더 그룹에 추가함
}

CTarotStageController* CTarotStageController::Create(DEVICE graphicDev)
{
    auto tarotStageController = new CTarotStageController(graphicDev);

    if (FAILED(tarotStageController->Ready_GameObject()))
    {
        Safe_Release(tarotStageController);
        MSG_BOX("CUI_TarotStageController Created Failed");
        return nullptr;
    }

    return tarotStageController;
}

void CTarotStageController::Create_Cards()
{

    //하트 설명 카드
    m_ExplainCard1 = CUITarotDescCard::Create(m_GraphicDev);
    m_ExplainCard1->Set_TextureIndex(4);
    m_ExplainCard1->Get_Transform()->Set_Scale(153.6f, 153.6f, 1.f);
    m_ExplainCard1->Get_Transform()->Set_Pos(-250.f, 0.f, 1.f);
    m_ExplainCard1->InitializeScale();

    //화살 설명 카드
    m_ExplainCard2 = CUITarotDescCard::Create(m_GraphicDev);
    m_ExplainCard2->Set_TextureIndex(5);
    m_ExplainCard2->Get_Transform()->Set_Scale(153.6f, 153.6f, 1.f);
    m_ExplainCard2->Get_Transform()->Set_Pos(250.f, 0.f, 1.f);
    m_ExplainCard2->InitializeScale();

    //하트 카드
    m_TarotCard1 = CUITarotCard::Create(m_GraphicDev);
    m_TarotCard1->Set_TarotType(TAROTTYPE::HEART);
    m_TarotCard1->Get_Transform()->Set_Scale(300.f, 500.f, 1.f);
    m_TarotCard1->Get_Transform()->Set_Pos(-250.f, 0.f, 1.f);
    m_TarotCard1->InitializeScale();
    //m_TarotCard1->SetOnClick([this](){})
    //TODO 유림 : 카드 뭐할지 결정하고 연결

    //화살 카드
    m_TarotCard2 = CUITarotCard::Create(m_GraphicDev);
    m_TarotCard2->Set_TarotType(TAROTTYPE::ARROW);
    m_TarotCard2->Get_Transform()->Set_Scale(300.f, 500.f, 1.f);
    m_TarotCard2->Get_Transform()->Set_Pos(250.f, 0.f, 1.f);
    m_TarotCard2->InitializeScale();
    CPlayerSkill* playerskill = dynamic_cast<CPlayerSkill*>(CManagement::GetInstance()->Get_Component(COMPONENTID::ID_STATIC, LAYERTYPE::GAMELOGIC, OBJTYPE::PLAYER, COMPONENTTYPE::PL_SKILL));
    m_TarotCard2->SetOnClick([this, playerskill]() {playerskill->Set_SkillUnlocked(true); playerskill->Set_CurrentCharges(3); });
}

void CTarotStageController::Free()
{
    Safe_Release(m_TarotCard1);
    Safe_Release(m_TarotCard2);
    Safe_Release(m_ExplainCard1);
    Safe_Release(m_ExplainCard2);

    CGameObject::Free();
}
