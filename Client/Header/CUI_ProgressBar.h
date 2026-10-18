#pragma once
#include "CUI.h"
BEGIN(Engine)
class CRcTex;
END
class CUI_ProgressBar : public CUI
{
private:
    explicit CUI_ProgressBar(DEVICE graphicDev);
    explicit CUI_ProgressBar(const CUI_ProgressBar& rhs);
    ~CUI_ProgressBar() override;

public:
    void Initialize_GameObject() override;
    HRESULT Ready_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;
    HRESULT    Add_Component();
    void    Set_Progress(_float progress) { m_Progress = progress; }

protected:
    void Free() override;

public:

    static CUI_ProgressBar* Create(DEVICE graphicDev);

private:
    _float m_Progress;
    CRcTex* m_ProgressBar;

private: // --- 플레이어 컴포넌트 캐싱 ---
    CTransform* m_PlayerTransform = nullptr;
};
