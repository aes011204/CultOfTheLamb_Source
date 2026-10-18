#pragma once
#include "CAttackBase.h"
#include "CBoxCollider.h"
#include "CCombatStat.h"

BEGIN(Engine)

class ENGINE_DLL CAttackBox : public CAttackBase
{
private:
    explicit CAttackBox(DEVICE graphicDev);
    explicit CAttackBox(const CAttackBox& rhs);
    ~CAttackBox() override;

public:
    HRESULT Ready_GameObject() override;
    _int Update_GameObject(const _float& timeDelta) override;
    void LateUpdate_GameObject(const _float& timeDelta) override;
    void Render_GameObject() override;

    virtual void OnBeginOverlap(CCollider* self, CCollider* other) override;
    virtual void OnEndOverlap(CCollider* self, CCollider* other) override;

public:
    static CAttackBox* Create(DEVICE graphicDev);

protected:
    virtual CCollider* Get_Collider() override { return m_BoxColCom; }

private:
    HRESULT Add_Component();
    void    Free() override;

private:
    CBoxCollider* m_BoxColCom;
};

END
