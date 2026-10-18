#pragma once

#include "CUI.h"
#include "Client_Struct.h"

BEGIN(Engine)
class CRcTex;
class CTransform;
class CTexture;
END

class CUI_Dialogue : public CUI
{
private:
    explicit CUI_Dialogue(DEVICE graphicDev);
    explicit CUI_Dialogue(const CUI_Dialogue& rhs);
    virtual ~CUI_Dialogue();

public:
    virtual HRESULT Ready_GameObject() override;
    virtual void    Initialize_GameObject() override;
    virtual _int    Update_GameObject(const _float& timeDelta) override;
    virtual void    LateUpdate_GameObject(const _float& timeDelta) override;
    virtual void    Render_GameObject() override;

public:
    // 대화
    void    Start_Dialogue(const string& dialogueId,
                            const _vec2 boxPos = {0.f, 200.f},
                            const _vec2& boxSize = {300.f, 80.f});
    void    End_Dialogue();

    void    Next_Line();
    void    Select_Choice(_bool isLeft); // 선택지 선택 (노예, 살해)
    
    _bool   IsActive() const { return m_IsActive; }
    _bool   IsShowingChoices() const { return m_IsShowingChoices; }

public:
    // 대화 종료
    using   OnDialougeEnd = function<void(const string&)>;
    void    Set_OnDialogueEnd(OnDialougeEnd callback) { m_OnDialogueEnd = callback; }

    // 크기 설정
    void    Set_BoxPos(const _vec2& pos) { m_BoxPos = pos; }
    void    Set_BoxSize(const _vec2& size) { m_BoxSize = size; }

private:
    HRESULT Add_Component();

    void    Render_DialogueBox();
    void    Render_SpeakerName();
    void    Render_DialogueText();
    void    Render_Choices();

    void    Handle_Input();

public:
    static CUI_Dialogue* Create(DEVICE graphicDev);

private:
    virtual void Free() override;

private:
    CRcTex*         m_BufferCom;
    CTransform*     m_TransformCom;  
    CTexture*       m_TextureCom;

    // 대화 상태
    Dialogue*       m_CurrentDialogue;
    _int            m_CurrentLineIndex;
    _bool           m_IsActive;

    // 선택지
    _bool           m_IsShowingChoices;
    bool            m_SelectedLeft;

    _vec2           m_BoxPos;   
    _vec2           m_BoxSize;
    _vec2           m_TextOffset;
    _vec2           m_SpeakerOffset;        // 말하는놈 위치
    _vec2           m_ChoiceStartOffset;
    _float          m_ChoiceSpacing;        // 선택지 간격

    // 대화 종료
    OnDialougeEnd   m_OnDialogueEnd;

    // 타이밍 효과 추가해보기
    _float  m_TypeTimer;        // 글자 출력 타이머
    _float  m_TypeSpeed;        // 글자당 출력 속도
    _int    m_CurrentCharIndex; // 현재까지 보여줄 글자 수
    _bool   m_IsTypingComplete;

    _float  m_TextAlpha;
    _float  m_FadeSpeed;
    
};
