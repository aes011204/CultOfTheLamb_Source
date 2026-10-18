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

class CStage_Boss1 : public CScene
{
private:
    explicit CStage_Boss1(DEVICE graphicDev);
    virtual ~CStage_Boss1() override;

public:
    HRESULT Ready_Scene() override;
    void    Initialize_Scene() override;
    _int    Update_Scene(const _float& timeDelta) override;
    void    LateUpdate_Scene(const _float& timeDelta) override;
    void    Render_Scene() override;

    HRESULT Ready_Light() override;

public:
    using   OnTriggerCallback = function<void()>;
    void    Create_Trigger(_vec3 pos, _vec3 size, OnTriggerCallback callback, _bool autoDestroy = true);;

private:
    HRESULT Ready_Environment_Layer(LAYERTYPE layerType);
    HRESULT Ready_GameLogic_Layer(LAYERTYPE layerType);
    HRESULT Ready_UI_Layer(LAYERTYPE layerType);

    void    OnDialogueEnd(const string& id);

    void    Begin_PlayerDecide();
    void    Move_PlayerUI(const _float timeDelta);
    void    Move_BossUI(const _float timeDelta);

    void    Hide_PlayerUI();
    void    Show_PlayerUI();

    void    Setup_UIAnimation(_float offsetY);
    void    OnRefusedEnd();

    // 대화 끝났을 때 호출
    void    On_BossStartEnd();
    void    On_SacrificeEnd();
    void    On_RejectEnd();
    void    On_DeadEnd();

    // 보스 스테이지 1 기본 카메라 세팅
    void    Reset_Cam();

    // 보스 거리 비례해서 카메라 세팅
    void    Setup_DynamicCam();

    // 콜백 설정 함수들
    void    Setup_TriggerCallbacks();
    void    Setup_DialogueCallbacks();
    void    Setup_BossCallbacks();
    void    Setup_PlayerCallbacks();

public:
    static CStage_Boss1* Create(DEVICE graphicDev);

private:
    void Free() override;

private:
    CTerrain*           m_Terrain;
    CGameObject*        m_Boss;
    CGameObject*        m_Player;

    CUI_Dialogue*       m_Dialogue;
    CCameraController*  m_CamController;

    CTransform*         m_PlayerTransformCom;
    CCombatStat*        m_PlayerCombatStatCom;
    CInventory*         m_PlayerInventoryCom;

    CWeaponEquip*       m_WeaponEquipCom;
    CPlayerSkill*       m_PlayerSkillCom;

    vector<CUI*>        m_Player_UI;

    CUI_BossHp*         m_BossHpbar;

private:
    _bool               m_IsUIMoving = false;
    _bool               m_IsBossUIMoving = false;
    _bool               m_IsUIShowing = false;

    _float              m_UIAnimTime = 0.f;
    _float              m_UIAnimDuration = 0.75f;

    vector<_vec3>       m_OriginalUIPositions;
    vector<_vec3>       m_TargetUIPositions;

    _float              m_DialogueDelayTimer = 0.f;
    _bool               m_WaitingForDialogue = false;

private:
    // Fog
    CFog*               m_Fog = nullptr;

    _vec3               m_FogPos = { 30.f, 3.f, 40.f };
    _float              m_FogSwaySpeed = 0.5f;
    _float              m_FogSwayAmount = 2.f;
    _float              m_FogAlpha = 0.5f;
    _vec3               m_FogScale = { 15.f, 10.f, 1.f };

};
