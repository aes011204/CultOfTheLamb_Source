#include "pch.h"
#include "CInventory.h"
#include "CDropSystem.h"
#include "CItemDB.h"
#include "CPlayer.h"
#include "CUIItemGetManager.h"
#include "ItemData.h"
#include "CCombatStat.h"

CInventory::CInventory(DEVICE graphicDev)
    : CComponent(graphicDev),
    invenSlotNum(0)
{
}

CInventory::CInventory(const CInventory& rhs)
    : CComponent(rhs),
    invenSlotNum(0)
{
}

CInventory::~CInventory()
{
}

HRESULT CInventory::Ready_Inventory()
{
    return S_OK;
}

void CInventory::SetInvenSlotNum(int slotNum)
{
    //클론 한 뒤에 무조건 호출 해야합니다.
    CPlayer* player = dynamic_cast<CPlayer*>(m_Owner);
    if (player != nullptr)
    {
        //플레이어인 경우
        invenSlotNum = 12;
        m_vecInven.resize(invenSlotNum);

        for (int i = 0; i < invenSlotNum; i++)
        {
            if (i < invenSlotNum / 2)
                m_vecInven[i].SlotType = ItemType::Material;
            else
                m_vecInven[i].SlotType = ItemType::FoodMaterial;
        }
    }
    else
    {
        //플레이어가 아닌경우 ex)몬스터
        invenSlotNum = slotNum;
        m_vecInven.resize(invenSlotNum);

        for (int i = 0; i < invenSlotNum; i++)
        {
            m_vecInven[i].SlotType = ItemType::None;
        }
    }
}

//void CInventory::DropItemFromCount(int DropItemNum, _vec3 DropPos, CScene* nowScene)
//{
//    //int size;
//    //
//    //if (DropItemNum == -1 || m_vecInven.size() <= DropItemNum)// 모든 아이템 드랍
//    //{
//    //    CDropSystem::GetInstance()->SpawnAllFromInven(m_GraphicDev, *this);
//    //}
//    //else
//    //    size = DropItemNum;
//    //m_vecInven
//}

void CInventory::DropItemfromSlot(int slotNum, _vec3 DropPos)
{
    int size;

    if (slotNum < 0 || slotNum >= m_vecInven.size())  // 범위 체크 먼저
        return;

    if (m_vecInven[slotNum].itemInst.itemId <= -1)     // 빈 슬롯 체크
        return;

    CDropSystem::GetInstance()->SpawnDrop(m_GraphicDev, m_vecInven[slotNum].itemInst, DropPos);
    // 드롭한 뒤에는 무조건 인벤에서 삭제
    RemoveItemBySlot(slotNum, 1);
}

bool CInventory::AddItem(ItemInstance itemInst)
{
    //m_AddedCount = 0;
    //ItemInstance의 id(= itemId)는 “인스턴스를 만들 때 즉시 넣어준다.”

    Item* itemData = CItemDB::GetInstance()->GetItemById(itemInst.itemId);

    // 스택가능한지


    for (auto& slot : m_vecInven)
    {
        if (slot.itemInst.itemId == itemInst.itemId && itemData->stackable == true)
        {
            slot.count++;

            // 플레이어 일떄만 UI addItem 이밴트 호출
            if (dynamic_cast<CPlayer*>(m_Owner) != nullptr)
            {
                m_UIItemAdded(&slot.itemInst);
            }
                AddItemOnMap(slot.itemInst.itemId);

            return true;
        }
    }

    if (dynamic_cast<CPlayer*>(m_Owner) != nullptr)
    {
        int slot;
        // 스택불가, 같은 아이템 없다
        switch (itemData->type)
        {
        case ItemType::FoodMaterial:
            slot = FindEmptySlot(ItemType::FoodMaterial);
            if (slot == -1)
                return false;
            else
            {
                m_vecInven[slot].itemInst = itemInst;
                m_vecInven[slot].count++;

                AddItemOnMap(m_vecInven[slot].itemInst.itemId);

                m_UIItemAdded(&m_vecInven[slot].itemInst);

                return true;
            }

            break;
        case ItemType::Food:
            // 음식은 노예용 플레이어는 소지 안함 먹을순 있는데 플레이어에서 처리
            return false;
            break;
        case ItemType::Material:
            slot = FindEmptySlot(ItemType::Material);
            if (slot == -1)
                return false;
            else
            {
                m_vecInven[slot].itemInst = itemInst;
                m_vecInven[slot].count++;


                AddItemOnMap(m_vecInven[slot].itemInst.itemId);

                m_UIItemAdded(&m_vecInven[slot].itemInst);
                return true;
            }
            break;
        case ItemType::Weapon:
            // 이건 플레이어에서 바로 처리하자 바로 장착이니까
            return false;
            break;

        case ItemType::CONSUMABLE:
            // 이건 플레이어에서 바로 처리하자 인벤에 들어오지 않음 근데 그냥 여기서 하자...
            CCombatStat* stat = dynamic_cast <CCombatStat*>(m_Owner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::COMBATSTAT));
            string str = "attack1";
            float AddHp = CItemDB::GetInstance()->GetStat(str, itemInst.itemId);
            stat->Set_Hp(stat->Get_Hp() + AddHp);
            return true;// 일단 트루
            break;
        }
    }
    else
    {
        int slot = FindEmptySlot(ItemType::None); // 이건 몬스터용
        if (slot != -1)
        {
            m_vecInven[slot].itemInst = itemInst;
            m_vecInven[slot].count++;

            AddItemOnMap(m_vecInven[slot].itemInst.itemId);


            return true;
        }
    }


    return false;
}

int CInventory::FindEmptySlot(ItemType itemtype)
{
    if (m_vecInven.empty()) // 인밴토리 칸이 하나도 없는경우
        return -1;

    for (size_t i = 0; i < m_vecInven.size(); ++i)
    {
        if (m_vecInven[i].SlotType == itemtype)
        {
            if (m_vecInven[i].itemInst.itemId == -1)
            {
                if (m_vecInven[i].itemInst.itemId == -1)
                    return static_cast<int>(i);
            }
        }
    }

    return -1;
}

bool CInventory::RemoveItem(int itemID, int count)
{
    int remainCount = count;
    for (auto& slot : m_vecInven)
    {
        if (itemID == slot.itemInst.itemId)
        {

            if (slot.count <= remainCount)
            {
                remainCount -= slot.count;
                slot.count = 0;
                slot.itemInst.itemId = -1;
                RemoveItemOnMap(itemID, count);


                if (remainCount == 0)
                      return true;
                    //break;
            }
            else
            {
                slot.count -= remainCount;
                remainCount = 0;
                RemoveItemOnMap(itemID, count);

                return true;
                //break;
            }
        }
    }

    
    return false;
}

bool CInventory::RemoveItemBySlot(int slotNum, int count)
{
    if (slotNum < 0 || slotNum >= m_vecInven.size())
        return false;

    auto& slot = m_vecInven[slotNum];

    if (slot.itemInst.itemId == -1)
        return false;

    if (slot.count < count)
        return false;

    slot.count -= count;

    if (slot.count == 0)
    {
        slot.itemInst.itemId = -1;
    }


    RemoveItemOnMap(slot.itemInst.itemId,count);

    return true;
}

void CInventory::AddItemOnMap(int itemID)
{
    if (m_CountingItem.count(itemID) == 0)// map에 해당 키를 쓰고 있지 않다
    {
        m_CountingItem[itemID] = 1; //m_CountingItem[itemID]는 find(itemID).second 비슷한거 + (없는 키는 자동 생성)
    }
    else
    {
        m_CountingItem[itemID]++;
    }
}

bool CInventory::RemoveItemOnMap(int itemID,int Num)
{
    auto iterator = m_CountingItem.find(itemID);

    if (iterator != m_CountingItem.end())
    {
        if (iterator->second >= Num)
        {
            iterator->second -= Num;
        }
        else
            return false;

        if (iterator->second <=0)
        {
            m_CountingItem.erase(itemID);
        }
    }
    return true;
}

int CInventory::GetItemCount(int itemID)
{
    auto it = m_CountingItem.find(itemID);

    return  (it != m_CountingItem.end()) ? it->second : 0;

}

CInventory* CInventory::Create(DEVICE graphicDev)
{
    auto Inven = new CInventory(graphicDev);

    if (FAILED(Inven->Ready_Inventory()))
    {
        MSG_BOX("Inven Create Failed");
        Safe_Release(Inven);
        return nullptr;
    }

    return Inven;
}

CComponent* CInventory::Clone()
{
    return new CInventory(*this);
}

void CInventory::Free()
{
    CComponent::Free();
}
