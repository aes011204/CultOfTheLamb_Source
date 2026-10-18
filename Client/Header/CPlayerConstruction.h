#pragma once
#include "CComponent.h"

class CUI_ProgressBar;
BEGIN(Engine)
class CCollider;
END

class CConstructCircle;

class CPlayerConstruction : public CComponent
{
private:
    explicit CPlayerConstruction(DEVICE graphicDev);
    explicit CPlayerConstruction(const CPlayerConstruction& rhs);
    ~CPlayerConstruction() override;

public:
    HRESULT Ready_PlayerConstruction();
    void    Initialize_Component() override;
    _int    Update_Component(const _float& timeDelta) override;
    void    LateUpdate_Component() override;

public:
    // Interaction Methods
    void Action_Construction(const _float& timeDelta);

    // Collision Callbacks
    void OnOverlapEnter(CCollider* self, CCollider* other);
    void OnOverlapExit(CCollider* self, CCollider* other);

    // GettersG
    CConstructCircle* Get_ConstructZone() const { return m_pConstructZone; }
    void Set_ProgressBar(CUI_ProgressBar* progressui) { m_ProgressBar = progressui; }
    void Set_IsActive(_bool active) { m_Active = active; }

    void BuildingCompleted();

private:
    CConstructCircle* m_pConstructZone = nullptr;

public:
    static CPlayerConstruction* Create(DEVICE graphicDev);
    CComponent* Clone() override;

    void Free() override;

private:
    CUI_ProgressBar* m_ProgressBar = nullptr;
    _bool m_IsCompleted = false;
};
