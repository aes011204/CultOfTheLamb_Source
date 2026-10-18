#include "pch.h"
#include "CResourceMgr.h"

#include "CAnimator.h"
#include "CBossController.h"
#include "CBoxCol.h"
#include "CBoxCollider.h"
#include "CBoxTex.h"
#include "CCameraCalc.h"
#include "CCameraController.h"
#include "CCameraMovement.h"
#include "CCombatStat.h"
#include "CCookingController.h"
#include "CEffectBaseCom.h"
#include "CEnumHelper.h"
#include "CEyeballController.h"
#include "CInteractionChecker.h"
#include "CInventory.h"
#include "CItemDB.h"
#include "CNPCController.h"
#include "CObjectSpawner.h"
#include "CPlayerAttack.h"
#include "CPlayerConstruction.h"
#include "CPlayerController.h"
#include "CPlayerGathering.h"
#include "CPlayerInput.h"
#include "CPlayerMovement.h"
#include "CPlayerSkill.h"
#include "CProjectile.h"
#include "CProtoMgr.h"
#include "CRcCol.h"
#include "CRectCollider.h"
#include "CRigidbody.h"
#include "CSphereCollider.h"
#include "CSpriteEff.h"
#include "CState.h"
#include "CTerrain.h"
#include "CTerrainRenderer.h"
#include "CTerrainTex.h"
#include "CTexture.h"
#include "CTrailEff.h"
#include "CTransform.h"
#include "CTriCol.h"
#include "CWallRenderer.h"
#include "CWeaponEquip.h"
#include "ItemData.h"
#include "ItemInstance.h"

IMPLEMENT_SINGLETON(CResourceMgr)

CResourceMgr::CResourceMgr()
{ }

CResourceMgr::~CResourceMgr()
{
    CResourceMgr::Free();
}

HRESULT CResourceMgr::Ready_Resource(DEVICE graphicDev)
{
    if (FAILED(Ready_Buffer(graphicDev))) return E_FAIL;
    if (FAILED(Ready_Player(graphicDev))) return E_FAIL;
    if (FAILED(Ready_Monster(graphicDev))) return E_FAIL;
    if (FAILED(Ready_UI(graphicDev))) return E_FAIL;
    if (FAILED(Ready_Item(graphicDev))) return E_FAIL;
    if (FAILED(Ready_Common(graphicDev))) return E_FAIL;
    if (FAILED(Ready_Terrain(graphicDev))) return E_FAIL;
    if (FAILED(Ready_Decoration(graphicDev))) return E_FAIL;
    if (FAILED(Ready_Building(graphicDev))) return E_FAIL;
    //if (FAILED(Ready_Follower(graphicDev))) return E_FAIL;
    if (FAILED(Ready_Particle(graphicDev))) return E_FAIL;
    //if (FAILED(Ready_MiniBoss(graphicDev))) return E_FAIL;

    return S_OK;
}

// 보스 리소스는 별도로 로딩
HRESULT CResourceMgr::Ready_BossResource(DEVICE graphicDev)
{
    if (FAILED(Ready_BossP1(graphicDev))) return E_FAIL;
    //if (FAILED(Ready_BossP2(graphicDev))) return E_FAIL;

    return S_OK;
}

HRESULT CResourceMgr::Ready_BossResource2(DEVICE graphicDev)
{
    if (FAILED(Ready_BossP2(graphicDev))) return E_FAIL;

    return S_OK;
}

HRESULT CResourceMgr::Ready_Common(DEVICE graphicDev)
{
    auto protoMgr = CProtoMgr::GetInstance();

    // Transform, Animator는 로고로 이동

    // Camera Controller
    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::CAM_CONTROLLER, CCameraController::Create(graphicDev))))
        return E_FAIL;

    // Camera Movement
    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::CAM_MOVEMENT, CCameraMovement::Create(graphicDev))))
        return E_FAIL;

    // Camera Calculator
    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::CAM_CALC, Engine::CCameraCalc::Create(graphicDev))))
        return E_FAIL;

    // Rect Collider
    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::RECT_COLL, CRectCollider::Create(graphicDev))))
        return E_FAIL;

    // Box Collider
    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::BOX_COLL, CBoxCollider::Create(graphicDev))))
        return E_FAIL;

    // Sphere Collider
    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::SPHERE_COLL, CSphereCollider::Create(graphicDev))))
        return E_FAIL;

    // State
    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::STATE, CState::Create(graphicDev))))
        return E_FAIL;

    // CombatStat
    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::COMBATSTAT, CCombatStat::Create(graphicDev))))
        return E_FAIL;

    // Inventory
    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::INVENTORY, CInventory::Create(graphicDev))))
        return E_FAIL;

    // Rigidbody
    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::RIGIDBODY, CRigidbody::Create(graphicDev))))
        return E_FAIL;

    // Projectile
    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::PROJECTILE, CProjectile::Create(graphicDev))))
        return E_FAIL;

    // Effect
    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::SPRITEEFFECT, CSpriteEff::Create(graphicDev))))
        return E_FAIL;

    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::TRAILEFFECT, CTrailEff::Create(graphicDev))))
        return E_FAIL;

    // Construction
    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::CONSTRUCTION, CPlayerConstruction::Create(graphicDev))))
        return E_FAIL;

    // cook
    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::COOKCTRL, CCookingController::Create(graphicDev))))
        return E_FAIL;

    ////////////
    vector<wstring> pathVec2;
    pathVec2.push_back(L"../Bin/Resource/Texture/BossPhase1/Bullet/Bullet.png");
    pathVec2.push_back(L"../Bin/Resource/Texture/BossPhase1/Bullet/Bullet1.png");
    pathVec2.push_back(L"../Bin/Resource/Texture/BossPhase1/Bullet/FireFly.png");

    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::TEX_BULLET, Engine::CTexture::Create(graphicDev, TEX_NORMAL, pathVec2))))
        return E_FAIL;

    //FadeOut Tex
    vector<wstring> pathVec1;
    pathVec1.push_back(L"../Bin/Resource/Texture/BossPhase1/ChainAttackGround.png");
    pathVec1.push_back(L"../Bin/Resource/Texture/UI/PlayerState/Sermon/Sermon_Icon.png");

    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::TEX_OUT, Engine::CTexture::Create(graphicDev, TEX_NORMAL, pathVec1))))
        return E_FAIL;

    return S_OK;
}

HRESULT CResourceMgr::Ready_Buffer(DEVICE graphicDev)
{
    auto protoMgr = CProtoMgr::GetInstance();

    // Buffer : Triangle Color
    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::TRI_COLOR, Engine::CTriCol::Create(graphicDev))))
        return E_FAIL;

    // Buffer : Rect Color
    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::RC_COLOR, Engine::CRcCol::Create(graphicDev))))
        return E_FAIL;

    // Buffer : Box Color
    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::BOX_COLOR, Engine::CBoxCol::Create(graphicDev))))
        return E_FAIL;

    // Buffer : Box Tex
    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::BOX_TEX, Engine::CBoxTex::Create(graphicDev))))
        return E_FAIL;

    // Buffer : Terrain Tex
    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::TERRAIN_TEX, Engine::CTerrainTex::Create(graphicDev, TILE_CNT_X, TILE_CNT_Z, 1))))
        return E_FAIL;

    // Buffer : Rect UV
    // CLogo 에서 이미 프로토타입 생성중이므로 비활성화
    //if (FAILED(protoMgr->Ready_Prototype(
    //    COMPONENTTYPE::RC_TEX, Engine::CRcTex::Create(graphicDev))))
    //    return E_FAIL;

    return S_OK;
}

HRESULT CResourceMgr::Ready_Player(DEVICE graphicDev)
{
    auto protoMgr = CProtoMgr::GetInstance();

    // Player Controller
    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::PL_CONTROLLER, CPlayerController::Create(graphicDev))))
        return E_FAIL;

    // Player Input
    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::PL_INPUT, CPlayerInput::Create(graphicDev))))
        return E_FAIL;

    // Player Movement
    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::PL_MOVEMENT, CPlayerMovement::Create(graphicDev))))
        return E_FAIL;

    // Player Attack
    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::PL_ATTACK, CPlayerAttack::Create(graphicDev))))
        return E_FAIL;

    // weapon equip
    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::WEAPON_EQUIP, CWeaponEquip::Create(graphicDev))))
        return E_FAIL;

    // Gathering
    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::GATHERING, CPlayerGathering::Create(graphicDev))))
        return E_FAIL;

    // skill
    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::PL_SKILL, CPlayerSkill::Create(graphicDev))))
        return E_FAIL;

    CTexture* playerTex = CTexture::Create(graphicDev, TEX_NORMAL, L"", 0);
    NULL_CHECK_RETURN(playerTex, E_FAIL);

#pragma region --- Tex: Idle ---

    if (FAILED(playerTex->Add_Texture(L"PlayerIdle", TEX_NORMAL,
        L"../Bin/Resource/Texture/Player/Idle/Lamb-idle%d.png", 150)))
        return E_FAIL;

#pragma endregion

#pragma region --- Tex: Run ---

    if (FAILED(playerTex->Add_Texture(L"PlayerRun_LEFT", TEX_NORMAL,
        L"../Bin/Resource/Texture/Player/RunHorizontal/Lamb-run-horizontal%d.png", 19)))
        return E_FAIL;

    if (FAILED(playerTex->Add_Texture(L"PlayerRun_UP", TEX_NORMAL,
        L"../Bin/Resource/Texture/Player/RunUp/Lamb-run-up%d.png", 19)))
        return E_FAIL;

    if (FAILED(playerTex->Add_Texture(L"PlayerRun_DOWN", TEX_NORMAL,
        L"../Bin/Resource/Texture/Player/RunDown/Lamb-run-down%d.png", 19)))
        return E_FAIL;

    if (FAILED(playerTex->Add_Texture(L"PlayerRun_LUP", TEX_NORMAL,
        L"../Bin/Resource/Texture/Player/RunUpDiagonal/Lamb-run-up-diagonal%d.png", 19)))
        return E_FAIL;

    if (FAILED(playerTex->Add_Texture(L"PlayerRun_LDOWN", TEX_NORMAL,
        L"../Bin/Resource/Texture/Player/Run/Lamb-run%d.png", 19)))
        return E_FAIL;

#pragma endregion

#pragma region --- Tex: Roll ---

    if (FAILED(playerTex->Add_Texture(L"PlayerRoll_LEFT", TEX_NORMAL,
        L"../Bin/Resource/Texture/Player/Roll/Lamb-roll%d.png", 19)))
        return E_FAIL;

    if (FAILED(playerTex->Add_Texture(L"PlayerRoll_UP", TEX_NORMAL,
        L"../Bin/Resource/Texture/Player/RollUp/Lamb-roll-up%d.png", 16)))
        return E_FAIL;

    if (FAILED(playerTex->Add_Texture(L"PlayerRoll_DOWN", TEX_NORMAL,
        L"../Bin/Resource/Texture/Player/RollDown/Lamb-roll-down%d.png", 16)))
        return E_FAIL;

#pragma endregion

#pragma region --- Tex: Sword ---

    if (FAILED(playerTex->Add_Texture(L"PlayerSwordCombo1", TEX_NORMAL,
        L"../Bin/Resource/Texture/Player/Sword_Combo1/Lamb-attack-combo1-%d.png", 31)))
        return E_FAIL;

    if (FAILED(playerTex->Add_Texture(L"PlayerSwordCombo2", TEX_NORMAL,
        L"../Bin/Resource/Texture/Player/Sword_Combo2/Lamb-attack-combo2-%d.png", 38)))
        return E_FAIL;

    if (FAILED(playerTex->Add_Texture(L"PlayerSwordCombo3", TEX_NORMAL,
        L"../Bin/Resource/Texture/Player/Sword_Combo3/Lamb-attack-combo3-%d.png", 38)))
        return E_FAIL;

    if (FAILED(playerTex->Add_Texture(L"PlayerSwordHeavy", TEX_NORMAL,
        L"../Bin/Resource/Texture/Player/Sword_Heavy/Lamb-attack-heavy-sword%d.png", 70)))
        return E_FAIL;

#pragma endregion

#pragma region --- Tex: Gauntlet ---

    if (FAILED(playerTex->Add_Texture(L"PlayerGauntletCombo1", TEX_NORMAL,
        L"../Bin/Resource/Texture/Player/Gauntlet_Combo1/Lamb-attack-combo1-gauntlets%d.png", 41)))
        return E_FAIL;

    if (FAILED(playerTex->Add_Texture(L"PlayerGauntletCombo2", TEX_NORMAL,
        L"../Bin/Resource/Texture/Player/Gauntlet_Combo2/Lamb-attack-combo2-gauntlets%d.png", 43)))
        return E_FAIL;

    if (FAILED(playerTex->Add_Texture(L"PlayerGauntletCombo3", TEX_NORMAL,
        L"../Bin/Resource/Texture/Player/Gauntlet_Combo3/Lamb-attack-combo3-gauntlets%d.png", 46)))
        return E_FAIL;

    if (FAILED(playerTex->Add_Texture(L"PlayerGauntletHeavy", TEX_NORMAL,
        L"../Bin/Resource/Texture/Player/Gauntlet_Attack_Heavy/Lamb-attack-heavy-gauntlet%d.png", 70)))
        return E_FAIL;

#pragma endregion

#pragma region --- Tex: Skill ---

    if (FAILED(playerTex->Add_Texture(L"PlayerSkillAttack", TEX_NORMAL,
        L"../Bin/Resource/Texture/Player/Spell1/Lamb-cast-spell%d.png", 32)))
        return E_FAIL;

    if (FAILED(playerTex->Add_Texture(L"PlayerSkillCharging", TEX_NORMAL,
        L"../Bin/Resource/Texture/Player/Spell2/Lamb-cast-spell2-%d.png",17)))
#pragma endregion
#pragma region --- Tex: CRAFT ---
    {
        //crafting
        if (FAILED(playerTex->Add_Texture(L"PlayerAction", TEX_NORMAL,
            L"../Bin/Resource/Texture/Player/Lamb_action/Lamb_action%d.png", 25)))
            return E_FAIL;
    }

    if (FAILED(playerTex->Add_Texture(L"PlayerStone", TEX_NORMAL,
        L"../Bin/Resource/Texture/Player/Lamb_actions_chop_stone/Lamb_actions_chop_stone%d.png", 45)))
        return E_FAIL;

    if (FAILED(playerTex->Add_Texture(L"PlayerWood", TEX_NORMAL,
        L"../Bin/Resource/Texture/Player/Lamb_actions_chop_wood/Lamb_actions_chop_wood%d.png", 39)))
        return E_FAIL;

    if (FAILED(playerTex->Add_Texture(L"PlayerBerry", TEX_NORMAL,
        L"../Bin/Resource/Texture/Player/Lamb_actions_collect_berries/Lamb_actions_collect_berries%d.png", 25)))
        return E_FAIL;

    if (FAILED(playerTex->Add_Texture(L"PlayerHammer", TEX_NORMAL,
        L"../Bin/Resource/Texture/Player/Action_Hammer/Lamb-actions-hammer%d.png", 29)))
        return E_FAIL;

#pragma region --- Tex: 기타 상호작용 ---
    // 무기 들기. 그런데 스프라이트가 조금 다른거같은데
    if (FAILED(playerTex->Add_Texture(L"Player_WeaponGet", TEX_NORMAL,
        L"../Bin/Resource/Texture/Player/Weapon_Get_Shot/Lamb-weapons-get%d.png", 77)))
        return E_FAIL;

    // Hit
    if (FAILED(playerTex->Add_Texture(L"Player_Hit", TEX_NORMAL,
        L"../Bin/Resource/Texture/Player/Knockback_Shot/Lamb-knockback%d.png", 34)))
        return E_FAIL;

    if (FAILED(protoMgr->Ready_Prototype(COMPONENTTYPE::TEX_PLAYER, playerTex)))
        return E_FAIL;

#pragma region --- Tex: Final
    if (FAILED(playerTex->Add_Texture(L"Decide_Start", TEX_NORMAL,
        L"../Bin/Resource/Texture/Player/Final/Decide_Start/Decide_Start%d.png", 41)))
        return E_FAIL;

    if (FAILED(playerTex->Add_Texture(L"Decide_Loop", TEX_NORMAL,
        L"../Bin/Resource/Texture/Player/Final/Decide_Loop/Decide_Loop%d.png", 50)))
        return E_FAIL;

    if (FAILED(playerTex->Add_Texture(L"Refuse_Sword", TEX_NORMAL,
        L"../Bin/Resource/Texture/Player/Final/Refuse_Sword/Refuse_Sword%d.png", 55)))
        return E_FAIL;

    if (FAILED(playerTex->Add_Texture(L"Refuse_Gauntlet", TEX_NORMAL,
        L"../Bin/Resource/Texture/Player/Final/Refuse_Guantlets/Refuse_Guantlets%d.png", 54)))
        return E_FAIL;

#pragma endregion

    if (FAILED(
        protoMgr->Ready_Prototype(COMPONENTTYPE::TEX_SKILL, Engine::CTexture::Create(graphicDev, TEX_NORMAL,
            L"../Bin/Resource/Texture/Player/Bullet/Fireball.png", 1))))
        return E_FAIL;

    //TarotNPC
    //플레이어는 아니긴 한데 마땅한 자리가 없어서 좀 빌리겠습니다ㅎㅎ;

    CTexture* TarotNpcTex = CTexture::Create(graphicDev, TEX_NORMAL, L"", 0);
    NULL_CHECK_RETURN(TarotNpcTex, E_FAIL);

    if (FAILED(TarotNpcTex->Add_Texture(L"TarotNpcIdle", TEX_NORMAL,
        L"../Bin/Resource/Texture/NPC/TarotSeller/TarotSeller%d.png", 87)))
        return E_FAIL;

    if (FAILED(protoMgr->Ready_Prototype(COMPONENTTYPE::TEX_TAROTNPC, TarotNpcTex)))
        return E_FAIL;

    return S_OK;
}

HRESULT CResourceMgr::Ready_Monster(DEVICE graphicDev)
{
    auto protoMgr = CProtoMgr::GetInstance();

    //HumanMonster
    CTexture* humanmonsterTex = CTexture::Create(graphicDev, TEX_NORMAL, L"", 0);
    NULL_CHECK_RETURN(humanmonsterTex, E_FAIL);

    if (FAILED(humanmonsterTex->Add_Texture(L"HumanMonsterIdle", TEX_NORMAL,
        L"../Bin/Resource/Texture/Monster/Human/Idle/Idle%d.png", 34)))
        return E_FAIL;

    if (FAILED(humanmonsterTex->Add_Texture(L"HumanMonsterRun", TEX_NORMAL,
        L"../Bin/Resource/Texture/Monster/Human/Run/Run%d.png", 18)))
        return E_FAIL;

    if (FAILED(humanmonsterTex->Add_Texture(L"HumanMonsterRunUp", TEX_NORMAL,
        L"../Bin/Resource/Texture/Monster/Human/Run_Up/Run_Up%d.png", 22)))
        return E_FAIL;

    if (FAILED(humanmonsterTex->Add_Texture(L"HumanMonsterAttack", TEX_NORMAL,
        L"../Bin/Resource/Texture/Monster/Human/Attack/Attack%d.png", 65)))
        return E_FAIL;

    if (FAILED(humanmonsterTex->Add_Texture(L"HumanMonsterHurt", TEX_NORMAL,
        L"../Bin/Resource/Texture/Monster/Human/Hurt/Hurt%d.png", 28)))
        return E_FAIL;

    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::TEX_HUMAN_MONSTER, humanmonsterTex)))
        return E_FAIL;

    // Floater
    CTexture* floaterTex = CTexture::Create(graphicDev, TEX_NORMAL, L"", 0);
    NULL_CHECK_RETURN(floaterTex, E_FAIL);

    if (FAILED(floaterTex->Add_Texture(L"Floater_Idle", TEX_NORMAL,
        L"../Bin/Resource/Texture/Monster/Floater/Floater_Idle/Floater_Idle%d.png", 32)))
        return E_FAIL;

    if (FAILED(floaterTex->Add_Texture(L"Floater_Hit", TEX_NORMAL,
        L"../Bin/Resource/Texture/Monster/Floater/Floater_Hit/Floater_Hit%d.png", 25)))
        return E_FAIL;

    if (FAILED(floaterTex->Add_Texture(L"Floater_Attack", TEX_NORMAL,
        L"../Bin/Resource/Texture/Monster/Floater/Floater_Attack/Floater_Attack%d.png", 34)))
        return E_FAIL;

    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::TEX_FLOATER_MONSTER, floaterTex)))
        return E_FAIL;

    // Bat
    CTexture* batTex = CTexture::Create(graphicDev, TEX_NORMAL, L"", 0);
    NULL_CHECK_RETURN(batTex, E_FAIL);

    if (FAILED(batTex->Add_Texture(L"Bat_Idle", TEX_NORMAL,
        L"../Bin/Resource/Texture/Monster/Bat2/Fly/Bat-fly%d.png", 34)))
        return E_FAIL;

    if (FAILED(batTex->Add_Texture(L"Bat_Charge", TEX_NORMAL,
        L"../Bin/Resource/Texture/Monster/Bat2/Charge/Bat-attackcharge%d.png", 59)))
        return E_FAIL;

    if (FAILED(batTex->Add_Texture(L"Bat_Attack", TEX_NORMAL,
        L"../Bin/Resource/Texture/Monster/Bat2/Attack/Bat-attack%d.png", 32)))
        return E_FAIL;

    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::TEX_BAT_MONSTER, batTex)))
        return E_FAIL;

    // HumanMonster2
    CTexture* human2Tex = CTexture::Create(graphicDev, TEX_NORMAL, L"", 0);
    NULL_CHECK_RETURN(human2Tex, E_FAIL);

    if (FAILED(human2Tex->Add_Texture(L"Human2_Idle", TEX_NORMAL,
        L"../Bin/Resource/Texture/Monster/Human2/Idle/Cultist-idle%d.png", 34)))
        return E_FAIL;

    if (FAILED(human2Tex->Add_Texture(L"Human2_Hit", TEX_NORMAL,
        L"../Bin/Resource/Texture/Monster/Human2/Hit/Cultist_Hit%d.png", 48)))
        return E_FAIL;

    if (FAILED(human2Tex->Add_Texture(L"Human2_Attack", TEX_NORMAL,
        L"../Bin/Resource/Texture/Monster/Human2/Attack/Cultist-attack%d.png", 38)))
        return E_FAIL;

    if (FAILED(human2Tex->Add_Texture(L"Human2_Run", TEX_NORMAL,
        L"../Bin/Resource/Texture/Monster/Human2/Run/Cultist-run%d.png", 29)))
        return E_FAIL;

    if (FAILED(human2Tex->Add_Texture(L"Human2_RunUp", TEX_NORMAL,
        L"../Bin/Resource/Texture/Monster/Human2/Run_up/Cultist-run-up%d.png", 34)))
        return E_FAIL;

    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::TEX_HUMAN_MONSTER2, human2Tex)))
        return E_FAIL;

    // 보물상자....
    CTexture* chest = CTexture::Create(graphicDev, TEX_NORMAL, L"", 0);
    NULL_CHECK_RETURN(floaterTex, E_FAIL);

    if (FAILED(chest->Add_Texture(L"Chest-reveal", TEX_NORMAL,
        L"../Bin/Resource/Texture/Chest/Chest-reveal/Chest-reveal%d.png", 57)))
        return E_FAIL;

    if (FAILED(chest->Add_Texture(L"Chest-open", TEX_NORMAL,
        L"../Bin/Resource/Texture/Chest/Chest-open/Chest-open%d.png", 50)))
        return E_FAIL;

    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::TEX_CHEST, chest)))
        return E_FAIL;

    return S_OK;
}

HRESULT CResourceMgr::Ready_BossP1(DEVICE graphicDev)
{
    auto protoMgr = CProtoMgr::GetInstance();

    CTexture* BossP1Tex = CTexture::Create(graphicDev, TEX_NORMAL, L"", 0);
    NULL_CHECK_RETURN(BossP1Tex, E_FAIL);

    if (FAILED(BossP1Tex->Add_Texture(L"BossIdle", TEX_NORMAL,
        L"../Bin/Resource/Texture/BossPhase1/TOWW-Spirit-animation/TOWW-Spirit-animation%d.png", 100)))
        return E_FAIL;
    if (FAILED(BossP1Tex->Add_Texture(L"BossAwake", TEX_NORMAL,
        L"../Bin/Resource/Texture/BossPhase1/TOWW-Spirit-transform2/TOWW-Spirit-transform%d.png", 105)))
        return E_FAIL;

    if (FAILED(BossP1Tex->Add_Texture(L"BossAttack", TEX_NORMAL,
        L"../Bin/Resource/Texture/BossPhase1/TOWW-Spirit-shoot/TOWW-Spirit-shoot%d.png", 97)))
        return E_FAIL;

    if (FAILED(BossP1Tex->Add_Texture(L"BossDead", TEX_NORMAL,
        L"../Bin/Resource/Texture/BossPhase1/TOWW-Spirit-hurt-talk/TOWW-Spirit-hurt-talk%d.png", 167)))
        return E_FAIL;

    if (FAILED(BossP1Tex->Add_Texture(L"BossMove_up", TEX_NORMAL,
        L"../Bin/Resource/Texture/BossPhase1/TOWW-Spirit-appear/TOWW-Spirit-appear%d.png", 50)))
        return E_FAIL;

    if (FAILED(BossP1Tex->Add_Texture(L"BossMove_Down", TEX_NORMAL,
        L"../Bin/Resource/Texture/BossPhase1/TOWW-Spirit-disappear/TOWW-Spirit-disappear%d.png", 28)))
        return E_FAIL;

    if (FAILED(BossP1Tex->Add_Texture(L"BossGroggy_start", TEX_NORMAL,
        L"../Bin/Resource/Texture/BossPhase1/TOWW-Spirit-hurt/TOWW-Spirit-hurt%d.png", 115)))
        return E_FAIL;

    if (FAILED(BossP1Tex->Add_Texture(L"BossGroggy_loop", TEX_NORMAL,
        L"../Bin/Resource/Texture/BossPhase1/TOWW-Spirit-loop/TOWW-Spirit-loop%d.png", 84)))
        return E_FAIL;

    if (FAILED(BossP1Tex->Add_Texture(L"BossGroggy_end", TEX_NORMAL,
        L"../Bin/Resource/Texture/BossPhase1/TOWW-Spirit-hurt-stop/TOWW-Spirit-hurt-stop%d.png", 67)))
        return E_FAIL;

    if (FAILED(BossP1Tex->Add_Texture(L"Before_Idle", TEX_NORMAL,
        L"../Bin/Resource/Texture/BossPhase1/TOWW_BeforeIdle/TOWW_BeforeIdle%d.png", 334)))
        return E_FAIL;

    if (FAILED(BossP1Tex->Add_Texture(L"Escape", TEX_NORMAL,
        L"../Bin/Resource/Texture/BossPhase1/TOWW_Escape/TOWW_Escape%d.png", 119)))
        return E_FAIL;

    // 사슬
    if (FAILED(BossP1Tex->Add_Texture(L"BossChainEff", TEX_NORMAL,
        L"../Bin/Resource/Texture/BossPhase1/ChainAttack/Summon_Chain%d.png", 34)))
        return E_FAIL;

    //// 바닥 이팩트
    //if (FAILED(BossP1Tex->Add_Texture(L"ChainGroundEff", TEX_NORMAL,
    //    L"../Bin/Resource/Texture/BossPhase1/ChainAttackGround.png", 1)))
    //    return E_FAIL;

    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::TEX_BOSSPHASE1, BossP1Tex)))
        return E_FAIL;

    ///////////////////////////////////////////////////////////////////////

    return S_OK;
}

HRESULT CResourceMgr::Ready_BossP2(DEVICE graphicDev)
{
    auto protoMgr = CProtoMgr::GetInstance();

    // Boss Controller
    if (FAILED(protoMgr->Ready_Prototype(COMPONENTTYPE::BOSS2_CONTROLLER,
        CBossController::Create(graphicDev))))
        return E_FAIL;

    // Boss Phase 2
    CTexture* bossPhase2 = CTexture::Create(graphicDev, TEX_NORMAL, L"", 0);
    NULL_CHECK_RETURN(bossPhase2, E_FAIL);

    // Summon Follower
    if (FAILED(bossPhase2->Add_Texture(L"Boss2_Summon", TEX_NORMAL,
        L"../Bin/Resource/Texture/BossPhase2/TOWW/TOWW_Summon/TOWW_Summon%d.png", 134)))
        return E_FAIL;

    // Start
    if (FAILED(bossPhase2->Add_Texture(L"Boss2_Start", TEX_NORMAL,
        L"../Bin/Resource/Texture/BossPhase2/TOWW/TOWW_Start_Eye/TOWW_Start_Eye%d.png", 85)))
        return E_FAIL;

    // Idle
    if (FAILED(bossPhase2->Add_Texture(L"Boss2_Idle", TEX_NORMAL,
        L"../Bin/Resource/Texture/BossPhase2/TOWW/TOWW_Idle/TOWW_Idle%d.png", 68)))
        return E_FAIL;

    // Idle Eyeball Alive
    if (FAILED(bossPhase2->Add_Texture(L"Boss2_Idle_Alive", TEX_NORMAL,
        L"../Bin/Resource/Texture/BossPhase2/TOWW/TOWW_Idle_Alive/TOWW_Idle_Alive%d.png", 68)))
        return E_FAIL;

    // Roar
    if (FAILED(bossPhase2->Add_Texture(L"Boss2_Roar", TEX_NORMAL,
        L"../Bin/Resource/Texture/BossPhase2/TOWW/TOWW_Roar/TOWW_Roar%d.png", 70)))
        return E_FAIL;

    // Roar Alive
    if (FAILED(bossPhase2->Add_Texture(L"Boss2_Roar_Alive", TEX_NORMAL,
        L"../Bin/Resource/Texture/BossPhase2/TOWW/TOWW_Roar_Alive/TOWW_Roar_Alive%d.png", 70)))
        return E_FAIL;

    // HandSlam
    if (FAILED(bossPhase2->Add_Texture(L"Boss2_HandSlam", TEX_NORMAL,
        L"../Bin/Resource/Texture/BossPhase2/TOWW/TOWW_Handslam/TOWW_Handslam%d.png", 68)))
        return E_FAIL;

    // Dead
    if (FAILED(bossPhase2->Add_Texture(L"Boss2_Dead", TEX_NORMAL,
        L"../Bin/Resource/Texture/BossPhase2/TOWW/TOWW_Die/TOWW_Die%d.png", 117)))
        return E_FAIL;

    // Warning Chain
    if (FAILED(bossPhase2->Add_Texture(L"Boss2_Warning", TEX_NORMAL,
        L"../Bin/Resource/Texture/BossPhase1/ChainAttackGround.png", 1)))
        return E_FAIL;

    // Sweep
    if (FAILED(bossPhase2->Add_Texture(L"Boss2_Sweep", TEX_NORMAL,
        L"../Bin/Resource/Texture/BossPhase2/TOWW/TOWW_Sweep/TOWW_Sweep%d.png", 82)))
        return E_FAIL;

    // Chain
    if (FAILED(bossPhase2->Add_Texture(L"Boss2_Chain", TEX_NORMAL,
        L"../Bin/Resource/Texture/BossPhase1/ChainAttack/Summon_Chain%d.png", 34)))
        return E_FAIL;

    // Down Start
    if (FAILED(bossPhase2->Add_Texture(L"Boss2_DownStart", TEX_NORMAL,
        L"../Bin/Resource/Texture/BossPhase2/TOWW/TOWW_DownStart/TOWW_DownStart%d.png", 64)))
        return E_FAIL;

    // Down Idle
    if (FAILED(bossPhase2->Add_Texture(L"Boss2_DownIdle", TEX_NORMAL,
        L"../Bin/Resource/Texture/BossPhase2/TOWW/TOWW_DownIdle/TOWW_DownIdle%d.png", 67)))
        return E_FAIL;

    // Transform Follower
    if (FAILED(bossPhase2->Add_Texture(L"Boss2_Follower_Transform", TEX_NORMAL,
        L"../Bin/Resource/Texture/BossPhase2/Slave/DeathCat/Transform/Boss_Transform%d.png", 62)))
        return E_FAIL;

    if (FAILED(bossPhase2->Add_Texture(L"Boss2_Follower_UnConverted", TEX_NORMAL,
        L"../Bin/Resource/Texture/BossPhase2/Slave/DeathCat/UnConverted/UnConverted%d.png", 50)))
        return E_FAIL;

    if (FAILED(protoMgr->Ready_Prototype(COMPONENTTYPE::TEX_BOSS2_PHASE2, bossPhase2)))
        return E_FAIL;

    return S_OK;
}

HRESULT CResourceMgr::Ready_BossP2_Eyeball(DEVICE graphicDev)
{
    auto protoMgr = CProtoMgr::GetInstance();

    // Eyeball Controller
    if (FAILED(protoMgr->Ready_Prototype(COMPONENTTYPE::EYEBALL_CONTROLLER,
        CEyeballController::Create(graphicDev))))
        return E_FAIL;

    CTexture* eyeballTex = CTexture::Create(graphicDev, TEX_NORMAL, L"", 0);
    NULL_CHECK_RETURN(eyeballTex, E_FAIL);

    // Start, Idle
    if (FAILED(eyeballTex->Add_Texture(L"Eyeball_Start", TEX_NORMAL,
        L"../Bin/Resource/Texture/BossPhase2/Eyeball/Eyeball_Idle/Eyeball_Idle%d.png", 59)))
        return E_FAIL;

    // Enter
    if (FAILED(eyeballTex->Add_Texture(L"Eyeball_Enter", TEX_NORMAL,
        L"../Bin/Resource/Texture/BossPhase2/Eyeball/Eyeball_Enter/Eyeball_Enter%d.png", 49)))
        return E_FAIL;

    // Exit
    if (FAILED(eyeballTex->Add_Texture(L"Eyeball_Exit", TEX_NORMAL,
        L"../Bin/Resource/Texture/BossPhase2/Eyeball/Eyeball-exit/Eyeball-exit%d.png", 34)))
        return E_FAIL;

    // Anticipate
    if (FAILED(eyeballTex->Add_Texture(L"Eyeball_Anti", TEX_NORMAL,
        L"../Bin/Resource/Texture/BossPhase2/Eyeball/Eyeball-attack-anticipate/Eyeball-attack-anticipate%d.png", 44)))
        return E_FAIL;

    // Attack Loop
    if (FAILED(eyeballTex->Add_Texture(L"Eyeball_Attack_Loop", TEX_NORMAL,
        L"../Bin/Resource/Texture/BossPhase2/Eyeball/Eyeball-attack-loop/Eyeball-attack-loop%d.png", 45)))
        return E_FAIL;

    // Attack End
    if (FAILED(eyeballTex->Add_Texture(L"Eyeball_Attack_End", TEX_NORMAL,
        L"../Bin/Resource/Texture/BossPhase2/Eyeball/Eyeball-attack-end/Eyeball-attack-end%d.png", 54)))
        return E_FAIL;

    // Die
    if (FAILED(eyeballTex->Add_Texture(L"Eyeball_Die", TEX_NORMAL,
        L"../Bin/Resource/Texture/BossPhase2/Eyeball/Eyeball-die/Eyeball-die%d.png", 25)))
        return E_FAIL;

    if (FAILED(protoMgr->Ready_Prototype(COMPONENTTYPE::TEX_BOSS2_EYEBALL, eyeballTex)))
        return E_FAIL;

    return S_OK;
}

HRESULT CResourceMgr::Ready_BossP2_Follower(DEVICE graphicDev)
{
    auto protoMgr = CProtoMgr::GetInstance();

    // 춘식이들
    CTexture* slaveTex = CTexture::Create(graphicDev, TEX_NORMAL, L"", 0);
    NULL_CHECK_RETURN(slaveTex, E_FAIL);

    // Cow3
    if (FAILED(slaveTex->Add_Texture(L"NPC_Cow3_Appear", TEX_NORMAL,
        L"../Bin/Resource/Texture/NPC/Follower/Cow3/Finalboss_Appear/Follower-finalboss-appear%d.png", 140)))
        return E_FAIL;

    if (FAILED(slaveTex->Add_Texture(L"NPC_Cow3_Idle", TEX_NORMAL,
        L"../Bin/Resource/Texture/NPC/Follower/Cow3/Finalboss_Idle/Follower-finalboss-idle%d.png", 50)))
        return E_FAIL;

    //if (FAILED(slaveTex->Add_Texture(L"NPC_Cow3_Freed", TEX_NORMAL,
    //    L"../Bin/Resource/Texture/NPC/Follower/Cow3/Finalboss_Freed/Follower-finalboss-freed%d.png", 102)))
    //    return E_FAIL;

    // Elephant
    if (FAILED(slaveTex->Add_Texture(L"NPC_Elephant_Appear", TEX_NORMAL,
        L"../Bin/Resource/Texture/NPC/Follower/Elephant/Finalboss_Appear/Follower-finalboss-appear%d.png", 142)))
        return E_FAIL;

    if (FAILED(slaveTex->Add_Texture(L"NPC_Elephant_Idle", TEX_NORMAL,
        L"../Bin/Resource/Texture/NPC/Follower/Elephant/Finalboss_Idle/Follower-finalboss-idle%d.png", 50)))
        return E_FAIL;

    //if (FAILED(slaveTex->Add_Texture(L"NPC_Elephant_Freed", TEX_NORMAL,
    //    L"../Bin/Resource/Texture/NPC/Follower/Elephant/Finalboss_Freed/Follower-finalboss-freed%d.png", 102)))
    //    return E_FAIL;

    // Pig2
    if (FAILED(slaveTex->Add_Texture(L"NPC_Pig2_Appear", TEX_NORMAL,
        L"../Bin/Resource/Texture/NPC/Follower/Pig2/Finalboss_Appear/Follower-finalboss-appear%d.png", 142)))
        return E_FAIL;

    if (FAILED(slaveTex->Add_Texture(L"NPC_Pig2_Idle", TEX_NORMAL,
        L"../Bin/Resource/Texture/NPC/Follower/Pig2/Finalboss_Idle/Follower-finalboss-idle%d.png", 50)))
        return E_FAIL;

    //if (FAILED(slaveTex->Add_Texture(L"NPC_Pig2_Freed", TEX_NORMAL,
    //    L"../Bin/Resource/Texture/NPC/Follower/Pig2/Finalboss_Freed/Follower-finalboss-freed%d.png", 102)))
    //    return E_FAIL;


    if (FAILED(protoMgr->Ready_Prototype(COMPONENTTYPE::TEX_SLAVE_TEST, slaveTex)))
        return E_FAIL;
}

HRESULT CResourceMgr::Ready_UI(DEVICE graphicDev)
{
    auto protoMgr = CProtoMgr::GetInstance();

    //Texture : Player Heart
    if (FAILED(protoMgr->Ready_Prototype(COMPONENTTYPE::TEX_UI_HEART,
        Engine::CTexture::Create(graphicDev, TEX_NORMAL,
            L"../Bin/Resource/Texture/UI/PlayerState/Hp/Hp%d.png", 3))))
        return E_FAIL;

    //circleColorTex
    if (FAILED(protoMgr->Ready_Prototype(COMPONENTTYPE::TEX_UI_COLOR,
        Engine::CTexture::Create(graphicDev, TEX_NORMAL,
            L"../Bin/Resource/Texture/UI/PlayerState/Sermon/Circle1.png", 1))))
        return E_FAIL;

    //circleTex
    if (FAILED(protoMgr->Ready_Prototype(COMPONENTTYPE::TEX_UI_CIRCLE,
        Engine::CTexture::Create(graphicDev, TEX_NORMAL,
            L"../Bin/Resource/Texture/UI/PlayerState/Sermon/Circle_Red.png", 1))))
        return E_FAIL;

    //circleTex
    if (FAILED(protoMgr->Ready_Prototype(COMPONENTTYPE::TEX_UI_WEAPON,
        Engine::CTexture::Create(graphicDev, TEX_NORMAL,
            L"../Bin/Resource/Texture/UI/PlayerState/Icon/Weapon%d.png", 2))))
        return E_FAIL;

    //circleTex
    if (FAILED(protoMgr->Ready_Prototype(COMPONENTTYPE::TEX_UI_SKILL,
        Engine::CTexture::Create(graphicDev, TEX_NORMAL,
            L"../Bin/Resource/Texture/UI/PlayerState/Icon/Skill%d.png", 3))))
        return E_FAIL;

    // 메인 메뉴 이미지

    CTexture* menuChar = CTexture::Create(graphicDev, TEX_NORMAL, L"", 0);
    NULL_CHECK_RETURN(menuChar, E_FAIL);

    if (FAILED(menuChar->Add_Texture(L"MenuChar", TEX_NORMAL,
        L"../Bin/Resource/Texture/MainMenu/MenuChar/Main menu-animation%d.png", 100)))
        return E_FAIL;

    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::TEX_MENU_CHAR, menuChar)))
        return E_FAIL;

    //UIICON 텍스쳐
    vector<wstring> pathVec1;
    pathVec1.push_back(L"../Bin/Resource/Texture/UI/MainMenu/Title0.png");
    pathVec1.push_back(L"../Bin/Resource/Texture/UI/PlayerState/Sermon/Sermon_Icon.png");
    pathVec1.push_back(L"../Bin/Resource/Texture/UI/Inventory/PlayerBG.png");
    pathVec1.push_back(L"../Bin/Resource/Texture/UI/Inventory/Lock.png");
    pathVec1.push_back(L"../Bin/Resource/Texture/UI/Inventory/OutfitCell.png");
    pathVec1.push_back(L"../Bin/Resource/Texture/UI/Inventory/Unlock.png");
    pathVec1.push_back(L"../Bin/Resource/Texture/UI/Inventory/Weapon1.png");
    pathVec1.push_back(L"../Bin/Resource/Texture/UI/Inventory/Weapon2.png");
    pathVec1.push_back(L"../Bin/Resource/Texture/UI/ItemGet/0.png");
    pathVec1.push_back(L"../Bin/Resource/Texture/UI/ItemGet/1.png");
    pathVec1.push_back(L"../Bin/Resource/Texture/Player/Skill/Arrow_White.png");
    pathVec1.push_back(L"../Bin/Resource/Texture/UI/Tarot/ExplainBox/ExplainBox.png");

    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::TEX_UI_ICON, Engine::CTexture::Create(graphicDev, TEX_NORMAL, pathVec1))))
        return E_FAIL;

    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::TEX_UI_SKILLCOUNT, CTexture::Create(graphicDev, TEX_NORMAL,
            L"../Bin/Resource/Texture/UI/PlayerState/Skill/Skillcount%d.png", 2))))
        return E_FAIL;

    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::TEX_UI_CELL, CTexture::Create(graphicDev, TEX_NORMAL,
            L"../Bin/Resource/Texture/UI/Inventory/InventoryIcon_BG%d.png",5))))
        return E_FAIL;

    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::TEX_UI_HIGHLIGHT, CTexture::Create(graphicDev, TEX_NORMAL,
            L"../Bin/Resource/Texture/UI/Inventory/Outline%d.png", 2))))
        return E_FAIL;

    //UIButton 텍스쳐
    vector<wstring> pathVec2;
    pathVec2.push_back(L"../Bin/Resource/Texture/UI/MainMenu/Null.png");
    pathVec2.push_back(L"../Bin/Resource/Texture/UI/MainMenu/Button Black.png");
    pathVec2.push_back(L"../Bin/Resource/Texture/UI/Inventory/ButtonTab0.png");
    pathVec2.push_back(L"../Bin/Resource/Texture/UI/Inventory/ButtonTab1.png");
    pathVec2.push_back(L"../Bin/Resource/Texture/UI/Tarot/ExplainBox/HeartExplain.png");
    pathVec2.push_back(L"../Bin/Resource/Texture/UI/Tarot/ExplainBox/SkillExplain.png");
    pathVec2.push_back(L"../Bin/Resource/Texture/UI/Tarot/ExplainBox/SkillExplain.png");
    pathVec2.push_back(L"../Bin/Resource/Texture/UI/FollowerCommand/Pray_Icon0.png");
    pathVec2.push_back(L"../Bin/Resource/Texture/UI/FollowerCommand/Pray_Icon1.png");
    pathVec2.push_back(L"../Bin/Resource/Texture/UI/FollowerCommand/Axe_Icon0.png");
    pathVec2.push_back(L"../Bin/Resource/Texture/UI/FollowerCommand/Axe_Icon1.png");
    pathVec2.push_back(L"../Bin/Resource/Texture/UI/FollowerCommand/Mining_Icon0.png");
    pathVec2.push_back(L"../Bin/Resource/Texture/UI/FollowerCommand/Mining_Icon1.png");
    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::TEX_UI_BUTTON, Engine::CTexture::Create(graphicDev, TEX_NORMAL, pathVec2))))
        return E_FAIL;

    //// Texture : MainMenu BackGround

    //UIButton 텍스쳐
    vector<wstring> BG;
    BG.push_back(L"../Bin/Resource/Texture/BackGround/MainMenu/MenuMask1.png");
    BG.push_back(L"../Bin/Resource/Texture/MainMenu/RitualStone_Activated.png");

    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::TEX_MAINBG, Engine::CTexture::Create(graphicDev, TEX_NORMAL, BG))))
        return E_FAIL;

    //if (FAILED(protoMgr->Ready_Prototype(COMPONENTTYPE::TEX_MAINBG,
    //    Engine::CTexture::Create(
    //        graphicDev,
    //        TEX_NORMAL,
    //        L"../Bin/Resource/Texture/BackGround/MainMenu/MenuMask1.png",
    //        1))))
    //    return E_FAIL;

    // Texture : Inventory BackGround
    if (FAILED(protoMgr->Ready_Prototype(COMPONENTTYPE::TEX_UI_INVEN,
        Engine::CTexture::Create(
            graphicDev,
            TEX_NORMAL,
            L"../Bin/Resource/Texture/UI/Inventory/Background.png",
            1))))
        return E_FAIL;

    // Texture : Inventory BackGround
    if (FAILED(protoMgr->Ready_Prototype(COMPONENTTYPE::TEX_UI_RITUALCARD,
        Engine::CTexture::Create(
            graphicDev,
            TEX_NORMAL,
            L"../Bin/Resource/Texture/UI/Inventory/RitualCard%d.png",
            3))))
        return E_FAIL;

    CTexture* tarotTex = CTexture::Create(graphicDev, TEX_NORMAL, L"", 0);
    NULL_CHECK_RETURN(tarotTex, E_FAIL);

    if (FAILED(
        tarotTex->Add_Texture(L"Tarot_Heart_Start", TEXTUREID::TEX_NORMAL,
            L"../Bin/Resource/Texture/UI/Tarot/Animation/Start/HeartCard/HeartCard%d.png", 64)))
        return E_FAIL;

    if (FAILED(
        tarotTex->Add_Texture(L"Tarot_Arrow_Start", TEXTUREID::TEX_NORMAL,
            L"../Bin/Resource/Texture/UI/Tarot/Animation/Start/ArrowCard/ArrowCard%d.png", 64)))
        return E_FAIL;

    if (FAILED(
        tarotTex->Add_Texture(L"Tarot_Heart_Choice", TEXTUREID::TEX_NORMAL, L"../Bin/Resource/Texture/UI/Tarot/Animation/Choice/HeartCard_Choice.png",
            1)))
        return E_FAIL;

    if (FAILED(
        tarotTex->Add_Texture(L"Tarot_Arrow_Choice", TEXTUREID::TEX_NORMAL, L"../Bin/Resource/Texture/UI/Tarot/Animation/Choice/ArrowCard_Choice.png",
            1)))
        return E_FAIL;

    if (FAILED(protoMgr->Ready_Prototype(COMPONENTTYPE::TEX_UI_TAROT, tarotTex)))
        return E_FAIL;

    // Texture : Heart Card Explain
    if (FAILED(protoMgr->Ready_Prototype(COMPONENTTYPE::TEX_UI_EXPLAIN,
        Engine::CTexture::Create(
            graphicDev,
            TEX_NORMAL,
            L"../Bin/Resource/Texture/UI/Tarot/ExplainBox/HeartExplain.png",
            1))))
        return E_FAIL;

    // UI_MonsterHp
    vector<wstring> monsterHp_Path;
    monsterHp_Path.emplace_back(L"../Bin/Resource/Texture/UI/HPBar/hpbar2.png");
    monsterHp_Path.emplace_back(L"../Bin/Resource/Texture/UI/HPBar/hpbarBG.png");
    monsterHp_Path.emplace_back(L"../Bin/Resource/Texture/UI/HPBar/hpbarWhite.png");
    monsterHp_Path.emplace_back(L"../Bin/Resource/Texture/UI/HPBar/Boss_HpBar_Background.png");
    monsterHp_Path.emplace_back(L"../Bin/Resource/Texture/UI/HPBar/Boss_HpBar_Fill.png");
    monsterHp_Path.emplace_back(L"../Bin/Resource/Texture/UI/HPBar/Boss_HpBar_Fill_gray.png");

    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::TEX_UI_MONSTERHP, CTexture::Create(graphicDev, TEX_NORMAL, monsterHp_Path))))
        return E_FAIL;

    // UI_Cooking
    vector<wstring> cookingUI;
    cookingUI.push_back(L"../Bin/Resource/Texture/UI/CookUI/CookingBar_White.png");
    cookingUI.push_back(L"../Bin/Resource/Texture/UI/CookUI/Cooking_Marker.png");
    cookingUI.push_back(L"../Bin/Resource/Texture/UI/CookUI/CookingBar_Border.png");
    cookingUI.push_back(L"../Bin/Resource/Texture/UI/CookUI/CookingBar_Green.png");
    cookingUI.push_back(L"../Bin/Resource/Texture/UI/CookUI/CookingBar_Red.png");
    cookingUI.push_back(L"../Bin/Resource/Texture/UI/CookUI/SpeechBubble.png");
    cookingUI.push_back(L"../Bin/Resource/Texture/UI/CookUI/ItemIndicator_Meal.png");
    cookingUI.push_back(L"../Bin/Resource/Texture/UI/CookUI/ItemIndicator_Burned.png");

    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::TEX_UI_COOK, CTexture::Create(graphicDev, TEX_NORMAL, cookingUI))))
        return E_FAIL;

    vector<wstring> buildTex;
    buildTex.emplace_back(L"../Bin/Resource/Texture/UI/Icon_Building/Icon_CookingFire.png");
    buildTex.emplace_back(L"../Bin/Resource/Texture/UI/Icon_Building/Icon_Temple.png");
    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::TEX_UI_BUILD, CTexture::Create(graphicDev, TEX_NORMAL, buildTex))))
        return E_FAIL;

    vector<wstring> decoTex;
    decoTex.emplace_back(L"../Bin/Resource/Texture/UI/Inventory/Title_Bracket_Left.png");
    decoTex.emplace_back(L"../Bin/Resource/Texture/UI/Inventory/Title_Bracket_Right.png");
    decoTex.emplace_back(L"../Bin/Resource/Texture/UI/Deco/Skeleton.png");
    decoTex.emplace_back(L"../Bin/Resource/Texture/UI/Deco/Timer.png");
    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::TEX_UI_DECO, CTexture::Create(graphicDev, TEX_NORMAL, decoTex))))
        return E_FAIL;

    // Dialogue
    CTexture* dialogueTex = CTexture::Create(graphicDev, TEX_NORMAL, L"", 0);
    NULL_CHECK_RETURN(dialogueTex, E_FAIL);

    if (FAILED(dialogueTex->Add_Texture(L"DialogueBox", TEX_NORMAL,
        L"../Bin/Resource/Texture/UI/Talk/SpeechBubble1.png", 1)))
        return E_FAIL;

    if (FAILED(protoMgr->Ready_Prototype(COMPONENTTYPE::TEX_DIALOGUE, dialogueTex)))
        return E_FAIL;

    //UI 상호작용 키 
    vector<wstring> interactionTex;
    interactionTex.emplace_back(L"../Bin/Resource/Texture/UI/Interaction/InterAction_E.png");
    interactionTex.emplace_back(L"../Bin/Resource/Texture/UI/Interaction/InteractionKey0.png");
    interactionTex.emplace_back(L"../Bin/Resource/Texture/UI/Interaction/InteractionKey1.png");
    interactionTex.emplace_back(L"../Bin/Resource/Texture/UI/Interaction/InteractionKey2.png");
    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::TEX_UI_INTERACTION, CTexture::Create(graphicDev, TEX_NORMAL, interactionTex))))
        return E_FAIL;

    //// Texture : Clear BackGround
    if (FAILED(protoMgr->Ready_Prototype(COMPONENTTYPE::TEX_UI_CLEAR,
        Engine::CTexture::Create(
            graphicDev,
            TEX_NORMAL,
            L"../Bin/Resource/Texture/UI/BG/ClearBG.png",
            1))))
        return E_FAIL;

    // Texture : MainMenu Extra
    //
    vector<wstring> extra;
    extra.push_back(L"../Bin/Resource/Texture/MainMenu/Follower_Front_0.png");
    extra.push_back(L"../Bin/Resource/Texture/MainMenu/Follower_SideFront0.png");
    extra.push_back(L"../Bin/Resource/Texture/MainMenu/Follower_SideFront0.png");
    extra.push_back(L"../Bin/Resource/Texture/MainMenu/Follower_Side_0.png");
    extra.push_back(L"../Bin/Resource/Texture/MainMenu/Follower_Side_0.png");
    extra.push_back(L"../Bin/Resource/Texture/MainMenu/Follower_SideBack_0.png");
    extra.push_back(L"../Bin/Resource/Texture/MainMenu/Follower_SideBack_0.png");
    extra.push_back(L"../Bin/Resource/Texture/MainMenu/Follower_Back_0.png");
    extra.push_back(L"../Bin/Resource/Texture/MainMenu/Follower_Back_0.png");
    extra.push_back(L"../Bin/Resource/Texture/MainMenu/Follower_SideBack_0.png");
    extra.push_back(L"../Bin/Resource/Texture/MainMenu/Follower_SideBack_0.png");
    extra.push_back(L"../Bin/Resource/Texture/MainMenu/Follower_Side_0.png");
    extra.push_back(L"../Bin/Resource/Texture/MainMenu/Follower_Side_0.png");
    extra.push_back(L"../Bin/Resource/Texture/MainMenu/Follower_SideFront0.png");
    extra.push_back(L"../Bin/Resource/Texture/MainMenu/Follower_SideFront0.png");
    extra.push_back(L"../Bin/Resource/Texture/MainMenu/Follower_Front_0.png");

    //extra.push_back(L"../Bin/Resource/Texture/MainMenu/Follower_0.png");

    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::TEX_MAINMENU, CTexture::Create(graphicDev, TEX_NORMAL, extra))))
        return E_FAIL;

    if (FAILED(protoMgr->Ready_Prototype(COMPONENTTYPE::TEX_UI_COMMAND,
        Engine::CTexture::Create(
            graphicDev,
            TEX_NORMAL,
            L"../Bin/Resource/Texture/UI/FollowerCommand/BackGround0.png",
            1))))
        return E_FAIL;

    return S_OK;
}

HRESULT CResourceMgr::Ready_Item(DEVICE graphicDev)
{
    auto protoMgr = CProtoMgr::GetInstance();

    //아이템
    vector<Item>    item = CItemDB::GetInstance()->GetVector();
    vector<wstring> pathVec;
    for (int i = 0; i < item.size(); i++)
    {
        wstring path = CItemDB::GetInstance()->Utf8ToWstring(item[i].UIPath);
        pathVec.push_back(path);
    }

    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::TEX_ITEM, Engine::CTexture::Create(graphicDev, TEX_NORMAL, pathVec))))
        return E_FAIL;

    return S_OK;
}

HRESULT CResourceMgr::Ready_Terrain(DEVICE graphicDev)
{
    auto protoMgr = CProtoMgr::GetInstance();

    // TerrainRenderer
    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::TERRAIN_RENDER, CTerrainRenderer::Create(graphicDev))))
        return E_FAIL;

    // WallRenderer
    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::WALL_RENDERER, CWallRenderer::Create(graphicDev))))
        return E_FAIL;

    // Monster Spawner
    if (FAILED(protoMgr->Ready_Prototype(COMPONENTTYPE::OBJECT_SPAWNER,
        CObjectSpawner::Create(graphicDev))))
        return E_FAIL;

    // Terrain_284
    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::TEX_TILE_284, CTexture::Create(graphicDev, TEX_NORMAL,
            L"../Bin/Resource/Texture/Terrain/TileAtlas_284_Blank.png", 1))))
        return E_FAIL;

    // Terrain_1024
    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::TEX_TILE_1024, CTexture::Create(graphicDev, TEX_NORMAL,
            L"../Bin/Resource/Texture/Terrain/TileAtlas_1024.png", 1))))
        return E_FAIL;

    // Terrain_Outline
    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::TEX_TILE_OUTLINE, CTexture::Create(graphicDev, TEX_NORMAL,
            L"../Bin/Resource/Texture/Terrain/TileOutline%d.png", 3))))
        return E_FAIL;

    // Terrain_Outline2
    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::TEX_TILE_OUTLINE2, CTexture::Create(graphicDev, TEX_NORMAL,
            L"../Bin/Resource/Texture/Terrain/TileOutline4.png", 1))))
        return E_FAIL;

    return S_OK;
}

HRESULT CResourceMgr::Ready_Decoration(DEVICE graphicDev)
{
    auto protoMgr = CProtoMgr::GetInstance();

    // Grass
    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::TEX_GRASS, CTexture::Create(graphicDev, TEX_NORMAL,
            L"../Bin/Resource/Texture/MapObject/WeedShrub/WeedShrub%d.png", 56))))
        return E_FAIL;

    // Rubble
    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::TEX_RUBBLE, CTexture::Create(graphicDev, TEX_NORMAL,
            L"../Bin/Resource/Texture/MapObject/Rubble/Rubble%d.png", 94))))
        return E_FAIL;

    // Deco
    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::TEX_DECO, CTexture::Create(graphicDev, TEX_NORMAL,
            L"../Bin/Resource/Texture/MapObject/Deco/Village_Deco%d.png", 83))))
        return E_FAIL;

    // Dungeon_Deco
    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::TEX_DUNGEON_DECO, CTexture::Create(graphicDev, TEX_NORMAL,
            L"../Bin/Resource/Texture/MapObject/Dungeon_Deco2/Dungeon_Deco2_%d.png", 165))))
        return E_FAIL;

    // ETC
    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::TEX_ETC, CTexture::Create(graphicDev, TEX_NORMAL,
            L"../Bin/Resource/Texture/MapObject/ETC/ETC%d.png", 56))))
        return E_FAIL;

    // 라즈베리 풀
    vector<wstring> pathVec2;
    pathVec2.push_back(L"../Bin/Resource/Texture/InteractiveObj/BerryBush1.png");
    pathVec2.push_back(L"../Bin/Resource/Texture/InteractiveObj/BerryBush1_stump.png");
    pathVec2.push_back(L"../Bin/Resource/Texture/InteractiveObj/BerryBush2.png");
    pathVec2.push_back(L"../Bin/Resource/Texture/InteractiveObj/BerryBush2_stump.png");
    pathVec2.push_back(L"../Bin/Resource/Texture/InteractiveObj/BerryBush3.png");
    pathVec2.push_back(L"../Bin/Resource/Texture/InteractiveObj/BerryBush3_stump.png");
    pathVec2.push_back(L"../Bin/Resource/Texture/InteractiveObj/BerryBushCombined.png");

    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::TEX_BERRY, Engine::CTexture::Create(graphicDev, TEX_NORMAL, pathVec2))))
        return E_FAIL;

    // 돌
    vector<wstring> pathVec3;
    pathVec3.push_back(L"../Bin/Resource/Texture/InteractiveObj/Rock0.png");
    pathVec3.push_back(L"../Bin/Resource/Texture/InteractiveObj/Rock1.png");
    pathVec3.push_back(L"../Bin/Resource/Texture/InteractiveObj/Rock2.png");

    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::TEX_ROCK, Engine::CTexture::Create(graphicDev, TEX_NORMAL, pathVec3))))
        return E_FAIL;

    // 나무
    vector<wstring> pathVec4;
    pathVec4.push_back(L"../Bin/Resource/Texture/InteractiveObj/RubbleTree1.png");
    pathVec4.push_back(L"../Bin/Resource/Texture/InteractiveObj/RubbleTree2.png");
    pathVec4.push_back(L"../Bin/Resource/Texture/InteractiveObj/RubbleTree3.png");
    pathVec4.push_back(L"../Bin/Resource/Texture/InteractiveObj/RubbleTree4.png");

    pathVec4.push_back(L"../Bin/Resource/Texture/InteractiveObj/SmallTree1.png");
    pathVec4.push_back(L"../Bin/Resource/Texture/InteractiveObj/SmallTree2.png");
    pathVec4.push_back(L"../Bin/Resource/Texture/InteractiveObj/SmallTree3.png");
    pathVec4.push_back(L"../Bin/Resource/Texture/InteractiveObj/SmallTree4.png");

    pathVec4.push_back(L"../Bin/Resource/Texture/InteractiveObj/BigTree1.png");
    pathVec4.push_back(L"../Bin/Resource/Texture/InteractiveObj/BigTree2.png");
    pathVec4.push_back(L"../Bin/Resource/Texture/InteractiveObj/BigTree3.png");
    pathVec4.push_back(L"../Bin/Resource/Texture/InteractiveObj/BigTree4.png");
    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::TEX_TREE, Engine::CTexture::Create(graphicDev, TEX_NORMAL, pathVec4))))
        return E_FAIL;

    return S_OK;
}

HRESULT CResourceMgr::Ready_Particle(DEVICE graphicDev)
{
    auto protoMgr = CProtoMgr::GetInstance();

    CTexture* particleTex = CTexture::Create(graphicDev, TEX_NORMAL, L"", 0);
    NULL_CHECK_RETURN(particleTex, E_FAIL);

    // Hit Star
    if (FAILED(particleTex->Add_Texture(L"HitEffect_Star", TEX_NORMAL,
        L"../Bin/Resource/Texture/Effect/HitEffect_Star/HitEffect_Star%d.png", 14)))
        return E_FAIL;

    // Blood
    if (FAILED(particleTex->Add_Texture(L"HitEffect_Blood", TEX_NORMAL,
        L"../Bin/Resource/Texture/Effect/HitEffect_Blood/HitEffect_Blood%d.png", 12)))
        return E_FAIL;

    // Disappear
    if (FAILED(particleTex->Add_Texture(L"Effect_Disappear", TEX_NORMAL,
        L"../Bin/Resource/Texture/Effect/Disappear/Disappear%d.png", 5)))
        return E_FAIL;

    // 추가 예정

    if (FAILED(particleTex->Add_Texture(L"Pickup_Item", TEX_NORMAL,
        L"../Bin/Resource/Texture/Effect/Pickup_Item/Pickup_Item%d.png", 5)))
        return E_FAIL;

    if (FAILED(protoMgr->Ready_Prototype(COMPONENTTYPE::TEX_PARTICLE, particleTex)))
        return E_FAIL;

    // FOG
    if (FAILED(protoMgr->Ready_Prototype(COMPONENTTYPE::TEX_FOG,
        CTexture::Create(graphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Effect/Fog/fog.png", 1))))
        return E_FAIL;

    // Rock
    if (FAILED(protoMgr->Ready_Prototype(COMPONENTTYPE::TEX_ROCK_PARTICLE,
        CTexture::Create(graphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Particle/Rock/Rock_Particle%d.png", 2))))
        return E_FAIL;

    // Grass
    if (FAILED(protoMgr->Ready_Prototype(COMPONENTTYPE::TEX_GRASS_PARTICLE,
        CTexture::Create(graphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Particle/Grass/Grass_Particle%d.png", 4))))
        return E_FAIL;

    // Tree
    if (FAILED(protoMgr->Ready_Prototype(COMPONENTTYPE::TEX_TREE_PARTICLE,
        CTexture::Create(graphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Particle/Tree/Tree_Particle%d.png", 6))))
        return E_FAIL;

    return S_OK;
}

HRESULT CResourceMgr::Ready_Building(DEVICE graphicDev)
{
    auto protoMgr = CProtoMgr::GetInstance();

    // Workbench
    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::TEX_WORKBENCH,
        CTexture::Create(graphicDev,TEX_NORMAL, L"../Bin/Resource/Texture/Building/Workbench.png", 1))))
        return E_FAIL;

    // Dotted Circle
    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::TEX_DOTTEDCIRCLE,
        CTexture::Create(graphicDev,TEX_NORMAL, L"../Bin/Resource/Texture/Building/Dotted_Circle.png", 1))))
        return E_FAIL;

    // Temple
    vector<wstring> textures;
    textures.emplace_back(L"../Bin/Resource/Texture/Building/Temple_Front.png");
    textures.emplace_back(L"../Bin/Resource/Texture/Building/Temple_Roof.png");

    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::TEX_TEMPLE,
        CTexture::Create(graphicDev, TEX_NORMAL, textures))))
        return E_FAIL;

    // Kitchen
    textures.clear();
    textures.emplace_back(L"../Bin/Resource/Texture/Building/Kitchen_Pot.png");
    textures.emplace_back(L"../Bin/Resource/Texture/Building/Kitchen_Stove.png");
    textures.emplace_back(L"../Bin/Resource/Texture/Building/Kitchen_RecipeBook.png");
    textures.emplace_back(L"../Bin/Resource/Texture/Building/Kitchen_Utensils.png");

    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::TEX_KITCHEN,
        CTexture::Create(graphicDev, TEX_NORMAL, textures))))
        return E_FAIL;

    return S_OK;
}

HRESULT CResourceMgr::Ready_Follower(DEVICE graphicDev)
{
    auto protoMgr = CProtoMgr::GetInstance();

#pragma region --- NPC Components ---

    // Interaction Checker
    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::NPC_INTERACTION_CHECKER, CInteractionChecker::Create(graphicDev))))
        return E_FAIL;

    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::NPC_CONTROLLER, CNPCController::Create(graphicDev))))
        return E_FAIL;

#pragma endregion

#pragma region --- Cow ---

    CTexture* cowTex = CTexture::Create(graphicDev, TEX_NORMAL, L"", 0);
    NULL_CHECK_RETURN(cowTex, E_FAIL);

    if (FAILED(cowTex->Add_Texture(L"CowIdle", TEX_NORMAL,
        L"../Bin/Resource/Texture/NPC/Follower/Cow3/Idle/Follower-idle%d.png", 49)))
        return E_FAIL;

    if (FAILED(cowTex->Add_Texture(L"CowRun", TEX_NORMAL,
        L"../Bin/Resource/Texture/NPC/Follower/Cow3/Run/Follower-run%d.png", 29)))
        return E_FAIL;

    if (FAILED(cowTex->Add_Texture(L"CowWalkPast", TEX_NORMAL,
        L"../Bin/Resource/Texture/NPC/Follower/Cow3/Walkpast/Follower-conversations-walkpast-nice%d.png", 86)))
        return E_FAIL;

    if (FAILED(cowTex->Add_Texture(L"CowBuild", TEX_NORMAL,
        L"../Bin/Resource/Texture/NPC/Follower/Cow3/Build/Follower-build%d.png", 53)))
        return E_FAIL;

    if (FAILED(cowTex->Add_Texture(L"CowChopWood", TEX_NORMAL,
        L"../Bin/Resource/Texture/NPC/Follower/Cow3/ChopWood/Follower-chop-wood%d.png", 81)))
        return E_FAIL;

    if (FAILED(cowTex->Add_Texture(L"CowMining", TEX_NORMAL,
        L"../Bin/Resource/Texture/NPC/Follower/Cow3/Mining/Follower-mining%d.png", 48)))
        return E_FAIL;

    if (FAILED(cowTex->Add_Texture(L"CowPray", TEX_NORMAL,
        L"../Bin/Resource/Texture/NPC/Follower/Cow3/Pray/Follower-pray%d.png", 62)))
        return E_FAIL;

    if (FAILED(cowTex->Add_Texture(L"CowPrayPeek", TEX_NORMAL,
        L"../Bin/Resource/Texture/NPC/Follower/Cow3/PrayPeek/Follower-pray-peek%d.png", 62)))
        return E_FAIL;

    if (FAILED(cowTex->Add_Texture(L"CowEat", TEX_NORMAL,
        L"../Bin/Resource/Texture/NPC/Follower/Cow3/Eat/Follower-food-food_eat%d.png", 50)))
        return E_FAIL;

    if (FAILED(cowTex->Add_Texture(L"CowSpawn", TEX_NORMAL,
        L"../Bin/Resource/Texture/NPC/Follower/Cow3/Spawn/Follower-spawn-in-base%d.png", 172)))
        return E_FAIL;

    if (FAILED(cowTex->Add_Texture(L"CowTransform1", TEX_NORMAL,
        L"../Bin/Resource/Texture/NPC/Follower/Cow3/Transform1/Follower-transform1-%d.png", 94)))
        return E_FAIL;

    if (FAILED(cowTex->Add_Texture(L"CowTransform2", TEX_NORMAL,
        L"../Bin/Resource/Texture/NPC/Follower/Cow3/Transform2/Follower-transform2-%d.png", 26)))
        return E_FAIL;

    if (FAILED(cowTex->Add_Texture(L"CowTransform3", TEX_NORMAL,
        L"../Bin/Resource/Texture/NPC/Follower/Cow3/Transform3/Follower-transform3-%d.png", 89)))
        return E_FAIL;

    if (FAILED(protoMgr->Ready_Prototype(COMPONENTTYPE::TEX_COW, cowTex)))
        return E_FAIL;

#pragma endregion

#pragma region --- Elephant ---

    CTexture* elephantTex = CTexture::Create(graphicDev, TEX_NORMAL, L"", 0);
    NULL_CHECK_RETURN(elephantTex, E_FAIL);

    if (FAILED(elephantTex->Add_Texture(L"ElephantIdle", TEX_NORMAL,
        L"../Bin/Resource/Texture/NPC/Follower/Elephant/Idle/Follower-idle%d.png", 49)))
        return E_FAIL;

    if (FAILED(elephantTex->Add_Texture(L"ElephantRun", TEX_NORMAL,
        L"../Bin/Resource/Texture/NPC/Follower/Elephant/Run/Follower-run%d.png", 29)))
        return E_FAIL;

    if (FAILED(elephantTex->Add_Texture(L"ElephantWalkPast", TEX_NORMAL,
        L"../Bin/Resource/Texture/NPC/Follower/Elephant/Walkpast/Follower-conversations-walkpast-love%d.png", 86)))
        return E_FAIL;

    if (FAILED(elephantTex->Add_Texture(L"ElephantBuild", TEX_NORMAL,
        L"../Bin/Resource/Texture/NPC/Follower/Elephant/Build/Follower-build%d.png", 53)))
        return E_FAIL;

    if (FAILED(elephantTex->Add_Texture(L"ElephantChopWood", TEX_NORMAL,
        L"../Bin/Resource/Texture/NPC/Follower/Elephant/ChopWood/Follower-chop-wood%d.png", 81)))
        return E_FAIL;

    if (FAILED(elephantTex->Add_Texture(L"ElephantMining", TEX_NORMAL,
        L"../Bin/Resource/Texture/NPC/Follower/Elephant/Mining/Follower-mining%d.png", 48)))
        return E_FAIL;

    if (FAILED(elephantTex->Add_Texture(L"ElephantPray", TEX_NORMAL,
        L"../Bin/Resource/Texture/NPC/Follower/Elephant/Pray/Follower-pray%d.png", 62)))
        return E_FAIL;

    if (FAILED(elephantTex->Add_Texture(L"ElephantPrayPeek", TEX_NORMAL,
        L"../Bin/Resource/Texture/NPC/Follower/Elephant/PrayPeek/Follower-pray-peek%d.png", 62)))
        return E_FAIL;

    if (FAILED(elephantTex->Add_Texture(L"ElephantEat", TEX_NORMAL,
        L"../Bin/Resource/Texture/NPC/Follower/Elephant/Eat/Follower-food-food_eat%d.png", 50)))
        return E_FAIL;

    if (FAILED(elephantTex->Add_Texture(L"ElephantSpawn", TEX_NORMAL,
        L"../Bin/Resource/Texture/NPC/Follower/Elephant/Spawn/Follower-spawn-in-base%d.png", 172)))
        return E_FAIL;

    if (FAILED(elephantTex->Add_Texture(L"ElephantTransform1", TEX_NORMAL,
        L"../Bin/Resource/Texture/NPC/Follower/Elephant/Transform1/Follower-transform1-%d.png", 94)))
        return E_FAIL;

    if (FAILED(elephantTex->Add_Texture(L"ElephantTransform2", TEX_NORMAL,
        L"../Bin/Resource/Texture/NPC/Follower/Elephant/Transform2/Follower-transform2-%d.png", 26)))
        return E_FAIL;

    if (FAILED(elephantTex->Add_Texture(L"ElephantTransform3", TEX_NORMAL,
        L"../Bin/Resource/Texture/NPC/Follower/Elephant/Transform3/Follower-transform3-%d.png", 89)))
        return E_FAIL;

    if (FAILED(protoMgr->Ready_Prototype(COMPONENTTYPE::TEX_ELEPHANT, elephantTex)))
        return E_FAIL;

#pragma endregion

#pragma region --- Pig ---

    CTexture* pigTex = CTexture::Create(graphicDev, TEX_NORMAL, L"", 0);
    NULL_CHECK_RETURN(pigTex, E_FAIL);

    if (FAILED(pigTex->Add_Texture(L"PigIdle", TEX_NORMAL,
        L"../Bin/Resource/Texture/NPC/Follower/Pig2/Idle/Follower-idle%d.png", 49)))
        return E_FAIL;

    if (FAILED(pigTex->Add_Texture(L"PigRun", TEX_NORMAL,
        L"../Bin/Resource/Texture/NPC/Follower/Pig2/Run/Follower-run%d.png", 29)))
        return E_FAIL;

    if (FAILED(pigTex->Add_Texture(L"PigWalkPast", TEX_NORMAL,
        L"../Bin/Resource/Texture/NPC/Follower/Pig2/Walkpast/Follower-conversations-walkpast-nice%d.png", 86)))
        return E_FAIL;

    if (FAILED(pigTex->Add_Texture(L"PigBuild", TEX_NORMAL,
        L"../Bin/Resource/Texture/NPC/Follower/Pig2/Build/Follower-build%d.png", 53)))
        return E_FAIL;

    if (FAILED(pigTex->Add_Texture(L"PigChopWood", TEX_NORMAL,
        L"../Bin/Resource/Texture/NPC/Follower/Pig2/ChopWood/Follower-chop-wood%d.png", 81)))
        return E_FAIL;

    if (FAILED(pigTex->Add_Texture(L"PigMining", TEX_NORMAL,
        L"../Bin/Resource/Texture/NPC/Follower/Pig2/Mining/Follower-mining%d.png", 48)))
        return E_FAIL;

    if (FAILED(pigTex->Add_Texture(L"PigPray", TEX_NORMAL,
        L"../Bin/Resource/Texture/NPC/Follower/Pig2/Pray/Follower-pray%d.png", 62)))
        return E_FAIL;

    if (FAILED(pigTex->Add_Texture(L"PigPrayPeek", TEX_NORMAL,
        L"../Bin/Resource/Texture/NPC/Follower/Pig2/PrayPeek/Follower-pray-peek%d.png", 62)))
        return E_FAIL;

    if (FAILED(pigTex->Add_Texture(L"PigEat", TEX_NORMAL,
        L"../Bin/Resource/Texture/NPC/Follower/Pig2/Eat/Follower-food-food_eat%d.png", 50)))
        return E_FAIL;

    if (FAILED(pigTex->Add_Texture(L"PigSpawn", TEX_NORMAL,
        L"../Bin/Resource/Texture/NPC/Follower/Pig2/Spawn/Follower-spawn-in-base%d.png", 172)))
        return E_FAIL;

    if (FAILED(pigTex->Add_Texture(L"PigTransform1", TEX_NORMAL,
        L"../Bin/Resource/Texture/NPC/Follower/Pig2/Transform1/Follower-transform1-%d.png", 94)))
        return E_FAIL;

    if (FAILED(pigTex->Add_Texture(L"PigTransform2", TEX_NORMAL,
        L"../Bin/Resource/Texture/NPC/Follower/Pig2/Transform2/Follower-transform2-%d.png", 26)))
        return E_FAIL;

    if (FAILED(pigTex->Add_Texture(L"PigTransform3", TEX_NORMAL,
        L"../Bin/Resource/Texture/NPC/Follower/Pig2/Transform3/Follower-transform3-%d.png", 89)))
        return E_FAIL;

    if (FAILED(protoMgr->Ready_Prototype(COMPONENTTYPE::TEX_PIG, pigTex)))
        return E_FAIL;

#pragma endregion

    return S_OK;
}

HRESULT CResourceMgr::Ready_MiniBoss(DEVICE graphicDev)
{
    auto protoMgr = CProtoMgr::GetInstance();

    CTexture* MiniBossTex = CTexture::Create(graphicDev, TEX_NORMAL, L"", 0);
    NULL_CHECK_RETURN(MiniBossTex, E_FAIL);

    if (FAILED(MiniBossTex->Add_Texture(L"Eligos_roar", TEX_NORMAL,
        L"../Bin/Resource/Texture/MiniBoss/Eligos_roar/Eligos_roar%d.png", 140)))
        return E_FAIL;

    if (FAILED(MiniBossTex->Add_Texture(L"Eligos_fly", TEX_NORMAL,
        L"../Bin/Resource/Texture/MiniBoss/Eligos_fly/Eligos_fly%d.png", 42)))
        return E_FAIL;

    if (FAILED(MiniBossTex->Add_Texture(L"Eligos_burpcharge", TEX_NORMAL,
        L"../Bin/Resource/Texture/MiniBoss/Eligos_burpcharge/Eligos_burpcharge%d.png", 50)))
        return E_FAIL;

    if (FAILED(MiniBossTex->Add_Texture(L"Eligos_burp", TEX_NORMAL,
        L"../Bin/Resource/Texture/MiniBoss/Eligos_burp/Eligos_burp%d.png", 42)))
        return E_FAIL;

    if (FAILED(MiniBossTex->Add_Texture(L"Eligos_attackcharge", TEX_NORMAL,
        L"../Bin/Resource/Texture/MiniBoss/Eligos_attackcharge/Eligos_attackcharge%d.png", 59)))
        return E_FAIL;

    if (FAILED(MiniBossTex->Add_Texture(L"Eligos_attack", TEX_NORMAL,
        L"../Bin/Resource/Texture/MiniBoss/Eligos_attack/Eligos_attack%d.png", 32)))
        return E_FAIL;

    if (FAILED(MiniBossTex->Add_Texture(L"Eligos_attackcharge_bullet", TEX_NORMAL,
        L"../Bin/Resource/Texture/MiniBoss/Eligos_attackcharge/Eligos_attackcharge%d.png", 59)))
        return E_FAIL;

    if (FAILED(MiniBossTex->Add_Texture(L"Eligos_attack_bullet", TEX_NORMAL,
        L"../Bin/Resource/Texture/MiniBoss/Eligos_attack/Eligos_attack%d.png", 32)))
        return E_FAIL;

    if (FAILED(protoMgr->Ready_Prototype(COMPONENTTYPE::TEX_MINIBOSS, MiniBossTex)))
        return E_FAIL;

    CTexture* flyTex = CTexture::Create(graphicDev, TEX_NORMAL, L"", 0);
    NULL_CHECK_RETURN(flyTex, E_FAIL);

    if (FAILED(flyTex->Add_Texture(L"Mortar_fly", TEX_NORMAL,
        L"../Bin/Resource/Texture/MiniBoss/Mortar_fly/Mortar_fly%d.png", 27)))
        return E_FAIL;

    if (FAILED(protoMgr->Ready_Prototype(COMPONENTTYPE::TEX_FLY, flyTex)))
        return E_FAIL;

    return S_OK;
}

void CResourceMgr::Free()
{ }
