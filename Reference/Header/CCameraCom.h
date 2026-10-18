#pragma once
#include "CComponent.h"

BEGIN(Engine)
class CTransform;

/// @brief 컴포넌트 기반의 View/Proj 변환용 컴포넌트
class ENGINE_DLL CCameraCom : public CComponent
{
public:
    // 카메라 컨트롤 모드
    enum CAM_MODE
    {
        CAM_FREE,               // 자유 카메라
        CAM_TARGET,             // 타겟 고정 카메라

        CAM_END
    };

    // 시점 타입
    enum VIEW_TYPE
    {
        VIEW_FPS,               // 1인칭
        VIEW_TPS,               // 3인칭
        VIEW_QUARTER,           // 쿼터뷰 (2.5D)

        VIEW_END
    };

    // 투영 타입
    enum PROJ_TYPE
    {
        PROJ_PERSPECTIVE,       // 원근 투영
        PROJ_ORTHOGRAPHIC,      // 직교 투영

        PROJ_END
    };

private:
    explicit CCameraCom();
    explicit CCameraCom(DEVICE graphicDev);
    explicit CCameraCom(const CCameraCom& rhs);
    ~CCameraCom() override;

public:
    HRESULT Ready_Camera();
    _int    Update_Component(const _float& timeDelta) override;
    void    LateUpdate_Component() override;

public: // Setter
    // 카메라 컨트롤 모드 변경
    void Set_CamMode(CAM_MODE camMode);

    // 시점 타입 변경
    void Set_ViewType(VIEW_TYPE viewType);

    /// @brief 투영 타입 변경
    /// @details UI등 Renderer에서 자동으로 직교 투영을 할 오브젝트가 아닌, 수동으로 직교 투영을 사용하고자 할 때 사용
    void Set_ProjType(PROJ_TYPE projType);

    // 카메라 오프셋 (타겟과의 거리) 설정
    void Set_Offset(const _vec3& offset) { m_Offset = offset; }

    /// @brief 타겟을 중심으로 하는 구면 좌표계 방식으로 오프셋을 설정하는 기능
    /// @details 거리와 X/Y축 회전 각도를 받아, 타겟 기준의 상대적 위치 벡터를 계산
    /// @param distance  타겟과 카메라 사이의 거리
    /// @param angleX    X축 회전 각도(Pitch: 상하 회전), Degree 값
    /// @param angleY    Y축 회전 각도(Yaw: 상하 회전), Degree 값
    void Set_Offset_Spherical(_float distance, _float angleX, _float angleY);

    void Set_FOV(const _float fov)
    {
        m_FOV     = fov;
        m_isDirty = true;
    }

    void Set_Aspect(const _float _aspect)
    {
        m_Aspect  = _aspect;
        m_isDirty = true;
    }

    void Set_Near(const _float _near)
    {
        m_Near    = _near;
        m_isDirty = true;
    }

    void Set_Far(const _float _far)
    {
        m_Far     = _far;
        m_isDirty = true;
    }

public: // Getter
    CAM_MODE  Get_CamMode() const { return m_CamMode; }
    VIEW_TYPE Get_ViewType() const { return m_ViewType; }
    PROJ_TYPE Get_ProjType() const { return m_ProjType; }

    const _matrix& Get_View() const { return m_matView; }    // 카메라의 뷰 행렬 Get
    const _matrix& Get_Proj() const { return m_matProj; }    // 카메라의 투영 행렬 Get

    const _vec3& Get_Offset() const { return m_Offset; }

    _float Get_FOV() const { return m_FOV; }
    _float Get_Aspect() const { return m_Aspect; }
    _float Get_Near() const { return m_Near; }
    _float Get_Far() const { return m_Far; }

private:
    void Compute_View_FPS();
    void Compute_View_TPS();
    void Compute_View_QUARTER();

    void Compute_Proj_Perspective();
    void Compute_Proj_Orthographic();

private:
    CAM_MODE  m_CamMode;
    VIEW_TYPE m_ViewType;
    PROJ_TYPE m_ProjType;

    _matrix m_matView, m_matProj;

    _vec3  m_Offset;        // 3인칭 카메라의 타겟 기준 오프셋 (거리)
    _vec3  m_Pos, m_Look,   m_Up,   m_Eye, m_At;
    _float m_FOV, m_Aspect, m_Near, m_Far;

    _bool m_isDirty;        // 투영 변환 재연산 트리거

public:
    static CCameraCom* Create(DEVICE graphicDev);
    CComponent*        Clone() override;

private:
    void Free() override;
};

END
