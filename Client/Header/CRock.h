#pragma once
#include "CWorldInteractive.h"

class CRock : public CWorldInteractive
{
public:
    enum ROCK { BIGROCK, MIDDLEROCK, SMALLROCK, END_ROCK };

protected:
    explicit CRock(DEVICE graphicDev, ROCK rock);
    explicit CRock(const CRock& rhs);
    ~CRock() override;

public:
    void OnInteractiveDamaged() override;

public:
    HRESULT Ready_GameObject() override;
    void    Initialize_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;

    void KnockBackEff(const _float& timeDelta);
protected:
    HRESULT Add_Component() override;

    void OnBeginOverlap(CCollider* self, CCollider* other) override;
    void OnEndOverlap(CCollider* self, CCollider* other) override;

protected:
    ROCK m_rock;
    _int m_deadTexIndex;
    _int m_TexIndex1;
    _int m_TexIndex2;

    _int m_CurTexIndex;

    _int m_ItemNum;

    // 넉백
    bool  m_OnDamage = false;
    _vec3 m_OrignPos;
    float m_KockBackTimer = 0;
    bool  m_KockDone      = false;
    bool  m_ReturnDone    = false;
    _vec3 m_MovedPos{ 0.f, 0.f, 0.f };
    float LerpTime = 0;

public:
    static CRock* Create(DEVICE graphicDev, ROCK rock);

protected:
    void Free() override;
};
