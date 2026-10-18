#pragma once
#include "CRunLootLog.h"
#include "CScene.h"

class CInventory;
class CWeaponEquip;
class CTerrain;
class CPlayer;
class CUI_Fade;
class CCameraMovement;
class CCameraController;
class CTarotNPC;

BEGIN(Engine)
class CCombatStat;
class CTransform;

END

struct RoomData
{
    _vec3   playerSpawnPos{};         
    string  terrainFilePath;    
    _vec3   terrainOffset{};    

    // 방 클리어 여부
    // 근데 이거 일단은 구현 X
    bool    isCleared = false;
    _int    requiredKills = 0;
};

class CStage_Dungeon : public CScene
{
public:
    enum class Room
    {
        START,
        MONSTER1,
        MONSTER2,
        TARO,
        BOSS,

        DUNGEON_END
    };

private:
    explicit CStage_Dungeon(DEVICE graphicDev);
    ~CStage_Dungeon() override;

public:
    HRESULT Ready_Scene() override;
    void    Initialize_Scene() override;
    _int    Update_Scene(const _float& timeDelta) override;
    void    LateUpdate_Scene(const _float& timeDelta) override;
    void    Render_Scene() override;
    HRESULT Ready_Light() override;

public:
    void    Spawn_Weapon();
    void    Spawn_FollowerCow_Rescue();
    void    Spawn_FollowerElephant_Rescue();
    void    Spawn_FollowerPig_Rescue();

    void    Create_RoomTriggers();
    void    Create_Trigger_Between(Room fromRoom, Room toRoom,
                _vec3 offsetFromRoom, _vec3 size);

    void    Create_Trigger_Between(Room fromRoom,
        _vec3 offsetFromRoom, _vec3 size, function<void()> callback);
    void    EnterDungeon();
    void    Change_Room(Room room);
    CRunLootLog& Get_RunLootLog() { return m_RunLootLog; }
    _float  Get_Timer() { return m_Timer; }
    void    Set_TimerRunning(bool timerRunning) { m_TimerRunning = timerRunning; }

    // 몬스터 죽었을 때 호출
    void    OnMonsterDeath(CGameObject* obj) override;
    _bool   IsCleared() { return m_DungeonCleared; }

private:
    const CRunLootLog& Get_RunLootLog() const { return m_RunLootLog; }
private:
    HRESULT Ready_Environment_Layer(LAYERTYPE layerType);
    HRESULT Ready_GameLogic_Layer(LAYERTYPE layerType);
    HRESULT Ready_UI_Layer(LAYERTYPE layerType);

    HRESULT Ready_Prototype();

public:
    static CStage_Dungeon* Create(DEVICE graphicDev);

private:
    void Free() override;
    void Fade_Check(const _float timeDelta);

private:
    CCombatStat*    m_PlayerCombatStatCom;
    CInventory*     m_PlayerInventoryCom;
    CWeaponEquip*   m_WeaponEquipCom;

    map<Room, CTerrain*> m_Terrains {};

private:
    RoomData        m_RoomData[5];   // 5개 방 미리 생성
    Room            m_CurrentRoom;   // 현재 방

    CGameObject*    m_Player;
    CTransform*     m_PlayerTransform;

    _bool           m_RequestChangeScene = false;
    SCENETYPE       m_NextSceneType = SCENETYPE::SC_END;

    CUI_Fade*       m_UI_Fade;

private:
    _bool           m_IsFading = false;
    Room            m_NextRoom;

    _float          m_PrevZoomSpeed = 0.f;

    // 카메라 무브먼트
    CCameraMovement* m_CamMovement;
    CCameraController* m_CamController;
    CRunLootLog         m_RunLootLog;
    _float          m_Timer;

    _bool           m_TimerRunning;
    _bool           m_DungeonCleared = false;
    _bool           m_IsClearMenuOpened;
    _bool           m_Start;
};


