#include "pch.h"
#include "CBoss2_Bullet.h"
#include "CProjectile.h"
#include "CTexture.h"
#include "CRcTex.h"
#include "CCreateHelper.h"
#include "CTransform.h"
#include "CRenderer.h"
#include "CSphereCollider.h"
#include "CWall.h"

CBoss2_Bullet::CBoss2_Bullet(DEVICE graphicDev)
    : CAttackSphere(graphicDev),
      m_TextureCom(nullptr),
      m_ProjectileCom(nullptr), m_BufferCom(nullptr),
      m_Owner(nullptr),
      m_Target(nullptr),
      m_ProjectileType(PROJ_TYPE::TYPE_END)
{
}

CBoss2_Bullet::CBoss2_Bullet(DEVICE graphicDev, CGameObject* owner, CGameObject* target, PROJ_TYPE phase)
    : CAttackSphere(graphicDev),
      m_TextureCom(nullptr),
      m_ProjectileCom(nullptr), m_BufferCom(nullptr),
      m_Owner(owner),
      m_Target(target),
      m_ProjectileType(PROJ_TYPE::TYPE_END)
{
}

CBoss2_Bullet::CBoss2_Bullet(const CBoss2_Bullet& rhs)
    : CAttackSphere(rhs),
    m_TextureCom(nullptr),
    m_ProjectileCom(nullptr),
    m_Owner(rhs.m_Owner),
    m_Target(rhs.m_Target),
    m_ProjectileType(PROJ_TYPE::TYPE_END)
{
}

CBoss2_Bullet::~CBoss2_Bullet()
{

}

HRESULT CBoss2_Bullet::Ready_GameObject()
{
    if (FAILED(CAttackSphere::Ready_GameObject()))
        return E_FAIL;

    m_TransformCom->Set_RotationAxis(ROT_X, D3DXToRadian(25.f));

    m_BufferCom->Set_Scale(2.f, 2.f);


    return S_OK;
}

void CBoss2_Bullet::Initialize_GameObject()
{
    CAttackSphere::Initialize_GameObject();

}

_int CBoss2_Bullet::Update_GameObject(const _float& timeDelta)
{
    _int exit = CAttackSphere::Update_GameObject(timeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

    m_SpawnTimer += timeDelta;

    if (!m_StartShoot)
        return exit;

    // 중력 적용
    if (m_UseGravity)
    {
        m_Velocity.y -= m_Gravity * timeDelta;

        _vec3 pos = m_TransformCom->Get_Pos();
        pos += m_Velocity * timeDelta;

        if (pos.y <= m_FloorY)
        {
            pos.y = m_FloorY;
            m_Velocity.y *= -m_BounceFactor;
            m_Velocity.x *= 0.9f; 
            m_Velocity.z *= 0.9f;
            m_BounceCount++;

            if (m_BounceCount >= m_MaxBounce)
            {
                Mark_As_PendingKill();
            }
        }

        m_TransformCom->Set_Pos(pos);
    }

    else if (m_ProjectileCom && m_ProjectileCom->Get_isSetDesc())
    {
        m_ProjectileCom->Update_Component(timeDelta);
    }

    return exit;
}

void CBoss2_Bullet::LateUpdate_GameObject(const _float& timeDelta)
{
    CAttackSphere::LateUpdate_GameObject(timeDelta);


}

void CBoss2_Bullet::Render_GameObject()
{
    CAttackSphere::Render_GameObject();  

    if (!m_TextureCom || !m_BufferCom) return;

    CTransform* transform = dynamic_cast<CTransform*>(
        Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));

    if (!transform) return;

    m_GraphicDev->SetTransform(D3DTS_WORLD, &transform->Get_World());

    m_GraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_GraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_GraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_GraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    m_GraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    m_GraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
    m_GraphicDev->SetRenderState(D3DRS_ALPHAREF, 0);

    {
        m_TextureCom->Set_Texture(2);
        m_BufferCom->Render_Buffer();
    }
    
    m_GraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    m_GraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    m_GraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

    // RenderObject 상속받을걸,,
}

void CBoss2_Bullet::Render_Editor()
{
    CAttackSphere::Render_Editor();


}

CBoss2_Bullet* CBoss2_Bullet::Create(DEVICE graphicDev, CGameObject* owner, CGameObject* target, PROJ_TYPE type)
{
    auto bullet = new CBoss2_Bullet(graphicDev, owner, target, type);

    if (FAILED(bullet->Ready_GameObject()))
    {
        Safe_Release(bullet);
        MSG_BOX("Boss2 Bullet Crate Failed");
        return nullptr;
    }

    return bullet;
}

void CBoss2_Bullet::OnBeginOverlap(CCollider* self, CCollider* other)
{
    CAttackSphere::OnBeginOverlap(self, other);

    if (other && other->Get_Owner())
    {
        auto wall = dynamic_cast<CWall*>(other->Get_Owner());

        if (wall && m_SpawnTimer >= m_Wall_ImmunityTime)
        {
            Mark_As_PendingKill();
        }
    }
}

CBoss2_Bullet* CBoss2_Bullet::Create(DEVICE graphicDev)
{
    auto bullet = new CBoss2_Bullet(graphicDev);

    if (FAILED(bullet->Ready_GameObject()))
    {
        Safe_Release(bullet);
        MSG_BOX("Boss2 Bullet Crate Failed");
        return nullptr;
    }

    return bullet;
}


HRESULT CBoss2_Bullet::Add_Component()
{
    if (FAILED(CAttackSphere::Add_Component()))
        return E_FAIL;

    m_BufferCom = CreateProtoComponent<CRcTex>(this, COMPONENTTYPE::RC_TEX);
    NULL_CHECK_RETURN(m_BufferCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::RC_TEX, m_BufferCom });

    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_BULLET);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_BULLET, m_TextureCom });

    m_ProjectileCom = CreateProtoComponent<CProjectile>(this, COMPONENTTYPE::PROJECTILE);
    NULL_CHECK_RETURN(m_ProjectileCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::PROJECTILE, m_ProjectileCom });

    return S_OK;
}

void CBoss2_Bullet::Free()
{
    CAttackSphere::Free();

}
