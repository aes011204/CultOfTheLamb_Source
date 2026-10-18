#pragma once

#include "CMonsterController.h"

class CStateMachine;

class CBossController : public CMonsterController
{
private:
    explicit CBossController(DEVICE graphicDev);
    explicit CBossController(const CBossController& rhs);
    virtual ~CBossController();

public:
    HRESULT Ready_Controller() override;
    void    Initialize_Component() override;
    _int    Update_Component(const _float& timeDelta) override;

    void    Render_Editor() override;

public:
    static CBossController* Create(DEVICE graphicDev);
    CComponent* Clone() override;

private:
    void Free() override;

private:


};

