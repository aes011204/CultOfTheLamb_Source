#pragma once
#include "CCombatStat.h"
#include "CGameObject.h"


BEGIN(Engine)
class CBoxCollider;
class CTransform;

class ENGINE_DLL CAttackBase : public CGameObject
{
protected:
    explicit CAttackBase(DEVICE graphicDev);
    explicit CAttackBase(const CAttackBase& rhs);
    virtual ~CAttackBase();

public:
    virtual _int    Update_GameObject(const _float& timeDelta) override;
    virtual void    LateUpdate_GameObject(const _float& timeDelta) override;
    virtual void    Render_GameObject() override;

    virtual void OnBeginOverlap(CCollider* self, CCollider* other) override;
    virtual void OnEndOverlap(CCollider* self, CCollider* other) override;

public:
    void Set_AttackData(const ATTACKDATA& hit) { m_AttackData = hit; }
    void Set_Instigator(CGameObject* instigator);
    void Set_LifeTime(float LifeTime) {m_LifeTime = LifeTime;}
    void Set_DestroyOnHit(_bool destroy) { m_DestroyOnHit = destroy; }

    virtual CCollider* Get_Collider() = 0;

public:
    /// @brief CAttackBox가 소멸할 때 자동으로 호출할 void 반환타입 람다 함수를 등록해두는 Setter
    /// @param callback 소멸시 자동 호출할 람다함수 등록
    void Set_OnDestroyCallback(function<void()> callback) { m_OnDestroyCallback = move(callback); }

protected:
    void Free() override;


protected:
    CTransform*     m_TransformCom;

    ATTACKDATA      m_AttackData;       // 데미지 구조체
    CGameObject*    m_Instigator;       // 공격자 (플레이어)
    float           m_LifeTime;         // 살아있는 시간 ex) 1초뒤 충돌체 삭제

    _bool           m_DestroyOnHit;

    function<void()> m_OnDestroyCallback;
};

END
