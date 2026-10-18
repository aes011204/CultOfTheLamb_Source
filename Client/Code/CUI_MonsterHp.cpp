#include "pch.h"
#include "CUI_MonsterHp.h"
#include "CRcTex.h"
#include "CCombatStat.h"
#include "CCreateHelper.h"
#include "CTexture.h"
#include "CTransform.h"
#include "CRenderer.h"

CUI_MonsterHp::CUI_MonsterHp(DEVICE graphicDev, CCombatStat* combat)
    : CUI(graphicDev),
    m_Owner(combat->Get_Owner()),
    m_OwnerCombatStat(combat),
    m_OwnerTransform(nullptr),
    m_HpRatio(1.0f),
    m_HpDelayRatio(1.0f),
    m_HpDelaySpeed(1.5f),   
    m_YOffset(3.0f),
    m_BarSize({ 0.6f, 0.7f, 1.f }),
    m_Buffer_Hp(nullptr), m_Buffer_Delay(nullptr)
{
    Set_Name(L"Monster Hp");

    if (m_Owner)
        m_Owner->AddRef();
}

CUI_MonsterHp::~CUI_MonsterHp()
{
}

HRESULT CUI_MonsterHp::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    m_YOffset = 1.5f;
    m_HpRatio = 1.f;
    m_HpDelaySpeed = 1.5f;

    //m_TransformCom->Set_RotationAxis(ROT_X, D3DXToRadian(45.f));

    return S_OK;
}

void CUI_MonsterHp::Initialize_GameObject()
{
    CUI::Initialize_GameObject();

    if (m_Owner)
    {
        m_OwnerCombatStat = dynamic_cast<CCombatStat*>(
            m_Owner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::COMBATSTAT));

        m_OwnerTransform = dynamic_cast<CTransform*>(
            m_Owner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));
    }

}

_int CUI_MonsterHp::Update_GameObject(const _float& timeDelta)
{
    _int exit = CUI::Update_GameObject(timeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

    if (!m_Owner)
        return exit;

    // HP 비율 계산
    if (m_OwnerCombatStat)
    {
        _float curHp = m_OwnerCombatStat->Get_Hp();
        _float maxHp = m_OwnerCombatStat->Get_MaxHp();

        if (maxHp > 0.f)
            m_HpRatio = curHp / maxHp;
        else
            m_HpRatio = 0.f;

        // 죽으면 삭제
        if (m_HpRatio <= 0.f)
            this->Mark_As_PendingKill();
    }

    if (m_OwnerTransform && m_TransformCom)
    {
        _vec3 ownerPos = m_OwnerTransform->Get_Pos();
        ownerPos.y += m_YOffset;
      
        _vec3 screenPos;
        D3DVIEWPORT9 vp;
        m_GraphicDev->GetViewport(&vp);

        _matrix view, proj, world;
        m_GraphicDev->GetTransform(D3DTS_VIEW, &view);
        m_GraphicDev->GetTransform(D3DTS_PROJECTION, &proj);

        D3DXMatrixIdentity(&world);
        D3DXVec3Project(&screenPos, &ownerPos, &vp, &proj, &view, &world);
  
        _float uiX = screenPos.x - WINCX * 0.5f;
        _float uiY = -(screenPos.y - WINCY * 0.5f);

        m_TransformCom->Set_Pos(_vec3(uiX, uiY, 0.f));
        m_TransformCom->Set_Scale(_vec3(1.f, 1.f, 1.f));
    }

    if (m_HpDelayRatio > m_HpRatio)
    {
        m_HpDelayRatio -= m_HpDelaySpeed * timeDelta;
        if (m_HpDelayRatio < m_HpRatio)
            m_HpDelayRatio = m_HpRatio;
    }

    return exit;
}

void CUI_MonsterHp::LateUpdate_GameObject(const _float& timeDelta)
{
    CUI::LateUpdate_GameObject(timeDelta);

}

void CUI_MonsterHp::Render_GameObject()
{
    if (!m_Owner || !m_TextureCom)
        return;
    
    m_GraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);

    _float ratio = clamp(m_HpRatio, 0.f, 1.f);
    _float delayRatio = clamp(m_HpDelayRatio, 0.f, 1.f);

    const _float FULL_HALF_WIDTH = 40.f;
    const _float FULL_HALF_HEIGHT = 5.f;

    const _matrix& world = m_TransformCom->Get_World();

    // 배경
    {
        const _float BG_WIDTH = 42.f;
        const _float BG_HEIGHT = 7.f;

        m_BufferCom->Set_Scale(BG_WIDTH, BG_HEIGHT);
        m_BufferCom->Set_Offset(0.f, 0.f);

        m_GraphicDev->SetTransform(D3DTS_WORLD, &world);
        m_TextureCom->Set_Texture(1);  // 배경 텍스처
        m_BufferCom->Render_Buffer();
    }
    // 딜레이 바
    if (delayRatio > 0.f)
    {
        _float hw = FULL_HALF_WIDTH * delayRatio;
        _float offsetX = FULL_HALF_WIDTH * (1.f - delayRatio);

        m_Buffer_Delay->Set_Scale(hw, FULL_HALF_HEIGHT);
        m_Buffer_Delay->Set_Offset(-offsetX, 0.f);

        m_GraphicDev->SetTransform(D3DTS_WORLD, &world);
        m_TextureCom->Set_Texture(2);  // 흰색 텍스처
        m_Buffer_Delay->Render_Buffer();
    }
    // HP 바
    if (ratio > 0.f)
    {
        _float hw = FULL_HALF_WIDTH * ratio;
        _float offsetX = FULL_HALF_WIDTH * (1.f - ratio);

        m_Buffer_Hp->Set_Scale(hw, FULL_HALF_HEIGHT);
        m_Buffer_Hp->Set_Offset(-offsetX, 0.f);

        m_GraphicDev->SetTransform(D3DTS_WORLD, &world);
        m_TextureCom->Set_Texture(0);  // HP 바 텍스처
        m_Buffer_Hp->Render_Buffer();
    }

    m_GraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
}

HRESULT CUI_MonsterHp::Add_Component()
{
    // Texture
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_UI_MONSTERHP);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_UI_MONSTERHP, m_TextureCom });

    // Transform
    m_TransformCom = CreateProtoComponent<CTransform>(this, COMPONENTTYPE::TRANSFORM);
    NULL_CHECK_RETURN(m_TransformCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::TRANSFORM, m_TransformCom });

    // Buffer
    m_BufferCom = CreateProtoComponent<CRcTex>(this, COMPONENTTYPE::RC_TEX);
    NULL_CHECK_RETURN(m_BufferCom, E_FAIL);

    m_Buffer_Delay = CreateProtoComponent<CRcTex>(this, COMPONENTTYPE::RC_TEX);
    NULL_CHECK_RETURN(m_Buffer_Delay, E_FAIL);

    m_Buffer_Hp = CreateProtoComponent<CRcTex>(this, COMPONENTTYPE::RC_TEX);
    NULL_CHECK_RETURN(m_Buffer_Hp, E_FAIL);

    m_Components[ID_STATIC].insert({ COMPONENTTYPE::RC_TEX, m_BufferCom });
    

    return S_OK;
}

CUI_MonsterHp* CUI_MonsterHp::Create(DEVICE graphicDev, CCombatStat* combat)
{
    CUI_MonsterHp* hp = new CUI_MonsterHp(graphicDev, combat);

    if (FAILED(hp->Ready_GameObject()))
    {
        Safe_Release(hp);
        MSG_BOX("Monster Hp Create Failed");
        return nullptr;
    }

    hp->Initialize_GameObject();

    return hp;
}

void CUI_MonsterHp::Free()
{
    Safe_Release(m_Buffer_Delay);
    Safe_Release(m_Buffer_Hp);
    Safe_Release(m_Owner);

    CUI::Free();
}
