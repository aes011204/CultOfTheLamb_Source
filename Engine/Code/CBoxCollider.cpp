#include "CBoxCollider.h"

#include "CBoxCol.h"
#include "CBoxTex.h"
#include "CCreateHelper.h"
#include "CRenderer.h"
#include "CSphereCollider.h"
#include "CTransform.h"

CBoxCollider::CBoxCollider(DEVICE graphicDev)
    : CCollider(graphicDev, COLLIDERTYPE::BOX)
      ,  m_Size(1.f, 1.f, 1.f)
{


}

CBoxCollider::CBoxCollider(const CBoxCollider& rhs)
    : CCollider(rhs),  m_Size(rhs.m_Size)
{
 

}

CBoxCollider::~CBoxCollider()
{ }

HRESULT CBoxCollider::Ready_BoxCollider(DEVICE graphicDev)
{
    if (FAILED(CCollider::Ready_Collider()))
        return E_FAIL;

    return S_OK;
}

_int CBoxCollider::Update_Component(const _float& timeDelta)
{
    _int exit = CCollider::Update_Component(timeDelta);

    return exit;
}

void CBoxCollider::LateUpdate_Component()
{
    CCollider::LateUpdate_Component();
}

void CBoxCollider::Render()
{
    if (m_ShowDebug == false)
        return;

    CTransform* transform = Get_Transform();
    if (transform == nullptr)
        return;

    _vec3 center = transform->Get_Pos() + m_Offset;
    _vec3 size = Get_Size();

    DWORD color = !m_ColliderSet.empty() ? D3DCOLOR_ARGB(255, 255, 0, 0) : D3DCOLOR_ARGB(255, 255, 255, 255);
    Engine::DrawDebugBox(m_GraphicDev, center, size, color);
}

bool CBoxCollider::CheckCollision(CCollider* other)
{
    if (other == nullptr)
        return false;

    switch (other->GetColliderType())
    {
    case COLLIDERTYPE::BOX:
        return CheckCollisionBox2Box(this, static_cast<CBoxCollider*>(other));

    case COLLIDERTYPE::SPHERE:
        return CheckCollisionSphere2Box(static_cast<CSphereCollider*>(other), this);
    }

    return false;
}

_vec3 CBoxCollider::Get_Size()
{
    const CTransform* transform = Get_Transform();

    _vec3 size = m_Size;

    if (transform)
    {
        _vec3 scale = transform->Get_Scale();

        size.x *= abs(scale.x);
        size.y *= abs(scale.y);
        size.z *= abs(scale.z);
    }

    return size;
}

CBoxCollider* CBoxCollider::Create(DEVICE GraphicDev)
{
    auto box = new CBoxCollider(GraphicDev);

    if (FAILED(box->Ready_BoxCollider(GraphicDev)))
    {
        Safe_Release(box);
        MSG_BOX("BoxCollider Create Failed");
        return nullptr;
    }

    return box;
}

CComponent* CBoxCollider::Clone()
{
    auto col = new CBoxCollider(*this);

    col->Ready_Collider();

    return col;
}

void CBoxCollider::Free()
{
    CCollider::Free();

}
