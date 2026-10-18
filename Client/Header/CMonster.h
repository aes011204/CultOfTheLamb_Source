#pragma once
#include "CGameObject.h"

BEGIN(Engine)
class CTransform;
class CTriCol;
class CRectCollider;
END

class CMonster : public CGameObject
{
private:
    explicit CMonster(DEVICE graphicDev);
    explicit CMonster(const CMonster& rhs);
    ~CMonster() override;

public:
    HRESULT Ready_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;

    void OnBeginOverlap(CCollider* self, CCollider* other) override;
    void OnEndOverlap(CCollider* self, CCollider* other) override;

private:
    HRESULT Add_Component();

private:
    Engine::CTriCol*       m_BufferCom;
    Engine::CTransform*    m_TransformCom;
    Engine::CRectCollider* m_RectColCom;

public:
    static CMonster* Create(DEVICE graphicDev);

private:
    void Free() override;
};
