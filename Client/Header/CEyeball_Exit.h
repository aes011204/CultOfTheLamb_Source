#pragma once

#include "CMonsterState.h"

class CMonsterController;
class CCharacterBase;
class CMonsterBase;

class CEyeball_Exit : public CMonsterState
{
private:
    explicit CEyeball_Exit(CMonsterBase* owner);
    virtual ~CEyeball_Exit();

public:
    void Enter() override;
    _int Update_State(const _float& timeDelta) override;
    void Exit() override;

    HRESULT Ready_State() override;

public:
    static CEyeball_Exit* Create(CMonsterBase* owner);

private:
    void Free() override;

private:
    CMonsterBase*       m_Owner;
    _float              m_Pattern_Cooldown;


};

