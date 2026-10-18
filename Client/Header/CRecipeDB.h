#pragma once
#include "CBase.h"
#include "RecipeData.h"


class CRecipeDB : public CBase
{
    DECLARE_SINGLETON(CRecipeDB);
private:
    explicit CRecipeDB();
    ~CRecipeDB() override;

public:
    HRESULT Ready_RecipeDB();
    HRESULT LoadFromJson(const string& fileName);

    int GetNumByItemId(int id);
    Recipe* GetRecipeById(int id);
    wstring  ToWString(const string& str);
    wstring Utf8ToWstring(const string& str);

    const vector<Recipe>& GetVector() const { return m_vecRecipes; }

private:
    vector<Recipe>               m_vecRecipes;
    unordered_map<int, size_t> m_RecipeIndex;

public:
    //	static CItemDB* Create(); 매니져 클래스라 필요없는듯

private:
    void Free() override;

};

