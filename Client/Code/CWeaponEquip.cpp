#include "pch.h"
#include "CWeaponEquip.h"

#include "CCombatStat.h"
#include "CDropSystem.h"
#include "CItemDB.h"
#include "CPlayer.h"
#include "CSoundMgr.h"
#include "CTransform.h"

CWeaponEquip::CWeaponEquip(DEVICE graphicDev)
    : CComponent(graphicDev),
      m_CurrentWeaponID(-1),
      m_CurrentWeaponSlot{},
      m_WeaponEquipped(false)
{
    // 빈 슬롯 시작이 -1이면, -1로 맞추는게 안전함.
    m_CurrentWeaponSlot.itemInst.itemId = -1;
    m_CurrentWeaponSlot.count           = 0;
}

CWeaponEquip::CWeaponEquip(const CWeaponEquip& rhs)
    : CComponent(rhs),
      m_CurrentWeaponID(-1),
      m_CurrentWeaponSlot{},
      m_WeaponEquipped(false)
{
    m_CurrentWeaponSlot.itemInst.itemId = -1;
    m_CurrentWeaponSlot.count           = 0;
}

CWeaponEquip::~CWeaponEquip()
{ }

void CWeaponEquip::Equip_Weapon(ItemInstance itemInst)
{
    if (m_CurrentWeaponSlot.itemInst.itemId != -1)
        Unequip_Weapon();

    m_CurrentWeaponSlot.itemInst = itemInst;
    m_CurrentWeaponID            = itemInst.itemId;

    Item* itemData = CItemDB::GetInstance()->GetItemById(m_CurrentWeaponID);

    if (itemData)
        ApplyWeaponStat(itemData);

    m_WeaponEquipped = true;

    CSoundMgr::GetInstance()->Play_Once(L"Player/Player_ChangeWeapon.wav",
                                    SOUND_EFFECT,
                                    0.5f);

    //cout << "Weapon Equipped" << endl;
}

void CWeaponEquip::Unequip_Weapon(bool weaponDrop)
{
    if (m_CurrentWeaponID == -1)
        return;

    RemoveWeaponStat();

    if (weaponDrop)
    {
        auto transform = dynamic_cast<CTransform*>(Get_Owner()->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));

        _vec3 DropPos = transform->Get_Pos();
        CDropSystem::GetInstance()->SpawnDrop(m_GraphicDev, m_CurrentWeaponSlot.itemInst, { DropPos.x,DropPos.y,DropPos.z + 2.f });
    }

    m_CurrentWeaponID = -1;
    m_WeaponEquipped  = false;
}

void CWeaponEquip::ApplyWeaponStat(Item* item)
{
    auto* player = dynamic_cast<CPlayer*>(Get_Owner());
    if (!player)
        return;

    auto* combatStat = dynamic_cast<CCombatStat*>(player->Get_Component(ID_STATIC, COMPONENTTYPE::COMBATSTAT));
    if (!combatStat)
        return;

    combatStat->Add_Attack(item->stats["attack1"]);

    //cout << combatStat->Get_Attack() << endl;

    //dynamic_cast<CPlayer*>(Get_Owner())->Get_CombatStat()->Add_Attack(item->stats["attack1"]);
    //cout << dynamic_cast<CPlayer*>(Get_Owner())->Get_CombatStat()->Get_Attack() << endl;
}

void CWeaponEquip::RemoveWeaponStat()
{
    auto* player = dynamic_cast<CPlayer*>(Get_Owner());
    if (!player)
        return;

    auto* combatStat = dynamic_cast<CCombatStat*>(player->Get_Component(ID_STATIC, COMPONENTTYPE::COMBATSTAT));

    if (!combatStat )
        return;

    Item* itemData = CItemDB::GetInstance()->GetItemById(m_CurrentWeaponID);

    combatStat->Add_Attack(-(itemData->stats["attack1"]));
}

CWeaponEquip* CWeaponEquip::Create(DEVICE graphicDev)
{
    auto weaponEquip = new CWeaponEquip(graphicDev);

    if (weaponEquip == nullptr)
    {
        MSG_BOX("weaponEquip Create Failed");
        Safe_Release(weaponEquip);
        return nullptr;
    }
    return weaponEquip;
}

void CWeaponEquip::Free()
{
    CComponent::Free();
}

Engine::CComponent* CWeaponEquip::Clone()
{
    return new CWeaponEquip(*this);
}
