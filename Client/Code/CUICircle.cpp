#include "pch.h"
#include "CUICircle.h"
#include "CFontMgr.h"
#include "CCreateHelper.h"
#include "CRcTex.h"
#include "CRenderer.h"
#include "CTexture.h"
#include "CTransform.h"
#include "CCombatStat.h"
#include "CPlayerSkill.h"

int CUICircle::m_nextID = 0;

CUICircle::CUICircle(DEVICE pGraphicDev)
    : CUI(pGraphicDev)
{
    m_ID = ++m_nextID;
}

CUICircle::CUICircle(const CUICircle& rhs)
    : CUI(rhs)
{
    m_ID = ++m_nextID;
}

CUICircle::~CUICircle()
{}

HRESULT CUICircle::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    m_TransformCom->Set_Pos(_vec3(-550.f, 270.f, 0.f));
    m_TransformCom->Set_Scale(_vec3(85.f, 85.f, 0.f));
    return S_OK;
}

_int CUICircle::Update_GameObject(const _float& timeDelta)
{
    _int exit = CUI::Update_GameObject(timeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

    return exit;
}

void CUICircle::LateUpdate_GameObject(const _float& timeDelta)
{
    CUI::LateUpdate_GameObject(timeDelta);
}

void CUICircle::Render_GameObject()
{
    CUI::Render_GameObject();

    //m_GraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
    m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());

    m_TextureCom->Set_Texture(0);

    m_BufferCom->Render_Buffer();

    _vec3 pos = m_TransformCom->Get_Pos();
    _vec2 fontOffset = { 0.f,10.f };
    _vec2 screen = { WINCX / 2 + pos.x + fontOffset.x,WINCY / 2 - pos.y + fontOffset.y };

    CPlayerSkill* playerskill = dynamic_cast<CPlayerSkill*>(CManagement::GetInstance()->Get_Component(COMPONENTID::ID_STATIC, LAYERTYPE::GAMELOGIC, OBJTYPE::PLAYER, COMPONENTTYPE::PL_SKILL));
    int currentCharges = playerskill->Get_CurrentCharges();

    CFontMgr::GetInstance()->Render_FontOutlined(L"NanumSquare_Skill", to_wstring(currentCharges), &screen, D3DXCOLOR(1.f, 1.f, 1.f, 1.f), D3DXCOLOR(0.f, 0.f, 0.f, 1.f), 2);

    //m_GraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);

}

HRESULT CUICircle::Add_Component()
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
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_UI_CIRCLE);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);

    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_UI_CIRCLE, m_TextureCom });

    return S_OK;
}

CUICircle* CUICircle::Create(DEVICE graphicDev)
{
    auto circle = new CUICircle(graphicDev);
    if (FAILED(circle->Ready_GameObject()))
    {
        Safe_Release(circle);
        MSG_BOX("UIHeartBar Created Failed");
        return nullptr;
    }
    return circle;
}

void CUICircle::Free()
{
    CUI::Free();
}
