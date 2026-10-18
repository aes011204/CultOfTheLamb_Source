#pragma once
#include "CBaseState.h"

BEGIN(Engine)
class CTransform;
class CTexture;
class CAnimator;
class CGameObject;
END

class CMonsterController;
class CPlayer;

class CMonsterState : public CBaseState
{
protected:
    explicit CMonsterState();
    virtual ~CMonsterState();

public:
    virtual void Enter() ;
    virtual void Exit() ;

public:
     //virtual HRESULT Ready_State() { return S_OK; }
     //virtual _int    Update_State(const _float& timeDelta) { return 0; }
     //virtual void    LateUpdate_State(const _float& timeDelta) {}
     //virtual void    Render_State() {}

protected:
    virtual void    Find_Player();
protected:
    CPlayer*    m_pPlayer;
    CTransform* m_pPlayerTransformCom;
    CTransform* m_pOwnerTransformCom;
    CAnimator*  m_pOwnerAnimatorCom;


    _float      m_fCurStateElapsedTime; // 누적시간
    _float      m_fMinStateTime;
    _bool       m_bMinStateTimePassed;

    _bool       DoneAnim = false;

    CMonsterController* m_Controller;

private:
    // CBase을(를) 통해 상속됨
    //void Free() override;
};

