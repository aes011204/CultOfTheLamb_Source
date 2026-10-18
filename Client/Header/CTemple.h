#pragma once
#include "CBuildingBase.h"

/// @brief 사원 건물 오브젝트
/// @details 텍스처 0번은 사원 앞면, 1번은 사원 지붕
class CTemple : public CBuildingBase
{
private:
    explicit CTemple(DEVICE graphicDev);
    explicit CTemple(const CTemple& rhs);
    ~CTemple() override;

public:
    HRESULT Ready_GameObject() override;
    void    Initialize_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;

public:
    void Render_Editor() override;

public:
    void OnBeginOverlap(CCollider* self, CCollider* other) override;
    void OnStayOverlap(CCollider* self, CCollider* other) override;
    void OnEndOverlap(CCollider* self, CCollider* other) override;

private:
    HRESULT Add_Component() override;

public:
    static CTemple* Create(DEVICE graphicDev);

private:
    void Free() override;
};
