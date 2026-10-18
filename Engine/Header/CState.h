#pragma once
#include "CComponent.h"
#include "Engine_Define.h"

BEGIN(Engine)
class CAnimator;

/// @brief 캐릭터의 현재 상태(State)를 결정하고 관리하는 "감독" 컴포넌트
class ENGINE_DLL CState : public CComponent
{
protected:
    explicit CState(DEVICE graphicDev);
    explicit CState(const CState& rhs);
    ~CState() override;

public:
    HRESULT Ready_StateComponent(DEVICE graphicDev);
    void    Initialize_Component() override;
    _int    Update_Component(const _float& timeDelta) override;
    void    LateUpdate_Component() override;

public:
    void Update_Animation();

    void Change_State(ACTORSTATE state);
    void Change_Dir(ACTORDIR dir);

    ACTORSTATE Get_State() const { return m_State; }
    ACTORDIR   Get_Dir() const { return m_Dir; }

    void Set_StateIndependentDir(_bool bEnable) { m_UseStateIndependentDir = bEnable; }

    void Set_AnimInfo(ACTORSTATE playerState, const wstring& key, ANIMSTATE animState);

    static CState* Create(DEVICE graphicDev);
    CComponent*    Clone() override;

private:
    // 방향에 따른 키 접미사와 반전 여부를 반환하는 헬퍼 함수
    pair<wstring, bool> Get_AnimDirInfo(ACTORSTATE state, ACTORDIR dir);

private:
    void Free() override;

private:
    ACTORSTATE m_State;
    ACTORSTATE m_PrevState;

    ACTORDIR m_Dir;
    ACTORDIR m_PrevDir;

    _bool m_UseStateIndependentDir = false;

    CAnimator* m_Animator;

    map<ACTORSTATE, wstring>   m_PlayerStateMap;
    map<ACTORSTATE, ANIMSTATE> m_AnimStateMap;
};

END
