#pragma once
#include "CCharacterBase.h"

BEGIN(Engine)
class CRigidbody;
END

class CInteractionChecker;
class CNPCController;
class CUI_FollowerCommand;

// 모든 추종자 NPC들의 기본 베이스
class CNPCBase : public CCharacterBase
{
protected:
    explicit CNPCBase(DEVICE graphicDev);
    explicit CNPCBase(const CNPCBase& rhs);
    ~CNPCBase() override;

public:
    HRESULT Ready_GameObject() override;
    void    Initialize_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;

public:
    void OnBeginOverlap(CCollider* self, CCollider* other) override;
    void OnStayOverlap(CCollider* self, CCollider* other) override;
    void OnEndOverlap(CCollider* self, CCollider* other) override;

public:
    CInteractionChecker* Get_InteractionChecker() { return m_Checker; }
    CNPCController*      Get_Controller() { return m_NPCController; }
    CRigidbody*          Get_Rigidbody() { return m_Rigidbody; }

    void Set_RescuedState(_bool doRescuedState);

private:
    bool Is_Hovering();

protected:
    HRESULT Add_Component() override;
    void    Free() override;

protected:
protected:
    CInteractionChecker* m_Checker        = nullptr;
    CNPCController*      m_NPCController  = nullptr;
    CRigidbody*          m_Rigidbody      = nullptr;
    _bool                m_bHovering      = false;
    _bool                m_bPlayerOverlap = false;
};
