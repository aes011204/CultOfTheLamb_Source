#include "pch.h"
#include "CEyeball_Idle.h"
#include "CBossController.h"
#include "CAnimator.h"
#include "CEyeball.h"
#include "CRandomMgr.h"
#include "CTransform.h"

CEyeball_Idle::CEyeball_Idle(CMonsterBase* owner)
    : m_Owner(owner),
    m_Pattern_Cooldown(0),
    m_Direction_ChangeTimer(1.5f),
    m_Direction_Timer(0), m_MoveSpeed(1),
    m_LerpSpeed(0)
{
}

CEyeball_Idle::~CEyeball_Idle()
{
}

void CEyeball_Idle::Enter()
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

    // 랜더 방향 설정
    _float randomAngle = static_cast<_float>(CRandomMgr::GetInstance()->Get_Int_Random(0, 360));
    _float radian = D3DXToRadian(randomAngle);
    m_Current_Direction = _vec3(cosf(radian), 0.f, sinf(radian));

    m_Target_Direction = m_Current_Direction;
    m_Direction_Timer = 0.f;

    m_fCurStateElapsedTime = 0.f;
}

_int CEyeball_Idle::Update_State(const _float& timeDelta)
{
    m_fCurStateElapsedTime += timeDelta;
    m_Direction_Timer += timeDelta;

	// 이동
	CTransform* transform = m_Controller->Get_OwnerTransform();

    if (transform)
    {
        // 일정 시간마다 새로운 방향 랜덤
        if (m_Direction_Timer >= m_Direction_ChangeTimer)
        {
            _float randomAngle = static_cast<_float>(CRandomMgr::GetInstance()->Get_Int_Random(0, 360));
            _float radian = D3DXToRadian(randomAngle);
            m_Current_Direction = _vec3(cosf(radian), 0.f, sinf(radian));

            m_Direction_Timer = 0.f;
        }

        m_Current_Direction = _vec3(m_Current_Direction.x + (m_Target_Direction.x - m_Current_Direction.x) * m_LerpSpeed,
            0.f,
            m_Current_Direction.z + (m_Target_Direction.z - m_Current_Direction.z) * m_LerpSpeed);

        D3DXVec3Normalize(&m_Current_Direction, &m_Current_Direction);

        transform->Move_Pos(m_Current_Direction, timeDelta, m_MoveSpeed);
    }

    if (m_fCurStateElapsedTime >= m_Pattern_Cooldown)
    {
        return CEyeball::PATTERN::EYEBALL_EXIT;
    }

    return CEyeball::PATTERN::EYEBALL_IDLE;
}

void CEyeball_Idle::Exit()
{
    CMonsterState::Exit();

    // 상태 끝날 때
}

HRESULT CEyeball_Idle::Ready_State()
{
    m_fMinStateTime = 0.5f;
    m_Pattern_Cooldown = 4.f;

    m_Current_Direction = {0.f, 0.f, 0.f};
    m_Target_Direction = {0.f, 0.f, 0.f};

    m_Direction_ChangeTimer = 1.5f;
    m_Direction_Timer = 0.f;
    m_MoveSpeed = 1;
    m_LerpSpeed = 0.05f;

    return S_OK;
}

CEyeball_Idle* CEyeball_Idle::Create(CMonsterBase* owner)
{
    auto state = new CEyeball_Idle(owner);

    if (FAILED(state->Ready_State()))
    {
        Safe_Release(state);
        MSG_BOX("Boss Start Create Failed");
        return nullptr;
    }

    return state;
}

void CEyeball_Idle::Free()
{
    
}
