#pragma once

#include "CGameObject.h"
#include "CPlayerSkill.h"
#include "CProjectile.h"
#include "CAttackSphere.h"

BEGIN(Engine)
class CTransform;
class CTexture;
class CRcTex;
END

class CProjectile;
struct SkillInfo;

class CFireball : public CAttackSphere
{
private:
    explicit CFireball(DEVICE graphicDev);
    explicit CFireball(const CFireball& rhs);
    ~CFireball() override;

public:
    void    Initialize_GameObject() override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;
    void    OnBeginOverlap(CCollider* self, CCollider* other) override;
    void    OnStayOverlap(CCollider* self, CCollider* other) override;
    void    OnEndOverlap(CCollider* self, CCollider* other) override;
    void    Render_Editor() override;
    HRESULT Add_Component() override;
    HRESULT Ready_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;

public:
    static CFireball* Create(DEVICE graphicDev);

protected:
    void Free() override;

private:
    CProjectile* m_ProjectileCom;
    CTexture*    m_TextureCom;
    CRcTex*      m_BufferCom;

    CGameObject* m_Owner;
};


