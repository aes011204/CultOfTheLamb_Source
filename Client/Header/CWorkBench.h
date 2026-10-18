#pragma once
#include "CBuildingBase.h"

class CUIBuildMenu;
class CSpriteEff;

/// @brief 제작대 오브젝트
/// @details 플레이어와 충돌 중 E 키 상호작용으로 건축 UI 띄울 예정
class CWorkBench : public CBuildingBase
{
private:
    explicit CWorkBench(DEVICE graphicDev);
    explicit CWorkBench(const CWorkBench& rhs);
    ~CWorkBench() override;

public:
    HRESULT Ready_GameObject() override;
    void    Initialize_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;

public:
    void Render_Editor() override;
    void Set_BuildMenu(CUIBuildMenu* buildMenu) { m_BuildMenu = buildMenu; }

public:
    void OnBeginOverlap(CCollider* self, CCollider* other) override;
    void OnStayOverlap(CCollider* self, CCollider* other) override;
    void OnEndOverlap(CCollider* self, CCollider* other) override;

private:
    HRESULT Add_Component() override;

public:
    static CWorkBench* Create(DEVICE graphicDev);

private:
    void Free() override;

private:
    CSpriteEff*         m_SpriteEffCom;
    CUIBuildMenu*       m_BuildMenu;
};
