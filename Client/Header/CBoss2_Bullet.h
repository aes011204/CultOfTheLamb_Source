#pragma once

#include "Client_Enum.h"
#include "CAttackSphere.h"

class CProjectile;

BEGIN(Engine)

class CTexture;
class CRcTex;

END

class CBoss2_Bullet : public CAttackSphere
{
private:
	explicit CBoss2_Bullet(DEVICE graphicDev);

    explicit CBoss2_Bullet(DEVICE graphicDev,
        CGameObject* owner, CGameObject* target, PROJ_TYPE phase);

    explicit CBoss2_Bullet(const CBoss2_Bullet& rhs);
    virtual ~CBoss2_Bullet();

public:
    void    Initialize_GameObject() override;
    HRESULT Ready_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;

    void    Render_Editor() override;

public:
    void    Start_Shoot() { m_StartShoot = true; }
    void    Set_TargetDir(_vec3 dir) { m_TargetDir = dir; }

    void Set_GravityMode(_bool enable) { m_UseGravity = enable; }
    void Set_Velocity(const _vec3& velocity) { m_Velocity = velocity; }
    void Set_MaxBounce(_int count) { m_MaxBounce = count; }

protected:
    void    Free() override;

public:
    static CBoss2_Bullet* Create(DEVICE graphicDev);

    static CBoss2_Bullet* Create(DEVICE graphicDev,
        CGameObject* owner, CGameObject* target, PROJ_TYPE type);

public:
    void OnBeginOverlap(CCollider* self, CCollider* other) override;

protected:
    HRESULT Add_Component() override;

private:
    CTexture*    m_TextureCom;
    CTexture*    m_ShadowTexCom; // Shadow Texture
    CProjectile* m_ProjectileCom;
	CRcTex*		 m_BufferCom;

    CGameObject* m_Owner;
    CGameObject* m_Target;

    PROJ_TYPE    m_ProjectileType;

    _vec3        m_StartPos;
    _vec3        m_TargetDir;
    _bool        m_StartShoot = false;

    _float       m_Speed = 15.f;

    _float       m_SpawnTimer = 0.f;
    _float       m_Wall_ImmunityTime = 0.7f; // 0.7초동안 벽과 충돌 ㄴ

private:
    // Bounce
    _bool       m_UseGravity = false;
    _vec3       m_Velocity = { 0.f,0.f,0.f };
    _float      m_Gravity = 20.f;
    _int        m_BounceCount = 0;
    _int        m_MaxBounce = 3;
    _float      m_BounceFactor = 0.6f; // 감쇠
    _float      m_FloorY = 0.5f; // 바닥 높이

};

