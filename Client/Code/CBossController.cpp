#include "pch.h"
#include "CBossController.h"
#include "CStateMachine.h"

CBossController::CBossController(DEVICE graphicDev)
    : CMonsterController(graphicDev)
{
}

CBossController::CBossController(const CBossController& rhs)
    : CMonsterController(rhs)

{
}

CBossController::~CBossController()
{

}

HRESULT CBossController::Ready_Controller()
{
    if (FAILED(CMonsterController::Ready_Controller()))
        return E_FAIL;

    return S_OK;
}

void CBossController::Initialize_Component()
{
    CMonsterController::Initialize_Component();

   
}

_int CBossController::Update_Component(const _float& timeDelta)
{
    _int exit = CMonsterController::Update_Component(timeDelta);

    

    return exit;
}

CBossController* CBossController::Create(DEVICE graphicDev)
{
    auto controller = new CBossController(graphicDev);

    if (FAILED(controller->Ready_Controller()))
    {
        Safe_Release(controller);
        MSG_BOX("Boss Controller Create Failed");
        return nullptr;
    }

    return controller;
}

CComponent* CBossController::Clone()
{
    return new CBossController(*this);
}

void CBossController::Free()
{
    CMonsterController::Free();


}

void CBossController::Render_Editor()
{
    CMonsterController::Render_Editor();

    ImGui::Text("=== Boss Phase2 ===");

    
}
