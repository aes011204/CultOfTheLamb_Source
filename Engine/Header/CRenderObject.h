#pragma once

#include "CGameObject.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CTransform;

class ENGINE_DLL CRenderObject : public CGameObject
{
protected:
    explicit CRenderObject(DEVICE graphicDev);
    explicit CRenderObject(const CRenderObject& rhs);
    virtual ~CRenderObject();

public:
    HRESULT Ready_GameObject() override;
    void    Initialize_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;

protected:
    // 자식이 컴포넌트 추가할 때 사용
    virtual HRESULT Add_Component();

    virtual void Render_Setting();
    virtual void Render_Reset();

    virtual void Set_Material();

protected:
    CTransform* m_TransformCom;
    CRcTex*     m_BufferCom;
    CTexture*   m_TextureCom;  // 자식에서 설정해줘야 함

protected:
    void Free() override;

};

END
