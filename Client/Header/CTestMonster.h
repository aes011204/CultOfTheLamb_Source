#pragma once
#include "CGameObject.h"

BEGIN(Engine)
class CTransform;
class CRcTex;
class CRectCollider;
class CBoxCollider;
class CTexture;
class CAnimator;
class CState;
END

class CTestMonster : public CGameObject
{
private:
    explicit CTestMonster(DEVICE graphicDev);
    explicit CTestMonster(const CTestMonster& rhs);
    ~CTestMonster() override;

public:
    HRESULT Ready_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;

    void Render_Setting();
    void Render_Reset();

    void OnBeginOverlap(CCollider* self, CCollider* other) override;
    void OnEndOverlap(CCollider* self, CCollider* other) override;

private:
    HRESULT Add_Component();
    void    Animation_Setting();

private:
    Engine::CRcTex* m_BufferCom;

    Engine::CTransform*   m_TransformCom;
    Engine::CTexture*     m_TextureCom;
    Engine::CAnimator*    m_AnimatorCom;
    Engine::CBoxCollider* m_BoxColCom;
    Engine::CState*       m_StateCom;

public:
    static CTestMonster* Create(DEVICE graphicDev);

private:
    void Free() override;
};
