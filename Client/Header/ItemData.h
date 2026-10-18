#pragma once
#include "pch.h"

enum ItemType
{
    FoodMaterial,
    Material,
    Food,
    Weapon,
    CONSUMABLE,
    None
};

struct Item
{
    int         id;
    ItemType    type;
    std::string name;
    std::string desc;
    std::string UIFileName;
    std::string UIPath;

    std::string additionalDesc;

    int  quality;
    bool stackable;
    int  maxStack;
    int  price;

    std::unordered_map<std::string, float> stats;
};
