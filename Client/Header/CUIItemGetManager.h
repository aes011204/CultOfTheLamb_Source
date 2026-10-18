#pragma once
#include "CGameObject.h"

class CInventory;
struct ItemInstance;
class CUIItemGet;

class CUIItemGetManager : public CGameObject
{
private:
    explicit CUIItemGetManager(DEVICE graphicDev);
    explicit CUIItemGetManager(const CUIItemGetManager& rhs);
    ~CUIItemGetManager() override;

public:
    HRESULT Ready_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;
    void    Add_UIItemGet(ItemInstance* itemInstance, int itemCount);
    void    SetInventory(CInventory* inventory) { m_Inventory = inventory; }
    void    Initialize();
    static CUIItemGetManager* Create(DEVICE graphicDev);
protected:
    void Free() override;

private:
    list<CUIItemGet*> m_UIItemGets;
    CInventory* m_Inventory;
    bool    m_IsAnyShifting;
    bool   m_NeedsReindex;

};


