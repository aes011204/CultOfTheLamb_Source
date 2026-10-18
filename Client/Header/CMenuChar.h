#pragma once
#include "CGameObject.h"

BEGIN(Engine)
class CAnimator;
class CTexture;
class CState;
class CTransform;
class CRcTex;
class CBoxCollider;
class CCombatStat;
class CState;
END

class CMenuChar : public CGameObject
{
private:
    explicit CMenuChar(DEVICE graphicDev);
    explicit CMenuChar(const CMenuChar& rhs);
    ~CMenuChar() override;

public:
    HRESULT Ready_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;

private:
    void    Animation_Setting();
    HRESULT Add_Component();

    void Render_Setting();
    void Render_Reset();

public:
    void OnBeginOverlap(CCollider* self, CCollider* other) override;
    void OnEndOverlap(CCollider* self, CCollider* other) override;

private:
    CRcTex*     m_BufferCom;
    CTransform* m_TransformCom;
    CTexture*   m_TextureCom;
    CAnimator*  m_AnimatorCom;
    CState*     m_StateCom;

    //바닥 문양용
    CRcTex* m_BufferComPattern;
    CTexture* m_TextureComPattern;
    CTransform* m_TransformComPattern;

public:
    static CMenuChar* Create(DEVICE graphicDev);

private:
    void Free() override;
};
