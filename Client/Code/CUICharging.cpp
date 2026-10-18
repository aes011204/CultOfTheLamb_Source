#include "pch.h"
#include "CUICharging.h"
#include "CCreateHelper.h"
#include "CPlayerSkill.h"
#include "CRcTex.h"
#include "CTransform.h"
#include "CTexture.h"
#include "CRenderer.h"

CUICharging::CUICharging(DEVICE graphicDev): CUI(graphicDev) {}
CUICharging::CUICharging(const CUICharging& rhs): CUI(rhs) {}
CUICharging::~CUICharging() {}

HRESULT CUICharging::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    Set_IsActive(false);
    return S_OK;
}
void CUICharging::Initialize_GameObject()
{
    CUI::Initialize_GameObject();

    m_TransformCom->Set_Scale(1.f, 1.f, 1.f);
    m_TransformCom->Set_Rotation(_vec3{ D3DXToRadian(20.f), 0.f, 0.f });
}

int CUICharging::Update_GameObject(const _float& timeDelta)
{
        int exit = CUI::Update_GameObject(timeDelta);

    if (m_Active)
    {
        CRenderer::GetInstance()->Add_RenderGroup(RENDERID::RENDER_ALPHA, this);

        float Length = 3.5f;
        float chargeTime = m_PlayerSkill->Get_ChargeTime();
        float maxChargeTime = m_PlayerSkill->Get_MaxChargeTime();

        m_ChargeRatio = chargeTime / maxChargeTime;

        float changeLength = m_ChargeRatio * Length;

        if (changeLength <= 0.f)
            changeLength = 0.f;

        m_TransformCom->Set_Scale(changeLength, 0.5f, 1.f);

        _vec3 dir = m_ArrowDir;

        D3DXVec3Normalize(&dir, &dir);
        dir.x = -dir.x;
        float angle = atan2f(dir.z, dir.x);
        dir.x = -dir.x;

        _float offset = m_PlayerSkill->Get_SkillInfo().spawnOffset;
        _vec3 playerPos = m_PlayerSkill->Get_Owner()->Get_Transform()->Get_Pos();

        _vec3 tailPos = playerPos + dir * offset;
        _vec3 centerPos = tailPos + dir * (changeLength * 0.5f);
        centerPos.y = 0.3;
        m_TransformCom->Set_Pos(centerPos);

        _vec3 rot{ D3DXToRadian(90.f), angle, 0.f };
        m_TransformCom->Set_Rotation(rot);

    }
    return exit;
}

void CUICharging::Render_GameObject()
{
        CUI::Render_GameObject();
        m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());
    if (m_Active)

    {
       m_GraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

        m_GraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
        m_GraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
        m_GraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

        m_GraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
        m_GraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
        m_GraphicDev->SetRenderState(D3DRS_ALPHAREF, 0);

        m_TextureCom->Set_Texture(10);
        m_BufferCom->Render_Buffer();

        m_GraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
        m_GraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
       m_GraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
     
    }
}

HRESULT CUICharging::Add_Component()
{
    m_BufferCom = CreateProtoComponent<CRcTex>(this, COMPONENTTYPE::RC_TEX);
    NULL_CHECK_RETURN(m_BufferCom, E_FAIL);

    m_Components[ID_STATIC].insert({ COMPONENTTYPE::RC_TEX, m_BufferCom });

    // transform
    m_TransformCom = CreateProtoComponent<CTransform>(this, COMPONENTTYPE::TRANSFORM);
    NULL_CHECK_RETURN(m_TransformCom, E_FAIL);

    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::TRANSFORM, m_TransformCom });

    // texture
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_UI_ICON);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);

    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_UI_ICON, m_TextureCom });

    return S_OK;
}

CUICharging* CUICharging::Create(DEVICE graphicDev)
{
    auto chargingUI = new CUICharging(graphicDev);

    if(FAILED(chargingUI->Ready_GameObject()))
    {
        Safe_Release(chargingUI);
        MSG_BOX("ChargingUI Create Failed");
        return nullptr;
    }
    return chargingUI;
}

void CUICharging::Free()
{
    CUI::Free();
}
