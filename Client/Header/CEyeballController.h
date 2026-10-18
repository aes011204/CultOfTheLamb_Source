#pragma once

#include "CMonsterController.h"

class CStateMachine;

class CEyeballController : public CMonsterController
{
private:
    explicit CEyeballController(DEVICE graphicDev);
    explicit CEyeballController(const CEyeballController& rhs);
    virtual ~CEyeballController();

public:
    HRESULT Ready_Controller() override;
    void    Initialize_Component() override;
    _int    Update_Component(const _float& timeDelta) override;

    void Render_Editor() override;

public:
    static CEyeballController* Create(DEVICE graphicDev);
    CComponent* Clone() override;

private:
    void Free() override;

private:


};

