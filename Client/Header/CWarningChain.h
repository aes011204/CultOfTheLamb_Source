#pragma once
#include "CGameObject.h"

BEGIN(Engine)
class CRcTex;
class CTransform;
class CTexture;
class CAnimator;
class CBoxCollider;
class CCombatStat;
END

class CBossChain;

class CWarningChain :
    public CGameObject
{
private:
    explicit CWarningChain(DEVICE graphicDev, CGameObject* owner);
    explicit CWarningChain(const CWarningChain& rhs);
    virtual ~CWarningChain();
public:

    virtual HRESULT Ready_GameObject() override;
    float easeOutElastic(float x);
    virtual _int    Update_GameObject(const _float& timeDelta) override;
    virtual void    LateUpdate_GameObject(const _float& timeDelta) override;
    virtual void    Render_GameObject() override;

    virtual void OnBeginOverlap(CCollider* self, CCollider* other) override;
    virtual void OnEndOverlap(CCollider* self, CCollider* other) override;

    void Animation_Setting();
    HRESULT Add_Component();
public:

    static CWarningChain* Create(DEVICE graphicDev, CGameObject* owner);

protected:

    CRcTex* m_BufferCom;
    CTransform* m_TransformCom;
    CTexture* m_TextureCom;
    CBoxCollider* m_BoxColCom;
    CCombatStat* m_CombatStatCom;

    CGameObject* m_pOwner;

    bool m_bFade = false;
    float m_fAlpha = 1;
    float m_fElapsedTime = 0;

    //CTransform* m_ChainTransformCom  = nullptr;
    //CAnimator* m_ChainAnimatorCom = nullptr;
    //CBossChain* chain = nullptr;

    bool m_flag = false;
    bool m_Sizeflag = false;
    _vec3 m_originSize;
private:
    void Free() override;

};

