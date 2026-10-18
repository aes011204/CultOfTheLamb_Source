#include "pch.h"
#include "CBossBullet.h"
#include "CCombatStat.h"
#include "CCreateHelper.h"
#include "CRcTex.h"
#include "CRenderer.h"
#include "CTexture.h"
#include "CTransform.h"
#include "CAttackBuilder.h"
#include "CProjectile.h"
#include "CTrailEff.h"



CBossBullet::CBossBullet(DEVICE graphicDev, CGameObject* owner, CGameObject* Target, PROJ_TYPE type)
    : CGameObject(graphicDev),
      m_pOwner(owner),
      m_pTarget(Target),
      m_type(type)
{
    m_pOwner->AddRef();

    m_pTarget->AddRef();
}

CBossBullet::CBossBullet(const CBossBullet& rhs)
    : CGameObject(rhs)
{}

CBossBullet::~CBossBullet()
{}

HRESULT CBossBullet::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    SetTexAspectInit(m_TextureCom, m_TransformCom, 0);

    SetAspectScale(m_TransformCom, { .25f, .25f, .25f });

    const auto ownerTransform =
        dynamic_cast<CTransform*>(m_pOwner->Get_Component(
            ID_DYNAMIC,
            COMPONENTTYPE::TRANSFORM));

    //TODO:: 나중에 땅의 위치를 알게 되면 수정 
    m_TransformCom->Set_Pos({ ownerTransform->Get_Pos().x, /*-ownerTransform->Get_Scale().y * .5f*/2.f, ownerTransform->Get_Pos().z });

    //m_TransformCom->Set_Rotation({ D3DXToRadian(90), 0, 0 });

    // if (m_BoxColCom)
    //     m_BoxColCom->Set_CollisionLayer(COLLISION_LAYERTYPE::BOSSPROJECTILE);

    return S_OK;
}

void CBossBullet::Initialize_GameObject()
{
// 여가 안들어옴ㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁ
    int i = 0;
}

_int CBossBullet::Update_GameObject(const _float& timeDelta)
{
    _int exit = CGameObject::Update_GameObject(timeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);
    CRenderer::GetInstance()->Add_RenderGroup(RENDER_TRAIL, this);

    m_TrailEff->Update_Component(timeDelta);

    // 업데이트 입장시
    if (m_flag == false)
    {
       
        m_TrailEff->Initialize_Component();

        // 생성시의 위치 값 저장
        m_StartPos = m_TransformCom->Get_Pos();

        CAttackBase* ATBox = nullptr;
        CAttackBuilder(m_GraphicDev, this)
            .Sphere()
            .Track(ATBox)
            .Damage(1.f)
            .Knockback(30.f)
            .Scale({ 0.2f, 0.2f, 0.2f })
            .InstigatorPos()
            .Offset(_vec3(0.f, 0.f, 0.f))
            .LifeTime(5.f) // 이건 혹시 충돌 안했을경우를 대비 해서
            .DestroyOnHit(true)
            .CollisionLayer(COLLISION_LAYERTYPE::BOSSPROJECTILE)
            .Spawn();

        auto sphereCol = static_cast<CSphereCollider*>(
            ATBox->Get_Collider());

        if (sphereCol)
            sphereCol->Set_Radius(0.5f);

        m_ATKBoxTransformCom = dynamic_cast<CTransform*>(
            ATBox->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));
        //dynamic_cast<CAttackBase*>(ATBox->Get_Component(ID_DYNAMIC, COMPONENTTYPE::CIRCLE_COLL));
        //ATBox
        //ATBox->Set_OnDestroyCallback([this,m_ATKBoxTransformCom]() {
        //    if (m_ATKBoxTransformCom) != nullptr)
        //    m_ATKBoxTransformCom = nullptr;
        //    this->Mark_As_PendingKill();});

        ATBox->Set_OnDestroyCallback([bullet = this]() { bullet->Mark_As_PendingKill(); });

        m_flag = true;
    }

    if (m_StartShoot && m_ProjectileCom->Get_isSetDesc() == true)
    {
        switch (m_type)
        {
        case PROJ_TYPE::LINEAR:

            m_ProjectileCom->Update_Component(timeDelta);
            m_ATKBoxTransformCom->Set_Pos(m_TransformCom->Get_Pos());

            break;
        case PROJ_TYPE::ORBIT:

            m_ProjectileCom->Update_Component(timeDelta);
            m_ATKBoxTransformCom->Set_Pos(m_TransformCom->Get_Pos());

            break;
        case PROJ_TYPE::HOMING:
            break;
        case PROJ_TYPE::TYPE_END:
            break;
        default:
            break;
        }
    }

    return exit;
}

void CBossBullet::Render_Trail()
{
    m_TrailEff->Render_Effect(m_TextureCom, 0); // 이거 맞나 모르겠네ㅔㅔ
}

void CBossBullet::LateUpdate_GameObject(const _float& timeDelta)
{
    CGameObject::LateUpdate_GameObject(timeDelta);
}

void CBossBullet::Render_GameObject()
{
    Render_Core();

}

void CBossBullet::Render_Core()
{
    


    //
    m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());

    m_GraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_GraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_GraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_GraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    m_GraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    m_GraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
    m_GraphicDev->SetRenderState(D3DRS_ALPHAREF, 0);
    //

    m_TextureCom->Set_Texture(2);


    m_BufferCom->Render_Buffer();


    m_GraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    m_GraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    m_GraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

    m_GraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    m_GraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

    ///////////////////////////////////////////////
    m_GraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS); // 알파 함수 기본값
    m_GraphicDev->SetRenderState(D3DRS_ALPHAREF, 0); // 알파 레퍼런스 기본값

    // D3D 기본 FVF 파이프라인 상태 복구
    m_GraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

    //m_GraphicDev->SetRenderState(
    //    D3DRS_TEXTUREFACTOR,
    //    D3DCOLOR_ARGB(255, 255, 255, 255)); // 알파 1.0, 색 그대로
}


void CBossBullet::OnBeginOverlap(CCollider* self, CCollider* other)
{
    CGameObject::OnBeginOverlap(self, other);


    //ATBox->Mark_As_PendingKill();

    //Mark_As_PendingKill();
}

void CBossBullet::OnEndOverlap(CCollider* self, CCollider* other)
{
    CGameObject::OnEndOverlap(self, other);
}

void CBossBullet::Animation_Setting()
{ }

HRESULT CBossBullet::Add_Component()
{
    // buffer
    m_BufferCom = CreateProtoComponent<CRcTex>(this, COMPONENTTYPE::RC_TEX);
    NULL_CHECK_RETURN(m_BufferCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::RC_TEX, m_BufferCom });

    // transform
    m_TransformCom = CreateProtoComponent<CTransform>(this, COMPONENTTYPE::TRANSFORM);
    NULL_CHECK_RETURN(m_TransformCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::TRANSFORM, m_TransformCom });

    // texture
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_BULLET);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_BULLET, m_TextureCom });

    // CombatStat
    m_CombatStatCom = CreateProtoComponent<CCombatStat>(this, COMPONENTTYPE::COMBATSTAT);
    NULL_CHECK_RETURN(m_CombatStatCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::COMBATSTAT, m_CombatStatCom });

    // Projectile
    m_ProjectileCom = CreateProtoComponent<CProjectile>(this, COMPONENTTYPE::PROJECTILE);
    NULL_CHECK_RETURN(m_ProjectileCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::PROJECTILE, m_ProjectileCom });

    // Eff
    m_TrailEff = CreateProtoComponent<CTrailEff>(this, COMPONENTTYPE::TRAILEFFECT);
    NULL_CHECK_RETURN(m_TrailEff, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TRAILEFFECT, m_TrailEff });

    

    // // BoxCollider
    // m_BoxColCom = CreateProtoComponent<CBoxCollider>(this, COMPONENTTYPE::BOX_COLL);
    // NULL_CHECK_RETURN(m_BoxColCom, E_FAIL);
    // m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::BOX_COLL, m_BoxColCom });

    return S_OK;
}

CBossBullet* CBossBullet::Create(DEVICE graphicDev, CGameObject* owner, CGameObject* Target, PROJ_TYPE type)
{
    auto BossBullet = new CBossBullet(graphicDev, owner, Target, type);

    if (FAILED(BossBullet->Ready_GameObject()))
    {
        MSG_BOX("BossBullet Create Failed");
        Safe_Release(BossBullet);
        return nullptr;
    }

    return BossBullet;
}

void CBossBullet::Free()
{
    //Safe_Release(m_TrailEff);

    Safe_Release(m_pOwner);
    Safe_Release(m_pTarget);

    CGameObject::Free();
}
