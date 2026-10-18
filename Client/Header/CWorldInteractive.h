#pragma once
#include "CRenderObject.h"

BEGIN(Engine)
class CAnimator;
class CState;
class CCombatStat;
class CBoxCollider;
END

class CInventory;
class CSpriteEff;

class CWorldInteractive : public CRenderObject
{
public:
    enum TYPE { ROCK, WOOD, BERRY, END_TYPE };

protected:
    explicit CWorldInteractive(DEVICE graphicDev);
    explicit CWorldInteractive(const CWorldInteractive& rhs);
    ~CWorldInteractive() override;

public:
    HRESULT Ready_GameObject() override;
    void    Initialize_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;

    TYPE GetInteractiveType() { return m_Type; }
    bool Get_Hovering() { return m_bHovering; }

    void Check_WorldState();

protected:
    HRESULT Add_Component() override;

    void OnBeginOverlap(CCollider* self, CCollider* other) override;
    void OnEndOverlap(CCollider* self, CCollider* other) override;

    bool Is_Hovering();
    virtual void OnInteractiveDamaged(); 

protected:
    //m_TransformCom, m_BufferCom, m_TextureCom 부모에 있음, 텍스쳐 만 자식에서 추가 설정해줘야 함
    //Engine::CAnimator* m_AnimatorCom;
    //Engine::CState* m_StateCom;
    Engine::CCombatStat*  m_CombatStatCom;
    Engine::CBoxCollider* m_BoxColCom;

    CInventory* m_InventoryCom;
    CSpriteEff* m_SpriteEffCom;

    bool m_bDead = false;
private:
    // 상태용
    bool m_bPlayerOverlap = false;
    bool m_bHovering      = false;

    bool m_bInvenDropped = false;

public:
    TYPE m_Type = END_TYPE;
protected:
    void Free() override;
};
