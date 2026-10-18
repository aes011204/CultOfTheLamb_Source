#include "pch.h"
#include "CUITabMenu.h"

#include <CCreateHelper.h>
#include <CDInputMgr.h>

#include "CMainEditorMgr.h"
#include "CUIInventory.h"
#include "CUIButton.h"
#include "CTransform.h"
#include "CRenderer.h"
#include "CUIPlayerPage.h"
#include "CSoundMgr.h"
CUITabMenu::CUITabMenu(DEVICE graphicDev)
    : CUI(graphicDev),
      m_Current(UITabCategory::INVENTORY),
      m_InvenBtn(nullptr),
      m_PlayerBtn(nullptr),
      m_GyodanBtn(nullptr),
      m_QuestBtn(nullptr),
      m_Inventory(nullptr),
      m_InventoryPage(nullptr),
      m_InvenTransformCom(nullptr),
      m_InvenBtnTransformCom(nullptr),
      m_PlayerBtnTransformCom(nullptr),
      m_TargetX(0),
      m_ClosedX(0),
      m_Speed(0),
      m_IsOpen(false),
      m_IsAnimating(false)
{}

CUITabMenu::CUITabMenu(const CUI& rhs)
    : CUI(rhs),
      m_Current(UITabCategory::INVENTORY),
      m_InvenBtn(nullptr),
      m_PlayerBtn(nullptr),
      m_GyodanBtn(nullptr),
      m_QuestBtn(nullptr),
      m_Inventory(nullptr),
      m_InventoryPage(nullptr),
      m_InvenTransformCom(nullptr),
      m_InvenBtnTransformCom(nullptr),
      m_PlayerBtnTransformCom(nullptr),
      m_TargetX(0),
      m_ClosedX(0),
      m_Speed(0),
      m_IsOpen(false),
      m_IsAnimating(false)
{ }

CUITabMenu::~CUITabMenu()
{
    
}

HRESULT CUITabMenu::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    m_TargetX = 0;
    m_ClosedX = -WINCX;
    m_Speed = 900.f;

    m_InventoryPage = CUIInventory::Create(m_GraphicDev);
    NULL_CHECK_RETURN(m_InventoryPage, E_FAIL);
    m_PlayerPage = CUIPlayerPage::Create(m_GraphicDev,this);
    NULL_CHECK_RETURN(m_PlayerPage, E_FAIL);

    m_InvenBtn = CUIButton::Create(m_GraphicDev, 3, 2);
    m_InvenBtn->Set_HasText();
    m_InvenBtn->Set_Text(L"소지품");
    m_PlayerBtn = CUIButton::Create(m_GraphicDev, 3, 2);
    m_PlayerBtn->Set_HasText();
    m_PlayerBtn->Set_Text(L"플레이어");
    m_GyodanBtn = CUIButton::Create(m_GraphicDev, 3, 2);
    m_GyodanBtn->Set_HasText();
    m_GyodanBtn->Set_Text(L"교단");
    m_QuestBtn = CUIButton::Create(m_GraphicDev, 3, 2);
    m_QuestBtn->Set_HasText();
    m_QuestBtn->Set_Text(L"임무");


    m_InvenTransformCom = dynamic_cast<CTransform*>(m_InventoryPage->Get_Component(COMPONENTID::ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));
    m_PlayerPageTransformCom = dynamic_cast<CTransform*>(m_PlayerPage->Get_Component(COMPONENTID::ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));
    m_InvenBtnTransformCom = dynamic_cast<CTransform*>(m_InvenBtn->Get_Component(COMPONENTID::ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));
    m_PlayerBtnTransformCom = dynamic_cast<CTransform*>(m_PlayerBtn->Get_Component(COMPONENTID::ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));
    m_GyodanBtnTransformCom = dynamic_cast<CTransform*>(m_GyodanBtn->Get_Component(COMPONENTID::ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));
    m_QuestBtnTransformCom = dynamic_cast<CTransform*>(m_QuestBtn->Get_Component(COMPONENTID::ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));

    m_TransformCom->Set_Pos(m_ClosedX, 0.f, 0.f);
    m_InvenBtn->SetCloneTexAspectScale({ 0.16f, 0.16f, 0.f });
    m_InvenBtn->Set_Selected(true);
    m_PlayerBtn->SetCloneTexAspectScale({ 0.16f, 0.16f, 0.f });
    m_GyodanBtn->SetCloneTexAspectScale({ 0.16f, 0.16f, 0.f });
    m_QuestBtn->SetCloneTexAspectScale({ 0.16f, 0.16f, 0.f });
    m_InvenBtn->SetOnClick([this]() {ChangePage(UITabCategory::INVENTORY); CSoundMgr::GetInstance()->Play_Once(L"UI/UI_PageFlip.wav",SOUNDTYPE::SOUND_UI,0.6f); });
    m_PlayerBtn->SetOnClick([this]() { ChangePage(UITabCategory::PLAYER); CSoundMgr::GetInstance()->Play_Once(L"UI/UI_PageFlip.wav", SOUNDTYPE::SOUND_UI,0.6f); });

    return S_OK;
} 

_int CUITabMenu::Update_GameObject(const _float& timeDelta)
{
   int exit = CUI::Update_GameObject(timeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

    if (CDInputMgr::GetInstance()->KeyDown(DIK_TAB))
    {
        m_IsOpen = !m_IsOpen;
        m_IsAnimating = true;

        switch (m_IsOpen)
        {
        case true:
            CSoundMgr::GetInstance()->Play_Once(L"UI/UI_Open_Menu.wav", SOUNDTYPE::SOUND_UI, 1.f);
            break;

        case false:
            CSoundMgr::GetInstance()->Play_Once(L"UI/UI_Close_Menu.wav", SOUNDTYPE::SOUND_UI, 1.f);
            break;
        }
        
        
    }
    
    float targetX = m_IsOpen ? m_TargetX : m_ClosedX; //열려있냐 닫혀있냐에 따라서 목표 X값이 달라짐
    _vec3 pos = m_TransformCom->Get_Pos();

    if (m_IsAnimating)
    {
        float dir = targetX < pos.x ? -1.f : 1.f; //현재 x좌표에 따라서 움직일 방향이 달라짐
        pos.x += dir * m_Speed * timeDelta;

        if ((dir > 0.f && pos.x >= targetX) || (dir < 0.f && pos.x <= targetX)) //목표한 X에 도달하면 애니메이션 동작 꺼주기
        {
            pos.x = targetX;
            m_IsAnimating = false;
        }
        m_TransformCom->Set_Pos(pos);
    }
    _vec2 invenOffset = { -300.f, 0.f };
    _vec2 invenButtonOffset = { -500.f, 250.f };
    _vec2 playerButtonOffset = { -350.f, 250.f };
    _vec2 GyodanButtonOffset = { -200.f, 250.f };
    _vec2 QuestButtonOffset = { -50.f, 250.f };

    if(m_IsOpen||m_IsAnimating)
    {
        switch (m_Current)
        {
        case UITabCategory::INVENTORY:
            m_InvenTransformCom->Set_Pos(pos.x + invenOffset.x, pos.y + invenOffset.y, 0.f);
            m_InventoryPage->Update_GameObject(timeDelta);
            break;

        case UITabCategory::PLAYER:
            m_PlayerPageTransformCom->Set_Pos(pos.x + invenOffset.x, pos.y + invenOffset.y,0.f);
            m_PlayerPage->Update_GameObject(timeDelta);
            break;
        }

        m_InvenBtnTransformCom->Set_Pos(pos.x + invenButtonOffset.x, pos.y + invenButtonOffset.y, 0.f);
        m_InvenBtn->Update_GameObject(timeDelta);
        m_PlayerBtnTransformCom->Set_Pos(pos.x + playerButtonOffset.x, pos.y + playerButtonOffset.y, 0.f);
        m_PlayerBtn->Update_GameObject(timeDelta);
        m_GyodanBtnTransformCom->Set_Pos(pos.x + GyodanButtonOffset.x, pos.y + GyodanButtonOffset.y, 0.f);
        m_GyodanBtn->Update_GameObject(timeDelta);
        m_QuestBtnTransformCom->Set_Pos(pos.x + QuestButtonOffset.x, pos.y + QuestButtonOffset.y, 0.f);
        m_QuestBtn->Update_GameObject(timeDelta);


    }

    return exit;
}

void CUITabMenu::ChangePage(UITabCategory newTab)
{
    m_Current = newTab;

    m_InvenBtn->Set_Selected(newTab == UITabCategory::INVENTORY);
    m_PlayerBtn->Set_Selected(newTab == UITabCategory::PLAYER);
}

HRESULT CUITabMenu::Add_Component()
{
    // transform
    m_TransformCom = CreateProtoComponent<CTransform>(this, COMPONENTTYPE::TRANSFORM);
    NULL_CHECK_RETURN(m_TransformCom, E_FAIL);

    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::TRANSFORM, m_TransformCom });

    return S_OK;
}

void CUITabMenu::Render_GameObject()
{
    CUI::Render_GameObject();

    if (!m_IsOpen && !m_IsAnimating)
      return;

    switch(m_Current)
    {
    case UITabCategory::INVENTORY:
        m_InventoryPage->Render_GameObject();
        break;

    case UITabCategory::PLAYER:
        m_PlayerPage->Render_GameObject();
        break;
    }

    m_InvenBtn->Render_GameObject();
    m_PlayerBtn->Render_GameObject();
    m_GyodanBtn->Render_GameObject();
    m_QuestBtn->Render_GameObject();

}
void CUITabMenu::SetInventory(CInventory* inventory)
{
    m_Inventory = inventory;
    m_InventoryPage->SetInventory(m_Inventory);
    if (!m_Inventory)
        return;
}
    
void CUITabMenu::LateUpdate_GameObject(const _float& timeDelta)
{
    CUI::LateUpdate_GameObject(timeDelta);
}

CUITabMenu* CUITabMenu::Create(DEVICE graphicDev)
{
    auto tabMenu = new CUITabMenu(graphicDev);

    if (FAILED(tabMenu->Ready_GameObject()))
    {
        Safe_Release(tabMenu);
        MSG_BOX("CUITabMenu Create Failed");
        return nullptr;
    }

    return tabMenu;
}

void CUITabMenu::Initialize()
{
    m_PlayerPage->Initialize();

}

void CUITabMenu::Free()
{
    Safe_Release(m_InvenBtn);
    Safe_Release(m_PlayerBtn);
    Safe_Release(m_GyodanBtn);
    Safe_Release(m_QuestBtn);
    Safe_Release(m_InventoryPage);
    Safe_Release(m_PlayerPage);

    CUI::Free();
}
