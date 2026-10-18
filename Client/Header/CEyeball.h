#pragma once

#include "CMonsterBase.h"

class CMonsterController;

BEGIN(Engine)
class CTexture;
class CRigidbody;
END

class CEyeball : public CMonsterBase
{
public:
    enum PATTERN
    {
        EYEBALL_START,
        EYEBALL_START_EXIT,
        EYEBALL_START_GROUND, // 플레이어 방향으로 직진
        EYEBALL_IDLE,
        EYEBALL_EXIT,
        EYEBALL_ENTER,
        EYEBALL_GROUND_MOVE, // 땅속에서 랜덤이동
        EYEBALL_MOVE,
        EYEBALL_ATTACK,
        EYEBALL_CENTER_ATTACK,
        EYEBALL_DEAD,

        EYEBALL_END
    };

private:
    explicit CEyeball(DEVICE graphicDev, CMonsterBase* boss);
    explicit CEyeball(const CEyeball& rhs, CMonsterBase* boss);
    ~CEyeball() override;

public:
    HRESULT Ready_GameObject() override;
    void    Initialize_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;

    void    Render_Editor() override;

public:
    void    OnBeginOverlap(CCollider* self, CCollider* other) override;
    void    OnEndOverlap(CCollider* self, CCollider* other) override;

public:
    // Eyeball 죽을 때
    using OnEyeDeadCallback = function<void()>;
    void Set_OnDeathCallback(OnEyeDeadCallback callback);
    void Trigger_OnDeath();

    // 시작 위치 도달
    using OnReachedTargetCallback = function<void()>;
    void Set_OnReachedCallback(OnReachedTargetCallback callback);
    void Trigger_OnReachedTarget();

    // 움직일 경로에 벽 있는지 판단
    _bool Trace_HitWall(const _vec3& pos, const _vec3& dir, _float distance);

private:
    OnEyeDeadCallback       m_OnEyeDead = nullptr;
    OnReachedTargetCallback m_OnReachedTarget = nullptr;
    _bool                   m_HasTriggeredReached = false;

private:
    void     Animation_Setting() override;
    HRESULT  Add_Component() override;

public:
    CMonsterController* Get_Controller() { return m_EyeballController; }
    const char*         Get_StateName(_int stateNum);

    _vec3               Get_MoveDirection() const { return m_MoveDirection; }
    void                Set_MoveDirection(const _vec3& dir) { m_MoveDirection = dir; }

    CMonsterBase*       Get_Boss() { return m_Boss; }

public:
    static CEyeball* Create(DEVICE graphicDev, CMonsterBase* boss);

protected:
    void Free() override;

private:
    CRigidbody*         m_RigidbodyCom;
    CMonsterController* m_EyeballController;
	CMonsterBase*		m_Boss;

    _vec3   m_MoveDirection = _vec3(0.f, 0.f, -4.f);
};

