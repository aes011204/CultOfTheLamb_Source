#pragma once
#include "CComponent.h"

class CWorldInteractive;
class CConstructCircle;
class CDroppedItem;

/// @brief 상호작용 오브젝트 감지기 용도의 스태틱 컴포넌트
/// @details
/// - 추종자 NPC 주변의 상호작용 가능한 오브젝트(바위, 나무, 건물 등)를 탐색한다.
/// - 매 프레임 또는 요청 시 주변의 CWorldInteractive 객체들을 검사하여 가장 가까운 대상을 반환한다.
class CInteractionChecker : public CComponent
{
private:
    explicit CInteractionChecker(DEVICE graphicDev);
    explicit CInteractionChecker(const CInteractionChecker& rhs);
    ~CInteractionChecker() override;

public:
    HRESULT Ready_InteractionChecker();
    void    Initialize_Component() override;
    _int    Update_Component(const _float& timeDelta) override;
    void    LateUpdate_Component() override;

public:
    // 기준 위치(srcPos)에서 가장 가까운 건설/건축물(Construction)을 반환한다.
    CConstructCircle* Get_NearestConstruction(const _vec3& srcPos);

    // 기준 위치(srcPos)에서 가장 가까운 나무(Tree)를 반환한다.
    CWorldInteractive* Get_NearestTree(const _vec3& srcPos);

    // 기준 위치(srcPos)에서 가장 가까운 바위(Rock)를 반환한다.
    CWorldInteractive* Get_NearestRock(const _vec3& srcPos);

    // 기준 위치(srcPos)에서 가장 가까운 음식(Food)을 반환한다.
    CDroppedItem* Get_NearestFood(const _vec3& srcPos);

public:
    static CInteractionChecker* Create(DEVICE graphicDev);
    CComponent*                 Clone() override;

protected:
    void Free() override;
};
