#pragma once

#include "CMonsterState.h"

class CMonsterController;
class CMonsterBase;

class CEyeball_Die : public CMonsterState
{
private:
    explicit CEyeball_Die(CMonsterBase* owner);
    virtual ~CEyeball_Die();

public:
    void Enter() override;
    _int Update_State(const _float& timeDelta) override;
    void Exit() override;

    HRESULT Ready_State() override;

public:
    static CEyeball_Die* Create(CMonsterBase* owner);

private:
    void Free() override;

private:
    CMonsterBase*       m_Owner;
    _float              m_Pattern_Cooldown;


};

