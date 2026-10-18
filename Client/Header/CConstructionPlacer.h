#pragma once
#include "CBase.h"

BEGIN(Engine)
class CGameObject;
END

class CConstructionPlacer : public CBase
{
private:
    explicit CConstructionPlacer(DEVICE graphicDev);
    ~CConstructionPlacer() override;

public:
    void Update_Placer(const _float& fTimeDelta);
    void Render_Preview();

    // 미리보기 오브젝트 설정
    void Set_Preview(BUILDING_TYPE type);

    // 배치 시도 (성공 시 true 반환)
    _bool Try_PlaceObject();

    // 현재 배치가 유효한지 확인 (주로 지형 위인지, 장애물 없는지)
    _bool Check_ValidPosition(_vec3* pOutPos = nullptr);

    // 사용 안하는 중
    void End_Preview();

public:
    static CConstructionPlacer* Create(DEVICE graphicDev);

private:
    void Free() override;

private:
    DEVICE m_GraphicDev;

    CGameObject* m_PreviewObj    = nullptr;         // 미리보기용 고스트 오브젝트
    CGameObject* m_PreviewCircle = nullptr;         // 프리뷰 범위 표시용 원

    BUILDING_TYPE m_CurBuildingType = BUILDING_END; // 현재 배치하려는 건물 종류

    _vec3  m_CurPickPos;                            // 현재 마우스 피킹 위치 저장
    _float m_PreviewOffsetY;                        // 미리보기 Y 오프셋
    _bool  m_CanPlace = false;                      // 현재 위치에 배치 가능한지 여부
};
