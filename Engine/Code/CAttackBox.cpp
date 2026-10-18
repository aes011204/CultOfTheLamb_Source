#include "CAttackBox.h"

#include "CBoxCollider.h"
#include "CCreateHelper.h"
#include "CRenderer.h"
#include "CTransform.h"

CAttackBox::CAttackBox(DEVICE graphicDev)
    : CAttackBase(graphicDev),
      m_BoxColCom(nullptr)
{
}

CAttackBox::CAttackBox(const CAttackBox& rhs)
    : CAttackBase(rhs),
      m_BoxColCom(nullptr)
{
}

CAttackBox::~CAttackBox()
{ }

HRESULT CAttackBox::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    return S_OK;
}

_int CAttackBox::Update_GameObject(const _float& timeDelta)
{
    _int exit = CAttackBase::Update_GameObject(timeDelta);


    return exit;
}

void CAttackBox::LateUpdate_GameObject(const _float& timeDelta)
{
    CAttackBase::LateUpdate_GameObject(timeDelta);
}

void CAttackBox::Render_GameObject()
{
    CAttackBase::Render_GameObject();
}


void CAttackBox::OnBeginOverlap(CCollider* self, CCollider* other)
{
    CAttackBase::OnBeginOverlap(self, other);

    
}

void CAttackBox::OnEndOverlap(CCollider* self, CCollider* other)
{
    CAttackBase::OnEndOverlap(self, other);

    
}

HRESULT CAttackBox::Add_Component()
{
    m_TransformCom = CreateProtoComponent<CTransform>(this, COMPONENTTYPE::TRANSFORM);
    NULL_CHECK_RETURN(m_TransformCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::TRANSFORM, m_TransformCom });

    // BoxCol
    m_BoxColCom = CreateProtoComponent<CBoxCollider>(this, COMPONENTTYPE::BOX_COLL);
    NULL_CHECK_RETURN(m_BoxColCom, E_FAIL);

    // 크기 기본값 지정
    m_BoxColCom->Set_Size(_vec3(3.f, 3.f, 3.f));

    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::BOX_COLL, m_BoxColCom });

    return S_OK;
}

CAttackBox* CAttackBox::Create(DEVICE graphicDev)
{
    CAttackBox* box = new CAttackBox(graphicDev);

    if (FAILED(box->Ready_GameObject()))
    {
        Safe_Release(box);
        MSG_BOX("AttackBox Create Failed");
        return nullptr;
    }

    return box;
}

void CAttackBox::Free()
{
    CAttackBase::Free();


}
