#include "CSphereCollider.h"

#include "CBoxCollider.h"
#include "CCreateHelper.h"
#include "CRectCollider.h"
#include "CTransform.h"

CSphereCollider::CSphereCollider(DEVICE graphicDev)
    : CCollider(graphicDev, COLLIDERTYPE::SPHERE)
{ }

CSphereCollider::CSphereCollider(const CSphereCollider& rhs)
    : CCollider(rhs), m_Radius(rhs.m_Radius)
{ }

CSphereCollider::~CSphereCollider()
{ }

HRESULT CSphereCollider::Ready_SphereCollider(DEVICE graphicDev)
{
    if (FAILED(CCollider::Ready_Collider()))
        return E_FAIL;

    return S_OK;
}

_int CSphereCollider::Update_Component(const _float& timeDelta)
{
    _int exit = CCollider::Update_Component(timeDelta);

    return exit;
}

void CSphereCollider::LateUpdate_Component()
{
    CCollider::LateUpdate_Component();
}

void CSphereCollider::Render()
{
    if (!m_ShowDebug)
        return;

    CTransform* transform = Get_Transform();
    NULL_CHECK(transform);

    _vec3 center = transform->Get_Pos() + m_Offset;
    _float radius = this->Get_Radius();

    DWORD color = !m_ColliderSet.empty() ? D3DCOLOR_ARGB(255, 255, 0, 0) : D3DCOLOR_ARGB(255, 255, 255, 255);  

    Engine::DrawDebugSphere(m_GraphicDev, center, radius, color);
}

bool CSphereCollider::CheckCollision(CCollider* other)
{
    switch (other->GetColliderType())
    {
    case COLLIDERTYPE::BOX:
        return CheckCollisionSphere2Box(this, static_cast<CBoxCollider*>(other));

    case COLLIDERTYPE::SPHERE:
        return CheckCollisionSphere2Sphere(this, static_cast<CSphereCollider*>(other));

    default:
        break;
    }

    return false;
}

CSphereCollider* CSphereCollider::Create(DEVICE graphicDev)
{
    auto sphere = new CSphereCollider(graphicDev);

    if (FAILED(sphere->Ready_SphereCollider(graphicDev)))
    {
        Safe_Release(sphere);
        MSG_BOX("SphereCollider Create Failed");
        return nullptr;
    }

    return sphere;
}

CComponent* CSphereCollider::Clone()
{
    auto col = new CSphereCollider(*this);

    col->Ready_Collider();

    return col;
}

void CSphereCollider::Free()
{
    CCollider::Free();
}
