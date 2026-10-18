#include "pch.h"
#include "CFireball.h"
#include "CProjectile.h"
#include "CCreateHelper.h"
#include "CAttackSphere.h"
#include "CRcTex.h"
#include "CTransform.h"
#include "CTexture.h"
#include "CRenderer.h"
#include "CAttackBuilder.h"
#include "CWall.h"

CFireball::CFireball(DEVICE graphicDev)
    : CAttackSphere(graphicDev),
      m_ProjectileCom(nullptr),
      m_Owner(nullptr)
{ }

CFireball::CFireball(const CFireball& rhs)
    : CAttackSphere(rhs),
      m_ProjectileCom(nullptr),
      m_Owner(nullptr)
{ }

CFireball::~CFireball()
{ }

HRESULT CFireball::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    auto collider = dynamic_cast<CSphereCollider*>(Get_Collider());

    if (collider)
        collider->Set_Radius(0.3f);

    return S_OK;
}

void CFireball::Initialize_GameObject()
{
    CAttackSphere::Initialize_GameObject();
}

_int CFireball::Update_GameObject(const _float& timeDelta)
{
    _int exit = CAttackSphere::Update_GameObject(timeDelta);

    if (m_ProjectileCom)
    {
        m_ProjectileCom->Update_Component(timeDelta);

        CTransform* projectileTransform = m_ProjectileCom->Get_Transform();
        if (projectileTransform && m_TransformCom)
            m_TransformCom->Set_Pos(projectileTransform->Get_Pos());
    }

    return exit;
}

CFireball* CFireball::Create(DEVICE graphicDev)
{
    auto fireball = new CFireball(graphicDev);

    if (FAILED(fireball->Ready_GameObject()))
    {
        Safe_Release(fireball);
        MSG_BOX("Fireball Create Failed");
        return nullptr;
    }

    return fireball;
}

void CFireball::LateUpdate_GameObject(const _float& timeDelta)
{
    CAttackSphere::LateUpdate_GameObject(timeDelta);
}

void CFireball::Render_GameObject()
{
    CAttackSphere::Render_GameObject();

    m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());

    m_GraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_GraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_GraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_GraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    m_GraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    m_GraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
    m_GraphicDev->SetRenderState(D3DRS_ALPHAREF, 0);

    m_TextureCom->Set_Texture(0);
    m_BufferCom->Render_Buffer();

    m_GraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    m_GraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    m_GraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CFireball::OnBeginOverlap(CCollider* self, CCollider* other)
{
    CAttackSphere::OnBeginOverlap(self, other);

    if (other && other->Get_Owner())
    {
        auto wall = dynamic_cast<CWall*>(other->Get_Owner());

        if (wall)
            Mark_As_PendingKill();
    }
}

void CFireball::OnStayOverlap(CCollider* self, CCollider* other)
{
    CAttackSphere::OnStayOverlap(self, other);
}

void CFireball::OnEndOverlap(CCollider* self, CCollider* other)
{
    CAttackSphere::OnEndOverlap(self, other);
}

void CFireball::Render_Editor()
{
    CAttackSphere::Render_Editor();
}

HRESULT CFireball::Add_Component()
{
    if (FAILED(CAttackSphere::Add_Component()))
        return E_FAIL;

    // buffer
    m_BufferCom = CreateProtoComponent<CRcTex>(this, COMPONENTTYPE::RC_TEX);
    NULL_CHECK_RETURN(m_BufferCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::RC_TEX, m_BufferCom });

    // texture
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_SKILL);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_SKILL, m_TextureCom });

    // Projectile
    m_ProjectileCom = CreateProtoComponent<CProjectile>(this, COMPONENTTYPE::PROJECTILE);
    NULL_CHECK_RETURN(m_ProjectileCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::PROJECTILE, m_ProjectileCom });

    return S_OK;
}

void CFireball::Free()
{
    Safe_Release(m_ProjectileCom);
    Safe_Release(m_TransformCom);
    Safe_Release(m_BufferCom);
    Safe_Release(m_TextureCom);

    //  if (m_CurrentAttackObj)
    //  {
    //      m_CurrentAttackObj->Set_OnDestroyCallback(nullptr);
    //      m_CurrentAttackObj = nullptr;
    //  }

    CAttackSphere::Free();
}
