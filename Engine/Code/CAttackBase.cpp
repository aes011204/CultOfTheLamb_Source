#include "CAttackBase.h"

#include "CBoxCollider.h"
#include "CCombatStat.h"
#include "CCreateHelper.h"
#include "CParticleHelper.h"
#include "CRenderer.h"
#include "CTimerMgr.h"
#include "CTransform.h"

CAttackBase::CAttackBase(DEVICE graphicDev)
    : CGameObject(graphicDev),
      m_TransformCom(nullptr),
      m_Instigator(nullptr),
      m_LifeTime(0.4f),
      m_DestroyOnHit(false)
{ }

CAttackBase::CAttackBase(const CAttackBase& rhs)
    : CGameObject(rhs),
      m_TransformCom(nullptr),
      m_AttackData(rhs.m_AttackData),
      m_Instigator(rhs.m_Instigator),
      m_LifeTime(rhs.m_LifeTime),
      m_DestroyOnHit(rhs.m_DestroyOnHit)
{ }

CAttackBase::~CAttackBase()
{ }

_int CAttackBase::Update_GameObject(const _float& timeDelta)
{
    CGameObject::Update_GameObject(timeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDERID::RENDER_NONALPHA, this);

    // LifeTime이 끝나면, 펜딩킬 마크해서 삭제 요청
    m_LifeTime -= timeDelta;
    if (m_LifeTime <= 0.f)
    {
        Mark_As_PendingKill();

        return 0;
    }

    return 0;
}

void CAttackBase::LateUpdate_GameObject(const _float& timeDelta)
{
    CGameObject::LateUpdate_GameObject(timeDelta);
}

void CAttackBase::Render_GameObject()
{
    CGameObject::Render_GameObject();

    if (m_TransformCom)
        m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());

    CCollider* collider = Get_Collider();
    if (collider)
        collider->Render();
}

void CAttackBase::OnBeginOverlap(CCollider* self, CCollider* other)
{
    CGameObject::OnBeginOverlap(self, other);
   
   // cout << "AttackBase hit -> PendingKill\n" << endl;

    if (!m_Instigator || !other)
        return;

    CGameObject* hitter = other->Get_Owner();
    if (!hitter || hitter == m_Instigator)
        return;

    // 맞은놈 CombatStat 찾기
    auto combat = dynamic_cast<CCombatStat*>(
        hitter->Get_Component(ID_DYNAMIC, COMPONENTTYPE::COMBATSTAT));

    if (!combat)
        return;

    // 무적처리
    if (combat->Is_Invincible())
        return;

    // 방향 보정
    CTransform* atkTransform = dynamic_cast<CTransform*>(
        m_Instigator->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));

    _vec3 atkPos = atkTransform->Get_Pos();

    CTransform* hitTransform = dynamic_cast<CTransform*>(
        hitter->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));

    NULL_CHECK(atkTransform);
    NULL_CHECK(hitTransform);

    _vec3 hitterPos = hitTransform->Get_Pos();

    _vec3 dir = hitterPos - atkPos;
    if (D3DXVec3LengthSq(&dir) > 0.0001f)
        D3DXVec3Normalize(&dir, &dir);

    // 파싱된 데이터값으로 세팅?
    HITDATA hit(m_AttackData);
    hit.dir      = dir;
    hit.attacker = m_Instigator;
    hit.hitter   = hitter;

    combat->Take_Damage(hit);

    // HitStop
    if (m_AttackData.hitStopDuration > 0.f)
    {
        CTimerMgr::GetInstance()->Start_HitStop(
            m_AttackData.hitStopDuration,
            m_AttackData.hitStopScale
        );
    }

    // 파티클 있는지 확인
    if (m_AttackData.hitParticle.IsValid())
    {
        const auto& particle = m_AttackData.hitParticle;

        // Pos나 Offset에서 랜덤값 줘야 좀 괜찮을듯
        CParticleHelper::Spawn_Particle(
            hitterPos,
            particle.key,
            particle.frames,
            particle.interval,
            particle.scale,
            particle.offset
        );
    }

    // 데미지를 줬으면, 삭제 요청 -> AttackBuilder에서 세팅
    if (m_DestroyOnHit)
        Mark_As_PendingKill();
}

void CAttackBase::OnEndOverlap(CCollider* self, CCollider* other)
{
    CGameObject::OnEndOverlap(self, other);
}

void CAttackBase::Set_Instigator(CGameObject* instigator)
{
    m_Instigator = instigator;

    CCollider* collider = Get_Collider();
    if (collider)
        collider->Set_Instigator(instigator);
}

void CAttackBase::Free()
{
   // cout << "AttackBase Free\n" << endl;
    // 오브젝트가 해제되기 직전, 등록된 콜백이 있다면 실행
    if (m_OnDestroyCallback)
        m_OnDestroyCallback();

    CGameObject::Free();
}
