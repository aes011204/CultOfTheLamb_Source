#include "pch.h"
#include "CEyeball_Start.h"
#include "CParticleHelper.h"
#include "CBossController.h"
#include "CAnimator.h"
#include "CEyeball.h"
#include "CTransform.h"

CEyeball_Start::CEyeball_Start(CMonsterBase* owner)
    : m_Owner(owner), m_Pattern_Cooldown(0), m_LerpSpeed(0), m_IsReachedTarget(false)
{
}

CEyeball_Start::~CEyeball_Start()
{
}

void CEyeball_Start::Enter()
{
    CMonsterState::Enter();

    auto eyeball = static_cast<CEyeball*>(m_Owner);
    _vec3 moveDir = eyeball->Get_MoveDirection();

    m_Controller = eyeball->Get_Controller();

    if (m_Controller)
    {
        auto animator = m_Controller->Get_OwnerAnimator();
        if (animator)
            animator->Play_Animation(L"Eyeball_Start", ANIMSTATE::LOOP);
    }

	CTransform* transform = m_Controller->Get_OwnerTransform();

    if (transform)
    {
        m_MoveTime = 0.f;

		m_StartPos = transform->Get_Pos();
		m_StartTargetPos = m_StartPos;
        m_StartTargetPos = m_StartPos + moveDir;

        _vec3 particlePos = _vec3(m_StartPos.x, m_StartPos.y, m_StartPos.z + 2.8f);
        CParticleHelper::Spawn_BurstParticle(particlePos, L"HitEffect_Blood", 12);
    }

    m_fCurStateElapsedTime = 0.f;
	m_IsReachedTarget = false;
}

_int CEyeball_Start::Update_State(const _float& timeDelta)
{
	// 이동
	CTransform* transform = m_Controller->Get_OwnerTransform();

    if (transform && !m_IsReachedTarget)
    {
        m_MoveTime += timeDelta;
        _float time = m_MoveTime / m_MoveDuration;

        // Duration 이동 지속시간을 넘어가면
        if (time >= 1.f)
        {
            transform->Set_Pos(m_StartTargetPos);
            m_IsReachedTarget = true;

            auto eyeball = static_cast<CEyeball*>(m_Owner);

            if (eyeball)
                eyeball->Trigger_OnReachedTarget();
        }
        else
        {
            _vec3 pos = Lerp_EaseIn(m_StartPos, m_StartTargetPos, time, 2.f);
            transform->Set_Pos(pos);
        }
    }

    if (m_IsReachedTarget)
    {
        m_fCurStateElapsedTime += timeDelta;
    }

    return CEyeball::PATTERN::EYEBALL_START;
}

void CEyeball_Start::Exit()
{
    CMonsterState::Exit();

    // 상태 끝날 때
}

HRESULT CEyeball_Start::Ready_State()
{
    m_fMinStateTime = 0.5f;
    m_Pattern_Cooldown = 1.0f;

	m_LerpSpeed = 0.08f;
	m_IsReachedTarget = false;

    return S_OK;
}

CEyeball_Start* CEyeball_Start::Create(CMonsterBase* owner)
{
    auto state = new CEyeball_Start(owner);

    if (FAILED(state->Ready_State()))
    {
        Safe_Release(state);
        MSG_BOX("Boss Start Create Failed");
        return nullptr;
    }

    return state;
}

void CEyeball_Start::Free()
{
    
}
