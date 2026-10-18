#include "pch.h"
#include "CEyeball_Ground_Move.h"
#include "CBossController.h"
#include "CAnimator.h"
#include "CBossPhase2.h"
#include "CEyeball.h"
#include "CRandomMgr.h"
#include "CTransform.h"
#include "CCollisionManager.h"
#include "CParticleHelper.h"

CEyeball_Ground_Move::CEyeball_Ground_Move(CMonsterBase* owner)
    : m_Owner(owner), m_Pattern_Cooldown(0), m_LerpSpeed(0), m_MaxMoveDistance(0)
{

}

CEyeball_Ground_Move::~CEyeball_Ground_Move()
{

}

void CEyeball_Ground_Move::Enter()
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

    // 플레이어 위치 찾기
    Find_Player();

	CTransform* transform = m_Controller->Get_OwnerTransform();

    if (transform)
    {
        m_StartPos = transform->Get_Pos();

        _float randomAngle = static_cast<_float>(CRandomMgr::GetInstance()->Get_Int_Random(0, 360));
        _float radian = D3DXToRadian(randomAngle);

        m_TargetDir = _vec3(cosf(radian), 0.f, sinf(radian));
    }

    m_fCurStateElapsedTime = 0.f;
    m_Direction_Timer = 0.f;
    m_ParticleTimer = 0.f;

    m_IsUnderground = false;
}

_int CEyeball_Ground_Move::Update_State(const _float& timeDelta)
{
    m_fCurStateElapsedTime += timeDelta;

	CTransform* transform = m_Controller->Get_OwnerTransform();
    auto animator = m_Controller->Get_OwnerAnimator();

    if (!m_IsUnderground && animator && animator->Is_AnimationFinished())
    {
        animator->Play_Animation(L"Eyeball_Idle", ANIMSTATE::LOOP);
        m_IsUnderground = true;
        m_fCurStateElapsedTime = 0.f; // 땅 속에서 이동 시간 리셋
    }

    // 땅에서 이동 중
    if (m_IsUnderground)
    {
        m_ParticleTimer += timeDelta;
        if (m_ParticleTimer >= 0.2f)
        {
            _vec3 particlePos = transform->Get_Pos();
            particlePos.y -= 0.5f;

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

        // 시간이 끝난다면, Enter로 위로 나와서 Attack으로 공격
        if (m_fCurStateElapsedTime >= m_Pattern_Cooldown)
        {
            // 보스가 특정 패턴 중이면 Enter로 이동 ㄴㄴ
            auto eyeball = static_cast<CEyeball*>(m_Owner);
            auto boss = dynamic_cast<CBossPhase2*>(eyeball->Get_Boss());

            if (boss && boss->Is_InSpecialPattern())
            {
                m_fCurStateElapsedTime = 0.f;
                return CEyeball::PATTERN::EYEBALL_GROUND_MOVE;
            }

            return CEyeball::PATTERN::EYEBALL_ENTER;
        }

        // 랜덤 이동
        m_Direction_Timer += timeDelta;
        if (m_Direction_Timer >= 1.f)
        {
            _vec3 currentPos = transform->Get_Pos();
            auto eyeball = static_cast<CEyeball*>(m_Owner);

            for (_int i = 0; i < 5; ++i)
            {
                _float randomAngle = static_cast<_float>(CRandomMgr::GetInstance()->Get_Int_Random(0, 360));
                _float radian = D3DXToRadian(randomAngle);
                _vec3 newDir = _vec3(cosf(radian), 0.f, sinf(radian));

                // 벽 충돌 판정
                if (!eyeball->Trace_HitWall(currentPos, newDir, 5.f))
                {
                    m_TargetDir = newDir;
                    break;
                }
            }
            m_Direction_Timer = 0.f;

        }

        const _float moveSpeed = 4.f;
        transform->Move_Pos(m_TargetDir, timeDelta, moveSpeed);
    }

    return CEyeball::PATTERN::EYEBALL_GROUND_MOVE;
}

void CEyeball_Ground_Move::Exit()
{
    CMonsterState::Exit();

    // 상태 끝날 때
}

HRESULT CEyeball_Ground_Move::Ready_State()
{
    m_fMinStateTime = 0.5f;
    m_Pattern_Cooldown = 3.f + static_cast<_float>(
        CRandomMgr::GetInstance()->Get_Int_Random(0, 200)) / 100.f;

    m_fCurStateElapsedTime = 0.f;

    m_StartPos = _vec3(0.f, 0.f, 0.f);
    m_TargetDir = _vec3(0.f, 0.f, 0.f);
    m_TargetPos = _vec3(0.f, 0.f, 0.f);

    _int random = CRandomMgr::GetInstance()->Get_Int_Random(-4, 8);

    m_MaxMoveDistance = 10.f + static_cast<_float>(random);

    return S_OK;
}

CEyeball_Ground_Move* CEyeball_Ground_Move::Create(CMonsterBase* owner)
{
    auto state = new CEyeball_Ground_Move(owner);

    if (FAILED(state->Ready_State()))
    {
        Safe_Release(state);
        MSG_BOX("Boss Start Create Failed");
        return nullptr;
    }

    return state;
}

void CEyeball_Ground_Move::Free()
{
    
}
