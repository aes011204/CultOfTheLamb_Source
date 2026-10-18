#pragma once

#include "CGameObject.h"

BEGIN(Engine)

class CBoxCollider;
class CTransform;

using TriggerCallback = function<void()>;

class ENGINE_DLL CTrigger : public CGameObject
{
protected:
    explicit CTrigger(DEVICE graphicDev);
    explicit CTrigger(const CTrigger& rhs);
    virtual ~CTrigger();

public:
    HRESULT Ready_GameObject() override;
    void    Initialize_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;

public:
    void    OnBeginOverlap(CCollider* self, CCollider* other) override;

public:
    void    Set_Bounds(_vec3 position, _vec3 size);
    void    Set_Callback(TriggerCallback callback);
    void    Set_TriggerOnce(_bool once) { m_TriggerOnce = once; }
    void    Set_ShowDebug(_bool show);

    _bool   Has_Triggered() const { return m_HasTriggered; }
    void    Reset_Trigger() { m_HasTriggered = false; }

private:
    HRESULT Add_Component();

public:
    static CTrigger* Create(DEVICE graphicDev);

protected:
    void Free() override;

protected:
    CBoxCollider*       m_ColliderCom;
    CTransform*         m_TransformCom;
    TriggerCallback     m_Callback;

    _bool               m_TriggerOnce;      // 한 번만 발동
    _bool               m_HasTriggered;     // 이미 발동했는지

};

END
