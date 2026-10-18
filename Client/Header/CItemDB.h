#pragma once
#include "CBase.h"
#include "ItemData.h"

class CItemDB : public CBase
{
    DECLARE_SINGLETON(CItemDB)
private:
    explicit CItemDB();
    ~CItemDB() override;

public:
    HRESULT Ready_ItemDB();
    HRESULT LoadFromJson(const string& fileName);

    int GetIndexById(int id);

    Item*    GetItemById(int id);
    int GetStat(string& key, int defaultValue);
    ItemType StringToItemType(string s);
    wstring  ToWString(const string& str);
    wstring Utf8ToWstring(const string& str);

    const vector<Item>& GetVector() const { return m_vecItems; }

private:
    vector<Item>               m_vecItems;
    unordered_map<int, size_t> m_itemIndex;

public:
    //	static CItemDB* Create(); 매니져 클래스라 필요없는듯

private:
    void Free() override;
};
