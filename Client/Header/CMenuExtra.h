#pragma once
#include "CRenderObject.h"
class CMenuExtra :
    public CRenderObject
{
private:
    explicit CMenuExtra(DEVICE graphicDev, int textureIndex);
    explicit CMenuExtra(const CMenuExtra& rhs);
    ~CMenuExtra() override;

public:
    HRESULT Ready_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;

private:

    HRESULT Add_Component();


public:
    int m_textureIndex = 0;

private:
    CRcTex* m_BufferComOffset1;
    CRcTex* m_BufferComOffset2;

    

public:
    static CMenuExtra* Create(DEVICE graphicDev, int textureIndex);

private:
    void Free() override;
};

