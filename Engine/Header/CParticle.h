#pragma once

#include "CRenderObject.h"

BEGIN(Engine)

class CAnimator;

class ENGINE_DLL CParticle : public CRenderObject
{
private:
    explicit CParticle(DEVICE graphicDev);
    explicit CParticle(const CParticle& rhs);
    virtual ~CParticle() override;

public:
    HRESULT Ready_GameObject() override;
    void    Initialize_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;

protected:
    HRESULT Add_Component() override;
    void    Particle_Settings();

public:
    void Set_AutoDestroy(bool value) { m_AutoDestroy = value; }
    void Set_LifeTime(float time) { m_LifeTime = time; }
    void Set_TextureType(COMPONENTTYPE type) { m_TextureType = type; }
    void Set_AnimKey(const wstring& key) { m_AnimKey = key; }
    void Set_ParticleInfo(_int frameCount, _float interval);

    static CParticle* Create(DEVICE graphicDev,
                                const _vec3& position,
                                COMPONENTTYPE textureType,
                                const wstring& animKey);

    void Set_Velocity(const _vec3& velocity) { m_Velocity = velocity; }
    void Set_Gravity(_float gravity) { m_Gravity = gravity; }
    void Set_Friction(_float frction) { m_Friction = frction; }

protected:
    void Free() override;

private:
    CAnimator*      m_AnimatorCom;

    _bool            m_AutoDestroy;   // 재생이 끝나면 삭제되게 할지?
    _float           m_LifeTime;      // 생명 시간 (-1이면 무한)
    _float           m_ElapsedTime;   // 경과 시간

    COMPONENTTYPE   m_TextureType;
    wstring         m_AnimKey;

    _uint           m_FrameCount;
    _float          m_FrameInterval;

    _vec3           m_Velocity = _vec3(0.f, 0.f, 0.f);
    _float          m_Gravity = 0.f;     
    _float          m_Friction = 0.f;    


};

END
