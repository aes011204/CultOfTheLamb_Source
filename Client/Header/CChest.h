#pragma once
#include "CRenderObject.h"

BEGIN(Engine)
class CAnimator;
class CState;
class CCombatStat;
class CBoxCollider;
END

class CInventory;

class CChest :
    public CRenderObject
{
public:
    enum class CHESTSTATE {REVEAL,OPEN,CHEST_END};
protected:

    explicit CChest(DEVICE graphicDev, _vec3 pos);
    explicit CChest(const CChest& rhs);
    ~CChest() override;

public:
    HRESULT Ready_GameObject() override;
    void    Initialize_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;

    bool AddItem(int id, int Num);

protected:
    HRESULT Add_Component() override;

    void Animation_Setting();
    void OnBeginOverlap(CCollider* self, CCollider* other) override;
    void OnEndOverlap(CCollider* self, CCollider* other) override;

    bool Get_Hovering() { return m_bHovering; }


protected:
    //m_TransformCom, m_BufferCom, m_TextureCom 부모에 있음, 텍스쳐 만 자식에서 추가 설정해줘야 함
    Engine::CAnimator* m_AnimatorCom;

    Engine::CCombatStat* m_CombatStatCom;
    Engine::CBoxCollider* m_BoxColCom;

    CInventory* m_InventoryCom;

    bool m_bDead = false;
private:
    // 상태용
    bool m_bPlayerOverlap = false;
    bool m_bHovering = false;

    bool m_RevealDone = false;
    bool m_bInvenDropped = false;

    _vec3 m_Pos;
public:

    static CChest* Create(DEVICE graphicDev,_vec3 pos);

    CHESTSTATE m_chest = CHESTSTATE::CHEST_END;

protected:
    void Free() override;
};

