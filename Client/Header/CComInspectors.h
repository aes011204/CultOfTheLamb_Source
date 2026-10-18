#pragma once
#include "CAnimator.h"
#include "CBoxCollider.h"
#include "CCameraCalc.h"
#include "CCameraMovement.h"
#include "CCombatStat.h"
#include "CDInputMgr.h"
#include "CEnumHelper.h"
#include "CInventory.h"
#include "CObjectSpawner.h"
#include "CPlayerAttack.h"
#include "CPlayerController.h"
#include "CPlayerInput.h"
#include "CPlayerMovement.h"
#include "CRigidbody.h"
#include "CSpriteAnimation.h"
#include "CState.h"
#include "CTerrainRenderer.h"
#include "CTransform.h"
#include "CWallRenderer.h"
#include "CWeaponEquip.h"
#include "CManagement.h"
#include "CScene.h"
#include "CLayer.h"

USING(Engine)

static void Change_Target(CCameraMovement* movement);

static void Transform_Inspector(CTransform* transform)
{
    if (!transform)
        return;

    ImGui::Text("[ Transform ]");
    ImGui::PushID("TransformPos");
    {
        _vec3 pos = transform->Get_Pos();

        if (ImGui::DragFloat3("##PositionDrag", &pos.x, 0.1f))
            transform->Set_Pos(pos);
    }
    ImGui::PopID();

    ImGui::NewLine();

    ImGui::Text("[ Rotation ]");

    static _vec3 tempRot;
    static bool  isInit = false;

    if (!isInit)
    {
        tempRot = transform->Get_Rotation();
        isInit  = true;
    }

    ImGui::PushID("TransformRot");
    {
        // X축
        ImGui::Text("Rot X : ");
        ImGui::SameLine();
        float x = tempRot.x;
        if (ImGui::DragFloat("##Rot X", &x, 0.005f))
        {
            tempRot.x = x;
            transform->Set_RotationAxis(ROT_X, tempRot.x);
        }

        // Y축
        ImGui::Text("Rot Y : ");
        ImGui::SameLine();
        float y = tempRot.y;
        if (ImGui::DragFloat("##Rot Y", &y, 0.005f))
        {
            tempRot.y = y;
            transform->Set_RotationAxis(ROT_Y, tempRot.y);
        }

        // Z축
        ImGui::Text("Rot Z : ");
        ImGui::SameLine();
        float z = tempRot.z;
        if (ImGui::DragFloat("##Rot Z", &z, 0.005f))
        {
            tempRot.z = z;
            transform->Set_RotationAxis(ROT_Z, tempRot.z);
        }
    }
    ImGui::PopID();

    ImGui::NewLine();

    ImGui::Text("[ Scale ]");
    ImGui::PushID("TransformScale");
    {
        _vec3 scale = transform->Get_Scale();

        if (ImGui::DragFloat3("##PositionDrag", &scale.x, 0.1f))
            transform->Set_Scale(scale);
    }
    ImGui::PopID();
}

static void State_Inspector(CState* state)
{
    if (state == nullptr)
        return;
    
    static bool showStateWindow = false;

    ImGui::PushID("State_Inspector");
    {
        ImGui::Checkbox("State Debug Window", &showStateWindow);
        int curStateIdx = static_cast<int>(state->Get_State());
        static const char* stateNames[] =
        {
            "IDLE", "RUN", "ATTACK",
            "PLAYER_ROLL",
            "PLAYER_SWORD_COMBO1", "PLAYER_SWORD_COMBO2", "PLAYER_SWORD_COMBO3",
            "PLAYER_SWORD_HEAVY",
            "PLAYER_GAUNTLET_COMBO1", "PLAYER_GAUNTLET_COMBO2", "PLAYER_GAUNTLET_COMBO3",
            "PLAYER_GAUNTLET_HEAVY",
            "PLAYER_SKILL_CHARGING", "PLAYER_SKILL_SINGLE", "PLAYER_SKILL_TRIPLE",
            "HIT",
            "PLAYER_ACTION", "PLAYER_WOOD", "PLAYER_STONE", "PLAYER_BERRY",
            "PLAYER_WEAPONGET", "PLAYER_HAMMER",
            "PLAYER_DECIDE", "PLAYER_REFUSE",
            "DEAD",
            "STATE_END"
        };

        // 별도 창으로 띄울지 여부
        if (showStateWindow)
        {
            ImGui::Begin("State Debug");
            {
                ImGui::Text("State : ");
                ImGui::SameLine();

                if (ImGui::Combo("##StateCombo", &curStateIdx, stateNames, IM_ARRAYSIZE(stateNames)))
                {
                    state->Change_State(static_cast<ACTORSTATE>(curStateIdx));
                }
            }
            ImGui::End();
        }
        else
        {
            const char* curState = ToString(state->Get_State());
            ImGui::Text("State : ");
            ImGui::SameLine();
            ImGui::TextUnformatted(curState);
        }

        // Dir 
        const char* curDir = ToString(state->Get_Dir());
        ImGui::Text("Dir   : ");
        ImGui::SameLine();
        ImGui::TextUnformatted(curDir);
    }

    ImGui::PopID();
}

static void CombatStat_Inspector(CCombatStat* stat)
{
    if (stat == nullptr)
        return;

    _float maxHp = stat->Get_MaxHp();
    _float hp    = stat->Get_Hp();

    _float maxMp = stat->Get_MaxMp();
    _float mp    = stat->Get_Mp();

    _float speed  = stat->Get_Speed();
    _float attack = stat->Get_Attack();

    ImGui::PushID("CombatStat_Inspector");
    {
        const float column_width = 80.0f;

        ImGui::Columns(2, "HpColumns", false);
        ImGui::SetColumnWidth(0, column_width);

        // --- Hp ---
        ImGui::Text("Max Hp : ");
        ImGui::NextColumn();
        if (ImGui::DragFloat("##MaxHp", &maxHp, 0.1f))
            stat->Set_MaxHp(maxHp);
        ImGui::NextColumn();

        ImGui::Text("Hp : ");
        ImGui::NextColumn();
        if (ImGui::DragFloat("##CurHp", &hp, 0.1f))
            stat->Set_Hp(hp);
        ImGui::NextColumn();

        ImGui::Columns(1);

        ImGui::NewLine();

        // --- Mp ---
        ImGui::Columns(2, "MpColumns", false);
        ImGui::SetColumnWidth(0, column_width);

        ImGui::Text("Max Mp : ");
        ImGui::NextColumn();
        if (ImGui::DragFloat("##MaxMp", &maxMp, 0.1f))
            stat->Set_MaxMp(maxMp);
        ImGui::NextColumn();

        ImGui::Text("Mp : ");
        ImGui::NextColumn();
        if (ImGui::DragFloat("##CurMp", &mp, 0.1f))
            stat->Set_Mp(mp);
        ImGui::NextColumn();

        ImGui::Columns(1);

        // --- Speed ---
        ImGui::Columns(2, "EtcColums", false);
        ImGui::SetColumnWidth(0, column_width);

        ImGui::Text("Speed : ");
        ImGui::NextColumn();
        if (ImGui::DragFloat("##Speed", &speed, 0.1f))
            stat->Set_Speed(speed);
        ImGui::NextColumn();

        // --- Attack ---
        ImGui::Text("Attack : ");
        ImGui::NextColumn();
        if (ImGui::DragFloat("##Attack", &attack, 0.1f))
            stat->Set_Attack(attack);
        ImGui::NextColumn();

        ImGui::Columns(1);
    }
    ImGui::PopID();
}

static void Terrain_Inspector(CTerrainRenderer* renderer)
{
    if (renderer == nullptr)
        return;

    if (ImGui::BeginTabBar("Terrain_TabBar"))
    {
        if (ImGui::BeginTabItem(u8"타일"))
        {
            renderer->Render_Editor();

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem(u8"타일 사이즈"))
        {
            ImGui::PushID("Terrain_Resize");
            {
                ImGui::SeparatorText(u8"Terrain 개수 설정");
                ImGui::NewLine();

                static _int   resizeX  = TILE_CNT_X;
                static _int   resizeZ  = TILE_CNT_Z;
                static _float cellSize = 1.f;

                const _int textWidth = 65.f;
                const _int boxwidth  = 65.f;

                ImGui::Columns(2, nullptr, false);
                ImGui::SetColumnWidth(0, textWidth);

                ImGui::Text("Size X :");
                ImGui::NextColumn();
                ImGui::SetNextItemWidth(boxwidth);
                ImGui::DragInt("##Terrain_SizeX", &resizeX, 1, 10, 129);
                ImGui::NextColumn();

                ImGui::Text("Size Z :");
                ImGui::NextColumn();
                ImGui::SetNextItemWidth(boxwidth);
                ImGui::DragInt("##Terrain_SizeZ", &resizeZ, 1, 10, 129);
                ImGui::NextColumn();

                ImGui::Text(u8"셀 크기 :");
                ImGui::NextColumn();
                ImGui::SetNextItemWidth(boxwidth);
                ImGui::DragFloat("##Terrain_CellSize", &cellSize, 0.1f, 0.1f, 10.f);

                ImGui::Columns(1);

                ImGui::NewLine();
                if (ImGui::Button(u8"Terrain 재생성"))
                    renderer->Resize_Terrain(resizeX, resizeZ, cellSize);

                ImGui::NewLine();

                ImGui::SeparatorText(u8"타일 정보");

                ImGui::Text(u8"현재 %d x %d (셀크기: %.1f)",
                            renderer->Get_SizeX(),
                            renderer->Get_SizeZ(),
                            renderer->Get_CellSize());
            }
            ImGui::PopID();

            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }
}

static void BoxColl_Inspector(CBoxCollider* collider)
{
    if (collider == nullptr)
        return;

    ImGui::PushID("BoxColl_Inspector");
    {
        _vec3 scale = collider->Get_RawSize();
        _vec3 offset = collider->Get_Offset();

        ImGui::Text("[ Size ]");

        if (ImGui::DragFloat3("##SizeDrag", &scale.x, 0.01f))
            collider->Set_RawSize(scale);

        //ImGui::NewLine();

        if (ImGui::DragFloat3("##OFfsetDrag", &offset.x, 0.01f))
            collider->Set_Offset(offset);

        ImGui::NewLine();

        // Collider Render 그리기
        bool renderCheck = collider->Get_ShowDebug();
        if (ImGui::Checkbox("Render Debug Box", &renderCheck))
            collider->Set_ShowDebug(renderCheck);

        // Collider Active 충돌체크
        bool collisionCheck = collider->IsCollisionActive();
        if (ImGui::Checkbox("Collision Active", &collisionCheck))
            collider->Set_CollisionActive(collisionCheck);
    }
    ImGui::PopID();
}

static void Inventory_Inspector(CInventory* inven)
{
    if (inven == nullptr)
        return;

    const vector<InventorySlot>& slots     = inven->GetVector();
    int                          slotCount = static_cast<_int>(slots.size());

    ImGui::PushID("Inventory_Inspector");
    {
        ImGui::Text("[ Inventory ]");
        ImGui::Text("Slot Count : %d", slotCount);
        ImGui::Spacing();

        if (slotCount == 0)
        {
            ImGui::Text("Empty Inventory");
            return;
        }

        ImGui::Columns(3, "ItemColumns");
        ImGui::Text("Index");
        ImGui::NextColumn();
        ImGui::Text("Item");
        ImGui::NextColumn();
        ImGui::Text("Count");
        ImGui::NextColumn();
        ImGui::Separator();

        for (_int i = 0; i < slotCount; i++)
        {
            const InventorySlot& slot = slots[i];

            // Index
            ImGui::Text("%d", i);
            ImGui::NextColumn();

            // Name
            Item* itemData = nullptr;
            if (slot.itemInst.itemId >= 0)
                itemData = CItemDB::GetInstance()->GetItemById(slot.itemInst.itemId);

            if (itemData)
            {
                ImGui::Text("%s", itemData->name.c_str());

                // 설명 툴팁도 나오도록
                if (ImGui::IsItemHovered())
                {
                    ImGui::SetNextWindowSize(ImVec2(300.f, 0.f));

                    ImGui::BeginTooltip();

                    ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + 280.f);

                    ImGui::TextWrapped("%s", itemData->desc.c_str());
                    if (!itemData->additionalDesc.empty())
                    {
                        ImGui::Separator();
                        ImGui::TextWrapped("%s", itemData->additionalDesc.c_str());
                    }
                    ImGui::EndTooltip();
                }
            }
            else
                ImGui::Text("-");
            ImGui::NextColumn();

            // 4) Count
            if (slot.itemInst.itemId >= 0)
                ImGui::Text("%d", slot.count);
            else
                ImGui::Text("-");
            ImGui::NextColumn();
        }

        ImGui::Columns(1);
    }
    ImGui::PopID();
}

static void Weapon_Inspector(CWeaponEquip* weapon)
{
    if (weapon == nullptr)
        return;

    ImGui::PushID("WeaponEquip_Inspector");
    {
        ImGui::Text("[ Weapon Equip ]");
        ImGui::Separator();

        const InventorySlot& slot     = weapon->m_CurrentWeaponSlot;
        int                  weaponId = weapon->m_CurrentWeaponID;

        if (weaponId < 0 || slot.itemInst.itemId < 0)
        {
            ImGui::Text("Equipped : None");
            ImGui::PopID();
            return;
        }

        Item* itemData = CItemDB::GetInstance()->GetItemById(weaponId);

        if (itemData == nullptr)
        {
            ImGui::Text("Equipped : Invalid Item (ID = %d)", weaponId);
            ImGui::PopID();
            return;
        }

        ImGui::Text("ID    : %d", itemData->id);
        ImGui::Text("Name  : %s", itemData->name.c_str());
        ImGui::Text("Type  : %s", "Weapon");
        ImGui::Text("Count : %d", slot.count);
        ImGui::Spacing();

        ImGui::TextWrapped("%s", itemData->desc.c_str());
        if (!itemData->additionalDesc.empty())
        {
            ImGui::Separator();
            ImGui::TextWrapped("%s", itemData->additionalDesc.c_str());
        }

        ImGui::Spacing();

        // 스탯 출력
        if (!itemData->stats.empty())
        {
            ImGui::Text("Stats");
            ImGui::Separator();

            for (const auto& kv : itemData->stats)
            {
                const std::string& key   = kv.first;
                float              value = kv.second;

                ImGui::Text("%s : %.1f", key.c_str(), value);
            }
        }

        ImGui::Spacing();

        // 장착 해제
        if (ImGui::Button("Unequip Weapon"))
            weapon->Unequip_Weapon();
    }
    ImGui::PopID();
}

static void RigidBody_Inspector(CRigidbody* rigid)
{
    NULL_CHECK(rigid);

    ImGui::PushID("Rigidbody_Inspector");
    {
        ImGui::Text("[ Rigidbody ]");
        ImGui::Separator();

        const float column_width = 120.0f;

        // Use Flags
        ImGui::Text("Physics Options:");

        bool useAccel = rigid->Get_UseAccel();
        if (ImGui::Checkbox("Use Acceleration", &useAccel))
            rigid->Set_UseAccel(useAccel);

        bool useFriction = rigid->Get_UseFriction();
        if (ImGui::Checkbox("Use Friction", &useFriction))
            rigid->Set_UseFriction(useFriction);

        bool useGravity = rigid->Get_UseGravity();
        if (ImGui::Checkbox("Use Gravity", &useGravity))
            rigid->Set_UseGravity(useGravity);

        bool isGround = rigid->Get_IsGround();
        if (ImGui::Checkbox("Is On Ground", &isGround))
            rigid->Set_IsGround(isGround);

        ImGui::Separator();

        // Physics Properties
        ImGui::Columns(2, "RigidbodyColumns", false);
        ImGui::SetColumnWidth(0, column_width);

        // Mass
        _float mass = rigid->Get_Mass();
        ImGui::Text("Mass");
        ImGui::NextColumn();
        if (ImGui::DragFloat("##Mass", &mass, 0.1f, 0.1f, 100.0f))
            rigid->Set_Mass(mass);
        ImGui::NextColumn();

        // Friction
        _float friction = rigid->Get_Friction();
        ImGui::Text("Friction");
        ImGui::NextColumn();
        if (ImGui::DragFloat("##Friction", &friction, 0.1f, 0.0f, 10.0f))
            rigid->Set_Friction(friction);
        ImGui::NextColumn();

        ImGui::Columns(1);
        ImGui::Separator();

        // Velocity (Read-only)
        ImGui::Text("Velocity : ");
        _vec3 velocity = rigid->Get_Velocity();
        ImGui::Text("  X: %.2f, Y: %.2f, Z: %.2f", velocity.x, velocity.y, velocity.z);

        // Velocity Limit
        ImGui::Text("Velocity Limit:");
        _vec3 velLimit = rigid->Get_VelocityLimit();
        if (ImGui::DragFloat3("##VelLimit", &velLimit.x, 0.1f, 0.0f, 100.0f))
            rigid->Set_VelocityLimit(velLimit);

        // Gravity
        ImGui::Text("Gravity:");
        _vec3 gravity = rigid->Get_Gravity();
        if (ImGui::DragFloat3("##Gravity", &gravity.x, 0.1f, -100.0f, 100.0f))
            rigid->Set_Gravity(gravity);
    }
    ImGui::PopID();
}

static void PlayerInput_Inspector(CPlayerInput* input)
{
    if (!input) return;

    ImGui::PushID("PlayerInput_Inspector");
    if (ImGui::CollapsingHeader("Player Input"))
    {
        _vec3 moveDir  = input->Get_MoveDir();
        _vec3 mouseDir = input->Get_MouseDir();

        ImGui::Text("Move Dir  : (%.2f, %.2f, %.2f)", moveDir.x, moveDir.y, moveDir.z);
        ImGui::Text("Mouse Dir : (%.2f, %.2f, %.2f)", mouseDir.x, mouseDir.y, mouseDir.z);
    }
    ImGui::PopID();
}

static void PlayerMovement_Inspector(CPlayerMovement* movement)
{
    if (!movement) return;

    ImGui::PushID("PlayerMovement_Inspector");
    if (ImGui::CollapsingHeader("Player Movement"))
    {
        _float rollTimer       = movement->Get_RollTimer();
        _float rollDuration    = movement->Get_RollDuration();
        _float invincibleTime  = movement->Get_RollInvincibleTime();
        _bool  invincibleEnded = movement->Get_IsInvincibleEnded();
        _vec3  rollDir         = movement->Get_RollDir();

        ImGui::Text("Roll Dir : (%.2f, %.2f, %.2f)", rollDir.x, rollDir.y, rollDir.z);

        if (ImGui::DragFloat("Roll Timer", &rollTimer, 0.01f, 0.f, 5.f)) movement->Set_RollTimer(rollTimer);
        if (ImGui::DragFloat("Roll Duration", &rollDuration, 0.01f, 0.f, 5.f)) movement->Set_RollDuration(rollDuration);
        if (ImGui::DragFloat("Invincible Time", &invincibleTime, 0.01f, 0.f, 5.f)) movement->Set_RollInvincibleTime(invincibleTime);
        if (ImGui::Checkbox("Invincible Ended", &invincibleEnded)) movement->Set_IsInvincibleEnded(invincibleEnded);
    }
    ImGui::PopID();
}

static void PlayerAttack_Inspector(CPlayerAttack* attack)
{
    if (!attack) return;

    ImGui::PushID("PlayerAttack_Inspector");
    if (ImGui::CollapsingHeader("Player Attack"))
    {
        _int   comboIndex     = attack->Get_ComboIndex();
        _float timer          = attack->Get_CurrentTimer();
        _bool  isHitboxActive = attack->Get_IsHitboxActive();
        _bool  canNextCombo   = attack->Get_CanNextCombo();
        _bool  isNextReserved = attack->Get_IsNextComboReserved();

        if (ImGui::InputInt("Combo Index", &comboIndex)) attack->Set_ComboIndex(comboIndex);
        if (ImGui::DragFloat("Timer", &timer, 0.01f, 0.f, 5.f)) attack->Set_CurrentTimer(timer);
        if (ImGui::Checkbox("Hitbox Active", &isHitboxActive)) attack->Set_IsHitboxActive(isHitboxActive);
        if (ImGui::Checkbox("Can Next Combo", &canNextCombo)) attack->Set_CanNextCombo(canNextCombo);
        if (ImGui::Checkbox("Next Combo Reserved", &isNextReserved)) attack->Set_IsNextComboReserved(isNextReserved);

        ImGui::Separator();
        ImGui::Text("Attack Info Data Setting");
        ImGui::Text("Choose Weapon and Attack. Then you can change values.");

        static int selectedWeapon = 0;
        static int selectedAttack = 0;

        const char* weaponNames[] = { "Sword", "Gauntlet" };
        const char* attackNames[] = { "Combo 1", "Combo 2", "Combo 3", "Heavy" };

        ImGui::Combo("Weapon", &selectedWeapon, weaponNames, IM_ARRAYSIZE(weaponNames));
        ImGui::Combo("Attack Type", &selectedAttack, attackNames, IM_ARRAYSIZE(attackNames));

        CPlayerAttack::AttackInfo& info = attack->Get_AttackInfo_Ref(
            static_cast<CPlayerAttack::PL_WEAPON_INDEX>(selectedWeapon),
            static_cast<CPlayerAttack::PL_ATTACK_INDEX>(selectedAttack)
        );

        ImGui::DragFloat("Duration", &info.duration, 0.01f, 0.f, 5.f);
        ImGui::DragFloat("Hitbox Start", &info.hitboxStartTime, 0.01f, 0.0f, info.duration);
        ImGui::DragFloat("Hitbox End", &info.hitboxEndTime, 0.01f, 0.0f, info.duration);
        ImGui::DragFloat("Combo Input", &info.comboInputTime, 0.01f, 0.0f, info.duration);
        ImGui::DragFloat("Damage Mult", &info.damageMultiplier, 0.1f, 0.0f, 100.0f);
        ImGui::DragFloat("Knockback", &info.knockbackPower, 0.1f, 0.0f, 100.0f);
        ImGui::DragFloat("Spawn Offset", &info.spawnOffset, 0.01f);
        ImGui::DragFloat3("Hitbox Scale", &info.hitboxScale.x, 0.01f, 0.1f, 10.0f);
    }
    ImGui::PopID();
}

static void PlayerController_Inspector(CPlayerController* controller)
{
    NULL_CHECK(controller);

    ImGui::PushID("PlayerController_Inspector");
    {
        ImGui::Text("[ Player Controller ]");
        ImGui::Separator();

        PlayerInput_Inspector(controller->Get_InputCom());
        PlayerMovement_Inspector(controller->Get_MovementCom());
        PlayerAttack_Inspector(controller->Get_AttackCom());
    }
    ImGui::PopID();
}

static void WallRenderer_Inspector(CWallRenderer* wall)
{
    NULL_CHECK(wall);

    wall->Render_Editor();
}

static void Animator_Inspector(CAnimator* animator)
{
    // TODO : 현재 재생되는 애니메이션, Offset, Scale 조절
    // 현재 프레임이 몇인지, 재생중인 애니메이션이 끝났는지정도 추가해주기

    NULL_CHECK(animator);

    const wstring& curAnimKey = animator->Get_CurKey();
    string         strCurAnimKey(curAnimKey.begin(), curAnimKey.end());

    const _int curFrame = animator->Get_CurFrame();
    const _int maxFrame = animator->Get_MaxFrame();

    CSpriteAnimation* curAnimation = animator->Get_CurAnimation();
    _bool             isFinished   = false;

    if (curAnimation)
        isFinished = curAnimation->IsFinished();

    _vec2 size   = animator->Get_CurSize();
    _vec2 offset = animator->Get_CurOffset();

    ImGui::PushID("Animation Inspector");
    {
        ImGui::Text(u8"[ 애니메이션 설정 ]");

        ImGui::Text(u8"현재 애니메이션 : ");
        ImGui::SameLine();
        ImGui::Text("%s", strCurAnimKey.c_str());

        ImGui::Text(u8"현재 프레임 : ");
        ImGui::SameLine();
        ImGui::Text("%d", curFrame);

        ImGui::Text(u8"최대 프레임 : ");
        ImGui::SameLine();
        ImGui::Text("%d", maxFrame);

        ImGui::SeparatorText(u8"위치, 크기 조절");

        const float columnsWidth = 80.f;

        ImGui::Columns(2, "##Size");
        ImGui::SetColumnWidth(0, columnsWidth);

        ImGui::Text("Size : ");
        ImGui::SameLine();

        ImGui::NextColumn();

        if (ImGui::DragFloat2("##AnimSize", &size.x, 0.01f, -15.f, 15.f))
            animator->Set_CurSize(size);

        ImGui::Columns(1);

        ImGui::Columns(2, "##Offset");
        ImGui::SetColumnWidth(0, columnsWidth);

        ImGui::Text("Offset : ");
        ImGui::SameLine();

        ImGui::NextColumn();

        if (ImGui::DragFloat2("##AnimOffset", &offset.x, 0.01f, -15.f, 15.f))
            animator->Set_CurOffset(offset);

        ImGui::Columns(1);
    }
    ImGui::PopID();
}

static void CamCalc_Inspector(CCameraCalc* camCalc)
{
    if (!camCalc) return;

    ImGui::PushID("CamCalc_Inspector");
    if (ImGui::CollapsingHeader("Camera Calc"))
    {
        // Projection Type
        const char* projItems[] = { "Perspective", "Orthographic" };
        int         projType    = static_cast<int>(camCalc->Get_ProjType());
        if (ImGui::Combo("Projection", &projType, projItems, IM_ARRAYSIZE(projItems)))
            camCalc->Set_ProjType(static_cast<PROJECTION_TYPE>(projType));

        // FOV
        _float fov = camCalc->Get_FOV();
        if (ImGui::DragFloat("FOV", &fov, 0.01f, 0.1f, 3.14f))
            camCalc->Set_FOV(fov);

        // Aspect
        _float aspect = camCalc->Get_Aspect();
        if (ImGui::DragFloat("Aspect", &aspect, 0.01f, 0.1f, 5.0f))
            camCalc->Set_Aspect(aspect);

        // Near / Far
        _float nearVal = camCalc->Get_Near();
        _float farVal  = camCalc->Get_Far();
        if (ImGui::DragFloat("Near", &nearVal, 0.1f, 0.1f, 1000.0f))
            camCalc->Set_Near(nearVal);
        if (ImGui::DragFloat("Far", &farVal, 1.0f, 1.0f, 10000.0f))
            camCalc->Set_Far(farVal);
    }
    ImGui::PopID();
}

static void CamMovement_Inspector(CCameraMovement* movement)
{
    if (!movement) return;

    ImGui::PushID("CamMovement_Inspector");
    if (ImGui::CollapsingHeader("Camera Movement"))
    {
        if (ImGui::Button("Reset Camera"))
            movement->Reset_Camera(CAM_PRESET(), false);

        ImGui::Separator();

        // Pivot Offset
        _vec3 pivotOffset = movement->Get_PivotOffset();
        if (ImGui::DragFloat3(u8"시점 위치", &pivotOffset.x, 0.1f))
            movement->Set_PivotOffset(pivotOffset);

        // Target Distance
        _float dist = movement->Get_TargetDistance();
        if (ImGui::DragFloat(u8"타겟과의 거리", &dist, 0.1f, 1.0f, 100.0f))
            movement->Set_TargetDistance(dist);

        // Rotation (Pitch/Yaw) - Convert to Degrees
        _float pitchRad = movement->Get_RotX();
        _float yawRad   = movement->Get_RotY();
        _float pitchDeg = D3DXToDegree(pitchRad);
        _float yawDeg   = D3DXToDegree(yawRad);

        if (ImGui::DragFloat(u8"x축 회전", &pitchDeg, 1.0f, -89.0f, 89.0f))
        {
            pitchRad = D3DXToRadian(pitchDeg);
            movement->Set_RotX(pitchRad);
        }
        if (ImGui::DragFloat(u8"Y축 회전", &yawDeg, 1.0f))
        {
            yawRad = D3DXToRadian(yawDeg);
            movement->Set_RotY(yawRad);
        }

        // Speeds
        _float lagSpeed = movement->Get_LagSpeed();
        if (ImGui::DragFloat(u8"보간 속도", &lagSpeed, 0.1f, 0.0f, 20.0f))
            movement->Set_LagSpeed(lagSpeed);

        _float zoomSpeed = movement->Get_ZoomSpeed();
        if (ImGui::DragFloat(u8"줌 속도", &zoomSpeed, 0.1f, 0.0f, 20.0f))
            movement->Set_ZoomSpeed(zoomSpeed);

        // 타겟 변경
        Change_Target(movement);

        ImGui::Separator();

        // Shake Test
        static _float shakeAmp   = 0.5f;
        static _float shakeDecay = 1.0f;
        static _float shakeFreqX = 50.0f;
        static _float shakeFreqY = 25.0f;

        ImGui::Text(u8"카메라 흔들림 테스트");
        ImGui::DragFloat(u8"진폭", &shakeAmp, 0.01f, 0.0f, 5.0f);
        ImGui::DragFloat(u8"감쇠", &shakeDecay, 0.05f, 0.0f, 10.0f);
        ImGui::DragFloat(u8"X 진동수", &shakeFreqX, 0.1f, 0.0f, 100.0f);
        ImGui::DragFloat(u8"Y 진동수", &shakeFreqY, 0.1f, 0.0f, 100.0f);

        if (ImGui::Button("Trigger Shake"))
            movement->Start_CamShake(shakeAmp, shakeDecay, shakeFreqX, shakeFreqY);
    }
    ImGui::PopID();
}

static void CamController_Inspector(CCameraController* camController)
{
    NULL_CHECK(camController);

    ImGui::PushID("CamController_Inspector");
    {
        ImGui::Text("[ Camera Controller ]");

        ImGui::Separator();

        // Sub Inspectors
        CamCalc_Inspector(camController->Get_OwnerCamCalc());
        CamMovement_Inspector(camController->Get_OwnerCamMovement());

        ImGui::Separator();

        ImGui::NewLine();
        ImGui::Text(u8"[ 카메라 Free 모드 조작 방법 ]");
        ImGui::NewLine();

        ImGui::Text(u8"[ 이동 ]");
        ImGui::TextWrapped(u8"[↑] 앞 | [↓] 뒤 | [←] 좌 | [→] 우");
        ImGui::NewLine();

        ImGui::Text(u8"[ 상/하 이동 ]");
        ImGui::TextWrapped(u8"[PageUp] 상 | [PageDown] 하");
        ImGui::NewLine();

        ImGui::Text(u8"[ 회전 ]");
        ImGui::TextWrapped(u8"[RShift + ↑] 상 | [RShift + ↓] 하");
        ImGui::TextWrapped(u8"[RShift + ←] 좌 | [RShift + →] 우");
    }
    ImGui::PopID();
}

static void Menu_CameraInspector(CCameraController* camController)
{
    NULL_CHECK(camController);

    ImGui::PushID("Main_CameraInspector");
    {
        // Cam Mode
        const char* camModeItems[] = { "Free", "Target" };
        int         camMode        = camController->Get_CamMode();

        ImGui::SetNextItemWidth(100.f);
        if (ImGui::Combo("Camera Mode", &camMode, camModeItems, IM_ARRAYSIZE(camModeItems)))
            camController->Set_CamMode(static_cast<CAM_MODE>(camMode));

        if (CDInputMgr::GetInstance()->KeyDown(DIK_F8))
        {
            camMode = (camController->Get_CamMode() == CAM_MODE::CAM_TARGET) ? CAM_MODE::CAM_FREE : CAM_MODE::CAM_TARGET;
            camController->Set_CamMode(static_cast<CAM_MODE>(camMode));
        }
    }
    ImGui::PopID();
}

static void MonsterSpawner_Inspector(CObjectSpawner* spawner)
{
    NULL_CHECK(spawner);

    spawner->Render_Editor();
}

static void Change_Target(CCameraMovement* movement)
{
    // =====================================
    // 카메라 타겟 변경
    // =====================================
    ImGui::Text(u8"카메라 타겟 변경");
    
    static vector<string> targetNames;
    static vector<CTransform*> targetTransforms;
    static int selectedTargetIdx = 0;

    
    if (ImGui::Button(u8"타겟 목록 새로고침"))
    {
        targetNames.clear();
        targetTransforms.clear();
        selectedTargetIdx = 0;

        CScene* scene = CManagement::GetInstance()->Get_Scene();

        if (scene)
        {
            CLayer* layer = scene->Get_Layer(LAYERTYPE::GAMELOGIC);
            if (layer)
            {
                const auto& objMap = layer->Get_Objects();

                for (const auto& objPair : objMap)
                {
                    OBJTYPE objType = objPair.first;
                    const vector<CGameObject*>& objList = objPair.second;

                    for (CGameObject* obj : objList)
                    {
                        if (!obj) continue;

                        CTransform* transform = obj->Get_Transform();
                        if (!transform) continue;

                        wstring wName = obj->Get_Name();
                        string name = ToString(objType);
                        name += " - ";
                        name += string(wName.begin(), wName.end());

                        targetNames.push_back(name);
                        targetTransforms.push_back(transform);
                    }
                }
            }
        }
    }
    // Combo로 선택
    if (!targetNames.empty())
    {
        vector<const char*> items;
        for (auto& name : targetNames)
            items.push_back(name.c_str());

        if (ImGui::Combo(u8"타겟 선택", &selectedTargetIdx, items.data(), (int)items.size()))
        {
            if (selectedTargetIdx < (int)targetTransforms.size())
            {
                // CamMovement의 owner인 카메라에서 CCameraController 가져오기
                CGameObject* camOwner = movement->Get_Owner();
                if (camOwner)
                {
                    auto camController = dynamic_cast<CCameraController*>(
                        camOwner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::CAM_CONTROLLER));
                    if (camController)
                    {
                        camController->Set_CamTarget(targetTransforms[selectedTargetIdx]);
                    }
                }
            }
        }
    }
    else
    {
        ImGui::TextDisabled(u8"목록 새로고침을 눌러주세요");
    }
    ImGui::Separator();
}
