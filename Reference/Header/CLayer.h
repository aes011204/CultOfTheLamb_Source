#pragma once
#include "CBase.h"
#include "Engine_Define.h"

BEGIN(Engine)
class CComponent;
class CGameObject;

/// @brief 동일 목적의 게임 오브젝트들을 묶어 관리하는 중간 클래스
/// @details
/// - Scene 내부에서 업데이트/렌더링 순서를 제어하며, 오브젝트 등록과 탐색을 담당한다.
/// - 레이어를 통해 오브젝트 간 결합을 줄이고 Scene 구성의 유연성을 높인다.
class ENGINE_DLL CLayer : public CBase
{
private:
    explicit CLayer();
    ~CLayer() override;

public:
    CComponent* Get_Component(COMPONENTID componentID, OBJTYPE objType, COMPONENTTYPE componentType);
    HRESULT     Add_GameObject(OBJTYPE objType, CGameObject* gameObject);

    const map<OBJTYPE, vector<CGameObject*>>& Get_Objects() const { return m_Objects; }

public:
    HRESULT Ready_Layer();
    void    Initialize_Layer();
    _int    Update_Layer(const _float& timeDelta);
    void    LateUpdate_Layer(const _float& timeDelta);

private:
    void Object_NameSetting(OBJTYPE objType, CGameObject* gameObject);

private:
    map<OBJTYPE, vector<CGameObject*>> m_Objects;

    unordered_map<OBJTYPE, _uint> m_NameCounter;

public:
    static CLayer* Create();

private:
    void Free() override;
};

END
