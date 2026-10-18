#pragma once
#include "CUI.h"
#include "CInventory.h"

// TAB 누를 시 나오는 창을 관리하는 최상위 패널
// 버튼 UI 생성/관리, 페이지 생성/관리, 인벤토리 UI 관리

namespace Engine
{
class CCombatStat;
}

class CUIPlayerPage;
class CUIInventory;
class CUIButton;


enum class UITabCategory
{
    INVENTORY,
    PLAYER
};

class CUITabMenu : public CUI
{
    public:
        explicit CUITabMenu(DEVICE graphicDev);
        explicit CUITabMenu(const CUI& rhs);
        ~CUITabMenu() override;

    public: 
        virtual HRESULT Ready_GameObject() override;
        virtual _int    Update_GameObject(const _float& timeDelta) override;
        virtual void    Render_GameObject() override;
        void LateUpdate_GameObject(const _float& timeDelta) override;

        void SetInventory(CInventory* inventory);
        HRESULT Add_Component();
        static CUITabMenu* Create(DEVICE graphicDev);
        void ChangePage(UITabCategory newTab);
        void Set_PlayerCombatStat(CCombatStat* combatStat){m_PlayerCombatStatCom = combatStat;}
        CCombatStat* Get_PlayerCombatStat() { return m_PlayerCombatStatCom; }
        void Initialize();
protected:
        void Free() override;

    private:
        UITabCategory   m_Current;
        CUIButton* m_InvenBtn;
        CUIButton* m_PlayerBtn;
        CUIButton* m_GyodanBtn;
        CUIButton* m_QuestBtn;
        CInventory* m_Inventory;
        CUIInventory* m_InventoryPage;
        CUIPlayerPage* m_PlayerPage;
        CCombatStat* m_PlayerCombatStatCom;
        CTransform* m_InvenTransformCom;
        CTransform* m_InvenBtnTransformCom;
        CTransform* m_PlayerBtnTransformCom;
        CTransform* m_PlayerPageTransformCom;
        CTransform* m_GyodanBtnTransformCom;
        CTransform* m_QuestBtnTransformCom;

        float m_TargetX;
        float m_ClosedX;
        float m_Speed;
        bool m_IsOpen;
        bool m_IsAnimating;
};

