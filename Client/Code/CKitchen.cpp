#include "pch.h"
#include "CKitchen.h"

#include "CBoxCollider.h"
#include "CCookingController.h"
#include "CCreateHelper.h"
#include "CDInputMgr.h"
#include "CPlayer.h"
#include "CRcTex.h"
#include "CSpriteEff.h"
#include "CTexture.h"
#include "CTransform.h"
#include "CUICookingMenu.h"
#include "CSoundMgr.h"

CKitchen::CKitchen(DEVICE graphicDev)
    : CBuildingBase(graphicDev),
      m_CookingCtrlCom(nullptr),
      m_CookingMenu(nullptr)
{ }

CKitchen::CKitchen(const CKitchen& rhs)
    : CBuildingBase(rhs),
      m_CookingCtrlCom(nullptr),
      m_CookingMenu(nullptr)
{ }

CKitchen::~CKitchen()
{ }

HRESULT CKitchen::Ready_GameObject()
{
    if (FAILED(CBuildingBase::Ready_GameObject()))
        return E_FAIL;

    return S_OK;
}

void CKitchen::Initialize_GameObject()
{
    CBuildingBase::Initialize_GameObject();

    m_TransformCom->Set_Pos(20.f, 1.f, 15.f);

    m_TransformCom->Set_Scale(1.f, 1.f, 1.f);
    m_TransformCom->Set_RotationAxis(ROT_X, D3DXToRadian(40.f));

    m_BoxCollCom->Set_RawSize(_vec3(5.f, 5.f, 5.f));
    m_BoxCollCom->Set_CollisionLayer(COLLISION_LAYERTYPE::KITCHEN);
}

_int CKitchen::Update_GameObject(const _float& timeDelta)
{
    _int exit = CBuildingBase::Update_GameObject(timeDelta);

    if (m_bPlayerOverlap == true && m_bHovering == true)
    {
        if (CDInputMgr::GetInstance()->MouseDown(DIM_LB))
        {
            if (m_CookingCtrlCom && m_CookingCtrlCom->IsCooking())
                return exit;

            if (m_CookingMenu)
            {
        
			CSoundMgr::GetInstance()->Play_Once(L"UI/UI_Open_Menu.wav",
				SOUND_EFFECT,
				1.f);
                m_CookingMenu->Set_IsActive(true);
                m_CookingMenu->Set_CookingController(m_CookingCtrlCom);
                m_CookingMenu->PlayOpenAnimation();
            }

            m_Active = true;
        }
    }

    return exit;
}

void CKitchen::LateUpdate_GameObject(const _float& timeDelta)
{
    CBuildingBase::LateUpdate_GameObject(timeDelta);
}

void CKitchen::Render_GameObject()
{
    NULL_CHECK(m_TextureCom);

    // 0번 텍스처: 냄비
    _matrix matWorld = m_TransformCom->Get_World();
    m_GraphicDev->SetTransform(D3DTS_WORLD, &matWorld);
    m_TextureCom->Set_Texture(0);

    //CBuildingBase::Render_GameObject();

    if (!m_IsPreview)
    {
        Render_Setting();
        if (m_bHovering == true && m_bPlayerOverlap == true)
            m_SpriteEff->OutLineEffMTRX(matWorld, m_BufferCom);
    }

    if (m_BufferCom)
        m_BufferCom->Render_Buffer();

    if (!m_IsPreview)
        Render_Reset();

    // 1번 텍스처 : 화덕
    matWorld = m_TransformCom->Get_World();
    _matrix matOffset, matScale, matTrans;

    D3DXMatrixScaling(&matScale, 1.f, 1.f, 1.f);
    D3DXMatrixTranslation(&matTrans, -1.3f, 0.f, -0.3f);

    matOffset = matTrans * matScale;
    matWorld  = matOffset * matWorld;

    m_GraphicDev->SetTransform(D3DTS_WORLD, &matWorld);
    m_TextureCom->Set_Texture(1);

    if (!m_IsPreview)
    {
        Render_Setting();
        if (m_bHovering == true && m_bPlayerOverlap == true)
            m_SpriteEff->OutLineEffMTRX(matWorld, m_BufferCom);
    }

    if (m_BufferCom)
        m_BufferCom->Render_Buffer();

    if (!m_IsPreview)
        Render_Reset();

    // 2번 텍스처 : 레시피 책
    matWorld = m_TransformCom->Get_World();

    D3DXMatrixScaling(&matScale, 1.f, 1.f, 1.f);
    D3DXMatrixTranslation(&matTrans, 1.3f, -0.1f, -0.4f);

    matOffset = matScale * matTrans;
    matWorld  = matOffset * matWorld;

    m_GraphicDev->SetTransform(D3DTS_WORLD, &matWorld);
    m_TextureCom->Set_Texture(2);

    if (!m_IsPreview)
    {
        Render_Setting();
        if (m_bHovering == true && m_bPlayerOverlap == true)
            m_SpriteEff->OutLineEffMTRX(matWorld, m_BufferCom);
    }

    // CBuildingBase::Render_GameObject();

    if (m_BufferCom)
        m_BufferCom->Render_Buffer();

    if (!m_IsPreview)
        Render_Reset();

    // 3번 텍스처 : 주방 도구
    matWorld = m_TransformCom->Get_World();

    D3DXMatrixScaling(&matScale, 1.f, 1.f, 1.f);
    D3DXMatrixTranslation(&matTrans, 0.f, 0.9f, 1.1f);

    matOffset = matScale * matTrans;
    matWorld  = matOffset * matWorld;

    m_GraphicDev->SetTransform(D3DTS_WORLD, &matWorld);
    m_TextureCom->Set_Texture(3);

    if (!m_IsPreview)
    {
        Render_Setting();
        if (m_bHovering == true && m_bPlayerOverlap == true)
            m_SpriteEff->OutLineEffMTRX(matWorld, m_BufferCom);
    }

    if (m_BufferCom)
        m_BufferCom->Render_Buffer();

    if (!m_IsPreview)
        Render_Reset();

    if (m_BoxCollCom)
        m_BoxCollCom->Render();
}

void CKitchen::Render_Editor()
{
    CBuildingBase::Render_Editor();

    ImGui::Text("=== Kitchen ===");
}

void CKitchen::OnBeginOverlap(CCollider* self, CCollider* other)
{
    CBuildingBase::OnBeginOverlap(self, other);

    CPlayer* ply = dynamic_cast<CPlayer*>(other->Get_Owner());
    if (ply)
        m_bPlayerOverlap = true;
}

void CKitchen::OnStayOverlap(CCollider* self, CCollider* other)
{
    CBuildingBase::OnStayOverlap(self, other);
}

void CKitchen::OnEndOverlap(CCollider* self, CCollider* other)
{
    CBuildingBase::OnEndOverlap(self, other);

    CPlayer* ply = dynamic_cast<CPlayer*>(other->Get_Owner());

    if (m_bPlayerOverlap == true && ply != nullptr)
        m_bPlayerOverlap = false;
}

HRESULT CKitchen::Add_Component()
{
    if (FAILED(CBuildingBase::Add_Component()))
        return E_FAIL;

    // Texture
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_KITCHEN);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_KITCHEN, m_TextureCom });

    m_CookingCtrlCom = CreateProtoComponent<CCookingController>(this, COMPONENTTYPE::COOKCTRL);
    NULL_CHECK_RETURN(m_CookingCtrlCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::COOKCTRL, m_CookingCtrlCom });

    return S_OK;
}

CKitchen* CKitchen::Create(DEVICE graphicDev)
{
    auto kitchen = new CKitchen(graphicDev);

    if (FAILED(kitchen->Ready_GameObject()))
    {
        MSG_BOX("Temple Create Failed");
        Safe_Release(kitchen);
        return nullptr;
    }

    return kitchen;
}

void CKitchen::Free()
{
    CBuildingBase::Free();
}
