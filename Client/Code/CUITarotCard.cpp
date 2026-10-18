#include "pch.h"
#include "CUITarotCard.h"
#include "CRenderer.h"
#include "CDInputMgr.h"
#include "CRcTex.h"
#include "CTransform.h"
#include "CTexture.h"
#include "CCreateHelper.h"
#include "CAnimator.h"
#include "CSpriteEff.h"
#include "CSoundMgr.h"

int CUITarotCard::m_NextID = 0;

CUITarotCard::CUITarotCard(DEVICE graphicDev)
    : CUI(graphicDev),
      m_TextureIndex(0),
      m_AnimatorCom(nullptr),
      m_CurrentState(TAROTSTATE::COUNT),
      m_PrevState(TAROTSTATE::COUNT),
      m_IsFinished(false),
      m_Hovered(false),
      m_Selected(false),
        m_RenderOff(false)
{
    m_ID = ++m_NextID;
}

CUITarotCard::CUITarotCard(const CUITarotCard& rhs)
    : CUI(rhs),
    m_TextureIndex(0),
    m_AnimatorCom(nullptr),
    m_CurrentState(TAROTSTATE::COUNT),
    m_PrevState(TAROTSTATE::COUNT),
    m_IsFinished(false),
    m_Hovered(false),
    m_Selected(false),
    m_RenderOff(false)
{
    m_ID = ++m_NextID;
}
CUITarotCard::~CUITarotCard()
{
    
}

HRESULT CUITarotCard::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    Animation_Setting();

    m_CurrentType = TAROTTYPE::HEART;
	m_PrevState = TAROTSTATE::COUNT;
    m_CurrentState = TAROTSTATE::START;

    return S_OK;
}

_int CUITarotCard::Update_GameObject(const _float& timeDelta)
{
   CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

    _int exit = CUI::Update_GameObject(timeDelta);

    if (m_FadeOut)
    {
        m_Alpha -= m_FadeSpeed * timeDelta;

        if (m_Alpha <= 0.f)
        {
            m_Alpha = 0.f;
            m_FadeOut = false;
            m_RenderOff = true;     // 완전 투명해지면 렌더 끄기(끝)
        }

    }

    if(m_CurrentState== TAROTSTATE::CHOICE)
    {
        
        _vec3 pos = m_TransformCom->Get_Pos();
        _vec3 size = m_TransformCom->Get_Scale();

        float screenX = WINCX * 0.5 + pos.x;
        float screenY = WINCY * 0.5 - pos.y;

        //상수는 텍스처 크기에 따른 보정값

        m_Rect.left = (LONG)(screenX - size.x*0.3);
        m_Rect.top = (LONG)(screenY - size.y*0.3);
        m_Rect.right = (LONG)(screenX + size.x*0.3);
        m_Rect.bottom = (LONG)(screenY + size.y*0.3);

        POINT pt;
        GetCursorPos(&pt);
        ScreenToClient(CDInputMgr::GetInstance()->Get_HWND(), &pt);

        bool hoveredNow = PtInRect(&m_Rect, pt) != 0;

        if(hoveredNow != m_Hovered)
        {
            m_Hovered = hoveredNow;

            if (m_Hovered) OnHoverStart();
            else           OnHoverEnd();
        }

        if (m_Hovered &&
            (CDInputMgr::GetInstance()->Get_DIMouseState(MOUSEKEYSTATE::DIM_LB) & 0x80))
        {
            if(!m_Selected)
            {
                m_Selected = true;

                if (m_OnClick)
                {
                    CSoundMgr::GetInstance()->Play_Once(L"UI/UI_Tarot_Close.wav",
                        SOUND_EFFECT,
                        1.f);
                    m_OnClick();
                }
            }
        }

    }
    //TODO 유림 : 상태 변화에 따른 카드 애니메이션 변화

    switch(m_CurrentType)
    {
        case TAROTTYPE::HEART:
            
                if (m_CurrentState != m_PrevState)
                {
                    switch (m_CurrentState)
                    {
                    case TAROTSTATE::START:   m_AnimatorCom->Play_Animation(L"Tarot_Heart_Start", ANIMSTATE::ONCE, true); break;
                    case TAROTSTATE::CHOICE:  break;//m_AnimatorCom->Play_Animation(L"Tarot_Heart_Choice", ANIMSTATE::ONCE, true); break;
                    case TAROTSTATE::RESULT:
                        if (!m_FadeOut)
                        {
                            m_FadeOut = true;
                            m_Alpha = 1.f;
                        }
                                            //m_RenderOff = true;
                        break;
                    //case TAROTSTATE::EXIT:    m_AnimatorCom->Play_Animation(ExitKey, ANIMSTATE::ONCE, true); break;
                    }

                    m_PrevState = m_CurrentState;
                }
            
            break;

        case TAROTTYPE::ARROW:

            if (m_CurrentState != m_PrevState)
            {
                switch (m_CurrentState)
                {
                case TAROTSTATE::START:   m_AnimatorCom->Play_Animation(L"Tarot_Arrow_Start", ANIMSTATE::ONCE, true); break;
                case TAROTSTATE::CHOICE:  break; //m_AnimatorCom->Play_Animation(L"Tarot_Arrow_Choice", ANIMSTATE::ONCE, true); break;
                case TAROTSTATE::RESULT:
                    if (!m_FadeOut)
                    {
                        m_FadeOut = true;
                        m_Alpha = 1.f;
                    }//m_RenderOff = true;
                    break;
                //case TAROTSTATE::EXIT:    m_AnimatorCom->Play_Animation(ExitKey, ANIMSTATE::ONCE, true); break;
                }

                m_PrevState = m_CurrentState;
            }
            break;
        }

        if (m_CurrentState != m_PrevState)
        {
            m_AnimatorCom->Play_Animation(m_AnimationType, ANIMSTATE::ONCE, FALSE);
        }

        //애니메이션 종료 체크 -> 애니메이션 전환은 Controller에서 담당
        if(m_AnimatorCom->Get_CurFrame()>=m_AnimatorCom->Get_MaxFrame()-1)
        {
            m_IsFinished = true;
        }

        m_TransformCom->Set_Scale(m_TargetScale);

        return exit;


}

void CUITarotCard::LateUpdate_GameObject(const _float& timeDelta)
{
    CUI::LateUpdate_GameObject(timeDelta);
}

void CUITarotCard::Render_GameObject()
{
    CUI::Render_GameObject();

    //Render_Editor();

    if (!m_RenderOff)
    {
        m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());

        if (m_FadeOut)           
            m_SpriteEffCom->AlphaRenderStart(m_Alpha);

        if (m_TextureCom && m_AnimatorCom)
        {
            const wstring& key = m_AnimatorCom->Get_CurKey();
            _int           frame = m_AnimatorCom->Get_CurFrame();

            m_TextureCom->Set_Texture(key, frame);
        }
     
        m_BufferCom->Render_Buffer();

        if(m_FadeOut)
        m_SpriteEffCom->AlphaRenderEnd();
    }
   
}

HRESULT CUITarotCard::Add_Component()
{
    // buffer
    m_BufferCom = CreateProtoComponent<CRcTex>(this, COMPONENTTYPE::RC_TEX);
    NULL_CHECK_RETURN(m_BufferCom, E_FAIL);

    m_Components[ID_STATIC].insert({ COMPONENTTYPE::RC_TEX, m_BufferCom });

    //texture
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_UI_TAROT);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);

    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_UI_TAROT, m_TextureCom });

    // transform
    m_TransformCom = CreateProtoComponent<CTransform>(this, COMPONENTTYPE::TRANSFORM);
    NULL_CHECK_RETURN(m_TransformCom, E_FAIL);

    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::TRANSFORM, m_TransformCom });

    // animatior
    m_AnimatorCom = CreateProtoComponent<CAnimator>(this, COMPONENTTYPE::ANIMATOR);
    NULL_CHECK_RETURN(m_AnimatorCom, E_FAIL);

    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::ANIMATOR, m_AnimatorCom });

    m_AnimatorCom->Set_TextureType(COMPONENTTYPE::TEX_UI_TAROT);

    m_SpriteEffCom = CreateProtoComponent<CSpriteEff>(this, COMPONENTTYPE::SPRITEEFFECT);
    NULL_CHECK_RETURN(m_SpriteEffCom, E_FAIL);

    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::SPRITEEFFECT, m_SpriteEffCom });

    return S_OK;
}

void CUITarotCard::Animation_Setting()
{
    m_AnimatorCom->Create_Animation(L"Tarot_Heart_Start", 64, 0.02f);
    m_AnimatorCom->Create_Animation(L"Tarot_Arrow_Start", 64, 0.02f);
    m_AnimatorCom->Create_Animation(L"Tarot_Heart_Choice", 1, 0.02f);
    m_AnimatorCom->Create_Animation(L"Tarot_Arrow_Choice", 1, 0.02f);
}
void CUITarotCard::InitializeScale()
{
    m_BaseScale = m_TransformCom->Get_Scale();
    m_TargetScale = m_BaseScale;

}
void CUITarotCard::OnHoverStart()
{
	CSoundMgr::GetInstance()->Play_Once(L"ui/ui_pointeron.wav",
		SOUNDTYPE::SOUND_UI,
		5.f);
    m_TargetScale = m_BaseScale * 1.15;
}

void CUITarotCard::OnHoverEnd()
{
    m_TargetScale = m_BaseScale;
}

CUITarotCard* CUITarotCard::Create(DEVICE graphicDev)
{
    auto tarotCard = new CUITarotCard(graphicDev);
    if (FAILED(tarotCard->Ready_GameObject()))
    {
        Safe_Release(tarotCard);
        MSG_BOX("CUITarotCard Created Failed");
        return nullptr;
    }

    return tarotCard;
}

void CUITarotCard::Free()
{
    CUI::Free();
}

void CUITarotCard::Render_Editor()
{
	ImGui::Text("=== TarotCard ===");
	//
// 상태 디버그 정보
//
	ImGui::Separator();
	ImGui::Text("State Debug Info");

	ImGui::Text("Current State: %d", static_cast<int>(m_CurrentState));
	ImGui::Text("Prev State:    %d", static_cast<int>(m_PrevState));
    ImGui::Text("Hover State:    %d", static_cast<int>(m_Hovered));
    ImGui::Text("Prev State:    %d", static_cast<int>(m_PrevState));
    //
	// 애니메이션 디버그 정보
	//
	if (m_AnimatorCom)
	{
		const wstring& key = m_AnimatorCom->Get_CurKey();
		_int curFrame = m_AnimatorCom->Get_CurFrame();
		_int maxFrame = m_AnimatorCom->Get_MaxFrame();

		// wstring → char 변환 필요
		std::string animKey = std::string(key.begin(), key.end());
		ImGui::Text("Animation Key: %s", animKey.c_str());

		ImGui::Text("Animation Key: %s", animKey.c_str());
		ImGui::Text("Current Frame: %d", curFrame);
		ImGui::Text("Max Frame:     %d", maxFrame);
	}
	else
	{
		ImGui::Text("Animator: <null>");
	}

	ImGui::Separator();
	// 현재 상태를 int로 캐스팅해서 편하게 씀
	int curState = static_cast<int>(m_CurrentState);

	const char* stateNames[] = { "START", "CHOICE", "RESULT", "EXIT" };

	for (int i = 0; i < static_cast<int>(TAROTSTATE::COUNT); ++i)
	{
		// 한 줄에 여러 버튼 두고 싶으면 SameLine 써도 됨
		if (ImGui::Button(stateNames[i]))
		{
			m_CurrentState = static_cast<TAROTSTATE>(i);
			// 상태가 바뀌었으니 애니메이션 다시 틀 수 있게 이전 상태 초기화
			m_PrevState = TAROTSTATE::START; // 혹은 어떤 값이든 "다름"만 보장
		}
		ImGui::SameLine();
	}
}
