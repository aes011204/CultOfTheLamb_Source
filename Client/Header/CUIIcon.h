#pragma once
#include "CUI.h"

class CUIIcon : public CUI
{
private:
    explicit CUIIcon(DEVICE pGraphicDev);
    explicit CUIIcon(const CUIIcon& rhs);
    ~CUIIcon() override;

public:
    HRESULT Ready_GameObject(COMPONENTTYPE texProtoTag);
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_Setting();
    void     Render_Reset();
    void    Render_GameObject() override;
    HRESULT Add_Component(COMPONENTTYPE texProtoTag);

public:
    static CUIIcon* Create(DEVICE graphicDev, COMPONENTTYPE texProtoTag);
    void    Set_Alpha(int alpha) { m_Alpha = alpha; }
    int    Get_Alpha() { return m_Alpha; }
    void        Set_TextureIndex(int textureIndex) { m_textureIndex = textureIndex; }
    CTransform* Get_Transform() const { return m_TransformCom; }
    bool SetCloneTexAspectScale(_vec3 vec);
    void SetRegister(bool enable) { m_Register = enable; }

private:
    static int m_nextID;
    int        m_textureIndex = 0;
    bool       m_Register;
    int m_Alpha =1 ;
private:
    void Free() override;
};
