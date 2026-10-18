#pragma once
#include "CMonsterState.h"

class CEligos;
class CEligos_StateMachine;

class CEligos_Attack_Fly :
    public CMonsterState
{
public:
    enum class PHASE {WAIT,SHOOT, READY };
    PHASE m_ePhase;
private:
    explicit CEligos_Attack_Fly(CEligos* pOwner, CEligos_StateMachine* pStateMachine);
    virtual ~CEligos_Attack_Fly();

public:
    virtual void Enter();
    virtual void Exit();


public:
    virtual HRESULT Ready_State();
    virtual _int    Update_State(const _float& timeDelta);
    virtual void    LateUpdate_State(const _float& timeDelta);
    virtual void    Render_State();

    void SpawnFlies(int fliesNum, float noiseAngle, float noiseDist, float baseDist);
    //void SpawnFlies(int fliesNum, float offsetAngle, float offserDist);
private:
    CEligos* m_pOwner;
    CEligos_StateMachine* m_pStateMachine;

public:
    static CEligos_Attack_Fly* Create(CEligos* pOwner, CEligos_StateMachine* pStateMachine);

private:
    virtual void Free();

    private:
        bool m_StartFlag = false;
};

