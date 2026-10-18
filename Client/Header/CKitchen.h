#pragma once
#include "CBuildingBase.h"

class CUICookingMenu;
class CCookingController;

/// @brief 요리용 화덕 오브젝트
/// @details 플레이어와 충돌 중 E키 상호작용으로 요리 UI 띄울 예정
class CKitchen : public CBuildingBase
{
private:
    explicit CKitchen(DEVICE graphicDev);
    explicit CKitchen(const CKitchen& rhs);
    ~CKitchen() override;

public:
    HRESULT Ready_GameObject() override;
    void    Initialize_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;

public:
    void Set_CookingMenu(CUICookingMenu* cookingMenu) { m_CookingMenu = cookingMenu; }

public:
    void Render_Editor() override;

public:
    void OnBeginOverlap(CCollider* self, CCollider* other) override;
    void OnStayOverlap(CCollider* self, CCollider* other) override;
    void OnEndOverlap(CCollider* self, CCollider* other) override;

private:
    HRESULT Add_Component() override;

public:
    static CKitchen* Create(DEVICE graphicDev);

private:
    void Free() override;

private:
    CCookingController* m_CookingCtrlCom;
    CUICookingMenu*     m_CookingMenu;
};
