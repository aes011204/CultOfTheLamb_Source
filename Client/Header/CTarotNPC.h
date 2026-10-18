#pragma once
#include "CCharacterBase.h"

class CTarotStageController;
BEGIN(Engine)
class CTexture;
END

class CTarotNPC : public CCharacterBase
{
private:
    explicit CTarotNPC(DEVICE graphicDev);
    explicit CTarotNPC(const CTarotNPC& rhs);
    ~CTarotNPC() override;

protected:
    void Render_Reset() override;

public:
    HRESULT Ready_GameObject() override;
    void    Initialize_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;
    bool    Is_Hovering();
    void    Set_TarotController(CTarotStageController* tarotStageController) { m_TarotStageController = tarotStageController; }
    void    OnBeginOverlap(CCollider* self, CCollider* other) override;
    void    OnEndOverlap(CCollider* self, CCollider* other) override;
    void OnStayOverlap(CCollider* self, CCollider* other) override;
    bool GetPlayerOverlap() { return m_bPlayerOverlap; }
     static CTarotNPC*   Create(DEVICE graphicDev);

protected:
    void    Animation_Setting() override;
    HRESULT Add_Component() override;
    void    Render_Setting() override;
    void    Free() override;


private:
    bool  m_bPlayerOverlap;
    bool m_bHovering;
    CTarotStageController* m_TarotStageController;
};


