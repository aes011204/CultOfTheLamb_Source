#pragma once
#include "CUI.h"

class CSpriteEff;

enum class TAROTSTATE
{
    START,
    CHOICE,
    RESULT,
    EXIT,
    COUNT
};

enum class TAROTTYPE
{
    HEART,
    ARROW
};

namespace Engine
{
class CAnimator;
}

class CUITarotCard : public CUI
{
private:
    explicit CUITarotCard(DEVICE graphicDev);
    explicit CUITarotCard(const CUITarotCard& rhs);
    ~CUITarotCard() override;

public:
    void        Set_TextureIndex(int texture) { m_TextureIndex = texture; }
    HRESULT     Ready_GameObject();
    _int        Update_GameObject(const _float& timeDelta) override;
    void        LateUpdate_GameObject(const _float& timeDelta) override;
    void        Render_GameObject() override;
    HRESULT     Add_Component();
    void        Animation_Setting();
    void        InitializeScale();

    bool            Get_Selected() { return m_Selected; }
    bool            Get_Hovered() { return m_Hovered; }
    TAROTSTATE      Get_TarotState() { return m_CurrentState; }
    TAROTTYPE       Get_TarotType() { return m_CurrentType; }
    const wstring   Get_AnimationType() { return m_AnimationType; }
    CAnimator*      Get_Animator() { return m_AnimatorCom; }
    CTransform*     Get_Transform() { return m_TransformCom; }
    bool            Get_IsFinished() { return m_IsFinished; }

    void            Set_IsFinished(bool tf) { m_IsFinished = tf; }
    void            Set_CurrentState(TAROTSTATE state) { m_CurrentState = state; }
    void            Set_Hovered(bool tf) { m_Hovered = tf; }
    void            Set_PrevState(TAROTSTATE state) { m_PrevState = state; }
    void            Set_TarotType(TAROTTYPE type) { m_CurrentType = type; }

    void OnHoverStart();
    void OnHoverEnd();

    void Render_Editor() override;

public:
    static CUITarotCard* Create(DEVICE graphicDev);
    void SetOnClick(function<void()> callback) { m_OnClick = callback; }
    void Free() override;

protected:

    static int m_NextID;
    bool m_Hovered;
    bool m_Selected;
    RECT m_Rect;
    int  m_TextureIndex;
    TAROTSTATE m_CurrentState;
    TAROTTYPE m_CurrentType;
    TAROTSTATE m_PrevState;
    wstring m_AnimationType;
    bool m_Start = true;
    bool m_IsFinished;
    bool m_RenderOff;
    function<void()> m_OnClick;
    _vec3       m_BaseScale;
    _vec3       m_TargetScale;

    CAnimator* m_AnimatorCom;
    CSpriteEff* m_SpriteEffCom;

    _bool   m_FadeOut = false;
    _float   m_FadeSpeed = 1.5f;
    _float m_Alpha = 0.8f;
};


