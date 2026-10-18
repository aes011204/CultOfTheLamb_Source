#pragma once
#include "CBase.h"
#include "Engine_Define.h"

BEGIN(Engine)
class CComponent;
class CCollider;
class CTransform;
class CTexture;

/// @brief 다양한 컴포넌트를 조합해 게임 내 오브젝트를 구현하는 클래스
/// @details
/// - 여러 컴포넌트를 담고, 생명 주기(Ready/Update/Render)를 순차적으로 실행한다.
/// - 외부 노출은 Get_Component()만 제공하여, 이를 통해서만 각 기능에 접근해야 한다
class ENGINE_DLL CGameObject : public CBase
{
protected:
    explicit CGameObject(DEVICE graphicDev);
    explicit CGameObject(const CGameObject& rhs);
    ~CGameObject() override;

public:
    CComponent* Get_Component(COMPONENTID componentID, COMPONENTTYPE componentType);

    _bool Get_IsActive() { return m_Active; }
    void  Set_IsActive(_bool setActive) { m_Active = setActive; }

public:
    virtual HRESULT Ready_GameObject();
    virtual void    Initialize_GameObject(); // Ready보다 한 단계 아래 생명주기
    virtual _int    Update_GameObject(const _float& timeDelta);
    virtual void    LateUpdate_GameObject(const _float& timeDelta);
    virtual void    Render_GameObject() PURE;
    //
    virtual void    Render_Trail() {};

public:
    virtual void OnBeginOverlap(CCollider* self, CCollider* other) {}
    virtual void OnStayOverlap(CCollider* self, CCollider* other) {}
    virtual void OnEndOverlap(CCollider* self, CCollider* other) {}

    //virtual void TakeDamage(HITSTRUCT hit);

    const map<COMPONENTTYPE, CComponent*>&
        Get_DynamicComponents() const { return m_Components[ID_DYNAMIC]; }

    const map<COMPONENTTYPE, CComponent*>&
        Get_StaticComponents() const { return m_Components[ID_STATIC]; }

    // 자주 사용해서 추가함
    CTransform* Get_Transform();

    virtual wstring Get_Name() { return m_Name; }
    void Set_Name(wstring name) { m_Name = name; }

    // Editor
    virtual void Render_Editor() { }

    // PendingKill -> 다음 프레임에서 제거할 대상 마크
    // CLayer -> LateUpdate에서 세팅 중
    void Mark_As_PendingKill() { m_PendingKill = true; }
    bool Is_PendingDestroy() { return m_PendingKill; }

public:
    bool SetTexAspectInit(CTexture* texCom, CTransform* transCom, int index, wstring*key = nullptr,bool is_width = true, float width = 2);
    void SetAspectScale(CTransform* transCom,_vec3 magnification);
    //void SetScaleAsp(_vec3 ScaleAsp) { m_ScaleAsp = ScaleAsp; }

private:
    _vec3 m_BaseScaleAsp;
    _vec3 m_ScaleAsp;

protected:
    map<COMPONENTTYPE, CComponent*> m_Components[ID_END];

    DEVICE m_GraphicDev;

    // TODO 기본 생성자에서 이름 세팅해주도록 구조 변경하기 -> Editor와 연동을 위해
    wstring m_Name;
    _bool m_Active;       // 오브젝트의 업데이트와 렌더를 하면 true, 안할거면 false로 바꾸면 되는 스위치

    _bool m_PendingKill;

private:
    CComponent* Find_Component(COMPONENTID componentID, COMPONENTTYPE componentType);

protected:
    void Free() override;
};

END
