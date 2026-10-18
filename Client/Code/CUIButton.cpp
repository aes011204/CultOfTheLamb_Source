#include "pch.h"

#include "CUIButton.h"
#include "CCreateHelper.h"
#include "CRcTex.h"
#include "CRenderer.h"
#include "CTexture.h"
#include "CTransform.h"
#include "CDInputMgr.h"
#include "CFontMgr.h"

int CUIButton::m_NextID = 0;

// 일단 대기 rcTex 고치고 보자

CUIButton::CUIButton(DEVICE graphicDev, int texOn, int texOff)
    : CUI(graphicDev),
      m_Rect(),
      m_TextureOn(texOn),
      m_TextureOff(texOff),
      m_HasText(false),
      m_TextureIndex(texOff)
{
    m_ID = ++m_NextID;

    m_TextureOn  = texOn;
    m_TextureOff = texOff;
}

CUIButton::CUIButton(const CUIButton& rhs, int texOn, int texOff)
    : CUI(rhs),
      m_Rect(),
      m_TextureOn(texOn),
      m_TextureOff(texOff),
      m_HasText(false),
      m_TextureIndex(texOff)
{
    m_ID = ++m_NextID;

    m_TextureOn  = texOn;
    m_TextureOff = texOff;
}

CUIButton::~CUIButton()
{
}

HRESULT CUIButton::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    m_TransformCom->Set_Pos(_vec3(0.f, 0.f, 1.f));

    if (m_TextureOn == 1)
    {
        m_TextureIndex = 0;
    }
    if (m_TextureOn == 3)
    {
        m_TextureIndex = 2;
    }
    return S_OK;
}

_int CUIButton::Update_GameObject(const _float& timeDelta)
{
    _int exit = CUI::Update_GameObject(timeDelta);
    CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

    _vec3 pos = m_TransformCom->Get_Pos();   // (0,0) = 화면 중앙, +Y = 위
    _vec3 size = m_TransformCom->Get_Scale(); // 픽셀 단위 가정

    // 2) 월드(중앙 기준) → 스크린(좌상단 기준) 변환
    float screenX = pos.x + WINCX * 0.5f;     // 중앙 0 → WINCX/2
    float screenY = WINCY * 0.5f - pos.y;     // 위가 + → 아래가 +로 뒤집기

    // 3) 버튼 화면 Rect 계산 (센터 기준)
    m_Rect.left = (LONG)(screenX - size.x);
    m_Rect.top = (LONG)(screenY - size.y);
    m_Rect.right = (LONG)(screenX + size.x);
    m_Rect.bottom = (LONG)(screenY + size.y);

    // 4) 마우스 좌표 (클라이언트 기준)
    POINT pt;
    GetCursorPos(&pt);
    ScreenToClient(CDInputMgr::GetInstance()->Get_HWND(), &pt);
    //cout << pt.x << " , " << pt.y << endl;

    bool hoveredNow = PtInRect(&m_Rect, pt) != 0;

    // 4) Hover 변화 감지
    if (hoveredNow != m_Hovered)
    {
        m_Hovered = hoveredNow;

        if (m_Hovered) OnHoverStart();
        else            OnHoverEnd();
    }

    // 5) 클릭 (괄호 주의)
    if (m_Hovered &&
        (CDInputMgr::GetInstance()->MouseDown(DIM_LB)))
    {
        if (m_OnClick)
        {
            m_OnClick();
        }
        // OnClick(); 같은 거 여기서 호출해도 됨
    }

    if (m_Hovered == true)
    {
        int i = 0;
    }

    if(m_HasText)
    {
        if(m_Selected||m_Hovered)
        {
            m_D3dxColor = { 1.f, 1.f, 1.f, 1.f };
        }
        else
        {
            m_D3dxColor = { 0.f, 0.f, 0.f, 1.f };
        }
    }

    return exit;
}

void CUIButton::LateUpdate_GameObject(const _float& timeDelta)
{
    CUI::LateUpdate_GameObject(timeDelta);

}

void CUIButton::Render_GameObject()
{
    CUI::Render_GameObject();

    m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());

    m_TextureCom->Set_Texture(m_TextureIndex);

    m_BufferCom->Render_Buffer();

    m_TextOffset = { m_TransformCom->Get_Pos().x,m_TransformCom->Get_Pos().y };

    _vec2 screen = { m_TextOffset.x + WINCX * 0.5f ,WINCY * 0.5f - m_TextOffset.y };

    if(m_HasText)
    {
    CFontMgr::GetInstance()->Render_Font(L"NanumSquare_Regular",
        m_Text,
        &screen,
        m_D3dxColor);
    }
   
}

HRESULT CUIButton::Add_Component()
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

void CUIButton::Set_Selected(bool tf)
{
    m_Selected = tf;

    if (m_Selected)
    {
        m_TextureIndex = m_TextureOn;
    }
    else
    {
        m_TextureIndex = m_TextureOff;
    }

}

void CUIButton::OnHoverStart()
{
    if (!m_Selected)
    {
        m_TextureIndex = m_TextureOn;
    }
}

void CUIButton::OnHoverEnd()
{
    if(!m_Selected)
    {
    m_TextureIndex = m_TextureOff;
    }

}

bool CUIButton::SetCloneTexAspectScale(_vec3 vec) // 텍스쳐 등록후
{
    float x, y;
    m_TextureCom->Get_TextureSize(0, x, y);
    if (false == SetTexAspectInit(m_TextureCom, m_TransformCom, 0,nullptr,true,x))
        return false;

    SetAspectScale(m_TransformCom, vec);

    return true;
}
CUIButton* CUIButton::Create(DEVICE graphicDev, int texOn, int texOff)
{
    auto Button = new CUIButton(graphicDev, texOn, texOff);
    if (FAILED(Button->Ready_GameObject()))
    {
        Safe_Release(Button);
        MSG_BOX("CUIButton Created Failed");
        return nullptr;
    }
    return Button;
}

void CUIButton::Free()
{
    CUI::Free();
}
