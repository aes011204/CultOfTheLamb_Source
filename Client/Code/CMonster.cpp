#include "pch.h"
#include "CMonster.h"

#include "CCreateHelper.h"
#include "CManagement.h"
#include "CRectCollider.h"
#include "CRenderer.h"
#include "CTransform.h"
#include "CTriCol.h"

CMonster::CMonster(DEVICE graphicDev)
    : CGameObject(graphicDev),
      m_BufferCom(nullptr),
      m_TransformCom(nullptr),
      m_RectColCom(nullptr)
{}

CMonster::CMonster(const CMonster& rhs)
    : CGameObject(rhs),
      m_BufferCom(nullptr),
      m_TransformCom(nullptr),
      m_RectColCom(nullptr)
{}

CMonster::~CMonster()
{ }

HRESULT CMonster::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    return S_OK;
}

_int CMonster::Update_GameObject(const _float& timeDelta)
{
    _int exit = CGameObject::Update_GameObject(timeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);

    return exit;
}

void CMonster::LateUpdate_GameObject(const _float& timeDelta)
{
    CGameObject::LateUpdate_GameObject(timeDelta);

    /*Engine::CTransform* pPlayerTransformCom = dynamic_cast<CTransform*>
        (Engine::CManagement::GetInstance()->Get_Component
        (ID_DYNAMIC, L"GameLogic_Layer", L"Player", L"Com_Transform"));

    if (nullptr == pPlayerTransformCom)
        return;*/

    auto playerTransformCom = dynamic_cast<CTransform*>(
        CManagement::GetInstance()->Get_Component(ID_DYNAMIC,
                                                  LAYERTYPE::GAMELOGIC,
                                                  OBJTYPE::PLAYER,
                                                  COMPONENTTYPE::TRANSFORM));

    NULL_CHECK(playerTransformCom);

    _vec3 vPlayerPos{};
    playerTransformCom->Get_Info(INFO_POS, &vPlayerPos);

    m_TransformCom->Chase_Target(&vPlayerPos, timeDelta, 5.f);
}

void CMonster::Render_GameObject()
{
    m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());
    m_GraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_BufferCom->Render_Buffer();

    m_GraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

    m_RectColCom->Render();
}

void CMonster::OnBeginOverlap(CCollider* self, CCollider* other)
{
    CGameObject::OnBeginOverlap(self, other);
}

void CMonster::OnEndOverlap(CCollider* self, CCollider* other)
{
    CGameObject::OnEndOverlap(self, other);
}

HRESULT CMonster::Add_Component()
{
    // buffer
    m_BufferCom = CreateProtoComponent<CTriCol>(this, COMPONENTTYPE::TRI_COLOR);
    NULL_CHECK_RETURN(m_BufferCom, E_FAIL);

    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TRI_COLOR, m_BufferCom });

    // transform
    m_TransformCom = CreateProtoComponent<CTransform>(this, COMPONENTTYPE::TRANSFORM);
    NULL_CHECK_RETURN(m_TransformCom, E_FAIL);

    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::TRANSFORM, m_TransformCom });

    // RectColCom
    m_RectColCom = CreateProtoComponent<CRectCollider>(this, COMPONENTTYPE::RECT_COLL);
    NULL_CHECK_RETURN(m_RectColCom, E_FAIL);
    m_RectColCom->Set_Size(_vec2(2.f, 2.f));

    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::RECT_COLL, m_RectColCom });

    return S_OK;
}

CMonster* CMonster::Create(DEVICE graphicDev)
{
    auto monster = new CMonster(graphicDev);

    if (FAILED(monster->Ready_GameObject()))
    {
        MSG_BOX("Monster Create Failed");
        Safe_Release(monster);
        return nullptr;
    }

    return monster;
}

void CMonster::Free()
{
    CGameObject::Free();
}
