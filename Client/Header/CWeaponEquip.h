#pragma once
#include "CComponent.h"
#include "CInventory.h"
#include "ItemInstance.h"

struct Item;

class CWeaponEquip : public Engine::CComponent
{
private:
    explicit CWeaponEquip(DEVICE graphicDev);
    explicit CWeaponEquip(const CWeaponEquip& rhs);
    ~CWeaponEquip() override;

public:
    void Equip_Weapon(ItemInstance itemInst);
    void Unequip_Weapon(bool weaponDrop = true);
    void ApplyWeaponStat(Item* item);
    void RemoveWeaponStat();
    bool Get_WeaponEquipped() { return m_WeaponEquipped; }
    int  Get_CurrentWeaponID() { return m_CurrentWeaponID; }

    static CWeaponEquip* Create(DEVICE graphicDev);
    CComponent*          Clone() override;

protected:
    void Free() override;

public:
    int           m_CurrentWeaponID;
    InventorySlot m_CurrentWeaponSlot;
    bool          m_WeaponEquipped;
};
