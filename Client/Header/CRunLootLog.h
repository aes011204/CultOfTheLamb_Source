#pragma once

#include "ItemInstance.h"
#include <vector>

class CRunLootLog
{
public:
    struct Slot
    {
        ItemInstance itemInst;
        int          count;
    };

    CRunLootLog();
    ~CRunLootLog();

    void Clear();
    void Add(const ItemInstance& itemInst,int count =1 );
    
    vector<CRunLootLog::Slot>& Get_Vector() { return m_ItemSlot; }

private:
    std::vector<Slot> m_ItemSlot;

};


