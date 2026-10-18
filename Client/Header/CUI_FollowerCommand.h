#pragma once
#include "CUI.h"

namespace Engine
{
class CBoxCollider;
class CAnimator;
}

class CNPCBase;
class CUIButton;

class CUI_FollowerCommand : public CUI
{
private:
    CUI_FollowerCommand(DEVICE graphicDev);
    CUI_FollowerCommand(const CUI_FollowerCommand& rhs);
    ~CUI_FollowerCommand() override;

public:
    HRESULT Ready_GameObject() override;
    void    Initialize_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;

    void OnBeginOverlap(CCollider* self, CCollider* other) override;
    void OnStayOverlap(CCollider* self, CCollider* other) override;
    void OnEndOverlap(CCollider* self, CCollider* other) override;

public:
    _bool Get_Active() { return m_Active; }

    void Set_TargetNPC(CNPCBase* npc) { m_TargetNPC = npc; }

public:
    static CUI_FollowerCommand* Create(DEVICE graphicDev);

protected:
    HRESULT Add_Component();
    void    Free() override;

private:
    CUIButton* m_PrayButton;
    CUIButton* m_MiningButton;
    CUIButton* m_AxeButton;

    CNPCBase* m_TargetNPC;
};
