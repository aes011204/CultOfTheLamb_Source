#include "CAttackSphere.h"
#include "CSphereCollider.h"
#include "CCreateHelper.h"
#include "CRenderer.h"
#include "CTransform.h"

CAttackSphere::CAttackSphere(DEVICE graphicDev)
    : CAttackBase(graphicDev),
    m_SphereColCom(nullptr)
{
}

CAttackSphere::CAttackSphere(const CAttackSphere& rhs)
    : CAttackBase(rhs),
    m_SphereColCom(nullptr)
{
}

CAttackSphere::~CAttackSphere()
{ }

HRESULT CAttackSphere::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    return S_OK;
}

void CAttackSphere::Initialize_GameObject()
{
    CAttackBase::Initialize_GameObject();

    if (m_SphereColCom && m_TransformCom)
    {
        float scale = m_TransformCom->Get_Scale().x;
        m_SphereColCom->Set_Radius(3.5f * scale); // 스케일이 커지면 radius도 커지고, 작으면 작아짐
    }
}

_int CAttackSphere::Update_GameObject(const _float& timeDelta)
{
    _int exit = CAttackBase::Update_GameObject(timeDelta);

    

    return exit;
}

void CAttackSphere::LateUpdate_GameObject(const _float& timeDelta)
{
    CAttackBase::LateUpdate_GameObject(timeDelta);
}

void CAttackSphere::Render_GameObject()
{
    CAttackBase::Render_GameObject();
}


void CAttackSphere::OnBeginOverlap(CCollider* self, CCollider* other)
{
    CAttackBase::OnBeginOverlap(self, other);

    
}

void CAttackSphere::OnEndOverlap(CCollider* self, CCollider* other)
{
    CAttackBase::OnEndOverlap(self, other);

    
}

HRESULT CAttackSphere::Add_Component()
{
    m_TransformCom = CreateProtoComponent<CTransform>(this, COMPONENTTYPE::TRANSFORM);
    NULL_CHECK_RETURN(m_TransformCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::TRANSFORM, m_TransformCom });

    // SphereCol
    m_SphereColCom = CreateProtoComponent<CSphereCollider>(this, COMPONENTTYPE::SPHERE_COLL);
    NULL_CHECK_RETURN(m_SphereColCom, E_FAIL);

    // 크기 기본값 지정
    m_SphereColCom->Set_Radius(3.5f);

    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::SPHERE_COLL, m_SphereColCom });

    return S_OK;
}

CAttackSphere* CAttackSphere::Create(DEVICE graphicDev)
{
    CAttackSphere* sphere = new CAttackSphere(graphicDev);

    if (FAILED(sphere->Ready_GameObject()))
    {
        Safe_Release(sphere);
        MSG_BOX("AttackSphere Create Failed");
        return nullptr;
    }

    return sphere;
}

void CAttackSphere::Free()
{
    CAttackBase::Free();


}
