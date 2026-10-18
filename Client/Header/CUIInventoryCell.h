#pragma once
#include "CUI.h"
#include "ItemInstance.h"

class CUIIcon;

class CUIInventoryCell : public CUI
{
private:
    explicit CUIInventoryCell(DEVICE pGraphicDev);
    explicit CUIInventoryCell(const CUIInventoryCell& rhs);
    ~CUIInventoryCell() override;

public:
    HRESULT Ready_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;
    void    Render_Editor() override;

    void OnBeginOverlap(CCollider* self, CCollider* other) override;
    void OnEndOverlap(CCollider* self, CCollider* other) override;
    
    HRESULT Add_Component();

    static CUIInventoryCell* Create(DEVICE graphicDev);

    void    Set_IsHovered(bool tf) { m_IsHovered = tf; }
    bool    Get_IsHovered() { return m_IsHovered; }
    void SetInventoryCell(const ItemInstance& itemInstance, int count)
    {
        m_ItemInst = itemInstance;
        m_Count    = count;
    }

    CUIIcon* Get_Icon() { return m_ItemIcon; }

protected:
    void Free() override;

private:
    CUIIcon*        m_ItemIcon;
    CUIIcon*        m_CardIcon;
    CUIIcon*        m_CardItemIcon;
    CTexture*       m_HighlightTexCom;
    ItemInstance    m_ItemInst;
    D3DXCOLOR       m_D3dxColor;
    int             m_Count;
    bool            m_IsHovered; //외곽선 그리기 여부 판단용
};
