#pragma once
#include "CComponent.h"

BEGIN(Engine)
class CRcTex;
class CBoxTex;
class CSpriteAnimation;
class CTexture;
class CVIBuffer;

struct AnimDesc
{
    CSpriteAnimation* anim;
    _vec2             size;
    _vec2             offset;
};

/// @brief 여러 개의 애니메이션을 보관하고 있다가, CState의 명령에 따라 애니메이션을 변경해서 재생하는 "재생장치" 컴포넌트
class ENGINE_DLL CAnimator : public CComponent
{
private:
    explicit CAnimator(DEVICE graphicDev);
    explicit CAnimator(const CAnimator& rhs);
    ~CAnimator() override;

public:
    _int                Update_Component(const _float& timeDelta) override;
    void                LateUpdate_Component() override;
    void                Render_Component();

public:
    // 애니메이션 등록 (이미 만들어진 CSpriteAnimation 넣기)
    HRESULT             Add_Animation(const wstring& key, CSpriteAnimation* animation);
    HRESULT             Create_Animation(const wstring& key, _uint frameCount, _float interval);

    // 사이즈 세팅 버전
    HRESULT             Add_Animation(
                            const wstring& key, CSpriteAnimation* animation,
                            const _vec2 size, const _vec2 offset = _vec2(0.f, 0.f));

    HRESULT             Create_Animation(const wstring& key, _uint frameCount,
                             _float interval, const _vec2& size, const _vec2 offset = _vec2(0.f, 0.f));

    void                Play_Animation(const wstring& key, ANIMSTATE state, bool reset = true);
    void                Stop_Animation();

    // 재생중인 애니메이션이 끝났는지. 보통 ONCE일때 사용
    _bool               Is_AnimationFinished();

    CSpriteAnimation*   Get_CurAnimation() const { return m_CurAnimation; } // 현재 재생중인 애니메이션
    const wstring&      Get_CurKey() { return m_CurKey; } // 현재 재생중인 키값
    _int                Get_CurFrame(); // 현재 재생중인 프레임
    _int                Get_MaxFrame(); // 최대 프레임

    _vec2               Get_CurSize() const;
    _vec2               Get_CurOffset() const;

    void                Set_CurSize(const _vec2& size);
    void                Set_CurOffset(const _vec2& offset);
    void                Set_FlipX(_bool flip);

    // 주의! Object마다 Set_TextureType 설정 꼭 해줘야함
    void                Set_TextureType(COMPONENTTYPE type);

    static CAnimator*   Create(DEVICE graphicDev);
    CComponent*         Clone() override;

    CSpriteAnimation*   Find_Animation(const wstring& Key);
private:
    HRESULT             Ready_Animator();

    void                Free() override;

private:
    map<wstring, AnimDesc>  m_Animations;
    CSpriteAnimation*       m_CurAnimation;
    wstring                 m_CurKey;

    _bool                   m_Play;

    Engine::CVIBuffer*      m_BufferCom;
    Engine::CTexture*       m_Texture;

    _bool                   m_Initialized = false;
    COMPONENTTYPE           m_TextureType;
};

END
