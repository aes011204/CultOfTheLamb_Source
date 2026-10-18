#pragma once
#include "CRenderObject.h"

BEGIN(Engine)
class CBoxCollider;
END

class CSpriteEff;

// 건축물 오브젝트의 베이스
class CBuildingBase : public CRenderObject
{
protected:
    explicit CBuildingBase(DEVICE graphicDev);
    explicit CBuildingBase(const CBuildingBase& rhs);
    ~CBuildingBase() override;

public:
    HRESULT Ready_GameObject() override;
    void    Initialize_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;

    virtual bool Is_Hovering();

public:
    void OnBeginOverlap(CCollider* self, CCollider* other) override;
    void OnStayOverlap(CCollider* self, CCollider* other) override;
    void OnEndOverlap(CCollider* self, CCollider* other) override;

public:
    void Set_Preview(bool preview) { m_IsPreview = preview; }

protected:
    HRESULT Add_Component() override;
    void    Free() override;

protected:
    Engine::CBoxCollider* m_BoxCollCom = nullptr;
    CSpriteEff*           m_SpriteEff  = nullptr;

    _bool m_IsPreview      = false;
    _bool m_bPlayerOverlap = false;
    _bool m_bHovering      = false;
    _bool m_OutlineEnabled = true;
};
