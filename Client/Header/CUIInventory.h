#pragma once
#include "CUI.h"

class CUIButton;
class CUIIcon;
class CUIInventoryCell;
class CInventory;

class CUIInventory : public CUI
{
private:
    explicit CUIInventory(DEVICE graphicDev);
    explicit CUIInventory(const CUIInventory& rhs);
    ~CUIInventory() override;

public:
void    Render_Editor() override;
HRESULT Ready_GameObject() override;
_int    Update_GameObject(const _float& timeDelta) override;
void    LateUpdate_GameObject(const _float& timeDelta) override;
void    Render_GameObject() override;

public:
void     SetInventory(CInventory* inventory);
void    SetPositions();
HRESULT  Add_Component();
void    Check_Hovered();
void CreateCells();

    static CUIInventory* Create(DEVICE graphicDev);

protected:
    void Free() override;

private:
    int     m_Cols;                         // 한 줄에 몇 칸?
    float   m_CellWidth;                    // 셀 가로 이동거리
    float   m_CellHeight;                   // 셀 세로 이동거리

    CInventory* m_Inventory;
    vector<CUIInventoryCell*> m_Cells;
};
