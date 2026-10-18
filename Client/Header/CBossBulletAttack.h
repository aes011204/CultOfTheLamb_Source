#pragma once
#include "CMonsterState.h"

class CBossPhase1;
class CBossStateMachine;
class CBossBullet;


class CBossBulletAttack :
    public CMonsterState
{
public:
    enum class PHASE { CIRCLE, ROTCROSS_SPREAD, SPREAD ,PHASE_END};

private:
    explicit CBossBulletAttack(CBossPhase1* pOwner, CBossStateMachine* pStateMachine);
    virtual ~CBossBulletAttack();

public:
    virtual void Enter();
    virtual void Exit();


public:
    virtual HRESULT Ready_State();
    virtual _int    Update_State(const _float& timeDelta);
    virtual void    LateUpdate_State(const _float& timeDelta);
    virtual void    Render_State();

    bool CircleBullet(const _float& timeDelta, int num, int radius, float interval);
    bool CrossBullet(const _float& timeDelta, int num, int length, float interval);
    bool SpreadBullet(const _float& timeDelta, int num, float interval, float Speed, int angleOffset = 0);
   
private:
    CBossPhase1* m_pOwner;
    CBossStateMachine* m_pStateMachine;
public:
    static CBossBulletAttack* Create(CBossPhase1* pOwner, CBossStateMachine* pStateMachine);

    static int m_BPatterCount;
private:
    PHASE m_ePhase;
    float m_fElapsedTime =0;
    float m_fExtraElapsedTime = 0;
    bool m_StartFlag = false;
    int m_Count = 0;

    vector<CBossBullet*> tmpBullet;
    bool m_TargetFlag = false;
    int m_CircleCount = 0;
    bool m_CircleEndFlag=false;
    float m_circleCountTime;
    bool m_Shoot=false;

    // 쓸거
    float m_offset;
    float m_CircleRadius;
    float m_Interval;
    float m_CrossSpreadInterval;
    float m_SpreadInterval;
    float m_GroundY;
    float m_CircleSpeed;

private:
    virtual void Free();

};

