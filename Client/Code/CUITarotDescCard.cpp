#include "pch.h"
#include "CUITarotDescCard.h"
#include "CTransform.h"
#include "CRcTex.h"
#include "CTexture.h"
#include "CCreateHelper.h"
#include "CRenderer.h"
#include "CUIButton.h"
#include "CDInputMgr.h"

CUITarotDescCard::CUITarotDescCard(DEVICE graphicDev)
    : CUI(graphicDev),
      m_Hovered(false),
      m_PlaySlide(false),
      m_Start(true),
      m_StartX(0),
      m_TargetX(0),
      m_RenderOff(false)
{ }

CUITarotDescCard::CUITarotDescCard(const CUITarotDescCard& rhs)
    : CUI(rhs),
      m_Hovered(false),
      m_PlaySlide(false),
      m_Start(true),
      m_StartX(0),
      m_TargetX(0),
      m_RenderOff(false)
{ }

CUITarotDescCard::~CUITarotDescCard()
{
    
}

HRESULT CUITarotDescCard::Ready_GameObject()
{
    if(FAILED(Add_Component()))
    {
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CUITarotDescCard::Add_Component()
{

    // buffer
    m_BufferCom = CreateProtoComponent<CRcTex>(this, COMPONENTTYPE::RC_TEX);
    NULL_CHECK_RETURN(m_BufferCom, E_FAIL);

    m_Components[ID_STATIC].insert({ COMPONENTTYPE::RC_TEX, m_BufferCom });

    // transform
    m_TransformCom = CreateProtoComponent<CTransform>(this, COMPONENTTYPE::TRANSFORM);
    NULL_CHECK_RETURN(m_TransformCom, E_FAIL);

    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::TRANSFORM, m_TransformCom });

    // texture
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_UI_BUTTON);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);

    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_UI_BUTTON, m_TextureCom });

    return S_OK;

}

_int CUITarotDescCard::Update_GameObject(const _float& timeDelta)
{
   
    _int exit = CUI::Update_GameObject(timeDelta);
    CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

    _vec3 pos = m_TransformCom->Get_Pos();  
    _vec3 size = m_TransformCom->Get_Scale(); 


    float screenX = pos.x + WINCX * 0.5f;     
    float screenY = WINCY * 0.5f - pos.y;    
    m_Rect.left = (LONG)(screenX - size.x);
    m_Rect.top = (LONG)(screenY - size.y);
    m_Rect.right = (LONG)(screenX + size.x);
    m_Rect.bottom = (LONG)(screenY + size.y);

    POINT pt;
    GetCursorPos(&pt);
    ScreenToClient(CDInputMgr::GetInstance()->Get_HWND(), &pt);


    bool hoveredNow = PtInRect(&m_Rect, pt) != 0;
    
    if (hoveredNow != m_Hovered)
    {
        m_Hovered = hoveredNow;

        if (m_Hovered) OnHoverStart();
        else            OnHoverEnd();
    }


    if (m_Hovered &&
        (CDInputMgr::GetInstance()->Get_DIMouseState(MOUSEKEYSTATE::DIM_LB) & 0x80))
    {
        if (m_OnClick)
        {
            m_OnClick();

        }
        
    }

    if(m_PlaySlide)
    {

        if(m_Start)
        {
            m_BaseScale = m_TransformCom->Get_Scale();
            m_TargetScale = m_BaseScale;

            m_StartX = m_TransformCom->Get_Pos().x;
            m_TargetX = m_StartX + 200.f;
            m_Start = false;
            
        }
        _vec3 pos = m_TransformCom->Get_Pos();
        float speed = 250.f;

        if(pos.x< m_TargetX)
        {
            pos.x += speed * timeDelta;

            m_TransformCom->Set_Pos(pos);
        }

    }

    m_TransformCom->Set_Scale(m_TargetScale);
    return exit;
}

void CUITarotDescCard::LateUpdate_GameObject(const _float& timeDelta)
{
    CUI::LateUpdate_GameObject(timeDelta);
}

void CUITarotDescCard::Render_GameObject()
{
    CUI::Render_GameObject();

    if(!m_RenderOff)
    {

        if (m_PlaySlide)
        {
            m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());

            m_TextureCom->Set_Texture(m_TextureIndex);

            m_BufferCom->Render_Buffer();
        }

    } 
}

void CUITarotDescCard::InitializeScale()
{
    m_BaseScale = m_TransformCom->Get_Scale();
    m_TargetScale = m_BaseScale;

}

void  CUITarotDescCard::OnHoverStart()
{
    m_TargetScale = m_BaseScale * 1.15;
}

void CUITarotDescCard::OnHoverEnd()
{
    m_TargetScale = m_BaseScale;
}

CUITarotDescCard* CUITarotDescCard::Create(DEVICE graphicDev)
{
    auto tarotDescCard = new CUITarotDescCard(graphicDev);
    if (FAILED(tarotDescCard->Ready_GameObject()))
    {
        Safe_Release(tarotDescCard);
        MSG_BOX("CUITarotDescCard Created Failed");
        return nullptr;
    }


    return tarotDescCard;
}
