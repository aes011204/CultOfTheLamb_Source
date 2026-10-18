#pragma once
#include "CComponent.h"
#include "CCombatStat.h"

class CUI_ProgressBar;
class CWorldInteractive;

namespace Engine
{
    class CCollider;
    class CState;
    class CombatStat;
}
class CWorldInteractive;

class CPlayerGathering :
    public CComponent
{
private:
    explicit CPlayerGathering(DEVICE graphicDev);
    explicit CPlayerGathering(const CPlayerGathering& rhs);
    ~CPlayerGathering() override;

public:
    HRESULT Ready_PlayerGathering();
    void    Initialize_Component() override;
    _int    Update_Component(const _float& timeDelta) override;
    void    LateUpdate_Component() override;

public:
    void    OnBeginOverlap(CCollider* self, CCollider* other);
    void    OnEndOverlap(CCollider* self, CCollider* other);
    _float CalcProgress();
    CWorldInteractive* GetInteractiveObj() { return m_Target; }
    void Set_ProgressBar(CUI_ProgressBar* progressui) { m_ProgressBar = progressui; }
    /* CWorldInteractive::TYPE  GetInteractiveType() { return m_Target->GetInteractiveType(); }*/
private:
    CState* m_OwnerStateCom = nullptr;
    CAnimator* m_OwnerAnimationCom;
    CWorldInteractive* m_Target = nullptr;
    CUI_ProgressBar* m_ProgressBar = nullptr;
    float m_AttackTimer = 0;
    HITDATA h;
    bool m_AttackFlag = false;
    _float m_Progress;
public:
    static CPlayerGathering* Create(DEVICE graphicDev);
    CComponent* Clone() override;

private:
    void Free() override;

  
};

