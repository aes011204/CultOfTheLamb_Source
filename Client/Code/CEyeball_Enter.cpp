#include "pch.h"
#include "CEyeball_Enter.h"

#include "CTransform.h"

#include "CAnimator.h"
#include "CEyeball.h"
#include "CMonsterController.h"

CEyeball_Enter::CEyeball_Enter(CMonsterBase* owner)
    : m_Owner(owner), m_Pattern_Cooldown(0), m_LerpSpeed(0), m_IsReachedTarget(false)
{
}

CEyeball_Enter::~CEyeball_Enter()
{
}

void CEyeball_Enter::Enter()
{
    CMonsterState::Enter();

    auto eyeball = static_cast<CEyeball*>(m_Owner);
    m_Controller = eyeball->Get_Controller();

    if (m_Controller)
    {
        auto animator = m_Controller->Get_OwnerAnimator();
        if (animator)
            animator->Play_Animation(L"Eyeball_Enter", ANIMSTATE::ONCE);
    }

    CTransform* transform = m_Controller->Get_OwnerTransform();

    if (transform)
    {
        m_StartPos = transform->Get_Pos();  

        m_TargetPos = m_StartPos;
        m_TargetPos.y = 1.f;  
    }

    m_fCurStateElapsedTime = 0.f;
    m_IsReachedTarget = false;  
}

_int CEyeball_Enter::Update_State(const _float& timeDelta)
{
    m_fCurStateElapsedTime += timeDelta;

    CTransform* transform = m_Controller->Get_OwnerTransform();

    if (transform && !m_IsReachedTarget)
    {
        _vec3 currentPos = transform->Get_Pos();

        _vec3 newPos;
        D3DXVec3Lerp(&newPos, &currentPos, &m_TargetPos, m_LerpSpeed);

        transform->Set_Pos(newPos);

        _vec3 dir = m_TargetPos - newPos;
        float distance = D3DXVec3Length(&dir);

        if (distance < 0.1f)
        {
            transform->Set_Pos(m_TargetPos);  
            m_IsReachedTarget = true;
        }
    }

    if (m_IsReachedTarget && m_fCurStateElapsedTime >= m_Pattern_Cooldown)
    {
        return CEyeball::PATTERN::EYEBALL_ATTACK; 
    }

    return CEyeball::PATTERN::EYEBALL_ENTER;
}

void CEyeball_Enter::Exit()
{
    CMonsterState::Exit();

    // 상태 끝날 때
}

HRESULT CEyeball_Enter::Ready_State()
{
    m_fMinStateTime = 0.5f;
    m_Pattern_Cooldown = 1.0f;

    m_LerpSpeed = 0.05f;
    m_IsReachedTarget = false;

    return S_OK;
}

CEyeball_Enter* CEyeball_Enter::Create(CMonsterBase* owner)
{
    auto state = new CEyeball_Enter(owner);

    if (FAILED(state->Ready_State()))
    {
        Safe_Release(state);
        MSG_BOX("Boss Idle Create Failed");
        return nullptr;
    }

    return state;
}

void CEyeball_Enter::Free()
{
    
}
