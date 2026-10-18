#include "CState.h"
#include "CAnimator.h"
#include "CGameObject.h"
#include "CRcTex.h"
#include "CTransform.h"

CState::CState(DEVICE graphicDev)
    : CComponent(graphicDev),
      m_State(ACTORSTATE::IDLE),
      m_PrevState(ACTORSTATE::STATE_END),
      m_Dir(ACTORDIR::LEFT),
      m_PrevDir(ACTORDIR::DIR_END),
      m_Animator(nullptr)
{ }

CState::CState(const CState& rhs)
    : CComponent(rhs),
      m_State(ACTORSTATE::IDLE),
      m_PrevState(ACTORSTATE::STATE_END),
      m_Dir(ACTORDIR::LEFT),
      m_PrevDir(ACTORDIR::DIR_END),
      m_Animator(nullptr)
{ }

CState::~CState()
{ }

HRESULT CState::Ready_StateComponent(DEVICE graphicDev)
{
    m_State     = ACTORSTATE::IDLE;
    m_PrevState = ACTORSTATE::STATE_END;

    m_Dir     = ACTORDIR::LEFT;
    m_PrevDir = ACTORDIR::DIR_END;

    return S_OK;
}

void CState::Initialize_Component()
{
    CComponent::Initialize_Component();

    // Animator를 안 들고 있으면, Owner에서 한번 탐색
    if (m_Animator == nullptr && m_Owner != nullptr)
    {
        m_Animator = dynamic_cast<CAnimator*>(
            m_Owner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::ANIMATOR));
    }
}

_int CState::Update_Component(const _float& timeDelta)
{
    _int exit = CComponent::Update_Component(timeDelta);

    Update_Animation();

    return exit;
}

void CState::LateUpdate_Component()
{
    CComponent::LateUpdate_Component();
}

void CState::Update_Animation()
{
    wstring str = this->Get_Name();

    // 상태 혹은 방향이 바뀌었을 때만 로직 수행
    if (m_Animator != nullptr && m_TransformCom != nullptr &&
        (m_State != m_PrevState || m_Dir != m_PrevDir))
    {
        // 플레이어, 몬스터 등 ACTORSTATE를 직접 사용하는 경우를 제외하고, 나머지 CCharacter 에 대해 사용
        if (m_UseStateIndependentDir)
        {
            CComponent* bufferCom = m_Owner->Get_Component(ID_STATIC, COMPONENTTYPE::RC_TEX);
            CRcTex*     rcTex     = dynamic_cast<CRcTex*>(bufferCom);

            if (rcTex)
            {
                bool bFlip = false;

                if (m_Dir == ACTORDIR::RIGHT ||
                    m_Dir == ACTORDIR::R_UP ||
                    m_Dir == ACTORDIR::R_DOWN)
                    bFlip = true;

                rcTex->Set_FlipX(bFlip);
            }
        }

        // 플레이어용 State 를 사용할 경우
        auto iter = m_PlayerStateMap.find(m_State);

        if (iter != m_PlayerStateMap.end())
        {
            wstring baseKey = iter->second;

            // 방향 정보 가져오기
            pair<wstring, bool> dirInfo = Get_AnimDirInfo(m_State, m_Dir);

            // 최종 키 생성 (예: "PlayerRun" + "_LEFT" -> "PlayerRun_LEFT")
            // "PLAYER_IDLE" + "" -> "PLAYER_IDLE"
            wstring finalKey = baseKey + dirInfo.first;

            // 애니메이션 상태(LOOP/ONCE) 가져오기
            ANIMSTATE animState = ANIMSTATE::LOOP;
            auto      animIter  = m_AnimStateMap.find(m_State);

            if (animIter != m_AnimStateMap.end())
                animState = animIter->second;

            // 상태가 바뀌었다면 무조건 리셋, 방향만 바뀌었다면 리셋하지 않음(자연스러운 연결)
            bool reset = (m_State != m_PrevState);
            m_Animator->Play_Animation(finalKey, animState, reset);

#pragma region Legacy
            // 스케일 반전 처리
            //_vec3 vScale = m_TransformCom->Get_Scale();

            //// 절대값을 씌워서 기존 스케일 크기는 유지하되 부호만 정리 (기존에 -였을 수도 있으므로)
            //vScale.x = abs(vScale.x);

            //if (dirInfo.second == true) // 반전 필요 (Right 계열)
            //    vScale.x *= -1.f;

            //m_TransformCom->Set_Scale(vScale.x, vScale.y, vScale.z);
#pragma endregion

            CComponent* bufferCom = m_Owner->Get_Component(ID_STATIC, COMPONENTTYPE::RC_TEX);
            CRcTex*     rcTex     = dynamic_cast<CRcTex*>(bufferCom);

            if (rcTex)
                rcTex->Set_FlipX(dirInfo.second);
        }

        m_PrevState = m_State;
        m_PrevDir   = m_Dir;
    }
}

void CState::Change_State(ACTORSTATE state)
{
    if (m_State == state)
        return;

    m_State = state;
}

void CState::Change_Dir(ACTORDIR dir)
{
    if (m_Dir == dir)
        return;

    m_Dir = dir;
}

void CState::Set_AnimInfo(ACTORSTATE playerState, const wstring& key, ANIMSTATE animState)
{
    m_PlayerStateMap[playerState] = key;
    m_AnimStateMap[playerState]   = animState;
}

CState* CState::Create(DEVICE graphicDev)
{
    CState* state = new CState(graphicDev);

    if (FAILED(state->Ready_StateComponent(graphicDev)))
    {
        Safe_Release(state);
        MSG_BOX("StateCom Create Failed");
        return nullptr;
    }

    return state;
}

CComponent* CState::Clone()
{
    return new CState(*this);
}

pair<wstring, bool> CState::Get_AnimDirInfo(ACTORSTATE state, ACTORDIR dir)
{
    wstring suffix;
    bool    bFlip = false;

    switch (state)
    {
    case ACTORSTATE::RUN: // 8방향 (UP, DOWN, LEFT, L_UP, L_DOWN 애니메이션 보유)
        switch (dir)
        {
        case ACTORDIR::LEFT:
            suffix = L"_LEFT";
            break;
        case ACTORDIR::RIGHT:
            suffix = L"_LEFT";
            bFlip = true;
            break; // 반전
        case ACTORDIR::UP:
            suffix = L"_UP";
            break;
        case ACTORDIR::DOWN:
            suffix = L"_DOWN";
            break;
        case ACTORDIR::L_UP:
            suffix = L"_LUP";
            break;
        case ACTORDIR::R_UP:
            suffix = L"_LUP";
            bFlip = true; // 반전
            break;
        case ACTORDIR::L_DOWN:
            suffix = L"_LDOWN";
            break;
        case ACTORDIR::R_DOWN:
            suffix = L"_LDOWN";
            bFlip = true; // 반전
            break;
        default:
            suffix = L"_LDOWN";
            break;
        }
        break;

    case ACTORSTATE::PLAYER_ROLL: // 4방향 (UP, DOWN, LEFT 애니메이션 보유)
        switch (dir)
        {
        case ACTORDIR::UP:
        case ACTORDIR::R_UP:
        case ACTORDIR::L_UP:
            suffix = L"_UP";
            break;
        case ACTORDIR::DOWN:
        case ACTORDIR::R_DOWN:
        case ACTORDIR::L_DOWN:
            suffix = L"_DOWN";
            break;
        case ACTORDIR::RIGHT:
            suffix = L"_LEFT";
            bFlip = true;
            break;
        default:
            suffix = L"_LEFT";
            break;
        }
        break;

    default: // 나머지 (IDLE, ATTACK, HIT, DEAD) -> LEFT만 보유 (2방향)
        // 보통 위/아래를 보고 있어도 IDLE은 옆모습을 보여주거나 함.
        // 만약 위/아래 별도 리소스가 없다면 좌/우 로직만 적용
        switch (dir)
        {
        case ACTORDIR::RIGHT:
        case ACTORDIR::R_UP:
        case ACTORDIR::R_DOWN:
            suffix = L"";
            bFlip = true;
            break;
        default: // LEFT, UP, DOWN 등 나머지는 원본(LEFT) 출력
            suffix = L"";
            bFlip = false;
            break;
        }
        break;
    }

    return { suffix, bFlip };
}

void CState::Free()
{
    CComponent::Free();
}
