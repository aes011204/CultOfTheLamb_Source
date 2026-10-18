#include "pch.h"
#include "CEyeball_StartGround.h"
#include "CBossController.h"
#include "CAnimator.h"
#include "CEyeball.h"
#include "CRandomMgr.h"
#include "CTransform.h"
#include "CParticleHelper.h"

CEyeball_StartGround::CEyeball_StartGround(CMonsterBase* owner)
    : m_Owner(owner), m_Pattern_Cooldown(0), m_LerpSpeed(0), m_MaxMoveDistance(0)
{
}

CEyeball_StartGround::~CEyeball_StartGround()
{
}

void CEyeball_StartGround::Enter()
{
    CMonsterState::Enter();

    auto eyeball = static_cast<CEyeball*>(m_Owner);
    m_Controller = eyeball->Get_Controller();

    if (m_Controller)
    {
        auto animator = m_Controller->Get_OwnerAnimator();
        if (animator)
            animator->Play_Animation(L"Eyeball_Start", ANIMSTATE::LOOP);
    }

    // 플레이어 위치 찾기
    Find_Player();

	CTransform* transform = m_Controller->Get_OwnerTransform();
    if (transform)
    {
        m_StartPos = transform->Get_Pos();

        _vec3 originalDir = eyeball->Get_MoveDirection();

        m_TargetDir = _vec3(-originalDir.x * 1.4f, -0.5f, originalDir.z);
        D3DXVec3Normalize(&m_TargetDir, &m_TargetDir);

        m_TargetPos = m_StartPos + m_TargetDir * m_MaxMoveDistance;
    }

    m_fCurStateElapsedTime = 0.f;
    m_ParticleTimer = 0.f;
}

_int CEyeball_StartGround::Update_State(const _float& timeDelta)
{
    m_fCurStateElapsedTime += timeDelta;

	// 이동
	CTransform* transform = m_Controller->Get_OwnerTransform();

    if (m_fCurStateElapsedTime >= m_Pattern_Cooldown)
    {
        return CEyeball::PATTERN::EYEBALL_ENTER;
    }

    if (transform && m_pPlayerTransformCom)
    {
        m_ParticleTimer += timeDelta;
        if (m_ParticleTimer >= 0.2f)
        {
            _vec3 particlePos = transform->Get_Pos();
            particlePos.y += 1.7f;

            CParticleHelper::Spawn_BurstParticle(
                particlePos,
                L"HitEffect_Blood",
                12,
                6,
                3.f,
                3.f,
                15.f,
                2.f,
                0.05f);

            m_ParticleTimer = 0.f;
        }

        _vec3 playerPos = m_pPlayerTransformCom->Get_Pos();
        _vec3 eyeballPos = transform->Get_Pos();

        _vec3 moveOffset = eyeballPos - m_StartPos;
        float moveDistance = D3DXVec3Length(&moveOffset);

        if (moveDistance >= m_MaxMoveDistance)
        {
            return CEyeball::PATTERN::EYEBALL_START_GROUND;
        }

        const _float moveSpeed = 8.f;
        transform->Move_Pos(m_TargetDir, timeDelta, moveSpeed);
    }

    return CEyeball::PATTERN::EYEBALL_START_GROUND;
}

void CEyeball_StartGround::Exit()
{
    CMonsterState::Exit();

    // 상태 끝날 때
}

HRESULT CEyeball_StartGround::Ready_State()
{
    m_fMinStateTime = 0.5f;
    m_Pattern_Cooldown = 2.5f;
    m_fCurStateElapsedTime = 0.f;

    m_StartPos = _vec3(0.f, -0.5f, 0.f);
    m_TargetDir = _vec3(0.f, 0.f, 0.f);
    m_TargetPos = _vec3(0.f, 0.f, 0.f);

    _int random = CRandomMgr::GetInstance()->Get_Int_Random(-2, 2);

    m_MaxMoveDistance = 10.f + static_cast<_float>(random);

    return S_OK;
}

CEyeball_StartGround* CEyeball_StartGround::Create(CMonsterBase* owner)
{
    auto state = new CEyeball_StartGround(owner);

    if (FAILED(state->Ready_State()))
    {
        Safe_Release(state);
        MSG_BOX("Boss Start Create Failed");
        return nullptr;
    }

    return state;
}

void CEyeball_StartGround::Free()
{
    
}
