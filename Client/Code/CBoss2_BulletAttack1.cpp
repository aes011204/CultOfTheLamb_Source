#include "pch.h"
#include "CBoss2_BulletAttack1.h"
#include "CSpriteAnimation.h"
#include "CBossController.h"
#include "CBossPhase2.h"
#include "CAnimator.h"
#include "CWarningChain.h"
#include "CTransform.h"
#include "CCreateHelper.h"
#include "CTimerMgr.h"
#include "Cboss2_Bullet.h"
#include "CAttackBuilder.h"
#include "CCameraController.h"
#include "CCameraMovement.h"
#include "CMainEditorMgr.h"
#include "CProjectile.h"

CBoss2_BulletAttack1::CBoss2_BulletAttack1(CGameObject* owner)
    : m_Owner(owner), m_Pattern_Cooldown(0)
{
}

CBoss2_BulletAttack1::~CBoss2_BulletAttack1()
{
}

void CBoss2_BulletAttack1::Enter()
{
    CMonsterState::Enter();

    auto boss = static_cast<CBossPhase2*>(m_Owner);
    m_Controller = boss->Get_Controller();

    // 패턴 시작 시, 모든 Eyeball을 GroundMove로 변경
    boss->Command_Ground();

    if (m_Controller)
    {
        auto animator = m_Controller->Get_OwnerAnimator();
        if (animator)
            animator->Play_Animation(L"Boss2_Roar", ANIMSTATE::ONCE);
    }

    m_fCurStateElapsedTime = 0.f;
    m_SlamCount = 0;
    m_HasFiredBullet = false;
    m_IsFinished = false;

    Find_Player();
    m_ChainAnim = ROAR;
}

_int CBoss2_BulletAttack1::Update_State(const _float& timeDelta)
{
    m_fCurStateElapsedTime += timeDelta;

    if (m_IsFinished)
    {
        return CBossPhase2::PATTERN::PHASE2_IDLE;
    }

    auto animator = m_Controller->Get_OwnerAnimator();

    switch (m_ChainAnim)
    {
    case ROAR:
        Play_Roar();
        break;
    case HANDSLMA:
        Play_HandSlam();
        break;
    }

    return CBossPhase2::PATTERN::PHASE2_BULLET1;
}

void CBoss2_BulletAttack1::Exit()
{
    CMonsterState::Exit();

    // 상태 끝날 때
}

HRESULT CBoss2_BulletAttack1::Ready_State()
{
    m_fMinStateTime = 0.5f;
    m_Pattern_Cooldown = 5.f;

    m_ChainAnim = ROAR;
    m_SlamCount = 0;
    m_HasFiredBullet = false;

    return S_OK;
}

void CBoss2_BulletAttack1::Play_Roar()
{
    auto animator = m_Controller->Get_OwnerAnimator();
    if (animator && animator->Is_AnimationFinished())
    {
        m_ChainAnim = HANDSLMA;
        animator->Play_Animation(L"Boss2_HandSlam", ANIMSTATE::LOOP);
        m_HasFiredBullet = false; 
    }

}

void CBoss2_BulletAttack1::Play_HandSlam()
{
    auto animator = m_Controller->Get_OwnerAnimator();
    if (!animator) return;

    _int curFrame = animator->Get_CurFrame();

    if (curFrame >= 30 && !m_HasFiredBullet)
    {
        auto camController = CMainEditorMgr::GetInstance()->Get_MainCameraControl();
        if (camController)
        {
            auto camMovement = camController->Get_OwnerCamMovement();
            if (camMovement)
                camMovement->Start_CamShake(0.42f, 0.7f, 55.7f, 10.5f);
        }

        m_HasFiredBullet = true;

        auto boss = static_cast<CBossPhase2*>(m_Owner);
        DEVICE device = boss->Get_GraphicDev();

        if (m_SlamCount == 0)
        {
            SpawnBullets_Horizontal(device);
        }
        else if (m_SlamCount == 1)
        {
            SpawnBullets_Vertical_BottomToTop(device);
        }
        else if (m_SlamCount == 2)
        {
            SpawnBullets_Horizontal(device);
        }
        else if (m_SlamCount == 3)
        {
            SpawnBullets_Vertical_TopToBottom(device);
        }
    }

    if (m_HasFiredBullet && curFrame < 10)
    {
        m_SlamCount++;
        m_HasFiredBullet = false;

        if (m_SlamCount >= 4)
            m_IsFinished = true;
    }
}

void CBoss2_BulletAttack1::SpawnBullets_Horizontal(DEVICE graphicDev)
{
    const _int bulletCount = 14;
    const _float spacing = 2.f; // 총알간 간격
    const _float speed = 15.f;
    const _float offsetX = 12.f;

    _vec3 bossPos = m_Controller->Get_OwnerTransform()->Get_Pos();
    _vec3 centerPos = bossPos;
    centerPos.z -= 10.f; 

    for (_int i = 0; i < bulletCount; i++)
    {
        // Center 기준으로 생성
        _float zPos = centerPos.z - spacing * (bulletCount / 2) + i * spacing;

        // 왼쪽에서 오른쪽으로
        _vec3 leftPos = _vec3(centerPos.x - offsetX, 1.f, zPos);
        _vec3 rightDir = _vec3(1.f, 0.f, 0.f);
        Spawn_SingleBullet(graphicDev, leftPos, rightDir, speed);

        // 오른쪽에서 왼쪽으로
        _vec3 rightPos = _vec3(centerPos.x + offsetX, 1.f, zPos);
        _vec3 leftDir = _vec3(-1.f, 0.f, 0.f);
        Spawn_SingleBullet(graphicDev, rightPos, leftDir, speed);
    }
}

void CBoss2_BulletAttack1::SpawnBullets_Vertical(DEVICE graphicDev)
{
    const _int bulletCount = 14;
    const _float spacing = 2.f;
    const _float speed = 15.f;
    const _float offsetZ = 12.f;

    _vec3 bossPos = m_Controller->Get_OwnerTransform()->Get_Pos();
    _vec3 centerPos = bossPos;
    centerPos.z -= 15.f;

    for (_int i = 0; i < bulletCount; ++i)
    {
        _float xPos = centerPos.x - spacing * (bulletCount / 2) + i * spacing;

        // 위에서 아래로 
        _vec3 topPos = _vec3(xPos, 1.f, centerPos.z - offsetZ);
        _vec3 downDir = _vec3(0.f, 0.f, 1.f);

        Spawn_SingleBullet(graphicDev, topPos, downDir, speed);

        // 아래에서 위로
        _vec3 bottomPos = _vec3(xPos, 1.f, centerPos.z + offsetZ);
        _vec3 upDir = _vec3(0.f, 0.f, -1.f);

        Spawn_SingleBullet(graphicDev, bottomPos, upDir, speed);
    }
}

void CBoss2_BulletAttack1::SpawnBullets_Vertical_BottomToTop(DEVICE graphicDev)
{
    const _int bulletCount = 14;
    const _float spacing = 2.f;
    const _float speed = 15.f;
    const _float offsetZ = 12.f;

    _vec3 bossPos = m_Controller->Get_OwnerTransform()->Get_Pos();
    _vec3 centerPos = bossPos;

    centerPos.z -= 15.f;

    for (_int i = 0; i < bulletCount; ++i)
    {
        _float xPos = centerPos.x - spacing * (bulletCount / 2) + i * spacing;

        // 아래에서 위로만
        _vec3 bottomPos = _vec3(xPos, 1.f, centerPos.z + offsetZ);
        _vec3 upDir = _vec3(0.f, 0.f, -1.f);

        Spawn_SingleBullet(graphicDev, bottomPos, upDir, speed);
    }
}

void CBoss2_BulletAttack1::SpawnBullets_Vertical_TopToBottom(DEVICE graphicDev)
{
    const _int bulletCount = 14;
    const _float spacing = 2.f;
    const _float speed = 15.f;
    const _float offsetZ = 12.f;

    _vec3 bossPos = m_Controller->Get_OwnerTransform()->Get_Pos();
    _vec3 centerPos = bossPos;

    centerPos.z -= 15.f;

    for (_int i = 0; i < bulletCount; ++i)
    {
        _float xPos = centerPos.x - spacing * (bulletCount / 2) + i * spacing;

        // 위에서 아래로만
        _vec3 topPos = _vec3(xPos, 1.f, centerPos.z - offsetZ);
        _vec3 downDir = _vec3(0.f, 0.f, 1.f);
        Spawn_SingleBullet(graphicDev, topPos, downDir, speed);
    }
}

void CBoss2_BulletAttack1::Spawn_SingleBullet(DEVICE graphicDev, const _vec3& pos, const _vec3 dir, _float speed)
{
    CBoss2_Bullet* bullet = nullptr;

    CAttackBuilder(graphicDev, m_Owner)
        .SetType<CBoss2_Bullet>()
        .Track(bullet)
        .Position(pos)
        .Damage(1.f)
        .Knockback(10.f)
        .Scale(0.3f)
        .LifeTime(5.f)
        .CollisionLayer(COLLISION_LAYERTYPE::BOSSPROJECTILE)
        .DestroyOnHit(false)
        .Particle(L"HitEffect_Star", 14, 0.02f)
        .Spawn();

    if (bullet)
    {
        CProjectile* projectileCom = dynamic_cast<CProjectile*>(
            bullet->Get_Component(ID_STATIC, COMPONENTTYPE::PROJECTILE));

        if (projectileCom)
        {
            BulletDesc desc;
            desc.ProjectileOwner = m_Owner;
            desc.moveType = PROJ_TYPE::LINEAR;
            desc.Speed = speed;
            desc.Direction = dir;
            projectileCom->SetMoveDesc(desc);
        }

        bullet->Start_Shoot();
        bullet->Initialize_GameObject();
    }

}

CBoss2_BulletAttack1* CBoss2_BulletAttack1::Create(CGameObject* owner)
{
    auto state = new CBoss2_BulletAttack1(owner);

    if (FAILED(state->Ready_State()))
    {
        Safe_Release(state);
        MSG_BOX("Boss Eyeball Create Failed");
        return nullptr;
    }

    return state;
}

void CBoss2_BulletAttack1::Free()
{
    
}
