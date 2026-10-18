#pragma once

#include "CMonsterState.h"

class CBossController;
class CCharacterBase;
class CMonsterBase;
class CEyeball;

BEGIN(Engine)
class CGameObject;
END

class CBoss2_SummonFollower : public CMonsterState
{
    enum class SummonPhase
    {
        ROAR, DIALOGUE, SUMMON
    };

private:
    explicit CBoss2_SummonFollower(CMonsterBase* owner);
    virtual ~CBoss2_SummonFollower();

public:
    void    Enter() override;
    _int    Update_State(const _float& timeDelta) override;
    void    Exit() override;

    HRESULT Ready_State() override;

    void    Resume_Summon() { m_DialogueFinished = true; }

private:
    void Summon_Follower();

public:
    static CBoss2_SummonFollower* Create(CMonsterBase* owner);

private:
    void Free() override;

private:
    CMonsterBase*   m_Owner;
    _float          m_Pattern_Cooldown;

    SummonPhase     m_Phase = SummonPhase::ROAR;

    _bool           m_DialogueFinished = false;
    _bool           m_IsSummon = false;

};



