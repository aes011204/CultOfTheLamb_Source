#pragma once

#include "CBase.h"

class CUI_Fade;

class CSceneMgr : public CBase
{
    DECLARE_SINGLETON(CSceneMgr)

private:
    explicit CSceneMgr();
    ~CSceneMgr() override;

public:
    virtual HRESULT Ready_SceneMgr(DEVICE pGraphicDev);
    virtual void    Change_SceneMgr(SCENETYPE typeInput);

    void            Update_SceneMgr(const _float& timeDelta);
    void            Render_SceneMgr();

    void            StartFadeIn(_float speed = 2.0f);
    void            StartFadeOut(_float speed = 2.0f);

    // Fade 중인지 확인
    _bool           Is_Changing() const { return m_IsChanging; }

    SCENETYPE       Get_SceneType() { return m_SceneType; }

private:
    void        Free() override;

private:
    DEVICE      m_GraphicDev;
    SCENETYPE   m_SceneType;

    CUI_Fade*   m_FadeUI;
    SCENETYPE   m_PendingScene;
    _float      m_FadeSpeed;
    _bool       m_IsChanging;      
    _bool       m_FadeOutComplete; 
};
