#pragma once
#include "CScene.h"

class CFog;
class CUI_BossHp;
class CTerrain;
class CUI_Dialogue;
class CCameraController;
class CPlayerSkill;
class CWeaponEquip;
class CInventory;

BEGIN(Engine)
class CUI;
class CCombatStat;
class CTransform;
END

class CStage_Boss2 : public CScene
{
private:
    enum class CAM_MOVE_TYPE
    {
        START_BOSS_ZOOM, // 줌인 하면서 카메라 쉐이킹
        SUMMON_ZOOM_OUT,
        EYEBALL_ZOOM,
        BOSS_DEAD,
        TRANSFORM_FOLLOWER,

        CAM_MOVE_END
    };

private:
    explicit CStage_Boss2(DEVICE graphicDev);
    ~CStage_Boss2() override;

public:
    HRESULT Ready_Scene() override;
    void    Initialize_Scene() override;
    _int    Update_Scene(const _float& timeDelta) override;
    void    LateUpdate_Scene(const _float& timeDelta) override;
    void    Render_Scene() override;

    HRESULT Ready_Light() override;

private:
    HRESULT Ready_Environment_Layer(LAYERTYPE layerType);
    HRESULT Ready_GameLogic_Layer(LAYERTYPE layerType);
    HRESULT Ready_UI_Layer(LAYERTYPE layerType);

private:
    void    Setup_DynamicCam();
    void    Setup_BossCallbacks();

    void    Start_CameraMoving(CAM_MOVE_TYPE type);
    void    Start_CameraShaking(CAM_MOVE_TYPE type);

public:
    void    Setup_DialogueCallbacks();
    void    OnDialogueEnd(const string& id);
    void    OnBoss2DeadEnd();

private:
    void    Move_PlayerUI(const _float timeDelta);
    void    Move_BossUI(const _float timeDelta);
    void    Start_UIAnimation(); 

public:
    static CStage_Boss2* Create(DEVICE graphicDev);

private:
    void Free() override;

private:
    CTerrain*               m_Terrain;
    CGameObject*            m_Boss;

    CGameObject*            m_Player;
    CCameraController*      m_CamController;
    CTransform*             m_PlayerTransformCom;

    // Point Light 설정
    _vec3                   m_LightPos = { 48.f, 15.f, 40.5f };
    COLOR                   m_LightDiffuse = { 1.f, 0.3f, 0.3f, 1.f };
    COLOR                   m_LightAmbient = { 0.1f, 0.05f, 0.05f, 1.f };
    COLOR                   m_LightSpecular = { 1.f, 0.5f, 0.5f, 1.f };
    _float                  m_LightRange = 43.f;
    _float                  m_Attenuation0 = 0.f;
    _float                  m_Attenuation1 = 0.01f;
    _float                  m_Attenuation2 = 0.f;

private:
    CUI_Dialogue*           m_Dialogue = nullptr;
    CAM_MOVE_TYPE           m_CamMove = CAM_MOVE_TYPE::START_BOSS_ZOOM;

    vector<CUI*>            m_Player_UI;
    CUI_BossHp*             m_BossHpbar = nullptr;

    CCombatStat*            m_PlayerCombatStatCom = nullptr;
    vector<_vec3>           m_OriginalUIPositions;
    vector<_vec3>           m_TargetUIPositions;

    _bool                   m_IsUIMoving = false;
    _bool                   m_IsBossUIMoving = false;
    _float                  m_UIAnimTime = 0.f;
    _float                  m_UIAnimDuration = 0.5f;
};
