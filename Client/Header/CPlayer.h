#pragma once

#include "CCharacterBase.h"

class CPlayerSkill;
class CInventory;
class CUIInventory;
class CPlayerController;
class CPlayerMovement;
class CPlayerAttack;
class CPlayerInput;
class CWeaponEquip;
class CPlayerGathering;
class CPlayerConstruction;
class CUI_Interactive;

BEGIN(Engine)
class CRigidbody;
class CTexture;
END

class CPlayer : public CCharacterBase
{
private:
    explicit CPlayer(DEVICE graphicDev);
    explicit CPlayer(const CPlayer& rhs);
    ~CPlayer() override;

public:
    HRESULT Ready_GameObject() override;
    void    Initialize_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;

    void    OnBeginOverlap(CCollider* self, CCollider* other) override;
    void    OnStayOverlap(CCollider* self, CCollider* other) override;
    void    OnEndOverlap(CCollider* self, CCollider* other) override;

    void    Render_Editor() override;

private:
    HRESULT Add_Component() override;
    void    Animation_Setting() override;

private:
#pragma region Engine Components
    Engine::CRigidbody* m_RigidbodyCom;

#pragma endregion
#pragma region Client Components
    CWeaponEquip*      m_WeaponEquipCom;
    CInventory*        m_InventoryCom;
    CPlayerController* m_PlayerControllerCom;
    CPlayerInput*      m_PlayerInputCom;
    CPlayerMovement*   m_PlayerMovementCom;
    CPlayerAttack*     m_PlayerAttackCom;
    CPlayerSkill*      m_PlayerSkillCom;
    CPlayerGathering*  m_PlayerGatherCom;
    CPlayerConstruction* m_PlayerConstructionCom;

#pragma endregion

    map<ACTORSTATE, wstring> m_StateAnim;

public:
    static CPlayer* Create(DEVICE graphicDev);

private:
    void Free() override;

private:
    CUI_Interactive* m_InteractiveUI;
    _bool           m_OverlapBegin;
};
