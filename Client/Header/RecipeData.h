#pragma once
#include "pch.h"
enum class RECIPETYPE
{
    COOK,
    BUILD
};

struct Recipe
{
    int         id;
    RECIPETYPE type;
    string name;
    string desc;
    string UIFileName;
    string UIPath;
    string good;
    string bad;

    int  quality;
    int  hunger;
    int result;

   unordered_map<int,int> ingredients;

   int GetRequiredCount(int ingredientId) const
   {
       auto it = ingredients.find(ingredientId);
       return (it != ingredients.end()) ? it->second : 0;
   }
    //int need = recipe->ingredients[itemId]; 이렇게 사용했을 때 없는 아이디 검색하는 실수 방지용
};
