#pragma once

#include "CUI.h"

BEGIN(Engine)
class CRcTex;
class CTexture;
class CTransform;
class CAnimator;
END

class CUI_LoadingCircle : public CUI
{
    explicit CUI_LoadingCircle(DEVICE graphicDev);
    explicit CUI_LoadingCircle(const CUI_LoadingCircle& rhs);
    virtual ~CUI_LoadingCircle();

public:
    HRESULT Ready_GameObject() override;
    void    Initialize_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;

    void    Set_Position(_vec2 pos) { m_Position = pos; }

private:
    HRESULT Add_Component();

public:
    static CUI_LoadingCircle* Create(DEVICE graphicDev);

private:
    void Free() override;

private:
    CRcTex*     m_BufferCom = nullptr;
    CTexture*   m_TextureCom = nullptr;
    CTransform* m_TransformCom = nullptr;
    CAnimator*  m_AnimatorCom = nullptr;

    _vec2       m_Position = {0.f, 0.f};
    _vec2       m_Scale = { 0.f, 0.f };
};
