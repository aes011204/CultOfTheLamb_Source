#pragma once
#include "CComponent.h"

BEGIN(Engine)
/// @brief 카메라의 View 및 Projection 행렬을 계산하고 관리하는 다이나믹 컴포넌트
/// @details 
/// - 카메라의 위치와 회전 정보를 기반으로 View 행렬 생성, FOV, Aspect, Near, Far 정보를 기반으로 Projection 행렬 생성
/// - 투영 방식(Perspective/Orthographic) 변경을 지원
class ENGINE_DLL CCameraCalc : public CComponent
{
private:
    explicit CCameraCalc(DEVICE graphicDev);
    explicit CCameraCalc(const CCameraCalc& rhs);
    ~CCameraCalc() override;

public:
    HRESULT Ready_CameraCalc();
    void    Initialize_Component() override;
    _int    Update_Component(const _float& timeDelta) override;
    void    LateUpdate_Component() override;

public:  // --- Getter ---
    const _matrix& Get_View() const { return m_matView; } // 현재 View 행렬 반환
    const _matrix& Get_Proj() const { return m_matProj; } // 현재 Projection 행렬 반환

    PROJECTION_TYPE Get_ProjType() const { return m_ProjType; } // 현재 투영 타입 반환 (Perspective / Orthographic)

    _float Get_FOV() const { return m_FOV; }
    _float Get_Aspect() const { return m_Aspect; }
    _float Get_Near() const { return m_Near; }
    _float Get_Far() const { return m_Far; }

public:  // --- Setter ---
    void Set_ProjType(PROJECTION_TYPE projType);

    // 시야각(FOV) 설정
    void Set_FOV(const _float _fov)
    {
        m_FOV     = _fov;
        m_isDirty = true;
    }

    // 종횡비(Aspect Ratio) 설정
    void Set_Aspect(const _float _aspect)
    {
        m_Aspect  = _aspect;
        m_isDirty = true;
    }

    // Near 평면 거리 설정
    void Set_Near(const _float _near)
    {
        m_Near    = _near;
        m_isDirty = true;
    }

    // Far 평면 거리 설정
    void Set_Far(const _float _far)
    {
        m_Far     = _far;
        m_isDirty = true;
    }

    // 투영 행렬 재계산 요청 플래그 설정
    void Set_Dirty() { m_isDirty = true; }

private: // --- 내부 로직 ---
    void Update_View_Matrix(); // View 행렬 갱신
    void Update_Proj_Matrix(); // Projection 행렬 갱신

private: // --- 멤버 변수 ---
    _matrix m_matView, m_matProj;

    PROJECTION_TYPE m_ProjType;   // 투영 타입

    _float m_FOV, m_Aspect, m_Near, m_Far;

    _bool m_isDirty;        // 투영 변환 재연산 플래그

public:
    static CCameraCalc* Create(DEVICE graphicDev);
    CComponent*         Clone() override;

private:
    void Free() override;
};

END
