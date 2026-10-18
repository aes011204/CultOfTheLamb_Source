#pragma once
#include "CCollider.h"

BEGIN(Engine)
class CRcCol;

class ENGINE_DLL CRectCollider : public CCollider
{
private:
    explicit CRectCollider(DEVICE graphicDev);
    explicit CRectCollider(const CRectCollider& rhs);
    ~CRectCollider() override;

public:
    HRESULT Ready_BoxCollider(DEVICE graphicDev);
    _int    Update_Component(const _float& timeDelta) override;
    void    LateUpdate_Component() override;
    void    Render() override;

public:
    bool CheckCollision(CCollider* other) override;

    const _vec2& Get_Size() const { return m_Size; }
    void         Set_Size(_vec2 size) { m_Size = size; }

    static CRectCollider* Create(DEVICE GraphicDev);
    CComponent*           Clone() override;

protected:
    void Free() override;

private:
    CRcCol* m_BufferCom;
    _vec2   m_Size{};
};

END
