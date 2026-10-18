#pragma once
#include "CWorldInteractive.h"
class CHarvestableHerb :
    public CWorldInteractive
{
public:
    enum BERRY { BIGBERRY, SMALLBERRY, END_BERRY };
protected:

    explicit CHarvestableHerb(DEVICE graphicDev, BERRY Berry);
    explicit CHarvestableHerb(const CHarvestableHerb& rhs);
    virtual ~CHarvestableHerb();

public:
    HRESULT Ready_GameObject() override;
    void    Initialize_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;

    void    OnInteractiveDamaged() override;

protected:
    virtual HRESULT Add_Component();

    void    OnBeginOverlap(CCollider* self, CCollider* other) override;
    void    OnEndOverlap(CCollider* self, CCollider* other) override;


protected:
    BERRY m_Berry;
    _int m_deadTexIndex;
    _int m_TexIndex1;
    _int m_TexIndex2;

    _int m_CurTexIndex;

    _int m_ItemNum;



public:
    static CHarvestableHerb* Create(DEVICE graphicDev, BERRY berryType);

protected:

    void Free() override;

};

