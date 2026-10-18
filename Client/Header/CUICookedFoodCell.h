#pragma once
#include "Client_Struct.h"
#include "CUI.h"

class CUIIcon;

class CUICookedFoodCell : public CUI
{
private:
    explicit CUICookedFoodCell(DEVICE graphicDev);
    explicit CUICookedFoodCell(const CUICookedFoodCell& rhs);
    ~CUICookedFoodCell() override;

public:
    HRESULT Ready_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;
    void    Render_Editor() override;
    void    Set_RecipeId(int recipeId) { m_RecipeId = recipeId; }
    HRESULT Add_Component();

    static CUICookedFoodCell* Create(DEVICE graphicDev);


protected:
    void Free() override;

private:
    CUIIcon* m_ItemIcon;
    int      m_RecipeId;
    RecipeInst m_RecipeInst;
};


