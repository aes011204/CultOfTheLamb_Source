#pragma once

#include "CRenderObject.h"

BEGIN(Engine)

class ENGINE_DLL CStaticParticle : public CRenderObject
{
private:
    explicit CStaticParticle(DEVICE graphicDev);
    explicit CStaticParticle(const CStaticParticle& rhs);
    virtual ~CStaticParticle() override;

public:
    HRESULT Ready_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;

protected:
    HRESULT Add_Component() override;

public:
    void Set_Velocity(const _vec3& velocity) { m_Velocity = velocity; }
    void Set_Gravity(_float gravity) { m_Gravity = gravity; }

    void Set_FloorY(_float y) { m_FloorY = y; }
    void Set_LifeTime(_float life) { m_LifeTime = life; }

    void Set_FadeTime(_float fade) { m_FadeTime = fade; }
    void Set_TextureIndex(_int index) { m_TextureIndex = index; }
    void Set_Alpha(_float alpha) { m_Alpha = alpha; }

    static CStaticParticle* Create(DEVICE graphicDev,
        const _vec3& position,
        COMPONENTTYPE textureType);

protected:
    void Free() override;
private:

    COMPONENTTYPE   m_TextureType = COMPONENTTYPE::COMPONENT_END;

    _int            m_TextureIndex = 0;
    _vec3           m_Velocity = _vec3(0.f, 0.f, 0.f);

    _float          m_Gravity = 15.f;
    _float          m_FloorY = 0.f;
    _float          m_LifeTime = 6.f;      // LifeTime 이후, 서서히 사라지도록
    _float          m_FadeTime = 2.f;       
    _float          m_ElapsedTime = 0.f;
    _float          m_Alpha = 1.f;
    _bool           m_IsOnFloor = false;
};

END
