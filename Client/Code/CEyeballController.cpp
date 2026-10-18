#include "pch.h"
#include "CEyeballController.h"
#include "CStateMachine.h"

CEyeballController::CEyeballController(DEVICE graphicDev)
    : CMonsterController(graphicDev)
{
}

CEyeballController::CEyeballController(const CEyeballController& rhs)
    : CMonsterController(rhs)
{
}

CEyeballController::~CEyeballController()
{

}

HRESULT CEyeballController::Ready_Controller()
{
    if (FAILED(CMonsterController::Ready_Controller()))
        return E_FAIL;

    return S_OK;
}

void CEyeballController::Initialize_Component()
{
    CMonsterController::Initialize_Component();


}

_int CEyeballController::Update_Component(const _float& timeDelta)
{
    _int exit = CMonsterController::Update_Component(timeDelta);



    return exit;
}

CEyeballController* CEyeballController::Create(DEVICE graphicDev)
{
    auto controller = new CEyeballController(graphicDev);

    if (FAILED(controller->Ready_Controller()))
    {
        Safe_Release(controller);
        MSG_BOX("Eyeball Controller Create Failed");
        return nullptr;
    }

    return controller;
}

CComponent* CEyeballController::Clone()
{
    return new CEyeballController(*this);
}

void CEyeballController::Free()
{
    CMonsterController::Free();


}

void CEyeballController::Render_Editor()
{
    CMonsterController::Render_Editor();

    ImGui::Text("=== Eyeball Controller ===");


}
