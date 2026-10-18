#include "CParticle.h"
#include "CCreateHelper.h"
#include "CTexture.h"
#include "CAnimator.h"
#include "CRcTex.h"
#include "CTransform.h"
#include "CRenderer.h"

CParticle::CParticle(DEVICE graphicDev)
    : CRenderObject(graphicDev),
      m_AnimatorCom(nullptr),
      m_AutoDestroy(true),
      m_LifeTime(-1.f),
      m_ElapsedTime(0.f),
      m_TextureType(COMPONENTTYPE::COMPONENT_END), m_FrameCount(0), m_FrameInterval(0), m_Gravity(0), m_Friction(0
      )

{
}

CParticle::CParticle(const CParticle& rhs)
    : CRenderObject(rhs),
      m_AnimatorCom(nullptr),
      m_AutoDestroy(rhs.m_AutoDestroy),
      m_LifeTime(rhs.m_LifeTime),
      m_ElapsedTime(0.f),
      m_TextureType(rhs.m_TextureType),
      m_AnimKey(rhs.m_AnimKey), m_FrameCount(rhs.m_FrameCount), m_FrameInterval(rhs.m_FrameInterval), m_Gravity(0),
      m_Friction(0)
{
}

CParticle::~CParticle()
{
}

HRESULT CParticle::Ready_GameObject()
{
    if (FAILED(CRenderObject::Ready_GameObject()))
        return E_FAIL;

    m_TransformCom->Set_Scale(_vec3(1.f, 1.f, 1.f));

    return S_OK;
}

void CParticle::Initialize_GameObject()
{
    CRenderObject::Initialize_GameObject();

    Particle_Settings();
}

_int CParticle::Update_GameObject(const _float& timeDelta)
{
    _int exit = CRenderObject::Update_GameObject(timeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

    // 시간 먼저 체크 후 리턴
    if (m_LifeTime > 0.f)
    {
        m_ElapsedTime += timeDelta;
        if (m_ElapsedTime >= m_LifeTime)
        {
            Mark_As_PendingKill();
            return exit;
        }
    }

    // 위치 이동
    if (m_TransformCom && D3DXVec3LengthSq(&m_Velocity) > 0.0001f)
    {
        _vec3 pos = m_TransformCom->Get_Pos();
    
        pos += m_Velocity * timeDelta;
        m_TransformCom->Set_Pos(pos);
    
        m_Velocity.y -= m_Gravity * timeDelta;
        m_Velocity *= (1.f - m_Friction * timeDelta);
    }

    // 애니메이션 재생 끝 체크
    if (m_AutoDestroy && m_AnimatorCom)
    {
        if (m_AnimatorCom->Is_AnimationFinished())
        {
            Mark_As_PendingKill();
        }
    }

    return exit;
}

void CParticle::Render_GameObject()
{
    // Transform 설정
    if (m_TransformCom)
        m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());

    Render_Setting();

    if (m_TextureCom && m_AnimatorCom)
    {
        const wstring& key = m_AnimatorCom->Get_CurKey();
        _int frame = m_AnimatorCom->Get_CurFrame();
        m_TextureCom->Set_Texture(key, frame);
    }

    // 버퍼 렌더링
    if (m_BufferCom)
        m_BufferCom->Render_Buffer();

    Render_Reset();
}

HRESULT CParticle::Add_Component()
{
    if (FAILED(CRenderObject::Add_Component()))
        return E_FAIL;

    // Texture
    m_TextureCom = CreateProtoComponent<CTexture>(this, m_TextureType);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);
    m_Components[ID_STATIC].insert({ m_TextureType, m_TextureCom });

    // Animator
    m_AnimatorCom = CreateProtoComponent<CAnimator>(this, COMPONENTTYPE::ANIMATOR);
    NULL_CHECK_RETURN(m_AnimatorCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::ANIMATOR, m_AnimatorCom });

    m_AnimatorCom->Set_TextureType(m_TextureType);

    return S_OK;
}

void CParticle::Particle_Settings()
{
    if (!m_AnimatorCom || m_AnimKey.empty())
        return;

    m_AnimatorCom->Create_Animation(m_AnimKey, m_FrameCount, m_FrameInterval);
    m_AnimatorCom->Play_Animation(m_AnimKey, ANIMSTATE::ONCE); // 한번만 재생
}

void CParticle::Set_ParticleInfo(_int frameCount, _float interval)
{
    m_FrameCount = frameCount;
    m_FrameInterval = interval;
}

CParticle* CParticle::Create(DEVICE graphicDev, const _vec3& position, COMPONENTTYPE textureType,
                             const wstring& animKey)
{
    auto particle = new CParticle(graphicDev);

    particle->m_TextureType = textureType;
    particle->m_AnimKey = animKey;

    if (FAILED(particle->Ready_GameObject()))
    {
        Safe_Release(particle);
        MSG_BOX("Particle Create Failed");
        return nullptr;
    }

    particle->m_TransformCom->Set_Pos(position);

    return particle;
}

void CParticle::Free()
{
    CRenderObject::Free();
}
