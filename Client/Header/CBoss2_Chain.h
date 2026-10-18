#pragma once
#include "CGameObject.h"

BEGIN(Engine)
class CRcTex;
class CTransform;
class CTexture;
class CAnimator;
class CBoxCollider;
class CCombatStat;
class CAttackBox;
END

class CBoss2_Chain : public CGameObject
{

private:
    explicit CBoss2_Chain(DEVICE graphicDev, CGameObject* owner);
    explicit CBoss2_Chain(const CBoss2_Chain& rhs);
    virtual ~CBoss2_Chain();

public:

    virtual HRESULT Ready_GameObject() override;
    virtual _int    Update_GameObject(const _float& timeDelta) override;
    virtual void    LateUpdate_GameObject(const _float& timeDelta) override;
    virtual void    Render_GameObject() override;

    virtual void    OnBeginOverlap(CCollider* self, CCollider* other) override;
    virtual void    OnEndOverlap(CCollider* self, CCollider* other) override;

    void            Animation_Setting();
    HRESULT         Add_Component();
public:

    static CBoss2_Chain* Create(DEVICE graphicDev, CGameObject* owner);

protected:

    CRcTex*         m_BufferCom;
    CTransform*     m_TransformCom;
    CTexture*       m_TextureCom;
    CAnimator*      m_AnimatorCom;
    CBoxCollider*   m_BoxColCom;
    CCombatStat*    m_CombatStatCom;

    CGameObject*    m_pOwner;
    bool            m_flag = false;

private:
    void Free() override;


};

