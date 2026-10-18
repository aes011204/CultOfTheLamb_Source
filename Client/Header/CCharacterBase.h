#pragma once
#include "CRenderObject.h"

BEGIN(Engine)
class CAnimator;
class CState;
class CCombatStat;
class CCollider;
class CBoxCollider;
class CRigidbody;
END

// 모든 캐릭터(Player, Monster, NPC)의 공통 베이스
class CCharacterBase : public CRenderObject
{
protected:
    explicit CCharacterBase(DEVICE graphicDev);
    explicit CCharacterBase(const CCharacterBase& rhs);
    virtual ~CCharacterBase();

public:
    HRESULT Ready_GameObject() override;
    void    Initialize_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;

public:
    // 충돌 처리
    void    OnBeginOverlap(CCollider* self, CCollider* other) override;
    void    OnEndOverlap(CCollider* self, CCollider* other) override;

protected:
    // 자식이 반드시 구현해야 함 캐릭터면 애니메이션이 있기 때문
    virtual void Animation_Setting() PURE;

    HRESULT Add_Component() override;
    virtual void Render_Setting() override;

protected:
    void Free() override;

protected:
#pragma region Engine Components
    Engine::CAnimator*      m_AnimatorCom;
    Engine::CState*         m_StateCom;
    Engine::CCombatStat*    m_CombatStatCom;
    Engine::CBoxCollider*   m_BoxColCom;
#pragma endregion


};

