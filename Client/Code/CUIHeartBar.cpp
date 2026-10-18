#include "pch.h"
#include "CUIHeartBar.h"
#include "CEnumHelper.h"
#include "CCombatStat.h"
#include "CCreateHelper.h"
#include "CPlayer.h"
#include "CRcTex.h"
#include "CTexture.h"
#include "CTransform.h"

int CUIHeartBar::m_nextID = 0;

CUIHeartBar::CUIHeartBar(DEVICE graphicDev)
    : CUI(graphicDev),
    m_BufferCom(nullptr),
    m_TextureCom(nullptr),
    m_TransformCom(nullptr)
{

    m_ID = ++m_nextID;

}

CUIHeartBar::CUIHeartBar(const CUIHeartBar& rhs)
    : CUI(rhs),
    m_BufferCom(nullptr),
    m_TextureCom(nullptr),
    m_TransformCom(nullptr)
{

    m_ID = ++m_nextID;

}

CUIHeartBar::~CUIHeartBar()
{
}

HRESULT CUIHeartBar::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;


   /* switch (m_ID)
    {
    case 1:
        m_TransformCom->Set_Pos(_vec3(-450.f, 280.f, 0.f));
        break;
    case 2:
        m_TransformCom->Set_Pos(_vec3(-410.f, 280.f, 0.f));
        break;
    case 3:
        m_TransformCom->Set_Pos(_vec3(-370.f, 280.f, 0.f));
        break;

    }*/

    m_TransformCom->Set_Scale(_vec3(16.f, 16.f, 1.f));

    return S_OK;
}

_int CUIHeartBar::Update_GameObject(const _float& timeDelta)
{
    _int exit = CUI::Update_GameObject(timeDelta);

    return exit;
}

void CUIHeartBar::LateUpdate_GameObject(const _float& timeDelta)
{
    CUI::LateUpdate_GameObject(timeDelta);
}

void CUIHeartBar::Render_GameObject()
{
    //m_GraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);


    // 월드 행렬 세팅
    m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());

    switch (m_HeartState)
    {
    case HEARTSTATE::EMPTY:
        m_TextureCom->Set_Texture(0);
        break;

    case HEARTSTATE::HALF:
        m_TextureCom->Set_Texture(1);
        break;

    case HEARTSTATE::FULL:
        m_TextureCom->Set_Texture(2);
        break;
    }

    m_BufferCom->Render_Buffer();

    //m_GraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
}

HRESULT CUIHeartBar::Add_Component()
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
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_UI_HEART);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);

    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_UI_HEART, m_TextureCom });

    return S_OK;
}

CUIHeartBar* CUIHeartBar::Create(DEVICE graphicDev)
{
    auto heartbar = new CUIHeartBar(graphicDev);
    if (FAILED(heartbar->Ready_GameObject()))
    {
        Safe_Release(heartbar);
        MSG_BOX("UIHeartBar Created Failed");
        return nullptr;
    }

    return heartbar;
}

void CUIHeartBar::Free()
{
    CUI::Free();
}
