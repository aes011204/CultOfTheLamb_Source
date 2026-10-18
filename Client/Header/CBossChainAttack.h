#pragma once
#include "CMonsterState.h"

class CBossPhase1;
class CBossStateMachine;

class CBossChainAttack :
    public CMonsterState
{
public:
    enum class PHASE { PLAYERLINE, CROSS, RANDCHAIN, CIRCLE, PHASE_END };
private:
    explicit CBossChainAttack(CBossPhase1* pOwner, CBossStateMachine* pStateMachine);
    virtual ~CBossChainAttack();

public:
    virtual void Enter();
    virtual void Exit();


public:
    virtual HRESULT Ready_State();
    virtual _int    Update_State(const _float& timeDelta);

    virtual void    LateUpdate_State(const _float& timeDelta);
    virtual void    Render_State();

    bool PlayerLineUpdate(const _float& timeDelta, _int MaxCount, _float m_Offset, float Speed);
    void RandomChain(const _float& timeDelta, _vec3 BaseCenter, int BaseLenght, float interval);
    bool CircleChain(const _float& timeDelta, int num, int length, float interval);

    float easeInQuint10(float x);

private:
    CBossPhase1* m_pOwner;
    CBossStateMachine* m_pStateMachine;

public:
    static CBossChainAttack* Create(CBossPhase1* pOwner, CBossStateMachine* pStateMachine);

    static int m_CPatterCount;
private:
    PHASE m_ePhase;
    float m_fElapsedTime;

    int m_Count = 0;
    _vec3 m_PlayerDir;
    int LineCount = 0;
    bool m_LineFlag = false;
    bool m_StartPhase = false;
    float m_RandChainTime = 0;
    bool m_StartFlag = false;

    // 쓸거
    float m_DirectionOffset;
    float m_MaxLineCount;
    float m_Interval;
    float m_CircleRadius;
    float m_GroundY;
    float m_CrossOffset;
    float m_CellSize;

private:
    virtual void Free();

    //

};

