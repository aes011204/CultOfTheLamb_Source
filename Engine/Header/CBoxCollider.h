#pragma once
#include "CCollider.h"

BEGIN(Engine)
class CBoxTex;
class CBoxCol;

class ENGINE_DLL CBoxCollider : public CCollider
{
protected:
    explicit CBoxCollider(DEVICE graphicDev);
    explicit CBoxCollider(const CBoxCollider& rhs);
    ~CBoxCollider() override;

public:
    HRESULT         Ready_BoxCollider(DEVICE graphicDev);
    _int            Update_Component(const _float& timeDelta) override;
    void            LateUpdate_Component() override;
    void            Render() override;

public:
    bool            CheckCollision(CCollider* other) override;

    const _vec3&    Get_RawSize() const { return m_Size; }
    void            Set_RawSize(const _vec3& size) { m_Size = size; }

    _vec3           Get_Size();
    void            Set_Size(_vec3 size) { m_Size = size; }

public:
    static CBoxCollider* Create(DEVICE GraphicDev);
    CComponent*          Clone() override;

protected:
    void Free() override;

private:
    //CBoxCol* m_BufferCom;
    _vec3    m_Size;

};

END
