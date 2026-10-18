#pragma once

#include "CMonsterBase.h"

BEGIN(Engine)
class CRcTex;
class CTransform;
class CTexture;
class CAnimator;
class CProtoMgr;
class CRectCollider;
class CBoxCollider;
class CState;
class CCombatStat;
END

class CUI_BossHp;
class CBossStateMachine;

class CBossPhase1 : public CMonsterBase
{
public:
    enum  BOSSPHASE1 { BOSSAWAKE = 0, BULLETATTACK, CHAINATTACK, BOSSDEAD, BOSSIDLE, BOSSMOVE, BOSSGROGGRY, TALK, BOSS_END };
private:
    explicit CBossPhase1(DEVICE graphicDev);
    explicit CBossPhase1(const CBossPhase1& rhs);
    ~CBossPhase1() override;
public:
    HRESULT Ready_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;
    void    Render_Editor() override;
    void    Initialize_GameObject() override; // Ready보다 한 단계 아래 생명주기

public:
    void    OnBeginOverlap(CCollider* self, CCollider* other) override;
    void    OnEndOverlap(CCollider* self, CCollider* other) override;

    DEVICE  Get_GraphicDev() { return m_GraphicDev; }

private:
    HRESULT Add_Component() override;
    void    Animation_Setting() override;

    const char* GetBossStateName(_int StateNum);

private:
    CBossStateMachine* m_StateMachine;

public:
    static CBossPhase1* Create(DEVICE graphicDev);

    _vec3 Get_Center() { return BaseCenter; }
    float Get_Length() { return BaseLength; }

    CBossStateMachine* Get_StateMachine() { return m_StateMachine; }

private:
    void Free() override;

private:
    _vec3   StartPos = { 48.2f ,3.8f, 57.f };
    _vec3   BaseCenter;
    int     BaseLength = 25;
    float   interval = 1.f;

    bool    m_bHurt = false;
    float   m_fAlpha = 0.8f;
    float   m_bHurtTime = 0;

public:
    // 보스 죽기 전에 대화 시작
    using   OnDeadTalkStartCallback = function<void()>;
    void    Set_OnDeadTalkStart(OnDeadTalkStartCallback callback);

    // 보스 죽었을 때
    using   OnBossDeadCallback = function<void()>;
    void    Set_OnBossDeadCallback(OnBossDeadCallback callback) { m_OnBossDead = callback; }

private:
    OnDeadTalkStartCallback m_OnDeadTalkStart = nullptr;
    OnBossDeadCallback m_OnBossDead = nullptr;

};
