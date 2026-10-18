#pragma once
#include "CComponent.h"

/// @brief 플레이어의 입력 처리를 담당하는 컴포넌트
/// @details
/// 사용자로부터 키보드 및 마우스 입력을 받아 게임 내에서 사용할 수 있는 데이터로 변환합니다.
/// - 이동 입력: WASD 입력을 받아 이동 방향 벡터(m_MoveDir)를 계산합니다.
/// - 시선 처리: 마우스 커서 위치를 추적하여 플레이어가 바라보는 방향(m_MouseDir)을 계산합니다.
/// @note
/// 이 컴포넌트는 직접적인 로직을 수행하기보다, Controller나 Movement 컴포넌트가 사용할 수 있는
/// 순수한 입력 데이터를 제공하는 역할에 집중합니다.
class CPlayerInput : public CComponent
{
private:
    explicit CPlayerInput(DEVICE graphicDev);
    explicit CPlayerInput(const CPlayerInput& rhs);
    ~CPlayerInput() override;

public:
    HRESULT Ready_PlayerInput();
    _int    Update_Component(const _float& timeDelta) override;
    void    LateUpdate_Component() override;

public:     // --- Getter ---
    const _vec3& Get_MoveDir() const { return m_MoveDir; }
    const _vec3& Get_MouseDir() const { return m_MouseDir; } // 플레이어 -> 마우스 방향벡터 반환

private:    // --- 멤버 변수 ---
    _vec3 m_MoveDir;    // 이동 방향
    _vec3 m_MouseDir;   // 플레이어 -> 마우스 방향

public:
    static CPlayerInput* Create(DEVICE graphicDev);
    CComponent* Clone() override;

private:
    void Free() override;
};
