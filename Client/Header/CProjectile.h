#pragma once
#include "CComponent.h"
#include "CGameObject.h"
#include "Client_Enum.h"

struct BulletDesc
{
    CGameObject* ProjectileOwner;

    PROJ_TYPE moveType;
    float Speed;
    //direct
    _vec3 Direction;
    //orbit
    _vec3 OrbitCenter;
    float StartAngle;
    float Radius;
    //homing
    CGameObject* Target;
    float TurnSpeed;
};

class CProjectile :
    public CComponent
{
public:
private:
    explicit CProjectile();
    explicit CProjectile(DEVICE graphicDev);
    explicit CProjectile(const CProjectile& rhs);
    ~CProjectile() override;

public:
    HRESULT Ready_Projectile();
    _int    Update_Component(const _float& timeDelta) override;
    void    LateUpdate_Component() override;
    
    void SetMoveDesc(BulletDesc& d)
    {
        m_ProjectileOwner = d.ProjectileOwner;

        if(m_ProjectileOwner)
        {
            m_ProjectileOwner->AddRef();
        }

        m_MoveType = d.moveType;
        m_Speed = d.Speed;
        m_Direction =d.Direction;
        m_OrbitCenter = d.OrbitCenter;
        m_StartAngle = d.StartAngle;
        m_Radius = d.Radius;
        m_Target = d.Target;
        m_TurnSpeed = d.TurnSpeed;

        isSetDesc = true;
    }

    bool Get_isSetDesc() { return isSetDesc; }
public:
    static CProjectile* Create(DEVICE graphicDev);
    CComponent* Clone() override;

private:
    CTransform* m_BulletTransform;
    bool m_UpdateFlag;

    bool isSetDesc;
private:
    CGameObject* m_ProjectileOwner;

    PROJ_TYPE m_MoveType;
    float m_Speed;

    //direct
    _vec3 m_Direction;

    //orbit
    _vec3 m_OrbitCenter;
    int m_StartAngle;
    float m_Radius;

    //homing
    CGameObject* m_Target;
    float m_TurnSpeed;


    //
    CTransform* m_BossTransform; // 캐싱용
private:
    void Free() override;
};

