#pragma once

#include "CRenderObject.h"

BEGIN(Engine)
class CTexture;
END

class CFog : public CRenderObject
{
private:
    explicit CFog(DEVICE graphicDev);
    explicit CFog(const CFog& rhs);
    virtual ~CFog() override;

public:
    HRESULT Ready_GameObject() override;
    void    Initialize_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;

public:
    void    Set_OriginalPos(const _vec3& pos);
    void    Set_SwaySpeed(_float speed) { m_SwaySpeed = speed; }
    void    Set_SwayAmount(_float amount) { m_SwayAmount = amount; }
    void    Set_Alpha(_float alpha) { m_Alpha = alpha; }

    _float  Get_SwaySpeed() const { return m_SwaySpeed; }
    _float  Get_SwayAmount() const { return m_SwayAmount; }
    _float  Get_Alpha() const { return m_Alpha; }

    CTransform* Get_Transform() { return m_TransformCom; }

    // 색상 조절
    void    Set_Color(const COLOR& color) { m_Color = color; }
    void    Set_Color(_float r, _float g, _float b) { m_Color = COLOR(r, g, b, 1.f); }
    COLOR   Get_Color() const { return m_Color; }

private:
    HRESULT Add_Component() override;
    void    Render_Setting() override;
    void    Render_Reset() override;

public:
    static CFog* Create(DEVICE graphicDev);
    void Free() override;

private:
    CTexture* m_TextureCom = nullptr;

    _vec3       m_OriginalPos = { 0.f, 0.f, 0.f };  // 원래 위치
    _float      m_Time = 0.f;                             
    _float      m_SwaySpeed = 0.5f;                       // 자연스럽게 흔들릴 속도
    _float      m_SwayAmount = 2.f;                       // 흔들리는 폭
    _float      m_Alpha = 0.5f;

    COLOR       m_Color = COLOR(1.f, 1.f, 1.f, 1.f);
};

