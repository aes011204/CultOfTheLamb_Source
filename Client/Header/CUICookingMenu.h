#pragma once
#include "CUI.h"

class CCookingController;
class CInventory;
class CUICookedFoodCell;
struct Recipe;
class CUIButton;
class CUICookingCell;

class CUICookingMenu : public CUI
{
private:
    explicit CUICookingMenu(DEVICE graphicDev);
    explicit CUICookingMenu(const CUICookingMenu& rhs) = delete;
    ~CUICookingMenu() override;

public:
    HRESULT                Ready_GameObject() override;
    void                   Initialize_GameObject() override;
    _int                   Update_GameObject(const _float& timeDelta) override;
    void                   LateUpdate_GameObject(const _float& timeDelta) override;
    void                   Render_GameObject() override;
    void                   PlayOpenAnimation();
    void                   PlayCloseAnimation();
    bool                   Get_IsOpen() { return m_IsOpen; }
    void                   Set_CookingController(CCookingController* controller) { m_CookingController = controller; }
    void                   Check_Hovered();
    HRESULT                Add_Component();
    static CUICookingMenu* Create(DEVICE graphicDev);
    void                   SetOnDeactivate(function<void()> deactivate) { m_Deactivate = deactivate; }
    void                   Free() override;
private:
    vector<CUICookingCell*>    m_CookingCells;
    vector<CUICookedFoodCell*> m_CookedFoodCells;
    //vector<int>             m_RecipeIds;
    CUIButton* m_StartButton{};
    CUIButton* m_CancelButton{};

    CInventory*         m_Inventory;
    CCookingController* m_CookingController;
    float               m_TargetX{};
    float               m_ClosedX{};
    float               m_Speed{};
    bool                m_IsOpen{};
    bool                m_IsAnimating{};

    int m_Cols;
    int m_CellWidth;
    int m_CellHeight;

    function<void()> m_Deactivate;
};
