#pragma once

#include "CMonsterState.h"

class CBossController;
class CCharacterBase;

BEGIN(Engine)
class CGameObject;
class CRcTex;
END

class CBoss2_Groggy : public CMonsterState
{
private:
    explicit CBoss2_Groggy(CGameObject* owner);
    virtual ~CBoss2_Groggy();

public:
    void Enter() override;
    _int Update_State(const _float& timeDelta) override;
    void Exit() override;

    HRESULT Ready_State() override;

public:
    static CBoss2_Groggy* Create(CGameObject* owner);

private:
    void Free() override;

private:
    CGameObject*     m_Owner;

    _float           m_Pattern_Cooldown;

};

