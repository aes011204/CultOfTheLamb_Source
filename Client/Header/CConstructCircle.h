#pragma once
#include "CBuildingBase.h"

class CConstructCircle : public CBuildingBase
{
private:
    explicit CConstructCircle(DEVICE graphicDev);
    explicit CConstructCircle(const CConstructCircle& rhs);
    ~CConstructCircle() override;

public:
    HRESULT Ready_GameObject() override;
    void    Initialize_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;

public:
    void OnBeginOverlap(CCollider* self, CCollider* other) override;
    void OnStayOverlap(CCollider* self, CCollider* other) override;
    void OnEndOverlap(CCollider* self, CCollider* other) override;

public:
    void Setup_Construction(BUILDING_TYPE type);
    _float Get_Progress() { return m_Progress; }
private:
    HRESULT Add_Component() override;

public:
    // 사이즈가 지정 가능하도록 해서 건축물에 따라 다른 크기로 생성
    static CConstructCircle* Create(DEVICE graphicDev,
                                    _vec3  scale = { 1.f, 1.f, 1.f });

private:
    void Free() override;

private:
    BUILDING_TYPE m_TargetBuildingType = BUILDING_END;  // 건설될 건축물 타입
    _float        m_Progress           = 0.f;           // 건축 진행도
};
