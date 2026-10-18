#include "pch.h"
#include "CRunLootLog.h"

CRunLootLog::CRunLootLog() {}

CRunLootLog::~CRunLootLog() {}

void CRunLootLog::Clear()
{
    m_ItemSlot.clear();
}

void CRunLootLog::Add(const ItemInstance& itemInst, int count)
{

    for (auto iter = m_ItemSlot.begin(); iter != m_ItemSlot.end(); ++iter)
    {
        if (iter->itemInst.itemId == itemInst.itemId)
        {
            iter->count += count;
            return;
        }
    }
    
    Slot slot;
    slot.itemInst = itemInst;
    slot.count = count;
    m_ItemSlot.push_back(slot);
}
