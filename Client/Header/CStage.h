#pragma once
#include "CScene.h"

BEGIN(Engine)
class CCombatStat;
END

class CTerrain;
class CPlayerSkill;
class CWorkBench;
class CWeaponEquip;
class CInventory;
class CUI_FollowerCommand;
class CUICookingMenu;
class CPlayerGathering;
class CPlayerConstruction;
class CFollowerCow;
class CKitchen;
class CUI_Text;

class CStage : public CScene
{
private:
    explicit CStage(DEVICE graphicDev);
    ~CStage() override;

public:
    HRESULT Ready_Scene() override;
    void    Initialize_Scene() override;
    _int    Update_Scene(const _float& timeDelta) override;
    void    LateUpdate_Scene(const _float& timeDelta) override;
    void    Render_Scene() override;

public:
    CUICookingMenu*      Get_CookingMenu() { return m_CookingMenu; }
    CUI_FollowerCommand* Get_FollowerCommand() { return m_FollowerCommand; }

private:
    HRESULT Ready_Environment_Layer(LAYERTYPE layerType);
    HRESULT Ready_GameLogic_Layer(LAYERTYPE layerType);
    HRESULT Ready_UI_Layer(LAYERTYPE layerType);

    HRESULT Ready_Prototype();

    HRESULT Ready_Light() override;

private:
    // 마을 -> 던전 씬 전환용 트리거
    void Create_Trigger();

    // 추종자 생성
    void Spawn_FollowerCow();
    void Spawn_FollowerElephant();
    void Spawn_FollowerPig();

public:
    static CStage* Create(DEVICE graphicDev);

private:
    void Free() override;

private:
    CCombatStat*         m_PlayerCombatStatCom;
    CInventory*          m_PlayerInventoryCom;
    CWeaponEquip*        m_WeaponEquipCom;
    CWorkBench*          m_WorkBench;
    CPlayerSkill*        m_PlayerSkillCom;
    CTerrain*            m_Terrain;
    CUICookingMenu*      m_CookingMenu;
    CUI_FollowerCommand* m_FollowerCommand;
    CPlayerGathering*    m_PlayerGatherCom;
    CPlayerConstruction* m_PlayerConstructionCom;
    CUI_Text*           m_VillageEnterUI;
    _float               m_Alpha = 0;
    _float               m_UIOpenTime = 3.f;
    _bool                m_StageBegin;

    //CWorkBench*   m_WorkBench;
    //CKitchen*     m_Kitchen;
    //CFollowerCow* m_FollowerCow;

private:
    _float m_FollowerSpawnTimer = 0.f;
    _int   m_FollowerSpawnStep  = 0;
};
