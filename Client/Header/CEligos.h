#pragma once
#include "CMonsterBase.h"

class CEligos_StateMachine;

class CEligos :
    public CMonsterBase
{
public:
    enum  ELIGOSSTATE { BOSSREVEAL = 0, ATTACK_FLY, ATTACK_BULLET, ATTACK_MELEE, BOSSDEAD, BOSSIDLE, BOSSMOVE, BOSS_END };
private:
    explicit CEligos(DEVICE graphicDev);
    explicit CEligos(const CEligos& rhs);
    ~CEligos() override;
public:
    HRESULT Ready_GameObject() override;
    void    Initialize_GameObject() override; // Ready보다 한 단계 아래 생명주기
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;
    void    Render_Editor() override;

public:
    void    OnBeginOverlap(CCollider* self, CCollider* other) override;
    void    OnEndOverlap(CCollider* self, CCollider* other) override;

    DEVICE  Get_GraphicDev() { return m_GraphicDev; }

private:
    HRESULT Add_Component() override;
    void    Animation_Setting() override;

    const char* GetBossStateName(_int StateNum);

private:
    CEligos_StateMachine* m_StateMachine;

    CRigidbody* m_RigidbodyCom;

public:
    static CEligos* Create(DEVICE graphicDev);

    //_vec3 Get_Center() { return BaseCenter; }
    //float Get_Length() { return BaseLength; }

    CEligos_StateMachine* Get_StateMachine() { return m_StateMachine; }

private:
    void Free() override;

private:
    //_vec3   StartPos = { 48.2f ,3.8f, 57.f };
    //_vec3   BaseCenter;
    //int     BaseLength = 25;
    //float   interval = 1.f;

    bool    m_bHurt = false;
    float   m_fAlpha = 0.8f;
    float   m_bHurtTime = 0;

public:
    //// 보스 죽기 전에 대화 시작
    //using   OnDeadTalkStartCallback = function<void()>;
    //void    Set_OnDeadTalkStart(OnDeadTalkStartCallback callback);

    //// 보스 죽었을 때
    //using   OnBossDeadCallback = function<void()>;
    //void    Set_OnBossDeadCallback(OnBossDeadCallback callback) { m_OnBossDead = callback; }

private:
    //OnDeadTalkStartCallback m_OnDeadTalkStart = nullptr;
    //OnBossDeadCallback m_OnBossDead = nullptr;


};

