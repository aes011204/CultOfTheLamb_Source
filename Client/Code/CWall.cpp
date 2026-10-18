#include "pch.h"
#include "CWall.h"
#include "CRenderer.h"
#include "CBoxCollider.h"
#include "CTransform.h"
#include "CCreateHelper.h"
#include "CRigidbody.h"

CWall::CWall(DEVICE graphicDev)
    : CRenderObject(graphicDev),
      m_BoxCollider(nullptr)
{ }

CWall::CWall(const CWall& rhs)
    : CRenderObject(rhs),
      m_BoxCollider(nullptr)
{ }

CWall::~CWall()
{ }

HRESULT CWall::Ready_GameObject()
{
    if (FAILED(CRenderObject::Ready_GameObject()))
        return E_FAIL;

    m_TransformCom->Set_Scale(_vec3(1.f, 1.f, 1.f));

    if (m_BoxCollider)
        m_BoxCollider->Set_CollisionLayer(COLLISION_LAYERTYPE::WALL);

    return S_OK;
}

void CWall::Initialize_GameObject()
{
    CRenderObject::Initialize_GameObject();
}

_int CWall::Update_GameObject(const _float& timeDelta)
{
    _int exit = CRenderObject::Update_GameObject(timeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDERID::RENDER_PRIORITY, this);

    return exit;
}

void CWall::LateUpdate_GameObject(const _float& timeDelta)
{
    CRenderObject::LateUpdate_GameObject(timeDelta);
}

void CWall::Render_GameObject()
{
    if (m_BoxCollider == nullptr)
        return;

    m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());
    m_BoxCollider->Render();
}

void CWall::Render_Editor()
{
    CRenderObject::Render_Editor();
}

void CWall::OnBeginOverlap(CCollider* self, CCollider* other)
{
    CRenderObject::OnBeginOverlap(self, other);
}

void CWall::OnStayOverlap(CCollider* self, CCollider* other)
{
    CRenderObject::OnStayOverlap(self, other);

    CGameObject* hitObject = other->Get_Owner();
    if (!hitObject) return;

    // 속도 제거
    CRigidbody* rigid = dynamic_cast<CRigidbody*>(
        hitObject->Get_Component(ID_DYNAMIC, COMPONENTTYPE::RIGIDBODY));

    if (rigid)
    {
        _vec3 vel = rigid->Get_Velocity();
        vel.x     = 0.f;
        vel.z     = 0.f;
        rigid->Set_Velocity(vel);
    }

    else
        return;

    CTransform* otherTransform = dynamic_cast<CTransform*>(
        hitObject->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));

    CBoxCollider* playerCollider = dynamic_cast<CBoxCollider*>(other);

    if (!otherTransform || !playerCollider) return;

    _vec3 wallPos = m_TransformCom->Get_Pos();
    _vec3 objPos  = otherTransform->Get_Pos();

    // 벽/플레이어 사이즈 합의 절반
    float halfX = (m_BoxCollider->Get_Size().x + playerCollider->Get_Size().x) * 0.5f;
    float halfZ = (m_BoxCollider->Get_Size().z + playerCollider->Get_Size().z) * 0.5f;

    // 중심 거리
    float dx = objPos.x - wallPos.x;
    float dz = objPos.z - wallPos.z;

    float penX = halfX - abs(dx);
    float penZ = halfZ - abs(dz);

    // 덜 침투한 축으로 밀어냄
    if (penX < penZ)
        objPos.x = wallPos.x + (dx > 0 ? halfX : -halfX) + (dx > 0 ? 0.01f : -0.01f);
    else
        objPos.z = wallPos.z + (dz > 0 ? halfZ : -halfZ) + (dz > 0 ? 0.01f : -0.01f);

    otherTransform->Set_Pos(objPos);
}

HRESULT CWall::Add_Component()
{
    if (FAILED(CRenderObject::Add_Component()))
        return E_FAIL;

    m_BoxCollider = CreateProtoComponent<CBoxCollider>(this, COMPONENTTYPE::BOX_COLL);
    NULL_CHECK_RETURN(m_BoxCollider, E_FAIL);

    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::BOX_COLL, m_BoxCollider });

    return S_OK;
}

CWall* CWall::Create(DEVICE graphicDev)
{
    CWall* wall = new CWall(graphicDev);

    if (FAILED(wall->Ready_GameObject()))
    {
        Safe_Release(wall);
        MSG_BOX("Create Failed Wall !");
        return nullptr;
    }

    return wall;
}

void CWall::Free()
{
    CRenderObject::Free();
}
