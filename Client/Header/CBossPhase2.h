#pragma once

#include "CMonsterBase.h"

class CUI_BossHp;
class CMonsterController;
class CBoss2_StateMachine;
class CEyeball;

class CBossPhase2 : public CMonsterBase
{
public:
    enum PATTERN
    {
        PHASE2_SUMMON = 0,
        PHASE2_START,
        PHASE2_IDLE,
        PHASE2_ROAR,
        PHASE2_EYEBALL,
        PHASE2_CHAIN,
        PHASE2_BULLET1,
        PHASE2_BULLET2,
        PHASE2_GROGGY,
        PHASE2_DEAD,
        PHASE2_FOLLOWER,

        PHASE2_END

    };

private:
    explicit CBossPhase2(DEVICE graphicDev);
    explicit CBossPhase2(const CBossPhase2& rhs);
    virtual ~CBossPhase2() override;

public:
    HRESULT Ready_GameObject() override;
    void    Initialize_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;

    void    Render_Editor() override;

public:
    CMonsterController* Get_Controller() { return m_BossController; }
    const char*         Get_Boss2_StateName(_int stateNum);
    DEVICE              Get_GraphicDev() { return m_GraphicDev; }

    void				Set_DeathCount(_int count) { m_Eyeball_DeathCount = count; }
    _int				Get_DeathCount() { return m_Eyeball_DeathCount; }

    void				Increase_DeathCount(_int value = 1) { m_Eyeball_DeathCount += value; }

    const vector<CEyeball*>& Get_Eyeballs() { return m_Eyeballs; }

public:
	void                Add_Eyeball(CEyeball* eyeball);
	void                Remove_Eyeball(CEyeball* eyeball);
	void                Command_Ground(); // Eyeball 땅으로 들어가도록 명령

    _bool               Is_InSpecialPattern();

public:
    // 콜백이 너무 많아진다. 나중에는 map에 콜백을 넣는 이벤트 시스템을 구현해봐야겠음

    // 전투 시작, DynamicCam 적용
    using               OnBattleStartCallback = function<void()>;
    void                Set_OnBattleStart(OnBattleStartCallback callback) { m_OnBattleStart = callback; };

    // 보스 죽었을 때 -> Follower로 스프라이트 교체
    using               OnDeadCallback = function<void()>;
    void                Set_OnDead(OnDeadCallback callback) { m_OnDead = callback; };
    void                Trigger_OnDead() { if (m_OnDead) m_OnDead(); }

    // Roar 이후 대사 -> Summon 스프라이트 재생
    using               OnSummonRoarEndCallback = function<void()>;
    void                Set_OnSummonRoarEnd(OnSummonRoarEndCallback callback) { m_OnSummonRoarEnd = callback; }
    void                Trigger_OnSummonRoarEnd();

    void                Resume_Summon();

    // Summon 시작, 끝
    using               OnSummonAnimStartCallback = function<void()>;
    using               OnSummonAnimEndCallback = function<void()>;

    void                Set_OnSummonAnimStart(OnSummonAnimStartCallback callback) { m_OnSummonAnimStart = callback; }
    void                Set_OnSummonAnimEnd(OnSummonAnimEndCallback callback) { m_OnSummonAnimEnd = callback; }

    void                Trigger_OnSummonAnimStart() { if (m_OnSummonAnimStart) m_OnSummonAnimStart(); }
    void                Trigger_OnSummonAnimEnd() { if (m_OnSummonAnimEnd) m_OnSummonAnimEnd(); }

    // Roar 시작 할 때
    using               OnSummonStartCallback = function<void()>;
    void                Set_OnSummonRoarStart(OnSummonStartCallback callback) { m_OnSummonStart = callback; }
    void                Trigger_OnSummonStart() { if (m_OnSummonStart) m_OnSummonStart(); }

    // 죽고 NPC로 변신
    using               OnFollowerStartCallback = function<void()>;
    void                Set_OnFollowerStart(OnFollowerStartCallback callback) { m_OnFollowerStart = callback; }
    void                Trigger_OnFollowerStart() { if (m_OnFollowerStart) m_OnFollowerStart(); }


private:
    OnBattleStartCallback       m_OnBattleStart = nullptr;
    OnDeadCallback              m_OnDead = nullptr;

    OnSummonRoarEndCallback     m_OnSummonRoarEnd = nullptr;
    OnSummonStartCallback       m_OnSummonStart = nullptr;

    OnFollowerStartCallback     m_OnFollowerStart = nullptr;

    OnSummonAnimStartCallback   m_OnSummonAnimStart = nullptr;
    OnSummonAnimEndCallback     m_OnSummonAnimEnd = nullptr;

public:
    static CBossPhase2*     Create(DEVICE graphicDev);

protected:
    void    Animation_Setting() override;
    HRESULT Add_Component() override;

protected:
    void    Free() override;

private:
    CMonsterController*     m_BossController;
    _int                    m_Eyeball_DeathCount = 0;

	vector<CEyeball*>		m_Eyeballs;

};

