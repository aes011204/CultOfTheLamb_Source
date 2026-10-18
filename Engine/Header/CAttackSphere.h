#pragma once
#include "CAttackBase.h"
#include "CCombatStat.h"
#include "CSphereCollider.h"

BEGIN(Engine)

class ENGINE_DLL CAttackSphere : public CAttackBase
{
protected:
    explicit CAttackSphere(DEVICE graphicDev);
    explicit CAttackSphere(const CAttackSphere& rhs);
    virtual ~CAttackSphere();

public:
    HRESULT Ready_GameObject() override;
    void Initialize_GameObject() override;
    _int Update_GameObject(const _float& timeDelta) override;
    void LateUpdate_GameObject(const _float& timeDelta) override;
    void Render_GameObject() override;

    virtual void OnBeginOverlap(CCollider* self, CCollider* other) override;
    virtual void OnEndOverlap(CCollider* self, CCollider* other) override;

public:
    static CAttackSphere* Create(DEVICE graphicDev);

protected:
    virtual CCollider* Get_Collider() override { return m_SphereColCom; }

protected:
    virtual HRESULT Add_Component();

protected:
    void Free() override;

protected:
    CSphereCollider* m_SphereColCom;
  

};

END
