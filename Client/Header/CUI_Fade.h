#pragma once

#include "CUI.h"

BEGIN(Engine)
class CRcTex;
class CTexture;
class CTransform;
END

class CUI_Fade : public CUI
{
private:
    explicit CUI_Fade(DEVICE graphicDev);
    explicit CUI_Fade(const CUI_Fade& rhs);
    virtual ~CUI_Fade();

public:
    HRESULT Ready_GameObject() override;
    void    Initialize_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;

public:
    _float  Get_Alpha() const { return m_Alpha; }
    void    Set_Alpha(_float alpha);

    void    Start_FadeIn(_float speed = 1.0f);
    void    Start_FadeOut(_float speed = 1.0f);
    _bool   IsFading() const { return m_IsActive; }
    _bool   IsFadeComplete() const { return !m_IsActive; }

private:
    HRESULT Add_Component();

public:
    static  CUI_Fade* Create(DEVICE graphicDev);
    void    Set_UseRenderer(_bool use) { m_UseRenderer = use; }

protected:
    void Free() override;

private:
    CRcTex*     m_BufferCom;
    CTransform* m_TransformCom;

    _float      m_Alpha;           
    _float      m_FadeSpeed;      
    _bool       m_IsFadingIn;    // true: Fade In, false: Fade Out
    _bool       m_IsActive;       

    _bool       m_UseRenderer;
};

