#include "CTrigger.h"

#include "CBoxCollider.h"
#include "CCreateHelper.h"
#include "CRenderer.h"
#include "CTransform.h"


CTrigger::CTrigger(DEVICE graphicDev)
    : CGameObject(graphicDev),
    m_ColliderCom(nullptr),
    m_TransformCom(nullptr),
    m_TriggerOnce(false),
    m_HasTriggered(false)
{
}

CTrigger::CTrigger(const CTrigger& rhs)
    : CGameObject(rhs),
    m_ColliderCom(nullptr),
    m_TransformCom(nullptr),
    m_TriggerOnce(rhs.m_TriggerOnce),
    m_HasTriggered(false)
{
}

CTrigger::~CTrigger()
{
}

HRESULT CTrigger::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    m_ColliderCom->Set_Instigator(this);
    m_ColliderCom->Set_CollisionLayer(COLLISION_LAYERTYPE::TRIGGER);
    m_ColliderCom->Set_ShowDebug(true);

    return S_OK;
}

void CTrigger::Initialize_GameObject()
{
    CGameObject::Initialize_GameObject();

}

_int CTrigger::Update_GameObject(const _float& timeDelta)
{
    _int exit = CGameObject::Update_GameObject(timeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDERID::RENDER_NONALPHA, this);

    return exit;
}

void CTrigger::LateUpdate_GameObject(const _float& timeDelta)
{
    CGameObject::LateUpdate_GameObject(timeDelta);
}

void CTrigger::Render_GameObject()
{
    CGameObject::Render_GameObject();

}

void CTrigger::OnBeginOverlap(CCollider* self, CCollider* other)
{
    CGameObject::OnBeginOverlap(self, other);

    // 한번만?
    if (m_TriggerOnce && m_HasTriggered)
        return;

    if (!m_Callback)
        return;

    // Callback 실행
    m_Callback();
    m_HasTriggered = true;
}

void CTrigger::Set_Bounds(_vec3 position, _vec3 size)
{
    if (m_TransformCom)
        m_TransformCom->Set_Pos(position);

    if (m_ColliderCom)
        m_ColliderCom->Set_Size(size);
}

void CTrigger::Set_Callback(TriggerCallback callback)
{
    m_Callback = callback;
}

void CTrigger::Set_ShowDebug(_bool show)
{
    if (m_ColliderCom)
        m_ColliderCom->Set_ShowDebug(show);
}

HRESULT CTrigger::Add_Component()
{
    m_TransformCom = CreateProtoComponent<CTransform>(this, COMPONENTTYPE::TRANSFORM);
    NULL_CHECK_RETURN(m_TransformCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::TRANSFORM, m_TransformCom });

    m_ColliderCom = CreateProtoComponent<CBoxCollider>(this, COMPONENTTYPE::BOX_COLL);
    NULL_CHECK_RETURN(m_ColliderCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::BOX_COLL, m_ColliderCom });

    return S_OK;
}

CTrigger* CTrigger::Create(DEVICE graphicDev)
{
    CTrigger* trigger = new CTrigger(graphicDev);
    if (FAILED(trigger->Ready_GameObject()))
    {
        Safe_Release(trigger);
        MSG_BOX("CTrigger Create Failed");
        return nullptr;
    }
    return trigger;
}

void CTrigger::Free()
{
    CGameObject::Free();
}
