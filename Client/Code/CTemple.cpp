#include "pch.h"
#include "CTemple.h"

#include "CBoxCollider.h"
#include "CCreateHelper.h"
#include "CRcTex.h"
#include "CRigidbody.h"
#include "CTexture.h"
#include "CTransform.h"

CTemple::CTemple(DEVICE graphicDev)
    : CBuildingBase(graphicDev)
{ }

CTemple::CTemple(const CTemple& rhs)
    : CBuildingBase(rhs)
{ }

CTemple::~CTemple()
{ }

HRESULT CTemple::Ready_GameObject()
{
    if (FAILED(CBuildingBase::Ready_GameObject()))
        return E_FAIL;

    return S_OK;
}

void CTemple::Initialize_GameObject()
{
    CBuildingBase::Initialize_GameObject();

    m_TransformCom->Set_Scale(4.f, 5.f / 1.5f, 1.f);
    m_TransformCom->Set_RotationAxis(ROT_X, D3DXToRadian(40.f));

    m_BoxCollCom->Set_RawSize(_vec3(1.2f, 1.5f, 1.5f));
    m_BoxCollCom->Set_CollisionLayer(COLLISION_LAYERTYPE::DEFAULT);
}

_int CTemple::Update_GameObject(const _float& timeDelta)
{
    _int exit = CBuildingBase::Update_GameObject(timeDelta);

    return exit;
}

void CTemple::LateUpdate_GameObject(const _float& timeDelta)
{
    CBuildingBase::LateUpdate_GameObject(timeDelta);
}

void CTemple::Render_GameObject()
{
    NULL_CHECK(m_TextureCom);

    // 앞면 텍스처 렌더링
    m_TextureCom->Set_Texture(0);

    CBuildingBase::Render_GameObject();

    // 다른 위치에 지붕 텍스처 렌더링
    _matrix matWorld = m_TransformCom->Get_World();
    _matrix matOffset, matScale, matTrans;

    D3DXMatrixScaling(&matScale, 1.f, 1.f, 1.f);
    D3DXMatrixTranslation(&matTrans, 0.f, 0.f, 0.35f);

    matOffset = matScale * matTrans;
    matWorld  = matOffset * matWorld;

    m_GraphicDev->SetTransform(D3DTS_WORLD, &matWorld);
    m_TextureCom->Set_Texture(1);

    if (!m_IsPreview)
        Render_Setting();

    if (m_BufferCom)
        m_BufferCom->Render_Buffer();

    if (!m_IsPreview)
        Render_Reset();
}

void CTemple::Render_Editor()
{
    CBuildingBase::Render_Editor();

    ImGui::Text("=== Temple ===");
}

void CTemple::OnBeginOverlap(CCollider* self, CCollider* other)
{
    CBuildingBase::OnBeginOverlap(self, other);
}

void CTemple::OnStayOverlap(CCollider* self, CCollider* other)
{
    CBuildingBase::OnStayOverlap(self, other);

    if (!m_IsPreview)
    {
        CGameObject* hitObject = other->Get_Owner();
        if (!hitObject) return;

        // 속도 제거
        auto rigid = dynamic_cast<CRigidbody*>(
            hitObject->Get_Component(ID_DYNAMIC, COMPONENTTYPE::RIGIDBODY));

        if (rigid)
        {
            _vec3 vel = rigid->Get_Velocity();
            vel.x     = 0.f;
            vel.z     = 0.f;
            rigid->Set_Velocity(vel);
        }

        else
            return;

        auto otherTransform = dynamic_cast<CTransform*>(
            hitObject->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));

        auto playerCollider = dynamic_cast<CBoxCollider*>(other);

        if (!otherTransform || !playerCollider) return;

        _vec3 wallPos = m_TransformCom->Get_Pos();
        _vec3 objPos  = otherTransform->Get_Pos();

        // 벽/플레이어 사이즈 합의 절반
        float halfX = (m_BoxCollCom->Get_Size().x + playerCollider->Get_Size().x) * 0.5f;
        float halfZ = (m_BoxCollCom->Get_Size().z + playerCollider->Get_Size().z) * 0.5f;

        // 중심 거리
        float dx = objPos.x - wallPos.x;
        float dz = objPos.z - wallPos.z;

        float penX = halfX - abs(dx);
        float penZ = halfZ - abs(dz);

        // 덜 침투한 축으로 밀어냄
        if (penX < penZ)
            objPos.x = wallPos.x + (dx > 0 ? halfX : -halfX) + (dx > 0 ? 0.01f : -0.01f);
        else
            objPos.z = wallPos.z + (dz > 0 ? halfZ : -halfZ) + (dz > 0 ? 0.01f : -0.01f);

        otherTransform->Set_Pos(objPos);
    }
}

void CTemple::OnEndOverlap(CCollider* self, CCollider* other)
{
    CBuildingBase::OnEndOverlap(self, other);
}

HRESULT CTemple::Add_Component()
{
    if (FAILED(CBuildingBase::Add_Component()))
        return E_FAIL;

    // Texture
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_TEMPLE);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_TEMPLE, m_TextureCom });

    return S_OK;
}

CTemple* CTemple::Create(DEVICE graphicDev)
{
    auto temple = new CTemple(graphicDev);

    if (FAILED(temple->Ready_GameObject()))
    {
        MSG_BOX("Temple Create Failed");
        Safe_Release(temple);
        return nullptr;
    }

    return temple;
}

void CTemple::Free()
{
    CBuildingBase::Free();
}
