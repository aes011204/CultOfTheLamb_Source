#include "pch.h"
#include "CSpriteEff.h"
#include "CGameObject.h"
#include "CTransform.h"
#include "CRcTex.h"

CSpriteEff::CSpriteEff()
{
}

CSpriteEff::CSpriteEff(DEVICE graphicDev) : CEffectBaseCom(graphicDev)
, is_SetSpriteDesc(false)
{
}

CSpriteEff::CSpriteEff(const CSpriteEff& rhs) : CEffectBaseCom(rhs)
{
    //컴포넌트는 현구조상 프로토 타입으로 클론하기 떄문에 깊은 복사 , 초기화 필수

}

CSpriteEff::~CSpriteEff()
{
}

HRESULT CSpriteEff::Ready_Effect()
{
    return S_OK;
}

void CSpriteEff::Initialize_Component()
{

    DIR8[0] = _vec3{ m_LineSize, 0,  0.001f };
    DIR8[1] = _vec3{ -m_LineSize, 0, 0.001f };
    DIR8[2] = _vec3{ 0, -m_LineSize, 0.001f };
    DIR8[3] = _vec3{ 0, m_LineSize,  0.001f };
    
    //DIR8[0] = _vec3{ m_LineSize, 0,  0 };   // 동
    //DIR8[1] = _vec3{ m_LineSize, 0, -m_LineSize };   // 남동
    //DIR8[2] = _vec3{ 0, 0, -m_LineSize };   // 남
    //DIR8[3] = _vec3{ -m_LineSize, 0, -m_LineSize };   // 남서
    //DIR8[4] = _vec3{ -m_LineSize, 0,  0 };   // 서
    //DIR8[5] = _vec3{ -m_LineSize, 0,  m_LineSize };   // 북서
    //DIR8[6] = _vec3{ 0, 0,  m_LineSize };   // 북
    //DIR8[7] = _vec3{ m_LineSize, 0,  m_LineSize };   // 북동
      

}

_int CSpriteEff::Update_Component(const _float& timeDelta)
{
    if (m_IsSetBouncingInit)
    {
        BounceEff(timeDelta); // 완료 되면 m_IsSetBouncingInit = false
    }


   

    
    return 0;
}

void CSpriteEff::LateUpdate_Component()
{
}

void CSpriteEff::Render_Effect()
{
    if (m_IsSetOutLineInit == true)
    {

    }
}

bool CSpriteEff::BounceEff(const _float& timeDelta)
{
  
        if (!m_Owner)
            return false;

        // 가속도 -> 속도
        m_Velocity.y += m_Gravity * timeDelta;

        //속도 -> 위치
        CTransform* ownerTransform = m_Owner->Get_Transform();
        ownerTransform->Set_Pos(ownerTransform->Get_Pos() + m_Velocity * timeDelta);

        if (ownerTransform->Get_Pos().y <= m_GroundY) // 바닥 충돌 처리
        {
            // ... (위치 강제 설정, Y축 튕김 계산, X/Z에 m_Damping 곱하기) ...
            ownerTransform->Set_Pos({ ownerTransform->Get_Pos().x, m_GroundY, ownerTransform->Get_Pos().z }); // 위치 보정

            m_Velocity.y = -m_Velocity.y * m_Restitution; // Y축 튕김 계산

            // 📌 충돌 직후 1회성 X/Z 감쇠 (이 코드가 누락되면 미끄러짐이 심해집니다)
            m_Velocity.x *= m_Damping;
            m_Velocity.z *= m_Damping;

            if (fabsf(m_Velocity.y) < m_STOP_VEL)
            {
                m_Velocity.y = 0.f;

                // Y축이 멈춘 상태(바닥에 붙은 상태)일 때, X/Z 속도에 지속적인 감쇠(마찰)를 적용합니다.
                // m_Damping 값을 사용하여 바닥 마찰을 흉내냅니다.
                m_Velocity.x *= powf(m_Damping, timeDelta * 10.f); // timeDelta에 10 정도를 곱해 감쇠 속도를 높여도 좋습니다.
                m_Velocity.z *= powf(m_Damping, timeDelta * 10.f);

                // 마지막 미세 떨림 방지 (X/Z가 0이 되는 최종 조건)
                if (fabsf(m_Velocity.x) < 0.5f && fabsf(m_Velocity.z) < 0.5f)
                {
                    m_Velocity.x = 0.f;
                    m_Velocity.z = 0.f;
                    // ... (Update 종료 로직) ...
                    //m_IsSetBouncingInit = false;
                   

                    return true;
                }
            }
        }

        // ... (이 부분은 공중에 있을 때의 공기 저항으로 유지) ...
        float factor = powf(m_DampingPerSecond, timeDelta);
        m_Velocity.x *= factor;
        m_Velocity.z *= factor;

        return false;

    
}

bool CSpriteEff::OutLineEff(Engine::CRcTex* m_BufferCom)
{
    if (m_IsSetOutLineInit == true)
    {

        m_TransformCom = m_Owner->Get_Transform();

        _vec3 OriginPos = m_TransformCom->Get_Pos();

        D3DXMATRIX originWorld = m_TransformCom->Get_World();
        
        for (int i = 0; i < 8; i++)
        {
            // 2. 오프셋 벡터를 행렬에 곱하여 로컬 회전을 적용합니다.
            _vec3 RotatedOffset;
            D3DXVec3TransformNormal(&RotatedOffset, &DIR8[i], &originWorld); // 회전/스케일만 적용
            //
            _vec3 tmpPos = OriginPos + RotatedOffset;
            //
            D3DXMATRIX tempWorld = m_TransformCom->Get_World();
            tempWorld._41 = tmpPos.x;
            tempWorld._42 = tmpPos.y;
            tempWorld._43 = tmpPos.z;

            m_GraphicDev->SetTransform(D3DTS_WORLD, &tempWorld);

            FlashRenderStart(D3DCOLOR_ARGB(255, 255, 255, 255));// AGRB
            if (m_BufferCom)
                m_BufferCom->Render_Buffer();
        }

        FlashRenderEnd();


        m_GraphicDev->SetTransform(D3DTS_WORLD, &originWorld);

        if (m_BufferCom)
            m_BufferCom->Render_Buffer();

        return true;
    }

    return false;
}

bool CSpriteEff::OutLineEffMTRX(D3DXMATRIX partWorld, Engine::CRcTex* m_BufferCom)
{
    if (m_IsSetOutLineInit == true)
    {

        //m_TransformCom = m_Owner->Get_Transform();

        //_vec3 OriginPos = m_TransformCom->Get_Pos();

        //D3DXMATRIX originWorld = m_TransformCom->Get_World();


        _vec3 partPos = { partWorld._41,partWorld._42,partWorld._43 };
        for (int i = 0; i < 8; i++)
        {
            // 2. 오프셋 벡터를 행렬에 곱하여 로컬 회전을 적용합니다.
            _vec3 RotatedOffset;
            D3DXVec3TransformNormal(&RotatedOffset, &DIR8[i], &partWorld); // 회전/스케일만 적용
            //
            _vec3 tmpPos = partPos + RotatedOffset;
            //
            D3DXMATRIX tempWorld = partWorld;
            tempWorld._41 = tmpPos.x;
            tempWorld._42 = tmpPos.y;
            tempWorld._43 = tmpPos.z;

            m_GraphicDev->SetTransform(D3DTS_WORLD, &tempWorld);

            FlashRenderStart(D3DCOLOR_ARGB(255, 255, 255, 255));// AGRB
            if (m_BufferCom)
                m_BufferCom->Render_Buffer();
        }

        FlashRenderEnd();


        m_GraphicDev->SetTransform(D3DTS_WORLD, &partWorld);

        if (m_BufferCom)
            m_BufferCom->Render_Buffer();

        return true;
    }

    return false;
}

CSpriteEff* CSpriteEff::Create(DEVICE graphicDev)
{
    auto SpriteEff = new CSpriteEff(graphicDev);

    if (FAILED(SpriteEff->Ready_Effect()))
    {
        MSG_BOX("SpriteEff Create Failed");
        Safe_Release(SpriteEff);
        return nullptr;
    }

    return SpriteEff;
}

CComponent* CSpriteEff::Clone()
{
    return new CSpriteEff(*this);
}

void CSpriteEff::Free()
{
    CEffectBaseCom::Free();
}


void CSpriteEff::FlashRenderStart(DWORD color)
{
    m_GraphicDev->SetRenderState(
        D3DRS_TEXTUREFACTOR,
        D3DCOLOR_ARGB(255, 255, 255, 255)
    );

    // TEXTURE * TFACTOR 로 변경하면 Tint 색 정확히 나옴
    m_GraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_ADD);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
}

void CSpriteEff::FlashRenderEnd()
{
    // Hurt tint가 남지 않게 기본 연산도 복구
    m_GraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);

}

void CSpriteEff::AlphaRenderStart(float alpha)
{
    //알파
    m_GraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB((int)(alpha * 255), 255, 255, 255));
    //
    m_GraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR); // ← texturefactor.alpha 사용

}

void CSpriteEff::AlphaRenderEnd()
{
    // 알파 리셋
    m_GraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
    m_GraphicDev->SetRenderState(
        D3DRS_TEXTUREFACTOR,
        D3DCOLOR_ARGB(255, 255, 255, 255)); // 알파 1.0, 색 그대로
}
