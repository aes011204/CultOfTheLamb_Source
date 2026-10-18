#pragma once
#include "CComponent.h"
#include "CItemDB.h"
#include "ItemInstance.h"
#include <functional>

BEGIN(Engine)
class CScene;
END

struct InventorySlot
{
    ItemType     SlotType;        // Material, Food, Weapon…
    ItemInstance itemInst;    // 들어 있는 아이템 (없으면 id = -1)
    int          count = 0;
};

class CInventory : public CComponent
{
private:
    explicit CInventory(DEVICE graphicDev);

    explicit CInventory(const CInventory& rhs);
    ~CInventory() override;

public:
    virtual HRESULT Ready_Inventory();
    void            SetInvenSlotNum(int slotNum); //무조건 만들어야함 

    void DropItemfromSlot(int slotNum, _vec3 DropPos);
    bool AddItem(ItemInstance itemInst);
    int  FindEmptySlot(ItemType itemtype);
    bool RemoveItem(int itemID, int count);
    bool RemoveItemBySlot(int slotNum, int count);

    void AddItemOnMap(int itemID);
    bool RemoveItemOnMap(int itemID, int Num);
    
    int  GetItemCount(int itemID);
    //void DropItemFromCount(int DropItemNum, _vec3 DropPos, CScene* nowScene);
    // void DropItem(int DropItemNum); // 오너가 죽엇을떄 이거 호출하면 드롭 아이템으로 -1 이면 전부
    //int Get_AddedCount() { return m_AddedCount; }



    const vector<InventorySlot>& GetVector() const { return m_vecInven; }
    const unordered_map<int,int>& GetCountMap() const { return m_CountingItem; }
    void SetOnItemAdded(std::function<void(ItemInstance*/*, int*/)> callback)
    {
        m_UIItemAdded = callback;
    }
public:
    static CInventory* Create(DEVICE graphicDev);
    CComponent*        Clone() override;
private:
    vector<InventorySlot> m_vecInven;
    unordered_map<int, int> m_CountingItem;// id,Num 수량 조사용
    int                   invenSlotNum;
    //int                   m_AddedCount;


    std::function<void(ItemInstance*/*, int*/)>      m_UIItemAdded;
public:
    void Free() override;
};

