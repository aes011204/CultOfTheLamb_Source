#pragma once

#include "CComponent.h"

class CStateMachine;

BEGIN(Engine)
class CAnimator;
class CCombatStat;
END

class CMonsterController : public CComponent
{
protected:
    explicit CMonsterController(DEVICE graphicDev);
    explicit CMonsterController(const CMonsterController& rhs);
    virtual ~CMonsterController();

public:
    virtual HRESULT Ready_Controller();
    void    Initialize_Component() override;
    _int    Update_Component(const _float& timeDelta) override;

    void    Set_StateMachine(CStateMachine* stateMachine);
    CStateMachine* Get_StateMachine() { return m_StateMachine; }

    void    Change_State(_int stateKey);
    _int    Get_CurrentState();

public:
    CTransform*     Get_OwnerTransform();
    CAnimator*      Get_OwnerAnimator();
    CCombatStat*    Get_OwnerCombatStat();

public:
    static CMonsterController* Create(DEVICE graphicDev);
    CComponent* Clone() override;

protected:
    void Cache_Components();
    void Free() override;

protected:
    CAnimator*      m_AnimatorCom;
    CCombatStat*    m_CombatStatCom;
    CStateMachine*  m_StateMachine;

    _bool           m_ComponentsCached;


};

