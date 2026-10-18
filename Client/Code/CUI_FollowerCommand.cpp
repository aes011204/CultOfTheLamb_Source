#include "pch.h"
#include "CUI_FollowerCommand.h"
#include "CNPCBase.h"
#include "CNPCController.h"
#include "CRcTex.h"
#include "CCreateHelper.h"
#include "CDInputMgr.h"
#include "CUI.h"
#include "CTransform.h"
#include "CTexture.h"
#include "CRenderer.h"
#include "CUIButton.h"
#include "CBoxCollider.h"

CUI_FollowerCommand::CUI_FollowerCommand(DEVICE graphicDev)
    : CUI(graphicDev),
      m_PrayButton(nullptr),
      m_MiningButton(nullptr),
      m_AxeButton(nullptr),
      m_TargetNPC(nullptr)
{}

CUI_FollowerCommand::CUI_FollowerCommand(const CUI_FollowerCommand& rhs)
    : CUI(rhs),
      m_PrayButton(nullptr),
      m_MiningButton(nullptr),
      m_AxeButton(nullptr),
      m_TargetNPC(nullptr)
{}

void CUI_FollowerCommand::Initialize_GameObject()
{
    CUI::Initialize_GameObject();

    m_TransformCom->Set_Scale(100.f, 100.f, 50.f);

    m_PrayButton = CUIButton::Create(m_GraphicDev, 8, 7);
    m_PrayButton->SetCloneTexAspectScale(_vec3(0.05f, 0.17f, 0.1f));

    m_PrayButton->SetOnClick([this]() {
        if (m_TargetNPC)
        {
            // NPC에게 기도 작업 지시
            if (auto controller = m_TargetNPC->Get_Controller())
            {
                controller->Set_Work(CNPCController::NPC_WORK::PRAY);
                cout << "PrayButton Click" << endl;
            }
            m_TargetNPC = nullptr;
            Set_IsActive(false);
        }
    });

    
    m_AxeButton = CUIButton::Create(m_GraphicDev, 10, 9);
    m_AxeButton->SetCloneTexAspectScale(_vec3(0.05f, 0.17f, 0.f));

    m_AxeButton->SetOnClick([this]() {
         if (m_TargetNPC)
         {
            if (auto controller = m_TargetNPC->Get_Controller())
            {
                controller->Set_Work(CNPCController::NPC_WORK::CHOPWOOD);
                cout << "AxeButton Click" << endl;
            }
            m_TargetNPC = nullptr;
            Set_IsActive(false);
         } 
    });
    
    m_MiningButton = CUIButton::Create(m_GraphicDev, 12, 11);
    m_MiningButton->SetCloneTexAspectScale(_vec3(0.05f, 0.17f, 0.f));

    m_MiningButton->SetOnClick([this]() {
         if (m_TargetNPC)
         {
             if (auto controller = m_TargetNPC->Get_Controller())
             {
                 controller->Set_Work(CNPCController::NPC_WORK::MINING);
                 cout << "MiningButton Click" << endl;
             }
             m_TargetNPC = nullptr;
             Set_IsActive(false);
         }
    });

    _vec3 pos = m_TransformCom->Get_Pos();
    _vec3 offset1 = { 0.f,40.f,0.f };
    _vec3 offset2 = { 40.f,40.f,0.f };
    _vec3 offset3 = { 40.f,40.f,0.f };
    
    m_PrayButton->Get_Transform()->Set_Pos(_vec3(pos.x, pos.y + offset1.y, 0.f));
    m_AxeButton->Get_Transform()->Set_Pos(_vec3(pos.x - offset2.x, pos.y - offset2.y, 0.f));
    m_MiningButton->Get_Transform()->Set_Pos(_vec3(pos.x + offset3.x, pos.y - offset3.y, 0.f));

    m_Active = false;

}

void CUI_FollowerCommand::OnBeginOverlap(CCollider* self, CCollider* other)
{
    CUI::OnBeginOverlap(self, other);
}

void CUI_FollowerCommand::OnStayOverlap(CCollider* self, CCollider* other)
{
    CUI::OnStayOverlap(self, other);
}

void CUI_FollowerCommand::OnEndOverlap(CCollider* self, CCollider* other)
{
    CUI::OnEndOverlap(self, other);
}

CUI_FollowerCommand::~CUI_FollowerCommand()
{
    CUI_FollowerCommand::Free();
}

HRESULT CUI_FollowerCommand::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    return S_OK;
}

_int CUI_FollowerCommand::Update_GameObject(const _float& timeDelta)
{
    if (!m_Active)
        return 0;

    if (CDInputMgr::GetInstance()->MouseDown(DIM_RB))
    {
        m_TargetNPC = nullptr;
        m_Active = false;
    }

    _int exit = CUI::Update_GameObject(timeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);


    m_PrayButton->Update_GameObject(timeDelta);
    m_AxeButton->Update_GameObject(timeDelta);
    m_MiningButton->Update_GameObject(timeDelta);


    return exit;
}

void CUI_FollowerCommand::LateUpdate_GameObject(const _float& timeDelta)
{
    CUI::LateUpdate_GameObject(timeDelta);
}

void CUI_FollowerCommand::Render_GameObject()
{
    if (!m_Active)
        return ;

    CUI::Render_GameObject();

    const _matrix& world = m_TransformCom->Get_World();
    m_GraphicDev->SetTransform(D3DTS_WORLD, &world);

    m_TextureCom->Set_Texture(0);
    m_BufferCom->Render_Buffer();

    m_PrayButton->Render_GameObject();
    m_AxeButton->Render_GameObject();
    m_MiningButton->Render_GameObject();

}

CUI_FollowerCommand* CUI_FollowerCommand::Create(DEVICE graphicDev)
{
    CUI_FollowerCommand* followerCommand = new CUI_FollowerCommand(graphicDev);

    if (FAILED(followerCommand->Ready_GameObject()))
    {
        Safe_Release(followerCommand);
        MSG_BOX("CUI_FollowerCommand Create Failed");
        return nullptr;
    }

    return followerCommand;
}

HRESULT CUI_FollowerCommand::Add_Component()
{

    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_UI_COMMAND);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_UI_COMMAND, m_TextureCom });

    m_TransformCom = CreateProtoComponent<CTransform>(this, COMPONENTTYPE::TRANSFORM);
    NULL_CHECK_RETURN(m_TransformCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::TRANSFORM, m_TransformCom });

    m_BufferCom = CreateProtoComponent<CRcTex>(this, COMPONENTTYPE::RC_TEX);
    NULL_CHECK_RETURN(m_BufferCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::RC_TEX, m_BufferCom });

    return S_OK;
}

void    CUI_FollowerCommand::Free()
{
    Safe_Release(m_PrayButton);
    Safe_Release(m_MiningButton);
    Safe_Release(m_AxeButton);
    CUI::Free();
}
