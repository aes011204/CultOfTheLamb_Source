#include "pch.h"
#include "CUIBuildMenu.h"
#include "CConstructionMgr.h"
#include "CCreateHelper.h"
#include "CDInputMgr.h"
#include "CFontMgr.h"
#include "CInventory.h"
#include "CRcTex.h"
#include "CRecipeDB.h"
#include "CRenderer.h"
#include "CTexture.h"
#include "CTransform.h"
#include "CUIBuildCell.h"
#include "CManagement.h"
#include "CUIIcon.h"
#include "CSoundMgr.h"

CUIBuildMenu::CUIBuildMenu(DEVICE graphicDev)
    : CUI(graphicDev),
      m_CookingFireCell(nullptr),
      m_TempleCell(nullptr),
      m_Inventory(nullptr),
      m_DecoIcon1(nullptr),
      m_DecoIcon2(nullptr),
      m_InteractionKey(nullptr),
      m_TargetX(0),
      m_ClosedX(0),
      m_Speed(0),
      m_IsOpen(false),
      m_IsAnimating(false),
      m_Cols(6),
      m_CellWidth(200.f),
      m_CellHeight(150.f)
{ }

CUIBuildMenu::CUIBuildMenu(const CUI& rhs)
    : CUI(rhs),
      m_CookingFireCell(nullptr),
      m_TempleCell(nullptr),
      m_Inventory(nullptr),
      m_DecoIcon1(nullptr),
      m_DecoIcon2(nullptr),
      m_InteractionKey(nullptr),
      m_TargetX(0),
      m_ClosedX(0),
      m_Speed(0),
      m_IsOpen(false),
      m_IsAnimating(false),
      m_Cols(0),
      m_CellWidth(0),
      m_CellHeight(0)
{ }

CUIBuildMenu::~CUIBuildMenu()
{ }

HRESULT CUIBuildMenu::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    m_TargetX = -300;
    m_ClosedX = -WINCX;
    m_Speed   = 900.f;

    //건축물 아이템 셀
    m_CookingFireCell = CUIBuildCell::Create(m_GraphicDev);
    m_CookingFireCell->Set_BuildIndex(BUILDITEM::COOKINGFIRE);
    m_CookingFireCell->Set_TextureIndex(0);
    m_CookingFireCell->Set_RecipeId(5001);
    m_CookingFireCell->Set_OnClick([this]() 
    {
        if (CheckResources(5001))
        {
            PlayCloseAnimation(); 
            CConstructionMgr::GetInstance()->Start_BuildMode(BUILDING_TYPE::KITCHEN);
            CSoundMgr::GetInstance()->Play_Once(L"UI/UI_Confirm_Selection", SOUNDTYPE::SOUND_UI, 0.7f);
        }
        else
        {
            cout << "Not enough resources for Kitchen!" << endl;
        }
    });
    m_CookingFireCell->Set_OnActive([this]() {return CheckResources(5001); });
    m_CookingFireCell->Set_BuildMenu(this);
    m_BuildCells.push_back(m_CookingFireCell);

    m_TempleCell = CUIBuildCell::Create(m_GraphicDev);
    m_TempleCell->Set_BuildIndex(BUILDITEM::TEMPLE);
    m_TempleCell->Set_TextureIndex(1);
    m_TempleCell->Set_RecipeId(5002);
    m_TempleCell->Set_OnClick([this]() 
    {
        if (CheckResources(5002))
        {
            PlayCloseAnimation(); 
            CConstructionMgr::GetInstance()->Start_BuildMode(BUILDING_TYPE::TEMPLE);
            CSoundMgr::GetInstance()->Play_Once(L"UI/UI_Confirm_Selection", SOUNDTYPE::SOUND_UI, 1.f);
        }
        else
        {
             cout << "Not enough resources for Temple!" << endl;
        }
    });
    m_TempleCell->Set_OnActive([this]() { return CheckResources(5002); });
    m_TempleCell->Set_BuildMenu(this);
    m_BuildCells.push_back(m_TempleCell);

    m_DecoIcon1 = CUIIcon::Create(m_GraphicDev, COMPONENTTYPE::TEX_UI_DECO);
    m_DecoIcon1->SetRegister(false);
    m_DecoIcon1->Get_Transform()->Set_Scale(42.f, 18.f, 0.f);
    m_DecoIcon1->Set_TextureIndex(0);

    m_DecoIcon2 = CUIIcon::Create(m_GraphicDev, COMPONENTTYPE::TEX_UI_DECO);
    m_DecoIcon2->SetRegister(false);
    m_DecoIcon2->Get_Transform()->Set_Scale(42.f, 18.f, 0.f);
    m_DecoIcon2->Set_TextureIndex(1);

    m_TransformCom->Set_Scale(450.f, 400.f, 1.f);
    m_TransformCom->Set_Pos(m_ClosedX, 0.f, 0.f);

    m_InteractionKey = CUIIcon::Create(m_GraphicDev, COMPONENTTYPE::TEX_UI_INTERACTION);
    m_InteractionKey->SetRegister(false);
    m_InteractionKey->Get_Transform()->Set_Scale(30.f, 30.f, 0.f);
    m_InteractionKey->Set_TextureIndex(0);

    return S_OK;
}

_int CUIBuildMenu::Update_GameObject(const _float& timeDelta)
{
    _int exit = CUI::Update_GameObject(timeDelta);

    if (!m_Active)
        return 0;

    if (nullptr == m_Inventory)
        m_Inventory = dynamic_cast<CInventory*>(CManagement::GetInstance()->Get_Component(
            ID_STATIC,
            LAYERTYPE::GAMELOGIC,
            OBJTYPE::PLAYER,
            COMPONENTTYPE::INVENTORY));

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

    m_DecoIcon1->Update_GameObject(timeDelta);
    m_DecoIcon2->Update_GameObject(timeDelta);
    m_InteractionKey->Update_GameObject(timeDelta);

	if (CDInputMgr::GetInstance()->KeyDown(DIK_E) && m_IsOpen == true)
	{
		CSoundMgr::GetInstance()->Play_Once(L"UI/UI_Close_Menu.wav", SOUNDTYPE::SOUND_UI, 1.f);
		PlayCloseAnimation();
	}
    float targetX = m_IsOpen ? m_TargetX : m_ClosedX; //열려있냐 닫혀있냐에 따라서 목표 X값이 달라짐
    _vec3 pos     = m_TransformCom->Get_Pos();

    if (m_IsAnimating)
    {
        float dir = targetX < pos.x ? -1.f : 1.f; //현재 x좌표에 따라서 움직일 방향이 달라짐
        pos.x += dir * m_Speed * timeDelta;

        if ((dir > 0.f && pos.x >= targetX) || (dir < 0.f && pos.x <= targetX)) //목표한 X에 도달하면 애니메이션 동작 꺼주기
        {
            pos.x         = targetX;
            m_IsAnimating = false;

            if (!m_IsOpen)
                m_Active = false;
        }
        m_TransformCom->Set_Pos(pos);
    }

    int cellsSize = m_BuildCells.size();

    int   materialRenderCount = 0;
    _vec2 BackPos             = { m_TransformCom->Get_Pos().x, m_TransformCom->Get_Pos().y };

    for (int i = 0; i < cellsSize; i++)
    {
        int   col             = materialRenderCount % m_Cols;
        float startOffset     = -80.f;
        float materialOffsetX = startOffset + col * m_CellWidth;

        m_BuildCells[i]->Update_GameObject(timeDelta);
        m_BuildCells[i]->Get_Transform()->Set_Pos(BackPos.x + materialOffsetX, BackPos.y - 20, 0.f);

        materialRenderCount++;
    }

    Check_Hovered();

    return exit;
}

void CUIBuildMenu::Render_GameObject()
{
    CUI::Render_GameObject();

    if (!m_Active)
        return;

    m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());

    //배경 그리기
    m_TextureCom->Set_Texture(0);
    m_BufferCom->Render_Buffer();;
    int cellsSize = m_BuildCells.size();
    for (int i = 0; i < cellsSize; i++)
    {
        m_BuildCells[i]->Render_GameObject();
    }

    _vec3 pos    = m_TransformCom->Get_Pos();
    _vec2 offset = { 15.f, 250.f };
    _vec2 screen = { WINCX / 2 + pos.x + offset.x, WINCY / 2 - pos.y - offset.y };

    CFontMgr::GetInstance()->Render_Font(L"NanumSquare_Big",
                                         L"건설",
                                         &screen,
                                         D3DXCOLOR{ 1.f, 1.f, 1.f, 1.f });

    _vec2 offset2 = { 180.f, 280.f };
    _vec2 screen2 = { WINCX / 2 + pos.x - offset2.x, WINCY / 2 - pos.y + offset2.y };
    CFontMgr::GetInstance()->Render_Font(L"NanumSquare_Regular",
                                         L"창 닫기",
                                         &screen2,
                                         D3DXCOLOR{ 1.f, 1.f, 1.f, 1.f });

    _vec2 decoOffset = { 100.f, 0.f };
    _vec2 keyOffset  = { 250.f, 280.f };
    m_DecoIcon1->Get_Transform()->Set_Pos(pos.x - decoOffset.x + offset.x, pos.y - decoOffset.y + offset.y, 0.f);
    m_DecoIcon2->Get_Transform()->Set_Pos(pos.x + decoOffset.x + offset.x, pos.y - decoOffset.y + offset.y, 0.f);
    m_InteractionKey->Get_Transform()->Set_Pos(pos.x - keyOffset.x, pos.y - keyOffset.y, 0.f);

    m_DecoIcon1->Render_GameObject();
    m_DecoIcon2->Render_GameObject();
    m_InteractionKey->Render_GameObject();
}

void CUIBuildMenu::LateUpdate_GameObject(const _float& timeDelta)
{
    CUI::LateUpdate_GameObject(timeDelta);
}

HRESULT CUIBuildMenu::Add_Component()
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
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_UI_INVEN);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);

    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_UI_INVEN, m_TextureCom });

    return S_OK;
}

CUIBuildMenu* CUIBuildMenu::Create(DEVICE graphicDev)
{
    auto buildMenu = new CUIBuildMenu(graphicDev);

    if (FAILED(buildMenu->Ready_GameObject()))
    {
        Safe_Release(buildMenu);
        MSG_BOX("UI_BuildMenu Create Failed");
        return nullptr;
    }
    return buildMenu;
}

void CUIBuildMenu::SetCellEvent(function<bool()> event)
{
    if (m_CookingFireCell && m_TempleCell)
    {
        m_CookingFireCell->Set_OnActive(event);
        m_TempleCell->Set_OnActive(event);
    }
}

void CUIBuildMenu::Initialize()
{ }

bool CUIBuildMenu::CheckResources(int recipeID)
{
    if (nullptr == m_Inventory)
    {
        cout << "CUIBuildMenu::CheckResources: m_Inventory is null" << endl;
        return false;
    }

    Recipe* recipe = CRecipeDB::GetInstance()->GetRecipeById(recipeID);
    if (nullptr == recipe)
    {
        cout << "CUIBuildMenu::CheckResources: Recipe not found for ID " << recipeID << endl;
        return false;
    }

    for (const auto& [itemID, count] : recipe->ingredients)
    {
        if (m_Inventory->GetItemCount(itemID) < count)
        {
            // cout << "CUIBuildMenu::CheckResources: Not enough items for " << itemID << ". Need " << count << endl;
            return false;
        }
    }

    return true;
}

void CUIBuildMenu::Check_Hovered()
{
    for (int i = 0; i < m_BuildCells.size(); i++)
    {
        _vec3 cellPos  = dynamic_cast<CTransform*>(m_BuildCells[i]->Get_Component(COMPONENTID::ID_DYNAMIC, COMPONENTTYPE::TRANSFORM))->Get_Pos();
        _vec3 cellSize = dynamic_cast<CTransform*>(m_BuildCells[i]->Get_Component(COMPONENTID::ID_DYNAMIC, COMPONENTTYPE::TRANSFORM))->Get_Scale();

        float screenX = cellPos.x + WINCX * 0.5f;
        float screenY = WINCY * 0.5f - cellPos.y;

        m_tRect.left   = static_cast<LONG>(screenX - cellSize.x);
        m_tRect.top    = static_cast<LONG>(screenY - cellSize.y);
        m_tRect.right  = static_cast<LONG>(screenX + cellSize.x);
        m_tRect.bottom = static_cast<LONG>(screenY + cellSize.y);

        POINT pt;
        GetCursorPos(&pt);
        ScreenToClient(CDInputMgr::GetInstance()->Get_HWND(), &pt);
        bool hoveredNow = PtInRect(&m_tRect, pt) != 0;

        if (hoveredNow != m_BuildCells[i]->Get_IsHovered())
        {
            if (hoveredNow&&m_BuildCells[i]->Get_IsActive()())
            {
                CSoundMgr::GetInstance()->Play_Once(L"ui/ui_pointeron.wav",
                    SOUNDTYPE::SOUND_UI,
                    5.f);
            };
            m_BuildCells[i]->Set_IsHovered(hoveredNow);
        }
    }
}

void CUIBuildMenu::PlayOpenAnimation()
{
    m_IsOpen      = true;
    m_IsAnimating = true;
}

void CUIBuildMenu::PlayCloseAnimation()
{
    m_IsOpen      = false;
    m_IsAnimating = true;
    m_Deactivate();
}

void CUIBuildMenu::Free()
{
    Safe_Release(m_DecoIcon1);
    Safe_Release(m_DecoIcon2);
    Safe_Release(m_CookingFireCell);
    Safe_Release(m_TempleCell);
    Safe_Release(m_InteractionKey);
    for (auto& cell : m_BuildCells)
    {
        Safe_Release(cell);
    }

    m_BuildCells.clear();

    CUI::Free();
}
