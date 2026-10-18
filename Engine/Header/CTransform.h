#pragma once
#include "CComponent.h"

BEGIN(Engine)
/// @brief 월드 행렬 변환과 값 저장용 다이나믹 컴포넌트
/// @details
/// - 월드 행렬을 기반으로 오브젝트의 위치, 회전, 크기를 관리
/// - 위치, 회전, 크기 등을 쉽게 조회, 갱신할 수 있도록 도와주는 역할
/// - 더티 플래그를 활용하여, 값 변경이 있는 프레임에서만 자동으로 월드 변환이 작동하도록 함
class ENGINE_DLL CTransform : public CComponent
{
private:
    explicit CTransform();
    explicit CTransform(DEVICE graphicDev);
    explicit CTransform(const CTransform& rhs);
    ~CTransform() override;

public:
    HRESULT Ready_Transform();
    _int    Update_Component(const _float& timeDelta) override;
    void    LateUpdate_Component() override;

public: // Getter       ====================================

    _vec3          Get_Right() const { return m_Info[INFO_RIGHT]; }
    _vec3          Get_Up() const { return m_Info[INFO_UP]; }
    _vec3          Get_Look() const { return m_Info[INFO_LOOK]; }
    _vec3          Get_Pos() const { return m_Info[INFO_POS]; }
    _vec3          Get_Scale() const { return m_Scale; }
    _vec3          Get_Rotation() const; // 회전의  X, Y, Z 축 회전 반환 (Pitch, Yaw, Roll)
    _float         Get_Rotation(ROTATION eType) const; // 원하는 축의 회전값 반환
    const _matrix& Get_World() const { return m_matWorld; }
    _vec3          Get_Euler() const { return m_EulerRad; }

    // 월드 행렬에서 값 직접 추출. 수업 코드 호환용
    void Get_Info(INFO type, _vec3* info);

    // 회전값 쿼터니온 반환.
    // 쿼터니온 값을 직접 쓰는 상황 외에는 안 쓰는 것을 추천
    _quat Get_Quat() const { return m_Rotation; }

    // 월드 업데이트 플래그 확인용
    bool Get_DirtyFlag() const { return m_IsDirty; }

public: // Setter       ====================================

    void Set_Pos(const _vec3& pos) { m_Info[INFO_POS] = pos; }
    void Set_Pos(_float x, _float y, _float z) { m_Info[INFO_POS] = { x, y, z }; }

    void Set_Scale(const _vec3& scale) { m_Scale = scale; }
    void Set_Scale(_float x, _float y, _float z) { m_Scale = { x, y, z }; }

    void Set_World(const _matrix& matrix) { m_matWorld = matrix; }

    // 인수) Rotation Setter
    // Radian 기준 Euler(Pitch=X, Yaw=Y, Roll=Z) 세팅
    void Set_Rotation(const _vec3& rad);
    void Set_RotationAxis(ROTATION axis, _float value);

public: // Transform Operations     ========================

    // direction * dt * speed 를 매 프레임 더해 오브젝트를 움직이는 함수
    void Move_Pos(const _vec3&  dir,
                  const _float& timeDelta,
                  const _float& speed);

    // 오브젝트 로컬 회전축 기준 회전 함수
    void Rotation(ROTATION rotType, const _float& radian, const _float& timeDelta);

    // 오브젝트 월드 회전축 기준 회전 함수
    void Rotation_World(ROTATION rotType, const _float& radian, const _float& timeDelta);

    // 수업 코드 기반의 추적. 쿼터뷰 환경에는 맞지 않으므로 사용시 수정하자
    void Chase_Target(const _vec3* targetPos, const _float& timeDelta, const _float& speed);

    void LookAt(const _vec3& targetPos);

private: // Internal Calculations   ========================

    _quat   Compute_LookAtTarget_Quat(const _vec3* pTargetPos);
    _matrix BuildLookRotation(const _vec3& inputForward, const _vec3& inputWorldUp);

    // 월드행렬 변환
    void Compute_WorldMatrix();

    /// @brief 모든 값 대해 변화를 체크하고 월드 변환을 할지를 판단
    /// @return 변화가 있으면 true, 없으면 false
    bool Check_Dirty();
    bool HasVectorChanged(const _vec3& now,
                          const _vec3& prev,
                          float        eps = 1e-4f);
    bool HasQuatChanged(const _quat& now,
                        const _quat& prev,
                        float        eps = 1e-4f);

private:
    _vec3 m_Info[INFO_END];             // 월드 방향, 위치
    _vec3 m_Scale;                      // 크기

    _quat   m_Rotation;                 // 쿼터니온 회전값
    _matrix m_matRot;                   // 회전 행렬 (쿼터니온 회전값 -> 행렬 변환 값 저장)

    _matrix m_matWorld;                 // 월드 행렬

    _vec3 m_EulerRad;

#pragma region 이전 프레임 값 저장 : 변화 체크 용도
    _vec3 m_PrevInfo[INFO_END];         // 이전 위치
    _vec3 m_PrevScale;                  // 이전 크기

    _quat m_PrevRotation;               // 이전 쿼터니온 회전값

    //_matrix m_matPrevParent;          // 부모 월드 행렬의 이전값 (필요시 사용)

    bool m_IsDirty;                     // 월드 변환 필요 여부 플래그
#pragma endregion

public:
    static CTransform* Create(DEVICE graphicDev);
    CComponent*        Clone() override;

private:
    void Free() override;
};

END
