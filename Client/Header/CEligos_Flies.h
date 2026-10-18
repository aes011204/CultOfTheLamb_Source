#pragma once
#include "CMonsterBase.h"

class CProjectile;

class CEligos_Flies :
    public CMonsterBase
{
private:
    explicit CEligos_Flies(DEVICE graphicDev, CGameObject* owner, CGameObject* Target);
    explicit CEligos_Flies(const CEligos_Flies& rhs);
    ~CEligos_Flies() override;

public:
    HRESULT Ready_GameObject() override;
    void    Initialize_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;

public:


    void    Update_Direction();

    void Animation_Setting();
public:
    void    OnBeginOverlap(CCollider* self, CCollider* other) override;
    void    OnEndOverlap(CCollider* self, CCollider* other) override;

    //void    Chase_Target(const float& timeDelta, _float speed);

private:

    HRESULT  Add_Component() override;

    CProjectile* m_ProjectileCom;
    bool m_flag = false;
public:
    static CEligos_Flies* Create(DEVICE graphicDev, CGameObject* owner, CGameObject* Target);

protected:
    void Free() override;

private:

    _bool           m_AttackSpawned;

    CGameObject* m_pOwner;
    CGameObject* m_pTarget;
    
};

