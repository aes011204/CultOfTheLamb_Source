#pragma once
#include "CEffectBaseCom.h"
#include "Client_Struct.h"
namespace Engine
{
    class CRcTex;
}


struct SpriteDesc
{
};

class CSpriteEff :
    public CEffectBaseCom
{
protected:
    explicit CSpriteEff();
    explicit CSpriteEff(DEVICE graphicDev);
    explicit CSpriteEff(const CSpriteEff& rhs);
    ~CSpriteEff() override;

public:
    HRESULT     Ready_Effect() override;
    void      Initialize_Component()override;
    _int      Update_Component(const _float& timeDelta)override;
    void      LateUpdate_Component()override;
    void       Render_Effect() override;

    bool BounceEff(const _float& timeDelta);
    bool OutLineEff(Engine::CRcTex* m_BufferCom);

    bool OutLineEffMTRX(D3DXMATRIX originWorld, Engine::CRcTex* m_BufferCom);


public:// 함수로만 호출 가능 한 애들
// 플레시 알파 이팩트 함수
    void FlashRenderStart(DWORD color);// AGRB
    void FlashRenderEnd();
    void AlphaRenderStart(float alpha); //0~1
    void AlphaRenderEnd();

    void SetSpriteDesc(SpriteDesc s)
    {

    }

public:
    void InitBounce(SpriteBounceDesc& b)
    {
        m_Pos = b.pos;
        m_Velocity = b.velocity;
        m_GroundY = b.groundY;
        m_Gravity = b.gravity;
        m_Restitution = b.restitution;
        m_Damping = b.damping;
        m_DampingPerSecond = b.m_DampingPerSecond;

        m_IsSetBouncingInit = true;
    }
    void InitOutLine(DWORD color,float lineSize, float alpha=1)
    {
        m_Color = color;
        m_Alpha = alpha;
        m_LineSize = lineSize;

        m_IsSetOutLineInit = true;
    }

public:
    static CSpriteEff* Create(DEVICE graphicDev);
    CComponent* Clone();

private:
    bool is_SetSpriteDesc;

    // 간단 물리//////
    _vec3 m_Pos;        // 위치
    _vec3 m_Velocity;   // 초기 힘
    float m_GroundY;    // 땅 위치
    float m_Gravity;
    float m_Restitution; // 튕김계수 (1: 비탄성 ~ 0: 탄성)
    float m_Damping;    // 수평 속도 감쇠 (튕길떄)
    float m_DampingPerSecond;    // 수평 속도 감쇠 (공기저항)
    float m_STOP_VEL = 5.f;


    bool m_IsSetBouncingInit = false;

    // 라인 이팩트 /////
    float m_LineSize;
    _vec3 DIR8[8];
    DWORD m_Color;
    float m_Alpha;

    bool m_IsSetOutLineInit;
protected:
    void Free() override;
};

