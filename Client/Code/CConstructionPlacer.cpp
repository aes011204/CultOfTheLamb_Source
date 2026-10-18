#include "pch.h"
#include "CConstructionPlacer.h"

#include "CBuildingBase.h"
#include "CConstructCircle.h"
#include "CGameObject.h"
#include "CKitchen.h"
#include "Client_Struct.h"
#include "CManagement.h"
#include "CPickingHelper.h"
#include "CScene.h"
#include "CTemple.h"
#include "CTransform.h"
#include "CVillageStateMgr.h"

CConstructionPlacer::CConstructionPlacer(DEVICE graphicDev)
    : m_GraphicDev(graphicDev),

      m_CurPickPos(0.f, 0.f, 0.f),
      m_PreviewOffsetY(0.f)
{
    m_GraphicDev->AddRef();
}

CConstructionPlacer::~CConstructionPlacer()
{ }

void CConstructionPlacer::Update_Placer(const _float& fTimeDelta)
{
    NULL_CHECK(m_PreviewObj);
    NULL_CHECK(m_PreviewCircle);

    // Picking Logic Test
    if (Engine::PickPlaneY(m_GraphicDev, g_hWnd, 0.f, m_CurPickPos))
    {
        // Check Validation
        m_CanPlace = Check_ValidPosition(&m_CurPickPos);

        _vec3 pickPos = m_CurPickPos;

        pickPos.y += m_PreviewOffsetY;

        // Update Preview Object Transform
        auto previewTransform = dynamic_cast<CTransform*>(
            m_PreviewObj->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));

        if (previewTransform)
            previewTransform->Set_Pos(pickPos);

        auto circleTransform = dynamic_cast<CTransform*>(
            m_PreviewCircle->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));

        if (circleTransform)
            circleTransform->Set_Pos(m_CurPickPos + _vec3(0.f, 0.2f, -0.5f));
    }
    else
    {
        // Check Validation
        m_CanPlace = false;
    }

    m_PreviewObj->Update_GameObject(fTimeDelta);
    m_PreviewCircle->Update_GameObject(fTimeDelta);
}

void CConstructionPlacer::Render_Preview()
{
    NULL_CHECK(m_PreviewObj);
    NULL_CHECK(m_PreviewCircle);

#pragma region --- RenderState 변경 ---
    m_GraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    m_GraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

    // Alpha Blending 켜기
    m_GraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_GraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_GraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    // [알파 혼합] Texture * TFactor (투명도)
    m_GraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
    m_GraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(128, 255, 255, 255));
#pragma endregion

    // 프리뷰 오브젝트 렌더
    m_PreviewObj->Render_GameObject();

#pragma region 프리뷰용 원은 상황에 따라 색상 변경
    m_GraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);

    // TFactor 설정 (Green/Red)
    DWORD previewColor = m_CanPlace
                         ? D3DCOLOR_ARGB(255, 64, 255, 64)
                         : D3DCOLOR_ARGB(255, 255, 64, 64);
    m_GraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, previewColor);
#pragma endregion

    // 프리뷰 범위 표시용 원 렌더
    m_PreviewCircle->Render_GameObject();

#pragma region --- 설정 복구 ---
    // 복구 순서 중요: 변경한 것들은 모두 원상복구 해야 함
    m_GraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
    m_GraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    m_GraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE); // 보통 켜둠

    // Texture Stage 복구 (기본값: Modulate / Texture / Current)
    m_GraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT); // TFACTOR -> CURRENT로 복구

    m_GraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    m_GraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(255, 255, 255, 255));
#pragma endregion
}

void CConstructionPlacer::Set_Preview(BUILDING_TYPE type)
{
    if (m_CurBuildingType == type && m_PreviewObj != nullptr)
        return;

    m_CanPlace = false;

    // 기존 미리보기 제거
    Safe_Release(m_PreviewObj);
    m_PreviewObj = nullptr;

    Safe_Release(m_PreviewCircle);
    m_PreviewCircle = nullptr;

    m_CurBuildingType = type;

    switch (m_CurBuildingType)
    {
    case BUILDING_TYPE::TEMPLE:
    {
        NULL_CHECK(m_PreviewObj = CTemple::Create(m_GraphicDev));

        // 프리뷰 원 생성
        NULL_CHECK(m_PreviewCircle =
            CConstructCircle::Create(m_GraphicDev, _vec3(4.f, 2.8f, 1.f)));

        m_PreviewOffsetY = 2.0f;
    }
    break;

    case BUILDING_TYPE::KITCHEN:
    {
        NULL_CHECK(m_PreviewObj = CKitchen::Create(m_GraphicDev));

        // 프리뷰 원 생성
        NULL_CHECK(m_PreviewCircle =
            CConstructCircle::Create(m_GraphicDev, _vec3(3.5f, 2.5f, 1.f)));

        m_PreviewOffsetY = 0.6f;
    }
    break;

    default: break;
    }

    dynamic_cast<CBuildingBase*>(m_PreviewObj)->Set_Preview(true);
    dynamic_cast<CBuildingBase*>(m_PreviewCircle)->Set_Preview(true);
    m_PreviewObj->Initialize_GameObject();
    m_PreviewCircle->Initialize_GameObject();

    // 초기 위치 설정
    auto previewTransform = dynamic_cast<CTransform*>(
        m_PreviewObj->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));

    if (previewTransform)
        previewTransform->Set_Pos(m_CurPickPos);

    auto circleTransform = dynamic_cast<CTransform*>(
        m_PreviewCircle->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));

    if (circleTransform)
        circleTransform->Set_Pos(m_CurPickPos);
}

_bool CConstructionPlacer::Try_PlaceObject()
{
    if (!m_CanPlace)
        return false;

    _vec3 scale = { 1.f, 1.f, 1.f };

    switch (m_CurBuildingType)
    {
    case BUILDING_TYPE::TEMPLE:
        scale = _vec3(4.f, 2.8f, 1.f);
        break;

    case BUILDING_TYPE::KITCHEN:
        scale = _vec3(3.5f, 2.5f, 1.f);
        break;

    default: break;
    }

    CConstructCircle* site = CConstructCircle::Create(m_GraphicDev, scale);
    NULL_CHECK_RETURN(site, false);

    site->Setup_Construction(m_CurBuildingType);
    site->Initialize_GameObject();

    // 위치 설정
    auto siteTransform = dynamic_cast<CTransform*>(
        site->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));

    // Preview와 동일한 위치 오프셋 적용
    _vec3 placePos = m_CurPickPos + _vec3(0.f, 0.2f, -0.5f);
    siteTransform->Set_Pos(placePos);

    // 씬에 추가
    CScene* curScene = CManagement::GetInstance()->Get_Scene();
    if (curScene)
    {
        // Construction Site는 GAMELOGIC 레이어
        curScene->AddObjectOnLayer(LAYERTYPE::GAMELOGIC, site, OBJTYPE::CONSTRUCTION_SITE);

        // 월드 상태 저장 (아직 건설 전)
        SavedBuilding savedInfo;
        savedInfo.pos     = placePos;  // 이 위치는 실제 Site의 위치
        savedInfo.type    = m_CurBuildingType;
        savedInfo.isBuilt = false;

        CVillageStateMgr::GetInstance()->Save_Building(savedInfo);
    }
    else
    {
        Safe_Release(site);
        return false;
    }

    // TODO: 인벤토리 차감 등 후처리

    return true;
}

_bool CConstructionPlacer::Check_ValidPosition(_vec3* pOutPos)
{
    if (pOutPos->x >= 4.5f && pOutPos->x <= 6.f &&
        pOutPos->z >= 23.f && pOutPos->z <= 27.f)
        return true;

    if (pOutPos->x >= 11.5f && pOutPos->x <= 13.f &&
        pOutPos->z >= 30.f && pOutPos->z <= 32.5f)
        return true;

    if (pOutPos->x >= 23.f && pOutPos->x <= 25.5f &&
        pOutPos->z >= 30.f && pOutPos->z <= 32.5f)
        return true;

    return false;
}

void CConstructionPlacer::End_Preview()
{
    // 기존 미리보기 제거
    Safe_Release(m_PreviewObj);
    m_PreviewObj = nullptr;

    Safe_Release(m_PreviewCircle);
    m_PreviewCircle = nullptr;
}

CConstructionPlacer* CConstructionPlacer::Create(DEVICE graphicDev)
{
    auto constructionPlacer = new CConstructionPlacer(graphicDev);

    NULL_CHECK_RETURN_MSG(constructionPlacer,
                          nullptr,
                          L"ConstructionPlacer Create Failed");

    return constructionPlacer;
}

void CConstructionPlacer::Free()
{
    End_Preview();

    Safe_Release(m_GraphicDev);
}
