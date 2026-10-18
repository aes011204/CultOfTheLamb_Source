#include "pch.h"

#ifdef _DEBUG
#ifdef new
#undef new
#endif
#endif

//  json, fstream 등 순수 C++ 라이브러리 include
#include <fstream>
#include <nlohmann/json.hpp>

//  그 다음에 엔진 쪽 헤더
#include "CItemDB.h"
//#include "Engine_Define.h"   // 필요하다면, 없으면 생략해도 됨

// (원하면) 다시 new 매크로 켜기 ? 이 파일에서도 디버그 new 쓰고 싶으면
#ifdef _DEBUG
#define new DBG_NEW
#endif

IMPLEMENT_SINGLETON(CItemDB)

CItemDB::CItemDB()
{ }

CItemDB::~CItemDB()
{
    CItemDB::Free();
}

HRESULT CItemDB::Ready_ItemDB()
{
    return S_OK;
}

HRESULT CItemDB::LoadFromJson(const string& fileName)
{
    // char buf[MAX_PATH]{};
    // GetCurrentDirectoryA(MAX_PATH, buf);
    // MessageBoxA(nullptr, buf, "Current Directory", MB_OK);

    ifstream file(fileName, ios_base::in);

    if (!file.is_open())
    {
        //MSG_BOX("itemJson Load Failed");w
        return E_FAIL;
    }

    nlohmann::json j;

    // BOM 제거 처리
    char c;
    file.read(&c, 1);

    if (static_cast<unsigned char>(c) != 0xEF) // BOM의 첫 바이트가 0xEF가 아니면
    {
        // BOM 없음 → 다시 스트림 처음으로 이동
        file.seekg(0);
    }
    else
    {
        // BOM 존재 → 나머지 2바이트 제거
        char c2, c3;
        file.read(&c2, 1);
        file.read(&c3, 1);
        // 이제 f는 JSON 본문 위치에 정확히 있음
    }

    // 파일 읽기
    try
    {
        file >> j;  // 여기서 문법 이상하면 바로 예외 발생
    }
    catch (const nlohmann::json::parse_error& e)
    {
        MessageBoxA(nullptr, e.what(), "JSON Parse Error", MB_OK);
        return E_FAIL;
    }

    for (auto& node : j["items"])
    {
        Item item; // 구조체 지역으로 생성

        item.id   = node["id"];
        item.type = ::CItemDB::GetInstance()->StringToItemType(node["type"]);
        item.name = node["name"];
        item.desc = node["desc"];
        // item.UIFileName = ToWString(node["UIFileName"].get<string>());
        // item.UIPath = ToWString(node["UIPath"].get<string>());
        item.UIFileName = node["UIFileName"];
        item.UIPath     = node["UIPath"];

        item.additionalDesc = node["additionalDesc"];

        //JSON에 해당 키가 없을 때 사용할 기본값을 지정 있으면 데이터
        item.quality   = node.value("quality", 0);
        item.stackable = node.value("stackable", false);
        item.maxStack  = node.value("maxStack", 1);
        item.price     = node.value("price", 0);

        //map stat에 넣을 데이터
        if (node.contains("stats"))
        {
            for (auto& s : node["stats"].items())
            {
                item.stats[s.key()] = s.value();
            }
        }
        m_vecItems.push_back(item);
        m_itemIndex[item.id] = m_vecItems.size() - 1;
    }




    ///



    return S_OK;
}

int CItemDB::GetIndexById(int id)
{
    auto it = m_itemIndex.find(id);
    assert(it != m_itemIndex.end());
    return it->second;
}

Item* CItemDB::GetItemById(int id)
{
    auto it = m_itemIndex.find(id);
    assert(it != m_itemIndex.end());
    return &m_vecItems[it->second];
}

int CItemDB::GetStat(string& key, int itemID) 
{
    Item* i = GetItemById(itemID);
    auto it = i->stats.find(key);
    if (it != i->stats.end())
        return it->second;
    return 999;
}
ItemType CItemDB::StringToItemType(string s)
{
    if (s == "FoodMaterial") return ItemType::FoodMaterial;
    if (s == "Material") return ItemType::Material;
    if (s == "Food") return ItemType::Food;
    if (s == "Weapon") return ItemType::Weapon;
    if (s == "Comsumable") return ItemType::CONSUMABLE;
    return ItemType::None;
}

wstring CItemDB::ToWString(const string& str)
{
    return wstring(str.begin(), str.end());
}

wstring CItemDB::Utf8ToWstring(const string& str)
{
    if (str.empty()) return wstring();

    int size_needed = MultiByteToWideChar(CP_UTF8,
                                          0,
                                          str.c_str(),
                                          -1,
                                          nullptr,
                                          0);

    wstring result(size_needed, 0);
    MultiByteToWideChar(CP_UTF8,
                        0,
                        str.c_str(),
                        -1,
                        &result[0],
                        size_needed);

    // 널문자 제거
    if (!result.empty() && result.back() == L'\0')
        result.pop_back();

    return result;
}

void CItemDB::Free()
{ }
