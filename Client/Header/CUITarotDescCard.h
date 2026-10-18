#pragma once
#include "CUI.h"
class CUITarotDescCard : public CUI
{
private:
    explicit CUITarotDescCard(DEVICE graphicDev);
    explicit CUITarotDescCard(const CUITarotDescCard& rhs);
    ~CUITarotDescCard() override;

public:
    HRESULT     Ready_GameObject() override;
    _int        Update_GameObject(const _float& timeDelta);
    void        LateUpdate_GameObject(const _float& timeDelta) override;
    void        Render_GameObject() override;
    HRESULT     Add_Component();
    CTransform* Get_Transform() const { return m_TransformCom; }
    void        Set_Slide(bool tf) { m_PlaySlide = tf; }
    void        InitializeScale();;
    void        SetOnClick(function<void()> callback) { m_OnClick = callback; }
    void        Set_RenderOff(bool tf) { m_RenderOff = tf; }
    void        OnHoverStart();
    void        OnHoverEnd();

    bool        Get_Hovered() { return m_Hovered; }
    void        Set_Hovered(bool tf) { m_Hovered = tf; }
public:
    static CUITarotDescCard* Create(DEVICE graphicDev);
    void        Set_TextureIndex(int textureIndex) { m_TextureIndex = textureIndex; }

private:
    function<void()> m_OnClick;
    static int  m_NextID;
    int         m_TextureIndex = 0;
    bool        m_Hovered;
    bool        m_PlaySlide;
    bool        m_Start;
    float       m_StartX;
    float       m_TargetX;
    bool        m_RenderOff;

    _vec3       m_BaseScale;
    _vec3       m_TargetScale;
    RECT        m_Rect;
};


