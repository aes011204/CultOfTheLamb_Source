#include "pch.h"
#include "CUI_SkillPanel.h"
#include "CTexture.h"
#include "CCreateHelper.h"
#include "CPlayerSkill.h"
#include "CTransform.h"
#include "CUIIcon.h"
#include "CRenderer.h"

CUI_SkillPanel::CUI_SkillPanel(DEVICE graphicDev)
    : CUI(graphicDev),
      m_PlayerSkillCom(nullptr),
      m_SingleSkillUI(nullptr),
      m_TripleSkillUI(nullptr) {}

CUI_SkillPanel::CUI_SkillPanel(const CUI_SkillPanel& rhs)
    : CUI(rhs),
      m_PlayerSkillCom(nullptr),
      m_SingleSkillUI(nullptr),
      m_TripleSkillUI(nullptr) {}

CUI_SkillPanel::~CUI_SkillPanel() {}

void CUI_SkillPanel::Initialize_GameObject()
{
    CUI::Initialize_GameObject();
}

HRESULT CUI_SkillPanel::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    m_SingleSkillUI = CUIIcon::Create(m_GraphicDev, COMPONENTTYPE::TEX_UI_SKILL);
    m_SingleSkillUI->SetRegister(false);
    m_SingleSkillUI->Get_Transform()->Set_Scale(25.f, 25.f, 0.f);
    m_SingleSkillUI->Get_Transform()->Set_Pos(-450.f, 220.f, 0.f);
    m_SingleSkillUI->Set_TextureIndex(1);

    m_TripleSkillUI = CUIIcon::Create(m_GraphicDev, COMPONENTTYPE::TEX_UI_SKILL);
    m_TripleSkillUI->SetRegister(false);
    m_TripleSkillUI->Get_Transform()->Set_Scale(25.f, 25.f, 0.f);
    m_TripleSkillUI->Get_Transform()->Set_Pos(-400.f, 220.f, 0.f);
    m_TripleSkillUI->Set_TextureIndex(2);

    m_Active = false;

    return S_OK;
}

_int CUI_SkillPanel::Update_GameObject(const _float& timeDelta)
{
    int exit = CUI::Update_GameObject(timeDelta);

    if(!m_PlayerSkillCom)
    {
        m_PlayerSkillCom = dynamic_cast<CPlayerSkill*>(CManagement::GetInstance()->Get_Component(ID_STATIC, LAYERTYPE::GAMELOGIC, OBJTYPE::PLAYER, COMPONENTTYPE::PL_SKILL));
    }

    if (m_PlayerSkillCom->Get_IsSkillUnlocked())
    {
        m_Active = true;
    }
    else
    {
        m_Active = false;
    }
    if (!m_Active)
        return 0;

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

    if (m_SingleSkillUI)
        m_SingleSkillUI->Update_GameObject(timeDelta);

    if (m_TripleSkillUI)
        m_TripleSkillUI->Update_GameObject(timeDelta);

    return exit;
}

void CUI_SkillPanel::LateUpdate_GameObject(const _float& timeDelta)
{
    CUI::LateUpdate_GameObject(timeDelta);
}

void CUI_SkillPanel::Render_GameObject()
{
    CUI::Render_GameObject();

    if (!m_Active)
        return;

    m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());

    if (m_SingleSkillUI)
        m_SingleSkillUI->Get_Transform()->Set_Pos(-450.f, 220.f, 0.f);
        m_SingleSkillUI->Render_GameObject();

    if (m_TripleSkillUI)
        m_TripleSkillUI->Get_Transform()->Set_Pos(-400.f, 220.f, 0.f);
        m_TripleSkillUI->Render_GameObject();

}

CUI_SkillPanel* CUI_SkillPanel::Create(DEVICE graphicDev)
{
    CUI_SkillPanel* skillPanel = new CUI_SkillPanel(graphicDev);

    if (FAILED(skillPanel->Ready_GameObject()))
    {
        Safe_Release(skillPanel);
        MSG_BOX("Boss Hp Create Failed");
        return nullptr;
    }

    skillPanel->Initialize_GameObject();

    return skillPanel;
}

HRESULT CUI_SkillPanel::Add_Component()
{
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_UI_MONSTERHP);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_UI_SKILL, m_TextureCom });

    m_TransformCom = CreateProtoComponent<CTransform>(this, COMPONENTTYPE::TRANSFORM);
    NULL_CHECK_RETURN(m_TransformCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::TRANSFORM, m_TransformCom });

    return S_OK;
}

void CUI_SkillPanel::Free()
{
    Safe_Release(m_SingleSkillUI);
    Safe_Release(m_TripleSkillUI);
    CUI::Free();
}
