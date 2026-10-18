#pragma once
#include "CGameObject.h"
#include "Engine_Define.h"

BEGIN(Engine)
class CRcTex;
class CTexture;
class CTransform;

class ENGINE_DLL CUI : public CGameObject
{
public:
    explicit CUI(DEVICE pGraphicDev);
    explicit CUI(const CUI& rhs);
    ~CUI() override;

public:
    HRESULT Ready_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;

    virtual int Get_ID() { return m_ID; }

    void  Set_Visible(_bool visible) { m_Invisible = !visible; }
    _bool Get_Visible() { return m_Invisible; }

protected:
    bool m_bHovered = false;
    bool m_bClicked = false;
    RECT m_tRect;

protected:
    void Free() override;

protected:
    CTransform* m_TransformCom;
    CTexture*   m_TextureCom;
    CRcTex*     m_BufferCom;
    int         m_ID;
    bool        m_Invisible;
};

END
