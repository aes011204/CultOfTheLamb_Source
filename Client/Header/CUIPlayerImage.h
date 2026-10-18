#pragma once
#include "CUI.h"

namespace Engine
{
class CState;
class CAnimator;
}

class CUIPlayerImage :public CUI
{
private:
    explicit CUIPlayerImage(DEVICE pGraphicDev);
    explicit CUIPlayerImage(const CUI& rhs);

public:
    HRESULT Ready_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;
    HRESULT Add_Component();
    static CUIPlayerImage* Create(DEVICE graphicDev);
protected:
    void Free() override;

private:
    CRcTex*          m_BufferCom;
    CTransform*      m_TransformCom;
    CTexture*        m_TextureCom;
    CAnimator*       m_AnimatorCom;
    CState*          m_StateCom;
};

