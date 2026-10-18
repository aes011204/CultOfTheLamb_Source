
#include "pch.h"
#include "CUICookingMenu.h"
#include "CCookingController.h"
#include "CTexture.h"
#include "CRcTex.h"
#include "CTransform.h"
#include "CCreateHelper.h"
#include "CRecipeDB.h"
#include "CUICookingCell.h"
#include "CUIButton.h"
#include "CDInputMgr.h"
#include "CRenderer.h"
#include "CFontMgr.h"
#include "CInventory.h"
#include "CUICookedFoodCell.h"
#include "CManagement.h"
#include "CSoundMgr.h"

CUICookingMenu::CUICookingMenu(DEVICE graphicDev)
    : CUI(graphicDev),
      m_CookingController(nullptr),
      m_Cols(6),
      m_CellWidth(100.f),
      m_CellHeight(100.f)
{}

CUICookingMenu::~CUICookingMenu() {}

HRESULT CUICookingMenu::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    m_TargetX = -300;
    m_ClosedX = -WINCX;
    m_Speed = 900.f;

    m_TransformCom->Set_Scale(450.f, 400.f, 1.f);
    m_TransformCom->Set_Pos(m_ClosedX, 0.f, 1.f);
	m_Active = false;

    return S_OK;
}

void CUICookingMenu::Initialize_GameObject()
{
    CUI::Initialize_GameObject();
    
   // m_RecipeIds.resize(6);
    m_CookingController = dynamic_cast<CCookingController*>(CManagement::GetInstance()->Get_Component(ID_DYNAMIC, LAYERTYPE::GAMELOGIC, OBJTYPE::BUILDING, COMPONENTTYPE::COOKCTRL));
    vector<Recipe> recipes = CRecipeDB::GetInstance()->GetVector();

    for (int i = 0; i < 6; i++)
    {
        auto* cookingCell = CUICookingCell::Create(m_GraphicDev);
        cookingCell->Set_CookingMenu(this);
        int texindex = 0;

        if (i < 3)
        {
            int recipeId = recipes[i].id;
            int IngredientId = CRecipeDB::GetInstance()->GetRecipeById(recipeId)->ingredients.begin()->first;

            cookingCell->Set_OnClick([this, recipeId]()
            {if (m_CookingController->CanCook(recipeId, m_Inventory))
                {
                m_CookingController->AddRecipe(recipeId, m_Inventory);
                CSoundMgr::GetInstance()->Play_Once(L"UI/UI_Confirm_Selection.wav",
                    SOUND_EFFECT,
                    0.7f);
                } });
            cookingCell->Set_OnActive([this, recipeId]() { return m_CookingController->CanCook(recipeId, m_Inventory); });
            cookingCell->Set_GetCount([this, IngredientId]() {return m_Inventory->GetItemCount(IngredientId); });
            cookingCell->Set_RecipeId(recipeId);
            cookingCell->Get_Transform()->Set_Pos(m_ClosedX, 0.f, 0.f);
        }
        else
        {
       
            cookingCell->Set_RecipeId(-1);
            cookingCell->Get_Transform()->Set_Pos(m_ClosedX, 0.f, 0.f);
        }

        m_CookingCells.push_back(cookingCell);
    }


    for (int i = 0; i < 12; i++)
    {
        auto* cookedFoodCell = CUICookedFoodCell::Create(m_GraphicDev);
        cookedFoodCell->Get_Transform()->Set_Pos(m_ClosedX, 0.f, 0.f);
        m_CookedFoodCells.push_back(cookedFoodCell);
    }


    m_StartButton = CUIButton::Create(m_GraphicDev, 3, 2);
    m_StartButton->Set_HasText();
    m_StartButton->Set_Text(L"요리하기");
    m_StartButton->SetCloneTexAspectScale({ 0.17f, 0.22f, 0.f });
    m_StartButton->SetOnClick([this]() { 
        PlayCloseAnimation();  
        m_CookingController->SetUIRecipeSlot(0, m_CancelButton, m_StartButton); // 콜백 재설정 
        m_CookingController->Set_Start(true);
        CSoundMgr::GetInstance()->Play_Once(L"UI/UI_Confirm_Selection.wav",
            SOUND_EFFECT,
            0.7f);
    });
    //m_StartButton->Set_Selected(true);

    m_CancelButton = CUIButton::Create(m_GraphicDev, 3, 2);
    m_CancelButton->Set_HasText();
    m_CancelButton->Set_Text(L"취소");
    m_CancelButton->SetCloneTexAspectScale({ 0.17f, 0.22f, 0.f });
    m_CancelButton->SetOnClick([this]() { PlayCloseAnimation(); m_CookingController->RemoveAllRecipe(m_Inventory);
                                          m_CookingController->Quit();
                                          CSoundMgr::GetInstance()->Play_Once(L"UI/UI_Close_Menu.wav", SOUNDTYPE::SOUND_UI, 1.f); });

}

_int CUICookingMenu::Update_GameObject(const _float& timeDelta)
{
    int exit = CUI::Update_GameObject(timeDelta);

    if (!m_Active)
        return 0;

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);
    m_Inventory = dynamic_cast<CInventory*>(CManagement::GetInstance()->Get_Component(ID_STATIC, LAYERTYPE::GAMELOGIC, OBJTYPE::PLAYER, COMPONENTTYPE::INVENTORY));

    if (nullptr == m_CookingController)
        return exit;

    vector<int> RecipeQueue = m_CookingController->Get_Vector();
   
     for (int i = 0; i < m_CookedFoodCells.size(); i++)
     {
         if(i< RecipeQueue.size())
         {
            m_CookedFoodCells[i]->Set_RecipeId(RecipeQueue[i]);
         }
         else
         {
             m_CookedFoodCells[i]->Set_RecipeId(-1);
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

            if (!m_IsOpen)
                m_Active = false;
        }
        m_TransformCom->Set_Pos(pos);
    }

    int cellsSize = m_CookingCells.size();

    int RenderCount = 0;

    _vec2 BackPos = { m_TransformCom->Get_Pos().x, m_TransformCom->Get_Pos().y };

    for (int i = 0; i < cellsSize; i++)
    {
        int col = RenderCount % m_Cols;
        _vec2 offset = { -120.f,0.f };
        float recipeOffsetX = offset.x + col * m_CellWidth;

        m_CookingCells[i]->Update_GameObject(timeDelta);
        m_CookingCells[i]->Get_Transform()->Set_Pos(BackPos.x + recipeOffsetX + offset.x, BackPos.y + 160, 0.f);

        RenderCount++;
    }

    int cookedCellsSize = m_CookedFoodCells.size();

    int RenderCount2 = 0;

    for (int i = 0; i < cookedCellsSize; i++)
    {
        int col2 = RenderCount2 % m_Cols;
        int row = i / m_Cols;
        _vec2 offset = { -120.f,0.f };
        float foodOffsetX = offset.x + col2 * m_CellWidth;
        float foodOffsetY = row * m_CellHeight;
        m_CookedFoodCells[i]->Update_GameObject(timeDelta);
        m_CookedFoodCells[i]->Get_Transform()->Set_Pos(BackPos.x + foodOffsetX + offset.x, BackPos.y -60 - foodOffsetY, 0.f);

        RenderCount2++;
    }
    _vec2 offset1 = { -100.f,-300.f };
    _vec2 offset2 = { +100.f,-300.f };

    m_StartButton->Get_Transform()->Set_Pos(pos.x + offset1.x, pos.y + offset1.y, 0.f);
    m_CancelButton->Get_Transform()->Set_Pos(pos.x + offset2.x, pos.y+ offset2.y, 0.f);

    m_StartButton->Update_GameObject(timeDelta);
    m_CancelButton->Update_GameObject(timeDelta);

    Check_Hovered();

    return exit;
}

void CUICookingMenu::Check_Hovered()
{
    for (int i = 0; i < m_CookingCells.size(); i++)
    {
        _vec3 cellPos = dynamic_cast<CTransform*>(m_CookingCells[i]->Get_Component(COMPONENTID::ID_DYNAMIC, COMPONENTTYPE::TRANSFORM))->Get_Pos();
        _vec3 cellSize = dynamic_cast<CTransform*>(m_CookingCells[i]->Get_Component(COMPONENTID::ID_DYNAMIC, COMPONENTTYPE::TRANSFORM))->Get_Scale();

        float screenX = cellPos.x + WINCX * 0.5f;
        float screenY = WINCY * 0.5f - cellPos.y;

        m_tRect.left = (LONG)(screenX - cellSize.x);
        m_tRect.top = (LONG)(screenY - cellSize.y);
        m_tRect.right = (LONG)(screenX + cellSize.x);
        m_tRect.bottom = (LONG)(screenY + cellSize.y);

        POINT pt;
        GetCursorPos(&pt);
        ScreenToClient(CDInputMgr::GetInstance()->Get_HWND(), &pt);
        bool hoveredNow = PtInRect(&m_tRect, pt) != 0;

        if (hoveredNow != m_CookingCells[i]->Get_IsHovered())
        {
			if (hoveredNow)
			{
                CSoundMgr::GetInstance()->Play_Once(L"ui/ui_pointeron.wav",
                    SOUNDTYPE::SOUND_UI,
                    5.f);
			}
            m_CookingCells[i]->Set_IsHovered(hoveredNow);
        }

    }
}


void CUICookingMenu::LateUpdate_GameObject(const _float& timeDelta)
{
    CUI::LateUpdate_GameObject(timeDelta);
}

void CUICookingMenu::Render_GameObject()
{
    CUI::Render_GameObject();

    if (!m_Active)
        return;

    m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());

    //배경 그리기
    m_TextureCom->Set_Texture(0);
    m_BufferCom->Render_Buffer();

    int cellsSize = m_CookingCells.size();
    for (int i = 0; i < cellsSize; i++)
    {
        m_CookingCells[i]->Render_GameObject();
    }


    int cookedCellsSize = m_CookedFoodCells.size();
    for (int i = 0; i < cookedCellsSize; i++)
    {
        m_CookedFoodCells[i]->Render_GameObject();
    }

    _vec3 pos = m_TransformCom->Get_Pos();
    _vec2 offset = { 245,250.f };
    _vec2 screen = { WINCX / 2 + pos.x - offset.x, WINCY / 2 - pos.y - offset.y };

    CFontMgr::GetInstance()->Render_Font(L"NanumSquare_Large",
        L"레시피",
        &screen,
        D3DXCOLOR{ 0.6f, 0.6f, 0.6f, 0.6f });

    _vec2 offset2 = { 255,-30.f };
    _vec2 screen2 = { WINCX / 2 + pos.x - offset2.x, WINCY / 2 - pos.y + offset2.y };

    CFontMgr::GetInstance()->Render_Font(L"NanumSquare_Large",
        L"요리",
        &screen2,
        D3DXCOLOR{ 0.6f, 0.6f, 0.6f, 0.6f });

    m_StartButton->Render_GameObject();
    m_CancelButton->Render_GameObject();

}

HRESULT CUICookingMenu::Add_Component()
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

CUICookingMenu* CUICookingMenu::Create(DEVICE graphicDev)
{
    auto cookingMenu = new CUICookingMenu(graphicDev);

    if (FAILED(cookingMenu->Ready_GameObject()))
    {
        Safe_Release(cookingMenu);
        MSG_BOX("UI_CookingMenu Create Failed");
        return nullptr;
    }

    return cookingMenu;
}

void CUICookingMenu::Free()
{
    CUI::Free();
    for (auto cell : m_CookingCells)
    {
        Safe_Release(cell);
    }
    m_CookingCells.clear();

    for (auto cell : m_CookedFoodCells)
    {
        Safe_Release(cell);
    }

    m_CookedFoodCells.clear();
    // m_RecipeIds.clear();

    Safe_Release(m_StartButton);
    Safe_Release(m_CancelButton);

    m_Inventory = nullptr;          
    m_CookingController = nullptr;  

    CUI::Free();
}

void            CUICookingMenu::PlayOpenAnimation()
{
    m_IsOpen = true;
    m_IsAnimating = true;
}
void CUICookingMenu::PlayCloseAnimation()
{
    m_IsOpen = false;
    m_IsAnimating = true;
   // m_Deactivate();
}
