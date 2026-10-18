#include "pch.h"
#include "CPlayerInput.h"
#include "CDInputMgr.h"
#include "CPickingHelper.h"
#include "CTransform.h"

CPlayerInput::CPlayerInput(DEVICE graphicDev)
    : CComponent(graphicDev),
      m_MoveDir(0.f, 0.f, 0.f),
      m_MouseDir(0.f, 0.f, 0.f)
{ }

CPlayerInput::CPlayerInput(const CPlayerInput& rhs)
    : CComponent(rhs),
      m_MoveDir(rhs.m_MoveDir),
      m_MouseDir(rhs.m_MouseDir)
{ }

CPlayerInput::~CPlayerInput()
{ }

HRESULT CPlayerInput::Ready_PlayerInput()
{
    return S_OK;
}

_int CPlayerInput::Update_Component(const _float& timeDelta)
{
    const auto inputMgr = CDInputMgr::GetInstance();

    _int axisX{ 0 }, axisZ{ 0 };

    // 키 입력 체크
    if (inputMgr->KeyPress(DIK_W)) axisZ++;
    if (inputMgr->KeyPress(DIK_S)) axisZ--;
    if (inputMgr->KeyPress(DIK_D)) axisX++;
    if (inputMgr->KeyPress(DIK_A)) axisX--;

    // MoveDir 벡터 생성
    m_MoveDir =
        _vec3(static_cast<_float>(axisX), 0.f, static_cast<_float>(axisZ));

    if (D3DXVec3LengthSq(&m_MoveDir) > 0.001f)
        D3DXVec3Normalize(&m_MoveDir, &m_MoveDir);

    // MouseDir 벡터 생성 (Picking)
    _vec3 vHitPos;

    if (Engine::PickPlaneY(m_GraphicDev, g_hWnd, 0.f, vHitPos))
    {
        if (m_TransformCom)
        {
            const _vec3 vPlayerPos = m_TransformCom->Get_Pos();

            m_MouseDir   = vHitPos - vPlayerPos;
            m_MouseDir.y = 0.f; // 평면 상의 방향이므로 y는 0으로 고정 (PickPlaneY가 이미 y=0 평면이긴 하지만 안전하게)

            if (D3DXVec3LengthSq(&m_MouseDir) > 0.001f)
                D3DXVec3Normalize(&m_MouseDir, &m_MouseDir);
        }
    }

    return 0;
}

void CPlayerInput::LateUpdate_Component()
{ }

CPlayerInput* CPlayerInput::Create(DEVICE graphicDev)
{
    auto playerInput = new CPlayerInput(graphicDev);

    if (FAILED(playerInput->Ready_PlayerInput()))
    {
        Safe_Release(playerInput);
        MSG_BOX("PlayerInput Create Failed");
        return nullptr;
    }

    return playerInput;
}

CComponent* CPlayerInput::Clone()
{
    return new CPlayerInput(*this);
}

void CPlayerInput::Free()
{
    CComponent::Free();
}
