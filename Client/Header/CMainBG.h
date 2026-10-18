#pragma once
#include "CGameObject.h"

BEGIN(Engine)
class CRcTex;
class CTexture;
class CTransform;
END

class CMainBG : public CGameObject
{
private:
    explicit CMainBG(DEVICE graphicDev);
    explicit CMainBG(const CMainBG& rhs);
    ~CMainBG() override;

public:
    HRESULT Ready_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;

private:
    HRESULT Add_Component();

private:
    CRcTex*   m_BufferCom;
    CTexture* m_TextureCom;
    CTransform* m_TransformCom;

public:
    static CMainBG* Create(DEVICE pGraphicDev);

protected:
    void Free() override;
};
