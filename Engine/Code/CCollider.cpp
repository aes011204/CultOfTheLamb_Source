#include "CCollider.h"

#include "CBoxCollider.h"
#include "CCollisionManager.h"
#include "CRectCollider.h"
#include "CSphereCollider.h"
#include "CTransform.h"

CCollider::CCollider(DEVICE graphicDev, COLLIDERTYPE colliderType)
    : CComponent(graphicDev), 
      m_ColliderType(colliderType),
      m_ShowDebug(true),
      m_IsColliderActive(true),
      m_CollisionLayer(COLLISION_LAYERTYPE::DEFAULT)

{
}

CCollider::CCollider(const CCollider& rhs)
    : CComponent(rhs),
    m_ColliderType(rhs.m_ColliderType),
    m_ShowDebug(rhs.m_ShowDebug),
    m_IsColliderActive(true),
	m_CollisionLayer(COLLISION_LAYERTYPE::DEFAULT)
{
}

CCollider::~CCollider()
{
    auto collider = CCollisionManager::GetInstance_NoCreate();

    if (collider)
        collider->RemoveCollider(this);

}

HRESULT CCollider::Ready_Collider()
{
    CCollisionManager::GetInstance()->AddCollider(this);

    return S_OK;
}

_int CCollider::Update_Component(const _float& timeDelta)
{
    _int exit = CComponent::Update_Component(timeDelta);

    return exit;
}

void CCollider::LateUpdate_Component()
{
    CComponent::LateUpdate_Component();
}

void CCollider::Render()
{
    if (m_ShowDebug == false)
        return;

}

bool CCollider::CheckCollision(CCollider* other)
{
    // 파생 클래스에서 구현
    return false;
}

bool CCollider::IsOverlapping(CCollider* other)
{
    bool check = m_ColliderSet.find(other) != m_ColliderSet.end();

    return check;
}

void CCollider::AddOverlap(CCollider* other)
{
    m_ColliderSet.insert(other);
}

void CCollider::RemoveOverlap(CCollider* other)
{
    m_ColliderSet.erase(other);
}

bool CCollider::CheckCollisionRect2Rect(CRectCollider* b1, CRectCollider* b2)
{
    if (b1 == nullptr || b2 == nullptr)
        return false;

    CTransform* t1 = b1->Get_Transform();
    CTransform* t2 = b2->Get_Transform();

    if (t1 == nullptr || t2 == nullptr)
        return false;

    _vec3 p1 = t1->Get_Pos() + b1->Get_Offset();
    _vec3 p2 = t2->Get_Pos() + b2->Get_Offset();;

    _vec2 s1 = b1->Get_Size();
    _vec2 s2 = b2->Get_Size();

    _float halfW1 = s1.x * 0.5f;
    _float halfH1 = s1.y * 0.5f;
    _float halfW2 = s2.x * 0.5f;
    _float halfH2 = s2.y * 0.5f;

    _float minX1 = p1.x - halfW1; _float maxX1 = p1.x + halfW1;
    _float minY1 = p1.y - halfH1; _float maxY1 = p1.y + halfH1;

    _float minX2 = p2.x - halfW2; _float maxX2 = p2.x + halfW2;
    _float minY2 = p2.y - halfH2; _float maxY2 = p2.y + halfH2;

    // 한 쪽이 완전히 왼쪽/오른쪽/위/아래에 있으면 충돌 ㄴㄴ
    if (maxX2 < minX1) return false;
    if (maxX1 < minX2) return false;
    if (maxY2 < minY1) return false;
    if (maxY1 < minY2) return false;

    return true;
}

// AABB(Box) vs AABB(Box)
bool CCollider::CheckCollisionBox2Box(CBoxCollider* b1, CBoxCollider* b2)
{
    NULL_CHECK_RETURN(b1, false);
    NULL_CHECK_RETURN(b2, false);

    CTransform* t1 = b1->Get_Transform();
    CTransform* t2 = b2->Get_Transform();

    NULL_CHECK_RETURN(t1, false);
    NULL_CHECK_RETURN(t2, false);

    _vec3 p1 = t1->Get_Pos() + b1->Get_Offset();
    _vec3 p2 = t2->Get_Pos() + b2->Get_Offset();

    _vec3 s1 = b1->Get_Size();
    _vec3 s2 = b2->Get_Size();

    _float halfW1 = s1.x * 0.5f;
    _float halfH1 = s1.y * 0.5f;
    _float halfD1 = s1.z * 0.5f;

    _float halfW2 = s2.x * 0.5f;
    _float halfH2 = s2.y * 0.5f;
    _float halfD2 = s2.z * 0.5f;

    _float minX1 = p1.x - halfW1; _float maxX1 = p1.x + halfW1;
    _float minY1 = p1.y - halfH1; _float maxY1 = p1.y + halfH1;
    _float minZ1 = p1.z - halfD1; _float maxZ1 = p1.z + halfD1;

    _float minX2 = p2.x - halfW2; _float maxX2 = p2.x + halfW2;
    _float minY2 = p2.y - halfH2; _float maxY2 = p2.y + halfH2;
    _float minZ2 = p2.z - halfD2; _float maxZ2 = p2.z + halfD2;

    // X축 분리
    if (maxX2 < minX1) return false;
    if (maxX1 < minX2) return false;

    // Y축 분리
    if (maxY2 < minY1) return false;
    if (maxY1 < minY2) return false;

    // Z축 분리
    if (maxZ2 < minZ1) return false;
    if (maxZ1 < minZ2) return false;

    return true;
}

// Circle(Sphere) vs AABB(Box)
bool CCollider::CheckCollisionSphere2Box(CSphereCollider* s1, CBoxCollider* b2)
{
    if (s1 == nullptr || b2 == nullptr)
        return false;

    CTransform* ts = s1->Get_Transform();
    CTransform* tb = b2->Get_Transform();

    if (ts == nullptr || tb == nullptr)
        return false;

    _vec3 cs3, cb3;
    cs3 = ts->Get_Pos() + s1->Get_Offset();
    cb3 = tb->Get_Pos() + b2->Get_Offset();

    _float radius = s1->Get_Radius();
    _vec3 boxSize = b2->Get_Size();

    _float halfX = boxSize.x * 0.5f;
    _float halfY = boxSize.y * 0.5f;
    _float halfZ = boxSize.z * 0.5f;

    _float minX = cb3.x - halfX;
    _float maxX = cb3.x + halfX;
    _float minY = cb3.y - halfY;
    _float maxY = cb3.y + halfY;
    _float minZ = cb3.z - halfZ;
    _float maxZ = cb3.z + halfZ;

    _float closestX = cs3.x;
    if (closestX < minX)
        closestX = minX;

    else if (closestX > maxX)
        closestX = maxX;

    _float closestY = cs3.y;
    if (closestY < minY)
        closestY = minY;

    else if (closestY > maxY)
        closestY = maxY;

    _float closestZ = cs3.z;
    if (closestZ < minZ)
        closestZ = minZ;

    else if (closestZ > maxZ)
        closestZ = maxZ;

    _float dx = cs3.x - closestX;
    _float dy = cs3.y - closestY;
    _float dz = cs3.z - closestZ;

    _float distSq = dx * dx + dy * dy + dz * dz;

    return distSq <= (radius * radius);
}

bool CCollider::CheckCollisionSphere2Sphere(CSphereCollider* s1, CSphereCollider* s2)
{
    if (s1 == nullptr || s2 == nullptr)
        return false;

    CTransform* t1 = s1->Get_Transform();
    CTransform* t2 = s2->Get_Transform();

    if (t1 == nullptr || t2 == nullptr)
        return false;

    _vec3 p1, p2;
    p1 = t1->Get_Pos() + s1->Get_Offset();
    p2 = t2->Get_Pos() + s2->Get_Offset();

    _float r1 = s1->Get_Radius();
    _float r2 = s2->Get_Radius();

    _float dx = p1.x - p2.x;
    _float dy = p1.y - p2.y;
    _float dz = p1.z - p2.z;

    _float distSq = dx * dx + dy * dy + dz * dz;
    _float sum = r1 + r2;

    return distSq <= (sum * sum);
}

void CCollider::Free()
{
    CComponent::Free();
}
