#include "pch.h"
#include "CEyeball_StartExit.h"
#include "CBossController.h"
#include "CAnimator.h"
#include "CEyeball.h"
#include "CTransform.h"
#include "CParticleHelper.h"

CEyeball_StartExit::CEyeball_StartExit(CMonsterBase* owner)
    : m_Owner(owner), m_Pattern_Cooldown(0), m_LerpSpeed(0), m_IsReachedTarget(false), m_IsAnimFinished(false),
      m_IsFinalPosSet(false)
{
}

CEyeball_StartExit::~CEyeball_StartExit()
{
}

void CEyeball_StartExit::Enter()
{
    CMonsterState::Enter();

    auto eyeball = static_cast<CEyeball*>(m_Owner);

    m_Controller = eyeball->Get_Controller();

    if (m_Controller)
    {
        auto animator = m_Controller->Get_OwnerAnimator();
        if (animator)
            animator->Play_Animation(L"Eyeball_Exit", ANIMSTATE::ONCE);
    }

	CTransform* transform = m_Controller->Get_OwnerTransform();

    if (transform)
    {
		m_StartPos = transform->Get_Pos();

		m_StartTargetPos = m_StartPos;
		m_StartTargetPos.y = FLOOR_POS1.y;
    }

    m_fCurStateElapsedTime = 0.f;

	m_DescendStarted = false;
	m_DescendTime = 0.f;

    m_IsReachedTarget = false;
    m_IsAnimFinished = false;
    m_IsFinalPosSet = false;
}

_int CEyeball_StartExit::Update_State(const _float& timeDelta)
{
    m_fCurStateElapsedTime += timeDelta;

	CTransform* transform = m_Controller->Get_OwnerTransform();
	CAnimator* animator = m_Controller->Get_OwnerAnimator();

	if (!transform || !animator)
		return CEyeball::PATTERN::EYEBALL_START_EXIT;

	_int curFrame = animator->Get_CurFrame();

	// 13프레임쯤 하강 시작
	if (!m_DescendStarted && curFrame >= 13)
	{
		m_DescendStarted = true;
		m_DescendStartPos = transform->Get_Pos();
		m_DescendTime = 0.f;
	}

	// 하강 중이고, 아직 목표지점에 도달하지 않았을 때
	if (m_DescendStarted && !m_IsReachedTarget)
	{
		m_DescendTime += timeDelta;

		_float time = m_DescendTime / m_DescendDuration;

		if (time >= 1.f)
		{
			transform->Set_Pos(m_StartTargetPos);
			m_IsReachedTarget = true;

            // Particle Spawn
		}
        else
        {
			_vec3 pos = Lerp_EaseIn(m_DescendStartPos, m_StartTargetPos, time, 3.f);

			transform->Set_Pos(pos);
        }
	}


	// 목표 도달 후 애니메이션 끝나면
	if (m_IsReachedTarget && !m_IsFinalPosSet)
	{
	    if (animator->Is_AnimationFinished())
	    {
			_vec3 finalPos = _vec3(transform->Get_Pos().x, FLOOR_POS2.y, transform->Get_Pos().z);
			transform->Set_Pos(finalPos);

			m_IsFinalPosSet = true;
	    }
	}

	if (m_IsFinalPosSet && m_fCurStateElapsedTime >= m_Pattern_Cooldown)
	{
		return CEyeball::PATTERN::EYEBALL_START_GROUND;
	}


    return CEyeball::PATTERN::EYEBALL_START_EXIT;  
}

void CEyeball_StartExit::Exit()
{
    CMonsterState::Exit();

    // 상태 끝날 때
}

HRESULT CEyeball_StartExit::Ready_State()
{
    m_fMinStateTime = 0.5f;
    m_Pattern_Cooldown = 1.0f;

	m_LerpSpeed = 0.05f;
    m_IsReachedTarget = false;
    m_IsAnimFinished = false;
    m_IsFinalPosSet = false;        


    return S_OK;
}

CEyeball_StartExit* CEyeball_StartExit::Create(CMonsterBase* owner)
{
    auto state = new CEyeball_StartExit(owner);

    if (FAILED(state->Ready_State()))
    {
        Safe_Release(state);
        MSG_BOX("Boss Start Create Failed");
        return nullptr;
    }

    return state;
}

void CEyeball_StartExit::Free()
{
    
}
