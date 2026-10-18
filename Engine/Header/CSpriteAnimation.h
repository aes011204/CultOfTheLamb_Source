#pragma once
#include "CBase.h"
#include "Engine_Define.h"

BEGIN(Engine)
/// @brief 하나의 동작(예: 걷기, 숨쉬기)에 대한 구체적인 프레임 정보를 담고 있는, CAnimator의 부품 역할 클래스
class ENGINE_DLL CSpriteAnimation : public CBase
{
private:
    explicit CSpriteAnimation();
    explicit CSpriteAnimation(const CSpriteAnimation& rhs);
    ~CSpriteAnimation() override;

public:
    HRESULT Ready_SpriteAnim(_uint  frameCount, _float interval);

    void Reset();
    void Update(const _float& timeDelta);

    _int Get_Frame() const { return m_Frame; }
    _int Get_MaxFrameX() const { return m_MaxFrame; }

    // 현재 UV 사각형
    void Get_UV(_float& u0, _float& v0, _float& u1, _float& v1) const;

    void      Set_AnimState(ANIMSTATE state) { m_AnimState = state; }
    ANIMSTATE Get_AnimState() const { return m_AnimState; }

    _bool IsFinished() const { return m_AnimState == ANIMSTATE::STOP; }

    static CSpriteAnimation* Create(_uint  frameCount,_float interval);

private:
    void Free() override;

private:
    // 스프라이트 시트 전체 개수 (가로 X 세로)
    _int m_MaxFrame;
    _int m_MaxFrameY;

    // 시작 프레임
    _int m_StartFrame;

    // 현재 프레임 좌표 (0 ~ max-1 까지가 범위임)
    _int m_Frame;

    ANIMSTATE m_AnimState;      // 재생 상태, Stop, Loop, Once
    _bool     m_XAxis;		    // true면 X축으로 재생, false면 y축으로 재생
    _float    m_Interval;		// 다음 프레임까지 간격
    _float    m_AccTime;		// 누적 시간
};

END;
