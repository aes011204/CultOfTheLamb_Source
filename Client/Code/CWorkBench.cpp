#include "pch.h"
#include "CWorkBench.h"

#include "CBoxCollider.h"
#include "CCreateHelper.h"
#include "CDInputMgr.h"
#include "CDInputMgr.h"
#include "CPlayer.h"
#include "CRcTex.h"
#include "CSpriteEff.h"
#include "CTexture.h"
#include "CTransform.h"
#include "CUIBuildMenu.h"
#include "CSoundMgr.h"

CWorkBench::CWorkBench(DEVICE graphicDev)
    : CBuildingBase(graphicDev),
      m_SpriteEffCom(nullptr),
      m_BuildMenu(nullptr)
{ }

CWorkBench::CWorkBench(const CWorkBench& rhs)
    : CBuildingBase(rhs),
      m_SpriteEffCom(nullptr),
      m_BuildMenu(nullptr)
{ }

CWorkBench::~CWorkBench()
{ }

HRESULT CWorkBench::Ready_GameObject()
{
    if (FAILED(CBuildingBase::Ready_GameObject()))
        return E_FAIL;

    return S_OK;
}

void CWorkBench::Initialize_GameObject()
{
    CBuildingBase::Initialize_GameObject();

    m_TransformCom->Set_Pos(12.7f, 1.1f, 24.7f);
    m_TransformCom->Set_Scale(1.2f, 1.2f, 1.2f);
    m_TransformCom->Set_RotationAxis(ROT_X, D3DXToRadian(30.f));

    m_BoxCollCom->Set_RawSize(_vec3(2.0f, 2.0f, 2.0f));
    m_BoxCollCom->Set_CollisionLayer(COLLISION_LAYERTYPE::WORKBENCH);

    m_BuildMenu->SetOnDeactivate([this]() { m_Active = false; });
}

_int CWorkBench::Update_GameObject(const _float& timeDelta)
{
    _int exit = CBuildingBase::Update_GameObject(timeDelta);

    if (m_bPlayerOverlap == true && m_bHovering == true)
    {
        if (CDInputMgr::GetInstance()->MouseDown(DIM_LB))
        {
            CSoundMgr::GetInstance()->Play_Once(L"UI/UI_Open_Menu.wav", SOUNDTYPE::SOUND_UI, 1.f);
            m_BuildMenu->Set_IsActive(true);
            m_BuildMenu->PlayOpenAnimation();

            m_Active = true;
        }
    }

    return exit;
}

void CWorkBench::LateUpdate_GameObject(const _float& timeDelta)
{
    CBuildingBase::LateUpdate_GameObject(timeDelta);
}

void CWorkBench::Render_GameObject()
{
    //if (m_bHovering == true)
    //{
    //    m_SpriteEffCom->OutLineEff(m_BufferCom);

    //    m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());

    //    if (m_BufferCom)
    //        m_BufferCom->Render_Buffer();
    //}

    NULL_CHECK(m_TextureCom);
    m_TextureCom->Set_Texture(0);

    CBuildingBase::Render_GameObject();
}

void CWorkBench::Render_Editor()
{
    CBuildingBase::Render_Editor();

    ImGui::Text("=== WorkBench ===");
}

void CWorkBench::OnBeginOverlap(CCollider* self, CCollider* other)
{
    CBuildingBase::OnBeginOverlap(self, other);

    CPlayer* ply = dynamic_cast<CPlayer*>(other->Get_Owner());
    if (ply)
        m_bPlayerOverlap = true;
}

void CWorkBench::OnStayOverlap(CCollider* self, CCollider* other)
{
    CBuildingBase::OnStayOverlap(self, other);
}

void CWorkBench::OnEndOverlap(CCollider* self, CCollider* other)
{
    CBuildingBase::OnEndOverlap(self, other);

    CPlayer* ply = dynamic_cast<CPlayer*>(other->Get_Owner());

    if (m_bPlayerOverlap == true && ply != nullptr)
        m_bPlayerOverlap = false;
}

HRESULT CWorkBench::Add_Component()
{
    // Texture
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_WORKBENCH);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_WORKBENCH, m_TextureCom });

    return CBuildingBase::Add_Component();
}

CWorkBench* CWorkBench::Create(DEVICE graphicDev)
{
    auto workBench = new CWorkBench(graphicDev);

    if (FAILED(workBench->Ready_GameObject()))
    {
        MSG_BOX("DottedCircle Create Failed");
        Safe_Release(workBench);
        return nullptr;
    }

    return workBench;
}

void CWorkBench::Free()
{
    Safe_Release(m_BuildMenu);
    CBuildingBase::Free();
}
