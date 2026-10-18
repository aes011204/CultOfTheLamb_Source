#pragma once

#include "CRenderObject.h"

BEGIN(Engine)
class CBoxCollider;
END

class CWall : public CRenderObject
{
private:
    explicit CWall(DEVICE graphicDev);
    explicit CWall(const CWall& rhs);
    virtual ~CWall();

public:
    HRESULT Ready_GameObject() override;
    void    Initialize_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;
    void    Render_Editor() override;

private:
    HRESULT Add_Component() override;

public:
    void    OnBeginOverlap(CCollider* self, CCollider* other) override;
    void    OnStayOverlap(CCollider* self, CCollider* other) override;

    static CWall* Create(DEVICE graphicDev);

protected:
    void Free() override;

private:
    CBoxCollider*   m_BoxCollider;
};

