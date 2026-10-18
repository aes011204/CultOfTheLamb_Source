#include "pch.h"
#include "CUISkillCount.h"
#include "CFontMgr.h"
#include "CCombatStat.h"
#include "CCreateHelper.h"
#include "CRcTex.h"
#include "CRenderer.h"
#include "CTexture.h"
#include "CTransform.h"
#include "CUICircle.h"

int CUISkillCount::m_nextID = 0;

CUISkillCount::CUISkillCount(DEVICE pGraphicDev) : CUI(pGraphicDev), m_SkillCount(2)
{

    m_ID = ++m_nextID;
}

CUISkillCount::CUISkillCount(const CUISkillCount& rhs) : CUI(rhs),m_SkillCount(2)
{
    m_ID = ++m_nextID;
}

CUISkillCount::~CUISkillCount()
{
}

HRESULT CUISkillCount::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    m_TransformCom->Set_Pos(_vec3(-560.f, 260.f, 0.f));
    m_TransformCom->Set_Scale(_vec3(12.f, 13.5f, 0.f));
    return S_OK;
}

_int CUISkillCount::Update_GameObject(const _float& timeDelta)
{
    _int exit = CUI::Update_GameObject(timeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

    return exit;
}

void CUISkillCount::LateUpdate_GameObject(const _float& timeDelta)
{
    CUI::LateUpdate_GameObject(timeDelta);
}

void CUISkillCount::Render_GameObject()
{
    m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());

    _vec3 pos = dynamic_cast<CTransform*>(m_UICircle->Get_Component(COMPONENTID::ID_DYNAMIC, COMPONENTTYPE::TRANSFORM))->Get_Pos();
    _vec2 fontOffset = { -20.f,0.f };
    _vec2 screen = { WINCX / 2 + pos.x,WINCY / 2 - pos.y };

    CFontMgr::GetInstance()->Render_FontOutlined(L"NanumSquare_Heavy", to_wstring(m_SkillCount), &screen, D3DXCOLOR(1.f, 1.f, 1.f, 1.f), D3DXCOLOR(0.f, 0.f, 0.f, 1.f), 3);

    m_BufferCom->Render_Buffer();
}

HRESULT CUISkillCount::Add_Component()
{

    // buffer
    m_BufferCom = CreateProtoComponent<CRcTex>(this, COMPONENTTYPE::RC_TEX);
    NULL_CHECK_RETURN(m_BufferCom, E_FAIL);

    m_Components[ID_STATIC].insert({ COMPONENTTYPE::RC_TEX, m_BufferCom });

    // transform
    m_TransformCom = CreateProtoComponent<CTransform>(this, COMPONENTTYPE::TRANSFORM);
    NULL_CHECK_RETURN(m_TransformCom, E_FAIL);

    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::TRANSFORM, m_TransformCom });

   // // texture
   // m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_UI_SKILLCOUNT);
   // NULL_CHECK_RETURN(m_TextureCom, E_FAIL);
   //
   // m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_UI_SKILLCOUNT, m_TextureCom });

    return S_OK;
}

CUISkillCount* CUISkillCount::Create(DEVICE graphicDev)
{
    auto skillCount = new CUISkillCount(graphicDev);
    if (FAILED(skillCount->Ready_GameObject()))
    {
        Safe_Release(skillCount);
        MSG_BOX("SkillCount Created Failed");
        return nullptr;
    }
    return skillCount;
}

void CUISkillCount::Free()
{

    CUI::Free();
}
