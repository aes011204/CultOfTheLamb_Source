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
#include "CRecipeDB.h"
//#include "Engine_Define.h"   // 필요하다면, 없으면 생략해도 됨

// (원하면) 다시 new 매크로 켜기 ? 이 파일에서도 디버그 new 쓰고 싶으면
#ifdef _DEBUG
#define new DBG_NEW
#endif

IMPLEMENT_SINGLETON(CRecipeDB)

CRecipeDB::CRecipeDB()
{
}

CRecipeDB::~CRecipeDB()
{
    CRecipeDB::Free();
}

HRESULT CRecipeDB::Ready_RecipeDB()
{
    return S_OK;
}

HRESULT CRecipeDB::LoadFromJson(const string& fileName)
{

    ifstream file(fileName, ios_base::in);

    if (!file.is_open())
    {
        MSG_BOX("RecipeJson Load Failed");
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

    for (auto& node : j["recipes"])
    {
        Recipe recipe;

        recipe.id = node["id"];
        recipe.name = node["name"];
        recipe.desc = node["desc"];
        recipe.result = node.value("result", 0);
        recipe.UIFileName = node["UIFileName"];
        recipe.UIPath = node["UIPath"];

        //JSON에 해당 키가 없을 때 사용할 기본값을 지정 있으면 데이터
        recipe.quality = node.value("quality", 0);
        recipe.hunger = node.value("hunger", 0);
        recipe.result = node.value("result", 0);
        recipe.good = node.value("good", 0);
        recipe.bad = node.value("bad", 0);

        //만들기 재료 종류,개수
        for (auto& ingNode : node["ingredients"])
        {
            int id = ingNode["id"];
            int count = ingNode["count"];

            recipe.ingredients[id] = count;
        }

        int index = m_vecRecipes.size();

        m_RecipeIndex[recipe.id] = index;
        m_vecRecipes.push_back(recipe);
    }
    return S_OK;
}

int CRecipeDB::GetNumByItemId(int ItemID)
{
    auto it = m_RecipeIndex.find(ItemID);
    assert(it != m_RecipeIndex.end());
    return it->second;
}

Recipe* CRecipeDB::GetRecipeById(int recipeId)
{
    auto it = m_RecipeIndex.find(recipeId);
    assert(it != m_RecipeIndex.end());
    return &m_vecRecipes[it->second];
}

wstring CRecipeDB::ToWString(const string& str)
{
    return wstring(str.begin(), str.end());
}

wstring CRecipeDB::Utf8ToWstring(const string& str)
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

void CRecipeDB::Free()
{
}
