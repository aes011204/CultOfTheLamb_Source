#pragma once
#include "Engine_Define.h"
#include "CGameObject.h"

BEGIN(Engine)
// const char* -> wstring 변환
static wstring CharToWString(const char* cstr)
{
    if (cstr == nullptr)
        return L"";

    int srcLen = static_cast<int>(strlen(cstr));
    if (srcLen == 0)
        return L"";

    int required = MultiByteToWideChar(CP_UTF8, 0, cstr, srcLen, nullptr, 0);
    if (required <= 0)
        return L"";    // 변환 실패

    wstring result;
    result.resize(required);

    // 실제 변환
    int converted = MultiByteToWideChar(
        CP_UTF8,
        0,
        cstr,
        srcLen,
        &result[0],
        required);

    if (converted <= 0)
        return L"";    // 변환 실패

    return result;
}

// wstring -> Utf8로 변환 [ImGui 띄울때 사용]
static string WStringToUtf8(const wstring& wstr)
{
    if (wstr.empty())
        return {};

    int sizeNeeded = WideCharToMultiByte(
        CP_UTF8,
        0,
        wstr.c_str(),
        static_cast<int>(wstr.size()),
        nullptr,
        0,
        nullptr,
        nullptr);

    if (sizeNeeded <= 0)
        return {};

    string result(sizeNeeded, 0);
    WideCharToMultiByte(
        CP_UTF8,
        0,
        wstr.c_str(),
        static_cast<int>(wstr.size()),
        result.data(),
        sizeNeeded,
        nullptr,
        nullptr);

    return result;
}

// ImGui와의 호환성때문에 기본적으로 const char*로 반환
static const char* ToString(ACTORSTATE state)
{
    switch (state)
    {
    case ACTORSTATE::IDLE:                   return "Idle";
    case ACTORSTATE::RUN:                    return "Run";
    case ACTORSTATE::ATTACK:                 return "Attack";
    case ACTORSTATE::HIT:                    return "Hit";
    case ACTORSTATE::DEAD:                   return "Dead";

    // Player Only
    case ACTORSTATE::PLAYER_ROLL:            return "Roll";
    case ACTORSTATE::PLAYER_SWORD_COMBO1:    return "Sword_Combo1";
    case ACTORSTATE::PLAYER_SWORD_COMBO2:    return "Sword_Combo2";
    case ACTORSTATE::PLAYER_SWORD_COMBO3:    return "Sword_Combo3";
    case ACTORSTATE::PLAYER_SWORD_HEAVY:     return "Sword_Heavy";

    case ACTORSTATE::PLAYER_GAUNTLET_COMBO1: return "Gauntlet_Combo1";
    case ACTORSTATE::PLAYER_GAUNTLET_COMBO2: return "Gauntlet_Combo2";
    case ACTORSTATE::PLAYER_GAUNTLET_COMBO3: return "Gauntlet_Combo3";
    case ACTORSTATE::PLAYER_GAUNTLET_HEAVY:  return "Gauntlet_Heavy";

    case ACTORSTATE::PLAYER_SKILL_CHARGING: return "Player_Skill_Charging";
    case ACTORSTATE::PLAYER_SKILL_SINGLE:    return "Player_Skill_Single";
    case ACTORSTATE::PLAYER_SKILL_TRIPLE:    return "Player_Skill_Triple";

    case ACTORSTATE::PLAYER_ACTION:          return "PlayerAction";
    case ACTORSTATE::PLAYER_WOOD:            return "PlayerWood";
    case ACTORSTATE::PLAYER_STONE:           return "PlayerStone";
    case ACTORSTATE::PLAYER_BERRY:           return "PlayerBerry";
    //case ACTORSTATE::GATHERING:            return "Gathering";
    
    case ACTORSTATE::PLAYER_WEAPONGET:       return "PlayerWeaponGet";
    case ACTORSTATE::PLAYER_HAMMER:          return "PlayerConstruct";

    case ACTORSTATE::PLAYER_DECIDE:          return "PlayerDecide";
    case ACTORSTATE::PLAYER_REFUSE:          return "PlayerRefuse";

    case ACTORSTATE::STATE_END:              return "StateEnd";

    default:                                 return "UnknownState";
    }
}

static const char* ToString(ACTORDIR dir)
{
    switch (dir)
    {
    case ACTORDIR::LEFT:        return "Left";
    case ACTORDIR::RIGHT:       return "Right";
    case ACTORDIR::UP:          return "Up";
    case ACTORDIR::DOWN:        return "Down";
    case ACTORDIR::L_UP:        return "Left Up";
    case ACTORDIR::R_UP:        return "Right Up";
    case ACTORDIR::L_DOWN:      return "Left Down";
    case ACTORDIR::R_DOWN:      return "Right Down";

    case ACTORDIR::DIR_END:     return "DirEnd";
    default:                    return "UnknownDir";
    }
}

static const char* ToString(COMPONENTTYPE type)
{
    switch (type)
    {
    // Geometry/Mesh Components
    case COMPONENTTYPE::TRI_COLOR:          return "TriColor";
    case COMPONENTTYPE::RC_COLOR:           return "RcColor";
    case COMPONENTTYPE::BOX_COLOR:          return "BoxColor";
    case COMPONENTTYPE::RC_TEX:             return "RcTex";
    case COMPONENTTYPE::BOX_TEX:            return "BoxTex";
    case COMPONENTTYPE::TERRAIN_TEX:        return "TerrainTex";

    // Core Components
    case COMPONENTTYPE::TRANSFORM:          return "Transform";
    case COMPONENTTYPE::STATE:              return "State";
    case COMPONENTTYPE::COMBATSTAT:         return "CombatStat";
    case COMPONENTTYPE::INVENTORY:          return "Inventory";
    case COMPONENTTYPE::ANIMATOR:           return "Animator";

    case COMPONENTTYPE::CAM_CONTROLLER:     return "CameraController";
    case COMPONENTTYPE::CAM_MOVEMENT:       return "CameraMovement";
    case COMPONENTTYPE::CAM_CALC:           return "CameraCalc";
    case COMPONENTTYPE::TERRAIN_RENDER:     return "TerrainRender";
    case COMPONENTTYPE::WALL_RENDERER:      return "WallRenderer";

    // Texture Components (General & Specific)
    case COMPONENTTYPE::TEXTURE:             return "Texture";
    case COMPONENTTYPE::TEX_PLAYER:          return "TexPlayer";
    case COMPONENTTYPE::TEX_MAINBG:          return "TexMainBg";
    case COMPONENTTYPE::TEX_MONSTER:         return "TexMonster";
    case COMPONENTTYPE::TEX_HUMAN_MONSTER:   return "TexHumanMonster";
    case COMPONENTTYPE::TEX_ITEM:            return "TexItem";
    case COMPONENTTYPE::TEX_OUT:             return "TexOut";

    case COMPONENTTYPE::TEX_GRASS:           return "TexGrass";
    case COMPONENTTYPE::TEX_RUBBLE:          return "TexRubble";
    case COMPONENTTYPE::TEX_DECO:            return "TexDeco";

    // UI Texture Components
    case COMPONENTTYPE::TEX_UI_CIRCLE:       return "UI_Circle";
    case COMPONENTTYPE::TEX_UI_COLOR:        return "UI_Color";
    case COMPONENTTYPE::TEX_UI_HEART:        return "UI_Heart";
    case COMPONENTTYPE::TEX_UI_WEAPON:       return "UI_Weapon";
    case COMPONENTTYPE::TEX_MENU_CHAR:       return "UI_ManuChaR";
    case COMPONENTTYPE::TEX_UI_ICON:         return "UI_Icon";
    case COMPONENTTYPE::TEX_UI_CELL:         return "UI_Cell";
    case COMPONENTTYPE::TEX_UI_SKILLCOUNT:   return "UI_SkillCount";
    case COMPONENTTYPE::TEX_UI_HIGHLIGHT:    return "UI_Highlight";
    case COMPONENTTYPE::TEX_UI_INVEN:        return "UI_Inven";
    case COMPONENTTYPE::TEX_UI_RITUALCARD:   return "UI_RitualCard";
    case COMPONENTTYPE::TEX_UI_BUTTON:       return "UI_BButton";
    case COMPONENTTYPE::TEX_BOSSPHASE1:      return "UI_BossPhase1";

    // Tile Texture Components
    case COMPONENTTYPE::TEX_TILE_284:        return "TexTile284";
    case COMPONENTTYPE::TEX_TILE_1024:       return "TexTile1024";
    case COMPONENTTYPE::TEX_TILE_OUTLINE:    return "TexTileOutline";

    // Collider Components
    case COMPONENTTYPE::RECT_COLL:           return "RectColl";
    case COMPONENTTYPE::CIRCLE_COLL:         return "CircleColl";
    case COMPONENTTYPE::BOX_COLL:            return "BoxColl";
    case COMPONENTTYPE::SPHERE_COLL:         return "SphereColl";

    // Physics Component
    case COMPONENTTYPE::RIGIDBODY:           return "Rigidbody";

    // Player Components
    case COMPONENTTYPE::WEAPON_EQUIP:        return "WeaponEquip";
    case COMPONENTTYPE::PL_CONTROLLER:       return "PlayerController";
    case COMPONENTTYPE::PL_INPUT:            return "PlayerInput";
    case COMPONENTTYPE::PL_MOVEMENT:         return "PlayerMovement";
    case COMPONENTTYPE::PL_ATTACK:           return "PlayerAttack";

    case COMPONENTTYPE::PROJECTILE:          return "Projectile";

    case COMPONENTTYPE::TEX_UI_HEARTCARD:    return "UI_HeartCard";
    case COMPONENTTYPE::TEX_UI_EXPLAIN:      return "UI_Explain";
    case COMPONENTTYPE::TEX_UI_TAROT:        return "UI_Tarot";
    case COMPONENTTYPE::TEX_BULLET:          return "Bullet";
    case COMPONENTTYPE::TEX_SLAVE_TEST:      return "Slave_Test";
    case COMPONENTTYPE::TEX_BOSS2_PHASE2:    return "Boss_Phase2";
    case COMPONENTTYPE::BOSS2_CONTROLLER:    return "Boss_Controller";
    case COMPONENTTYPE::TEX_DUNGEON_DECO:    return "Dungeon_Deco";
    case COMPONENTTYPE::TEX_ETC:             return "ETC";

    case COMPONENTTYPE::EYEBALL_CONTROLLER:  return "Eyeball_Controller";
    case COMPONENTTYPE::TEX_BOSS2_EYEBALL:   return "Eyeball";

    case COMPONENTTYPE::SPRITEEFFECT:        return "Sprit_Effect";
    case COMPONENTTYPE::TEX_PARTICLE:        return "Particle";

    

    case COMPONENTTYPE::OBJECT_SPAWNER:     return "MonsterSpawner";
    case COMPONENTTYPE::GATHERING:           return "Gathering";
    case COMPONENTTYPE::CONSTRUCTION:        return "Construction";
    case COMPONENTTYPE::PL_SKILL:            return "PlayerSkill";
    case COMPONENTTYPE::TEX_FLOATER_MONSTER: return "TexFloaterMonster";
    case COMPONENTTYPE::TEX_UI_MONSTERHP:    return "UI_MonsterHP";
    case COMPONENTTYPE::TEX_UI_BUILD:        return "UI_Build";
    case COMPONENTTYPE::TEX_WORKBENCH:       return "TexWorkBench";
    case COMPONENTTYPE::TEX_DOTTEDCIRCLE:    return "TexDottedCircle";
    case COMPONENTTYPE::TEX_TEMPLE:          return "TexTemple";
    case COMPONENTTYPE::TEX_KITCHEN:         return "TexKitchen";
    case COMPONENTTYPE::TEX_BERRY:           return "TexBerry";
    case COMPONENTTYPE::TEX_ROCK:            return "TexRock";
    case COMPONENTTYPE::TEX_TREE:            return "TexTree";
    case COMPONENTTYPE::TEX_SKILL:           return "TexSkill";
    case COMPONENTTYPE::TEX_WARNING_CHAIN:   return "TexWarningChain";
    case COMPONENTTYPE::TEX_CHAIN:           return "TexChain";

    

    case COMPONENTTYPE::NPC_INTERACTION_CHECKER:  return "NPCInteractionChecker";
    case COMPONENTTYPE::NPC_CONTROLLER:           return "NPCController";
    case COMPONENTTYPE::TEX_COW:                  return "TexCow";
    case COMPONENTTYPE::TEX_ELEPHANT:             return "TexElephant";
    case COMPONENTTYPE::TEX_PIG:                  return "TexPig";

    case COMPONENTTYPE::COMPONENT_END:       return "ComponentEnd"; // End Marker

    case COMPONENTTYPE::COOKCTRL:
    case COMPONENTTYPE::TEX_UI_DECO: 
    case COMPONENTTYPE::TEX_DIALOGUE:
    case COMPONENTTYPE::TEX_UI_COOK: 
    default:                                 return "UnknownComponent";
    }
}

static const char* ToString(LAYERTYPE type)
{
    switch (type)
    {
    case LAYERTYPE::ENVIRONMENT:    return "Environment";
    case LAYERTYPE::GAMELOGIC:      return "GameLogic";
    case LAYERTYPE::UI:             return "UI";

    case LAYERTYPE::LAYER_END:      return "LayerEnd";
    default:                        return "UnknownLayerType";
    }
}

static const char* ToString(OBJTYPE type)
{
    switch (type)
    {
    case OBJTYPE::PLAYER:            return "Player";
    case OBJTYPE::MONSTER:           return "Monster";
    case OBJTYPE::CAMERA:            return "Camera";
    case OBJTYPE::ITEM:              return "Item";
    case OBJTYPE::UI:                return "Ui";
    case OBJTYPE::BOSS2:             return "Boss2";
    case OBJTYPE::HUMANMONSTER:      return "HumanMonster";
    case OBJTYPE::TERRAIN:           return "Terrain";
    case OBJTYPE::PLACEMENT:         return "Placement";
    case OBJTYPE::ATTACK_COL:        return "Attack_Collider";
    case OBJTYPE::BOSS1:             return "Boss_Phase1";
    case OBJTYPE::WALL:              return "Wall";
    case OBJTYPE::NPC:               return "NPC";
    case OBJTYPE::LOGIC:             return "Logic";
    case OBJTYPE::PARTICLE:          return "Particle";
    case OBJTYPE::TRIGGER:           return "Trigger";

    case OBJTYPE::INTERACTIVE:       return "Interactive";
    case OBJTYPE::BUILDING:          return "Building";
    case OBJTYPE::BULLET:            return "Bullet";
    case OBJTYPE::CONSTRUCTION_SITE: return "ConstructionSite";
    case OBJTYPE::FOG:               return "Fog";

    case OBJTYPE::OBJ_END:           return "ObjectEnd";
    default:                         return "UnknownObjectType";
    }
}

static const char* ToString(SCENETYPE type)
{
    switch (type)
    {
    case SCENETYPE::SC_LOGO:        return "Logo";
    case SCENETYPE::SC_LOGO_BOSS1:  return "BossLoading1";
    case SCENETYPE::SC_LOGO_BOSS2:  return "BossLoading2";

    case SCENETYPE::SC_MAINMENU:    return "MainMenu";

    case SCENETYPE::SC_STAGE:       return "Stage";
    case SCENETYPE::SC_DUNGEON:      return "Stage1";
    case SCENETYPE::SC_TAROT_TEST:  return "Tarot_Test";
    case SCENETYPE::SC_BOSS_STAGE1: return "Boss_Stage";
    case SCENETYPE::SC_BOSS_STAGE2: return "Boss_Stage2";

    case SCENETYPE::SC_EDIT:        return "Edit";

    case SCENETYPE::SC_END:         return "SceneEnd";
    default:                        return "UnknownScene";
    }
}

static void SetNameFromComponentType(CGameObject* obj, COMPONENTTYPE type)
{
    if (!obj) return;

    const char* base  = ToString(type);
    wstring     wBase = CharToWString(base);

    obj->Set_Name(wBase);
}

END
