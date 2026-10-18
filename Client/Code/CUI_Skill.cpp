#include "pch.h"
#include "CUI_Skill.h"
#include <CCreateHelper.h>
#include "CFireball.h"
#include "CPlayerSkill.h"
#include "CRcTex.h"
#include "CTransform.h"
#include "CTexture.h"
#include "CRenderer.h"

CUI_Skill::CUI_Skill(DEVICE graphicDev)
    : CUI(graphicDev),
      m_TextureIndex(0) {}

CUI_Skill::CUI_Skill(const CUI_Skill& rhs)
    : CUI(rhs),
      m_TextureIndex(0) {}

CUI_Skill::~CUI_Skill() {}

HRESULT CUI_Skill::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

  //  m_TransformCom->Set_Pos(_vec3(-450.f, 220.f, 0.f));
    m_TransformCom->Set_Scale(_vec3(27.f, 27.f, 0.f));

    m_Invisible = true;

    return S_OK;
}

_int CUI_Skill::Update_GameObject(const _float& timeDelta)
{
    _int exit = CUI::Update_GameObject(timeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

    CPlayerSkill* playerskill = dynamic_cast<CPlayerSkill*>(CManagement::GetInstance()->Get_Component(COMPONENTID::ID_STATIC, LAYERTYPE::GAMELOGIC, OBJTYPE::PLAYER, COMPONENTTYPE::PL_SKILL));
    if(playerskill->Get_IsSkillUnlocked())
    {
        m_Invisible = false;
    }
    return CUI::Update_GameObject(timeDelta);
}

void CUI_Skill::LateUpdate_GameObject(const _float& timeDelta)
{
    CUI::LateUpdate_GameObject(timeDelta);
}

void CUI_Skill::Render_GameObject()
{
    if (!m_Invisible)
    {
        CUI::Render_GameObject();

        m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());
        m_GraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
        m_GraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
        m_GraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
       // CPlayerSkill* playerskill = dynamic_cast<CPlayerSkill*>(CManagement::GetInstance()->Get_Component(COMPONENTID::ID_STATIC, LAYERTYPE::GAMELOGIC, OBJTYPE::PLAYER, COMPONENTTYPE::PL_SKILL));
       // if(playerskill->Get_SingleSkillAvailability())
       // {
       //     m_TextureIndex = 1;
       // }
       // else if(playerskill->Get_TripleSkillAvailability())
       // {
       //     m_TextureIndex = 2;
       // }
       // else
       // {
       //     m_TextureIndex = 0;
       // }
        m_TextureCom->Set_Texture(m_TextureIndex);
        m_BufferCom->Render_Buffer();
    }
}

HRESULT    CUI_Skill::Add_Component()
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
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_UI_SKILL);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);

    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_UI_SKILL, m_TextureCom });

    return S_OK;
}
CUI_Skill* CUI_Skill::Create(DEVICE graphicDev)
{
    auto skillUI = new CUI_Skill(graphicDev);
    if (FAILED(skillUI->Ready_GameObject()))
    {
        Safe_Release(skillUI);
        MSG_BOX("UI_Skill Created Failed");
        return nullptr;
    }
    return skillUI;
}

void CUI_Skill::Free()
{
    CUI::Free();
}
