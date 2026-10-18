#include "pch.h"
#include "CPlayer.h"

#include "CAnimator.h"
#include "CAttackBuilder.h"
#include "CAttackSphere.h"
#include "CBoxCollider.h"
#include "CCombatStat.h"
#include "CCreateHelper.h"
#include "CDInputMgr.h"
#include "CDropSystem.h"
#include "CInventory.h"
#include "CPlayerAttack.h"
#include "CPlayerConstruction.h"
#include "CPlayerController.h"
#include "CPlayerGathering.h"
#include "CPlayerInput.h"
#include "CPlayerMovement.h"
#include "CPlayerSkill.h"
#include "CRigidbody.h"
#include "CSoundMgr.h"
#include "CState.h"
#include "CTexture.h"
#include "CTransform.h"
#include "CWeaponEquip.h"
#include "CWorldInteractive.h"
#include "CUI_Interactive.h"
#include "CLayer.h"
#include "CBuildingBase.h"

CPlayer::CPlayer(DEVICE graphicDev)
    : CCharacterBase(graphicDev),
      m_RigidbodyCom(nullptr),
      m_WeaponEquipCom(nullptr),
      m_InventoryCom(nullptr),
      m_PlayerControllerCom(nullptr),
      m_PlayerInputCom(nullptr),
      m_PlayerMovementCom(nullptr),
      m_PlayerAttackCom(nullptr),
      m_PlayerSkillCom(nullptr),
      m_PlayerGatherCom(nullptr),
      m_PlayerConstructionCom(nullptr)

{ }

CPlayer::CPlayer(const CPlayer& rhs)
    : CCharacterBase(rhs),
      m_RigidbodyCom(nullptr),
      m_WeaponEquipCom(nullptr),
      m_InventoryCom(nullptr),
      m_PlayerControllerCom(nullptr),
      m_PlayerInputCom(nullptr),
      m_PlayerMovementCom(nullptr),
      m_PlayerAttackCom(nullptr),
      m_PlayerSkillCom(nullptr),
      m_PlayerGatherCom(nullptr),
      m_PlayerConstructionCom(nullptr)
{ }

CPlayer::~CPlayer()
{ }

HRESULT CPlayer::Ready_GameObject()
{
    if (FAILED(CCharacterBase::Ready_GameObject()))
        return E_FAIL;

    m_CombatStatCom->Set_Hp(100.f);
    m_CombatStatCom->Set_MaxHp(100.f);
    m_CombatStatCom->Set_Attack(50.f);
    m_CombatStatCom->Set_Mp(5.f);
    m_CombatStatCom->Set_Speed(5.f);

    // Hit 상태로 전환할 것인가? [ 보스1, 보스2 ]만 False로 세팅할듯 추가로 중간보스가 있다면 세팅
    m_CombatStatCom->Set_HitState(true);

    m_CombatStatCom->Set_OnDamagedEvent([&]()
    {
        CSoundMgr::GetInstance()->Play_Once(L"Player/Player_GetHit.wav",
                                        SOUND_EFFECT,
                                        0.5f);
    });


    m_TransformCom->Set_Pos(_vec3(20.f, 1.f, 15.f));
    m_TransformCom->Set_Scale(_vec3(1.f, 1.f, 1.f));
    m_TransformCom->Set_RotationAxis(ROT_X, D3DXToRadian(45.f));

    //!! Rigidbody에서 중력만 사용 안함
    m_RigidbodyCom->Set_UseAccel(true);
    m_RigidbodyCom->Set_UseFriction(true);
    m_RigidbodyCom->Set_UseGravity(false);

    if (m_BoxColCom)
        m_BoxColCom->Set_CollisionLayer(COLLISION_LAYERTYPE::PLAYER);

    return S_OK;
}

void CPlayer::Initialize_GameObject()
{
    CCharacterBase::Initialize_GameObject();

    m_PlayerControllerCom->Init_OwnerComponent();
    m_PlayerMovementCom->Init_OwnerComponent();
    m_PlayerAttackCom->Init_OwnerComponent();
    m_PlayerSkillCom->Init_OwnerComponent();

    m_InteractiveUI = CUI_Interactive::Create(m_GraphicDev);
    CManagement::GetInstance()->Get_Scene()->Get_Layer(LAYERTYPE::UI)->Add_GameObject(OBJTYPE::UI, m_InteractiveUI);
}

_int CPlayer::Update_GameObject(const _float& timeDelta)
{
    _int exit = CCharacterBase::Update_GameObject(timeDelta);

    // 안은수 테스트
    if (CDInputMgr::GetInstance()->Get_DIKeyState(DIK_BACKSPACE))
        CDropSystem::GetInstance()->SpawnAllFromInven(m_GraphicDev, m_InventoryCom);

    return exit;
}

void CPlayer::LateUpdate_GameObject(const _float& timeDelta)
{
    CCharacterBase::LateUpdate_GameObject(timeDelta);
}

void CPlayer::Render_GameObject()
{
    CCharacterBase::Render_GameObject();
}

void CPlayer::OnBeginOverlap(CCollider* self, CCollider* other)
{
    CCharacterBase::OnBeginOverlap(self, other);
    m_PlayerGatherCom->OnBeginOverlap(self, other);
    m_PlayerConstructionCom->OnOverlapEnter(self, other);

    m_OverlapBegin = true;
}

void CPlayer::OnStayOverlap(CCollider* self, CCollider* other)
{
    CCharacterBase::OnStayOverlap(self, other);

   CWorldInteractive* worldInteractiveObj = dynamic_cast<CWorldInteractive*>(other->Get_Owner());
   CBuildingBase* building = dynamic_cast<CBuildingBase*>(other->Get_Owner());
   CCharacterBase* character = dynamic_cast<CCharacterBase*>(other->Get_Owner());
   
   if ((worldInteractiveObj||building||character)&&m_OverlapBegin)
   {
       m_InteractiveUI->Set_TextureIndex(2);
       m_InteractiveUI->Set_IsActive(true);
       m_InteractiveUI->Get_Transform()->Set_Scale(110.f,35.f,1.f);

   
       if (CDInputMgr::GetInstance()->KeyDown(DIK_E))
       {
           m_InteractiveUI->Set_IsActive(false);
           m_OverlapBegin = false;
       }
   }
    

}

void CPlayer::OnEndOverlap(CCollider* self, CCollider* other)
{
    CCharacterBase::OnEndOverlap(self, other);

    m_PlayerGatherCom->OnEndOverlap(self, other);
    m_PlayerConstructionCom->OnOverlapExit(self, other);
    CWorldInteractive* worldInteractiveObj = dynamic_cast<CWorldInteractive*>(other->Get_Owner());
    CBuildingBase* building = dynamic_cast<CBuildingBase*>(other->Get_Owner());
    CCharacterBase* character = dynamic_cast<CCharacterBase*>(other->Get_Owner());
    if (worldInteractiveObj|| building|| character)
    {
        m_InteractiveUI->Set_IsActive(false);

    }

    //cout << "Player HitOut" << endl;
}

void CPlayer::Render_Editor()
{
    CCharacterBase::Render_Editor();

    ImGui::Text("=== Player ===");
}

HRESULT CPlayer::Add_Component()
{
    // 부모쪽에 Transform, Buffer, Animator, State, CombatStat, BoxCollider 있음
    if (FAILED(CCharacterBase::Add_Component()))
        return E_FAIL;

    // Texture
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_PLAYER);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_PLAYER, m_TextureCom });

    m_AnimatorCom->Set_TextureType(COMPONENTTYPE::TEX_PLAYER);

    // Inventory
    m_InventoryCom = CreateProtoComponent<CInventory>(this, COMPONENTTYPE::INVENTORY);
    NULL_CHECK_RETURN(m_InventoryCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::INVENTORY, m_InventoryCom });

    m_InventoryCom->SetInvenSlotNum(12);

    // Weapon Equip
    m_WeaponEquipCom = CreateProtoComponent<CWeaponEquip>(this, COMPONENTTYPE::WEAPON_EQUIP);
    NULL_CHECK_RETURN(m_WeaponEquipCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::WEAPON_EQUIP, m_WeaponEquipCom });

    // Rigidbody
    m_RigidbodyCom = CreateProtoComponent<CRigidbody>(this, COMPONENTTYPE::RIGIDBODY);
    NULL_CHECK_RETURN(m_RigidbodyCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::RIGIDBODY, m_RigidbodyCom });

    // Player Controller
    m_PlayerControllerCom = CreateProtoComponent<CPlayerController>(this, COMPONENTTYPE::PL_CONTROLLER);
    NULL_CHECK_RETURN(m_PlayerControllerCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::PL_CONTROLLER, m_PlayerControllerCom });

    // Player Input
    m_PlayerInputCom = CreateProtoComponent<CPlayerInput>(this, COMPONENTTYPE::PL_INPUT);
    NULL_CHECK_RETURN(m_PlayerInputCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::PL_INPUT, m_PlayerInputCom });

    // Player Movement
    m_PlayerMovementCom = CreateProtoComponent<CPlayerMovement>(this, COMPONENTTYPE::PL_MOVEMENT);
    NULL_CHECK_RETURN(m_PlayerMovementCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::PL_MOVEMENT, m_PlayerMovementCom });

    // Player Attack
    m_PlayerAttackCom = CreateProtoComponent<CPlayerAttack>(this, COMPONENTTYPE::PL_ATTACK);
    NULL_CHECK_RETURN(m_PlayerAttackCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::PL_ATTACK, m_PlayerAttackCom });

    // Player Skill
    m_PlayerSkillCom = CreateProtoComponent<CPlayerSkill>(this, COMPONENTTYPE::PL_SKILL);
    NULL_CHECK_RETURN(m_PlayerSkillCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::PL_SKILL, m_PlayerSkillCom });

    // Gathering
    m_PlayerGatherCom = CreateProtoComponent<CPlayerGathering>(this, COMPONENTTYPE::GATHERING);
    NULL_CHECK_RETURN(m_PlayerGatherCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::GATHERING, m_PlayerGatherCom });

    // Construction
    m_PlayerConstructionCom = CreateProtoComponent<CPlayerConstruction>(this, COMPONENTTYPE::CONSTRUCTION);
    NULL_CHECK_RETURN(m_PlayerConstructionCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::CONSTRUCTION, m_PlayerConstructionCom });

    return S_OK;
}

void CPlayer::Animation_Setting()
{
#pragma region --- IDLE ---
    m_AnimatorCom->Create_Animation(L"PlayerIdle", 150, 0.02f, _vec2(0.75f, 0.80f));

    m_StateCom->Set_AnimInfo(ACTORSTATE::IDLE, L"PlayerIdle", ANIMSTATE::LOOP);
#pragma endregion

#pragma region --- RUN ---
    m_AnimatorCom->Create_Animation(L"PlayerRun_LEFT", 19, 0.02f, _vec2(0.68f, 0.8f));
    m_AnimatorCom->Create_Animation(L"PlayerRun_UP", 19, 0.02f, _vec2(0.68f, 0.8f));
    m_AnimatorCom->Create_Animation(L"PlayerRun_DOWN", 19, 0.02f, _vec2(0.65f, 0.8f));
    m_AnimatorCom->Create_Animation(L"PlayerRun_LUP", 19, 0.02f, _vec2(0.68f, 0.8f));
    m_AnimatorCom->Create_Animation(L"PlayerRun_LDOWN", 19, 0.02f, _vec2(0.68f, 0.8f));

    m_StateCom->Set_AnimInfo(ACTORSTATE::RUN, L"PlayerRun", ANIMSTATE::LOOP);
#pragma endregion

#pragma region --- ROLL ---
    m_AnimatorCom->Create_Animation(L"PlayerRoll_LEFT",
                                    19,
                                    0.02f,
                                    _vec2(1.05f, 1.05f),    // Scale
                                    _vec2(0.f, -0.1f));     // Offset

    m_AnimatorCom->Create_Animation(L"PlayerRoll_UP",
                                    16,
                                    0.02f * 19 / 16,
                                    _vec2(0.7f, 0.7f),
                                    _vec2(0.f, 0.f));

    m_AnimatorCom->Create_Animation(L"PlayerRoll_DOWN",
                                    16,
                                    0.02f * 19 / 16,
                                    _vec2(0.6f, 0.7f),
                                    _vec2(0.f, 0.f));

    m_StateCom->Set_AnimInfo(ACTORSTATE::PLAYER_ROLL, L"PlayerRoll", ANIMSTATE::ONCE);
#pragma endregion

#pragma region --- SWORD ---
    m_AnimatorCom->Create_Animation(L"PlayerSwordCombo1",
                                    31,
                                    0.02f,
                                    _vec2(1.75f, 1.75f),
                                    _vec2(0.f, 0.16f));

    m_StateCom->Set_AnimInfo(ACTORSTATE::PLAYER_SWORD_COMBO1,
                             L"PlayerSwordCombo1",
                             ANIMSTATE::ONCE);

    m_AnimatorCom->Create_Animation(L"PlayerSwordCombo2",
                                    38,
                                    0.02f,
                                    _vec2(1.6f, 1.5f),
                                    _vec2(0.f, 0.08f));

    m_StateCom->Set_AnimInfo(ACTORSTATE::PLAYER_SWORD_COMBO2,
                             L"PlayerSwordCombo2",
                             ANIMSTATE::ONCE);

    m_AnimatorCom->Create_Animation(L"PlayerSwordCombo3",
                                    38,
                                    0.02f,
                                    _vec2(1.8f, 1.4f),
                                    _vec2(0.02f, 0.15f));

    m_StateCom->Set_AnimInfo(ACTORSTATE::PLAYER_SWORD_COMBO3,
                             L"PlayerSwordCombo3",
                             ANIMSTATE::ONCE);

    m_AnimatorCom->Create_Animation(L"PlayerSwordHeavy",
                                    70,
                                    0.02f,
                                    _vec2(2.f, 2.f),
                                    _vec2(0.f, 0.f));

    m_StateCom->Set_AnimInfo(ACTORSTATE::PLAYER_SWORD_HEAVY,
                             L"PlayerSwordHeavy",
                             ANIMSTATE::ONCE);
#pragma endregion

#pragma region --- Gauntlet ---
    m_AnimatorCom->Create_Animation(L"PlayerGauntletCombo1",
                                    41,
                                    0.02f,
                                    _vec2(2.2f, 1.5f),
                                    _vec2(0.f, 0.3f));

    m_StateCom->Set_AnimInfo(ACTORSTATE::PLAYER_GAUNTLET_COMBO1,
                             L"PlayerGauntletCombo1",
                             ANIMSTATE::ONCE);

    m_AnimatorCom->Create_Animation(L"PlayerGauntletCombo2",
                                    43,
                                    0.02f,
                                    _vec2(2.2f, 1.5f),
                                    _vec2(0.f, 0.f));

    m_StateCom->Set_AnimInfo(ACTORSTATE::PLAYER_GAUNTLET_COMBO2,
                             L"PlayerGauntletCombo2",
                             ANIMSTATE::ONCE);

    m_AnimatorCom->Create_Animation(L"PlayerGauntletCombo3",
                                    46,
                                    0.02f,
                                    _vec2(2.6f, 2.f),
                                    _vec2(0.f, 0.45f));

    m_StateCom->Set_AnimInfo(ACTORSTATE::PLAYER_GAUNTLET_COMBO3,
                             L"PlayerGauntletCombo3",
                             ANIMSTATE::ONCE);

    m_AnimatorCom->Create_Animation(L"PlayerGauntletHeavy",
                                    70,
                                    0.02f,
                                    _vec2(2.2f, 2.2f),
                                    _vec2(0.f, 0.f));

    m_StateCom->Set_AnimInfo(ACTORSTATE::PLAYER_GAUNTLET_HEAVY,
                             L"PlayerGauntletHeavy",
                             ANIMSTATE::ONCE);

#pragma region --- Skill ---

    m_AnimatorCom->Create_Animation(L"PlayerSkillCharging",
                                    17,
                                    0.02f,
                                    _vec2(0.6f, 0.8f),
                                    _vec2(0.f, 0.f));

    m_StateCom->Set_AnimInfo(ACTORSTATE::PLAYER_SKILL_CHARGING,
                             L"PlayerSkillCharging",
                             ANIMSTATE::LOOP);

    m_AnimatorCom->Create_Animation(L"PlayerSkillAttack",
                                    32,
                                    0.02f,
                                    _vec2(1.f, 1.2f),
                                    _vec2(0.f, 0.4f));

    m_StateCom->Set_AnimInfo(ACTORSTATE::PLAYER_SKILL_SINGLE,
                             L"PlayerSkillAttack",
                             ANIMSTATE::ONCE);

    m_StateCom->Set_AnimInfo(ACTORSTATE::PLAYER_SKILL_TRIPLE,
                             L"PlayerSkillAttack",
                             ANIMSTATE::ONCE);

#pragma endregion

#pragma region -----------Crafting-----------

    m_AnimatorCom->Create_Animation(L"PlayerAction",
                                    25,
                                    0.02f,
                                    _vec2(0.75f, 0.80f),
                                    _vec2(0.f, 0.f));

    m_StateCom->Set_AnimInfo(ACTORSTATE::PLAYER_ACTION,
                             L"PlayerAction",
                             ANIMSTATE::LOOP);

    m_AnimatorCom->Create_Animation(L"PlayerWood",
                                    39,
                                    0.02f,
                                    _vec2(1.6f, 1.5f),
                                    _vec2(0.f, -0.45f));

    m_StateCom->Set_AnimInfo(ACTORSTATE::PLAYER_WOOD,
                             L"PlayerWood",
                             ANIMSTATE::LOOP);

    m_AnimatorCom->Create_Animation(L"PlayerStone",
                                    45,
                                    0.02f,
                                    _vec2(1.3f, 1.2f),
                                    _vec2(0.f, 0.2f));

    m_StateCom->Set_AnimInfo(ACTORSTATE::PLAYER_STONE,
                             L"PlayerStone",
                             ANIMSTATE::LOOP);

    m_AnimatorCom->Create_Animation(L"PlayerBerry",
                                    25,
                                    0.02f,
                                    _vec2(0.65f, 0.70f),
                                    _vec2(0.05f, -0.1f));

    m_StateCom->Set_AnimInfo(ACTORSTATE::PLAYER_BERRY,
                             L"PlayerBerry",
                             ANIMSTATE::LOOP);

    m_AnimatorCom->Create_Animation(L"PlayerHammer",
                                    29,
                                    0.02f,
                                    _vec2(1.f, 1.f),
                                    _vec2(0.f, -0.1f));

    m_StateCom->Set_AnimInfo(ACTORSTATE::PLAYER_HAMMER,
                             L"PlayerHammer",
                             ANIMSTATE::LOOP);

#pragma endregion

#pragma region 기타 상호작용
    m_AnimatorCom->Create_Animation(L"Player_WeaponGet",
                                    77,
                                    0.02f,
                                    _vec2(1.3f, 1.67f),
                                    _vec2(0.01f, 0.1f));

    m_StateCom->Set_AnimInfo(ACTORSTATE::PLAYER_WEAPONGET,
                             L"Player_WeaponGet",
                             ANIMSTATE::ONCE);

    m_AnimatorCom->Create_Animation(L"Player_Hit",
                                    34,
                                    0.01f,
                                    _vec2(1.f, 0.97f),
                                    _vec2(0.f, 0.18f));

    m_StateCom->Set_AnimInfo(ACTORSTATE::HIT,
                             L"Player_Hit",
                             ANIMSTATE::ONCE);

#pragma endregion

#pragma region Final
    m_AnimatorCom->Create_Animation(L"Decide_Start",
                                    41,
                                    0.04f,
                                    _vec2(0.77f, 1.12f),
                                    _vec2(0.f, 0.32f));

    m_AnimatorCom->Create_Animation(L"Decide_Loop",
                                    50,
                                    0.04f,
                                    _vec2(0.76f, 0.73f),
                                    _vec2(0.f, -0.05f));

    m_AnimatorCom->Create_Animation(L"Refuse_Sword",
                                    55,
                                    0.04f,
                                    _vec2(2.24f, 2.32f),
                                    _vec2(0.f, 1.43f));

    m_AnimatorCom->Create_Animation(L"Refuse_Gauntlet",
                                    54,
                                    0.04f,
                                    _vec2(2.24f, 2.32f),
                                    _vec2(0.f, 1.06f));

    m_StateCom->Set_AnimInfo(ACTORSTATE::PLAYER_DECIDE, L"Decide_Start", ANIMSTATE::ONCE);
    //m_StateCom->Set_AnimInfo(ACTORSTATE::PLAYER_REFUSE, L"Refuse_Sword", ANIMSTATE::ONCE);

#pragma endregion
}

CPlayer* CPlayer::Create(DEVICE graphicDev)
{
    auto player = new CPlayer(graphicDev);

    if (FAILED(player->Ready_GameObject()))
    {
        MSG_BOX("Player Create Failed");
        Safe_Release(player);
        return nullptr;
    }

    return player;
}

void CPlayer::Free()
{
    CCharacterBase::Free();
}
