#pragma once

#include "CUI.h"

class CUICookingMenu;
class CUIIcon;
class CUIBuildCell;

class CUICookingCell : public CUI
{
private:
    explicit CUICookingCell(DEVICE graphicDev);
    explicit CUICookingCell(const CUICookingCell& rhs);
    ~CUICookingCell() override;

public:
    HRESULT Ready_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;
    void    Render_Editor() override;

    HRESULT Add_Component();

    static CUICookingCell* Create(DEVICE graphicDev);

    void    Set_IsHovered(bool tf) { m_IsHovered = tf; }
    bool    Get_IsHovered() { return m_IsHovered; }
    void    Set_RecipeId(int recipeId) { m_RecipeId = recipeId; }
    void    Set_TextureIndex(int textureIndex) { m_TextureIndex = textureIndex; }
    void    Set_OnClick(function<void()> onclick) { m_OnClick = onclick; }
    void    Set_OnActive(function<bool()> active) { m_IsActive = active; }
    void    Set_GetCount(function<int()> getCount) { m_GetCount = getCount; }
    void    Set_CookingMenu(CUICookingMenu* cookingmenu) { m_CookingMenu = cookingmenu; }

    void Free() override;

private:
    CUICookingMenu* m_CookingMenu;
    CUIIcon*        m_ItemIcon;
    CUIIcon*        m_CardIcon;
    CUIIcon*        m_IngredientIcon;
    CUIIcon*        m_CardItemIcon;
    CUIIcon*        m_Highlight;

    int             m_TextureIndex;
    int             m_RecipeId;
    bool            m_IsHovered;

    _vec3           m_TargetScale;
    _vec3           m_BaseScale;


    _vec3           m_ItemTargetScale;
    _vec3           m_ItemBaseScale;

    function<void()> m_OnClick;
    function<bool()> m_IsActive;
    function<int()> m_GetCount;
};


