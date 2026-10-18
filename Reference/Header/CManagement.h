#pragma once
#include "CBase.h"
#include "Engine_Define.h"

BEGIN(Engine)
class CComponent;
class CGameObject;
class CScene;

/// @brief Scene의 생성/교체/업데이트를 총괄하는 엔진의 최상위 관리자
/// @details
/// - Scene 전환 시 메모리를 안전하게 해제하고, 외부 시스템의 접근 지점을 단일화한다.
/// - 게임 루프 단계(Update/LateUpdate/Render)를 Scene에 위임해 실행 순서를 제어한다.
class ENGINE_DLL CManagement : public CBase
{
    DECLARE_SINGLETON(CManagement)

private:
    explicit CManagement();
    ~CManagement() override;

public:
    CComponent* Get_Component(COMPONENTID   componentID,
                              LAYERTYPE     layerType,
                              OBJTYPE       objType,
                              COMPONENTTYPE componentType);

public: // Persistent Object 관리 함수

    /// @brief CManagement가 보유 중인 Persistent 오브젝트를 조회하는 함수
    /// @param key       조회할 오브젝트의 고유 키 (문자열)
    /// @param outObject 조회된 오브젝트의 포인터를 out 파라미터로 전달
    /// @return 대상 오브젝트가 존재하면 S_OK, 존재하지 않으면 E_FAIL
    HRESULT Get_PersistentObject(const wstring& key, CGameObject** outObject);

    /// @brief 지정한 키의 Persistent 오브젝트를 새 오브젝트로 교체하는 함수
    /// @details
    /// - 기존 오브젝트는 Release되고, 새 오브젝트는 AddRef하여
    /// Persistent 영역의 소유권을 확보한다.
    /// @param key           갱신할 오브젝트의 고유 키 (문자열)
    /// @param objectToCache Persistent 영역에 저장할 오브젝트
    /// @return 성공 시 S_OK, null 전달이면 E_FAIL
    HRESULT Cache_PersistentObject(const wstring& key, CGameObject* objectToCache);

public:
    CScene* Get_Scene() const { return m_Scene; }
    HRESULT Set_Scene(CScene* scene);   // 강제 씬 전환. 게임 시작시에만 사용중

    _int Update_Scene(const _float& timeDelta);
    void LateUpdate_Scene(const _float& timeDelta);
    void Render_Scene(DEVICE graphicDev);

public:
    HRESULT Request_ChangeScene(CScene* nextScene);    // 씬 전환 예약
    void    Commit_ChangeScene();   // 프레임 경계(다음 프레임 업데이트 시작)에서 씬 실제 전환

private:
    CScene* m_Scene;                // 현재 씬
    CScene* m_NextScene;            // 전환 예약 씬

    _bool m_ShouldChangeScene;      // 전환 플래그

    map<wstring, CGameObject*> m_PersistentObjects; // 문자열 키 기반 Persistent 오브젝트 컨테이너

public:
    void Free() override;
};

END
