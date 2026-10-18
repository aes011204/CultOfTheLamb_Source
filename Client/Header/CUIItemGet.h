#pragma once
#include "CUI.h"
#include "CUIIcon.h"
#include "ItemInstance.h"

typedef struct UIItem
{
    int   itemId;
    float remainTime;
    int   count;
    int   newcount;
};

class CUIIcon;

class CUIItemGet : public CUI
{
private:
    explicit CUIItemGet(DEVICE graphicDev);
    explicit CUIItemGet(const CUIItemGet& rhs);
    ~CUIItemGet() override;

public:
    HRESULT Ready_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;
    HRESULT Add_Component();
    //void    Render_Setting();
    //void    Render_Reset();
    void    Set_UIItem(int itemId, int itemCount);
    void    Set_Index(int index) { m_Index = index; }

    UIItem  Get_UIItem() { return m_UIItem; }
    bool    Get_IsDead() { return m_IsDead; }
    bool    Get_IsShifting() { return m_IsShifting; }
    static CUIItemGet* Create(DEVICE graphicDev);
    void               Render_Editor() override;

private:
    CUIIcon* m_UIItemIcon;
    UIItem   m_UIItem;
    bool     m_IsOpen;
    bool     m_IsShifting;
    bool     m_CloseAnimation;
    bool     m_OpenAnimaiton;
    bool     m_IsDead;
    float    m_TargetX;
    float    m_ClosedX;
    float    m_Speed;
    int      m_Index;

private:
    void Free() override;
};
