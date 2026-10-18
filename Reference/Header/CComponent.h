#pragma once
#include "CBase.h"
#include "Engine_Define.h"

BEGIN(Engine)
class CGameObject;
class CTransform;

/// @brief 오브젝트에 부착되어 기능을 모듈화하는 엔진의 최소 단위 클래스
/// @details
/// - 기본적으로 프로토타입 패턴을 사용해 동일 구성을 빠르게 복제하고 일관된 초기화를 유지한다.
/// - 생명 주기는 소유 오브젝트와 참조 카운트에 의해 관리되며, 자원 해제는 Free()를 통해 수행된다
/// - 자신의 소유자인 오브젝트 m_Owner, 소유자의 Transform 컴포넌트 m_TransformCom 를 가진다
class ENGINE_DLL CComponent : public CBase
{
protected:
    explicit CComponent();
    explicit CComponent(DEVICE graphicDev);
    explicit CComponent(const CComponent& rhs);
    ~CComponent() override;

public:
    virtual void      Initialize_Component() { }
    virtual _int      Update_Component(const _float& timeDelta) { return 0; }
    virtual void      LateUpdate_Component() { }

    CGameObject*      Get_Owner() { return m_Owner; }
    
    CTransform*       Get_Transform() { return m_TransformCom; }
    const CTransform* Get_Transform() const { return m_TransformCom; }

    COMPONENTTYPE     Get_Type() const { return m_ComponentType; }
    virtual wstring   Get_Name() { return m_ComponentName; }

    void              Set_Owner(CGameObject* owner);
    void              Set_Type(COMPONENTTYPE type) { m_ComponentType = type; }
    void              Set_Name(wstring name) { m_ComponentName = name; }

    // Editor
    virtual void Render_Editor() {}

protected:
    COMPONENTTYPE m_ComponentType;
    wstring       m_ComponentName;

    DEVICE m_GraphicDev;

    CGameObject* m_Owner;
    CTransform*  m_TransformCom;

    _bool m_IsClone;

     // 컴포넌트의 업데이트와 렌더를 하면 true, 안할거면 false로 바꾸면 되는 스위치
    _bool m_Active; 

public:
    virtual CComponent* Clone() PURE;

protected:
    void Free() override;
};

END
