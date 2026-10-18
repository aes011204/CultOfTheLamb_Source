#pragma once
#include "CGameObject.h"
#include "CLayer.h"
#include "CManagement.h"

BEGIN(Engine)
/// @brief Persistent 오브젝트를 조회해서, 존재하지 않을 경우 생성·등록까지 수행하는 함수
/// @tparam T         생성할 오브젝트 타입 (CGameObject 파생 클래스)
/// @param objType    레이어 등록 시 사용할 오브젝트 타입
/// @param key        매니지먼트 캐싱/조회 시 사용할 고유 키
/// @param graphicDev 그래픽 디바이스
/// @param layer      생성 또는 조회된 오브젝트를 추가할 레이어
/// @param outObject  결과로 얻은 오브젝트 포인터를 전달받는 out 파라미터
/// @return 성공 시 S_OK, 생성·등록 실패 또는 잘못된 인자일 경우 E_FAIL
template <typename T>
static HRESULT AcquirePersistentObject(OBJTYPE        objType,
                                       const wstring& key,
                                       DEVICE         graphicDev,
                                       CLayer*        layer,
                                       CGameObject**  outObject)
{
    const auto management = Engine::CManagement::GetInstance();

    // 1. 키 값으로 조회
    if (FAILED(management->Get_PersistentObject(key, outObject)))
    {
        // 2. 없으면 생성
        *outObject = T::Create(graphicDev);

        NULL_CHECK_RETURN_MSG(
            *outObject,
            E_FAIL,
            L"Obejct create failed");

        // 3. 레이어 추가 (OBJTYPE 사용)
        FAILED_CHECK_MSG(
            layer->Add_GameObject(objType, *outObject),
            L"Add outObject in layer has failed");

        // 4. 매니지먼트 캐싱 (키 값 사용)
        FAILED_CHECK_MSG(
            management->Cache_PersistentObject(key, *outObject),
            L"caching outObject in CManagement class has failed");
    }
    else
    {
        // 5. 이미 있으면 AddRef 후 레이어 추가
        (*outObject)->AddRef();

        FAILED_CHECK_MSG(
            layer->Add_GameObject(objType, *outObject),
            L"Add outObject in layer has failed");
    }

    return S_OK;
}

END
