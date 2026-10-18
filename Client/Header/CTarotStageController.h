#pragma once
#include "CGameObject.h"

class CUITarotDescCard;
class CUIButton;
class CUITarotCard;
class CUI_Dialogue;

enum class STAGESTATE
{
    NONE,
    DIALOGUE,
    CARD,
    ENDING,
    THE_END
};

class CTarotStageController : public CGameObject
{
private:
    explicit CTarotStageController(DEVICE graphicDev);
    explicit CTarotStageController(const CTarotStageController& rhs);
    ~CTarotStageController() override;

public:
    void    Render_Editor() override;
    HRESULT Ready_GameObject() override;
    void    Initialize_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;
    void    Set_Active(bool isActive) { m_Active = isActive; }

    static CTarotStageController* Create(DEVICE graphicDev);
    void Create_Cards();

    HRESULT Add_Component();

protected:
    void Free() override;

private:
    STAGESTATE             m_CurrentState;
    CUITarotCard* m_TarotCard1;
    CUITarotCard* m_TarotCard2;
    CUITarotDescCard* m_ExplainCard1;
    CUITarotDescCard* m_ExplainCard2;
    CUI_Dialogue* m_Dialogue;
    _float                 m_WaitingTime = 5.f;

    _bool                  m_DialogueStarted = false;
    _bool                  m_DialogueEnded = false;
    _bool                  m_SoundEnded = false;
    _bool                  m_IsChoiced = false;
};
