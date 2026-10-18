#include "CSpriteAnimation.h"

CSpriteAnimation::CSpriteAnimation()
    : m_MaxFrame(1),
      m_MaxFrameY(1),
      m_StartFrame(0),
      m_Frame(0),
      m_AnimState(ANIMSTATE::STOP),
      m_XAxis(true),
      m_Interval(0.f),
      m_AccTime(0.f)
{}

CSpriteAnimation::CSpriteAnimation(const CSpriteAnimation& rhs)
{
    m_MaxFrame = rhs.m_MaxFrame;
    m_MaxFrameY = rhs.m_MaxFrameY;

    m_StartFrame = rhs.m_StartFrame;

    m_Frame   = rhs.m_Frame;
    m_AnimState    = rhs.m_AnimState;
    m_XAxis    = rhs.m_XAxis;
    m_Interval = rhs.m_Interval;
    m_AccTime  = 0.f;
}

CSpriteAnimation::~CSpriteAnimation()
{
    CSpriteAnimation::Free();
}

HRESULT CSpriteAnimation::Ready_SpriteAnim(
    _uint  frameCount,
    _float interval)
{
    m_MaxFrame = static_cast<_int>(frameCount);
    m_MaxFrameY = 1;

    m_Frame = 0;

    m_Interval = interval;
    m_XAxis    = true;
    m_AnimState    = ANIMSTATE::STOP;
    m_AccTime  = 0.f;

    return S_OK;
}

void CSpriteAnimation::Reset()
{
    m_AccTime = 0.f;
    m_Frame  = m_StartFrame;
}

void CSpriteAnimation::Update(const _float& timeDelta)
{
    if (m_AnimState == ANIMSTATE::STOP) // Stop이면 스프라이트 재생 X
        return;

    m_AccTime += timeDelta;

    // 누적 시간이 Interval을 넘기면, 스프라이트의 다음 프레임으로 이동
    while (m_AccTime >= m_Interval)
    {
        m_AccTime -= m_Interval;

        if (m_XAxis) // 가로 재생
        {
            if (m_AnimState == ANIMSTATE::LOOP) // 계속 반복
            {
                m_Frame++;

                if (m_Frame >= m_MaxFrame)
                    m_Frame = 0;
            }

            else if (m_AnimState == ANIMSTATE::ONCE) // 한번만 재생 후 Stop
            {
                m_Frame++;

                if (m_Frame >= m_MaxFrame)
                {
                    m_Frame = m_MaxFrame - 1;
                    m_AnimState  = ANIMSTATE::STOP;
                }
            }

            else if (m_AnimState == ANIMSTATE::REVERSE_LOOP)    // 반대로 반복재생
            {
                m_Frame--;

                if (m_Frame < 0)
                    m_Frame = m_MaxFrame - 1;
            }

            else if (m_AnimState == ANIMSTATE::REVERSE_ONCE)    // 반대로 한번만 재생
            {
                m_Frame--;

                if (m_Frame < 0)
                {
                    m_Frame = 0;
                    m_AnimState  = ANIMSTATE::STOP;
                }
            }
        }
    }
}

void CSpriteAnimation::Get_UV(_float& u0, _float& v0, _float& u1, _float& v1) const
{
#pragma region Sprite 방식 이제 안써서 삭제 예정

    /*if (m_MaxFrame > 1)
    {
        u0 = (m_Frame) / static_cast<float>(m_MaxFrame);
        u1 = (m_Frame + 1.f) / static_cast<float>(m_MaxFrame);
    }

    else
    {
        u0 = 0.f;
        u1 = 1.f;
    }

    if (m_MaxFrameY > 1)
    {
        v0 = (m_FrameY) / static_cast<float>(m_MaxFrameY);
        v1 = (m_FrameY + 1.f) / static_cast<float>(m_MaxFrameY);
    }
    else
    {
        v0 = 0.f;
        v1 = 1.f;
    }*/
#pragma endregion

    
}

CSpriteAnimation* CSpriteAnimation::Create(_uint frameCount, _float interval)
{
    auto sprite = new CSpriteAnimation();

    if (FAILED(sprite->Ready_SpriteAnim(frameCount, interval)))
    {
        Safe_Release(sprite);
        MSG_BOX("Sprite Create Failed");
        return nullptr;
    }

    return sprite;
}

void CSpriteAnimation::Free()
{ }
