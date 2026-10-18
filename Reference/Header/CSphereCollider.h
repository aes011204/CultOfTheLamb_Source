#pragma once

#include "CCollider.h"

BEGIN(Engine)
class ENGINE_DLL CSphereCollider : public CCollider
{
private:
    explicit CSphereCollider(DEVICE graphicDev);
    explicit CSphereCollider(const CSphereCollider& rhs);
    ~CSphereCollider() override;

public:
    HRESULT Ready_SphereCollider(DEVICE graphicDev);
    _int    Update_Component(const _float& timeDelta) override;
    void    LateUpdate_Component() override;
    void    Render() override;

public:
    bool CheckCollision(CCollider* other) override;

    _float Get_Radius() const { return m_Radius; }
    void   Set_Radius(_float radius) { m_Radius = radius; }

    static CSphereCollider* Create(DEVICE graphicDev);
    CComponent*             Clone() override;

protected:
    void Free() override;

private:
    _float m_Radius = 0.f;
};

END
