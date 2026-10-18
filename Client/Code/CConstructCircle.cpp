#include "pch.h"
#include "CConstructCircle.h"

#include <CSoundMgr.h>
#include "CStage.h"
#include "CBoxCollider.h"
#include "CCollider.h"
#include "CCreateHelper.h"
#include "CDInputMgr.h"
#include "CKitchen.h"
#include "CManagement.h"
#include "CNPCBase.h"
#include "CNPCController.h"
#include "CNPCStateMachine.h"
#include "CPlayer.h"
#include "CRcTex.h"
#include "CScene.h"
#include "CState.h"
#include "CTemple.h"
#include "CTexture.h"
#include "CTimerMgr.h"
#include "CTransform.h"
#include "CVillageStateMgr.h"
#include "CPlayerConstruction.h"

CConstructCircle::CConstructCircle(DEVICE graphicDev)
    : CBuildingBase(graphicDev)
{ }

CConstructCircle::CConstructCircle(const CConstructCircle& rhs)
    : CBuildingBase(rhs)
{ }

CConstructCircle::~CConstructCircle()
{ }

HRESULT CConstructCircle::Ready_GameObject()
{
    if (FAILED(CBuildingBase::Ready_GameObject()))
        return E_FAIL;

    return S_OK;
}

void CConstructCircle::Initialize_GameObject()
{
    CBuildingBase::Initialize_GameObject();

    m_TransformCom->Set_RotationAxis(ROT_X, D3DXToRadian(90.f));

    m_BoxCollCom->Set_RawSize(_vec3(1.5f, 1.f, 3.f));
    m_BoxCollCom->Set_CollisionLayer(COLLISION_LAYERTYPE::TRIGGER);

    m_OutlineEnabled = false;
}

_int CConstructCircle::Update_GameObject(const _float& timeDelta)
{
    _int exit = CBuildingBase::Update_GameObject(timeDelta);

    return exit;
}

void CConstructCircle::LateUpdate_GameObject(const _float& timeDelta)
{
    CBuildingBase::LateUpdate_GameObject(timeDelta);
}

void CConstructCircle::Render_GameObject()
{
    NULL_CHECK(m_TextureCom);

    if (!m_IsPreview)
    {
        m_TextureCom->Set_Texture(0);
        CBuildingBase::Render_GameObject();
    }
    else
    {
        if (m_TransformCom)
            m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());

        m_TextureCom->Set_Texture(0);

        m_BufferCom->Render_Buffer();
    }
}

void CConstructCircle::OnBeginOverlap(CCollider* self, CCollider* other)
{
    CBuildingBase::OnBeginOverlap(self, other);
}

void CConstructCircle::OnStayOverlap(CCollider* self, CCollider* other)
{
    CBuildingBase::OnStayOverlap(self, other);

    if (m_IsPreview)
        return;

    CGameObject* obj = other->Get_Owner();
    NULL_CHECK(obj);

    // 플레이어 체크
    if (auto player = dynamic_cast<CPlayer*>(obj))
    {
        auto playerState = dynamic_cast<CState*>(
            obj->Get_Component(ID_DYNAMIC, COMPONENTTYPE::STATE));

        if (playerState && playerState->Get_State() == ACTORSTATE::PLAYER_HAMMER)
            m_Progress += 10.f * CTimerMgr::GetInstance()->Get_TimeDelta();
    }

    // NPC 체크
    else if (auto npc = dynamic_cast<CNPCBase*>(obj))
    {
        auto controller = dynamic_cast<CNPCController*>(
            npc->Get_Component(ID_DYNAMIC, COMPONENTTYPE::NPC_CONTROLLER));

        if (controller && controller->Get_CurNPCState() == CNPCStateMachine::NPC_STATE::BUILD)
            m_Progress += 5.f * CTimerMgr::GetInstance()->Get_TimeDelta();
    }

    // 건축 완료 로직
    if (m_Progress >= 100.f)
    {
        CGameObject* newObj = nullptr;
        switch (m_TargetBuildingType)
        {
        case BUILDING_TYPE::TEMPLE:
            newObj = CTemple::Create(m_GraphicDev);
            break;
        case BUILDING_TYPE::KITCHEN:
            newObj = CKitchen::Create(m_GraphicDev);
            break;
        default: break;
        }

        if (newObj)
        {
            if (auto kitchen = dynamic_cast<CKitchen*>(newObj))
            {
                if (auto scene = CManagement::GetInstance()->Get_Scene())
                {
                    if (auto stage = dynamic_cast<CStage*>(scene))
                        kitchen->Set_CookingMenu(stage->Get_CookingMenu());
                }
            }

            newObj->Initialize_GameObject();

            auto* transform = dynamic_cast<CTransform*>(
                newObj->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));

            if (transform)
            {
                _vec3 myPos = m_TransformCom->Get_Pos();

                if (m_TargetBuildingType == BUILDING_TYPE::TEMPLE)
                    myPos.y += 2.f;
                else if (m_TargetBuildingType == BUILDING_TYPE::KITCHEN)
                    myPos.y += 0.6f;

                transform->Set_Pos(myPos);
            }

            if (CScene* curScene = CManagement::GetInstance()->Get_Scene())
                curScene->AddObjectOnLayer(LAYERTYPE::GAMELOGIC, newObj, OBJTYPE::BUILDING);

            // 월드 상태 업데이트 (건설 완료)
            _vec3 myPos = m_TransformCom->Get_Pos();
            CVillageStateMgr::GetInstance()->Update_Building(myPos, true);

            CGameObject* playerObj;
            CManagement::GetInstance()->Get_PersistentObject(L"Player", &playerObj);
            if (playerObj)
            {
                CPlayerConstruction* playerConstruction = dynamic_cast<CPlayerConstruction*>(
                    playerObj->Get_Component(ID_STATIC, COMPONENTTYPE::CONSTRUCTION));

                if (playerConstruction)
                    playerConstruction->BuildingCompleted();
            }

            this->Mark_As_PendingKill();
        }
    }
}

void CConstructCircle::OnEndOverlap(CCollider* self, CCollider* other)
{
    CBuildingBase::OnEndOverlap(self, other);
}

void CConstructCircle::Setup_Construction(BUILDING_TYPE type)
{
    m_IsPreview          = false;
    m_TargetBuildingType = type;
}

HRESULT CConstructCircle::Add_Component()
{
    if (FAILED(CBuildingBase::Add_Component()))
        return E_FAIL;

    // Texture
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_DOTTEDCIRCLE);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_DOTTEDCIRCLE, m_TextureCom });

    return S_OK;
}

CConstructCircle* CConstructCircle::Create(DEVICE graphicDev, _vec3 scale)
{
    auto dottedCircle = new CConstructCircle(graphicDev);

    if (FAILED(dottedCircle->Ready_GameObject()))
    {
        MSG_BOX("DottedCircle Create Failed");
        Safe_Release(dottedCircle);
        return nullptr;
    }

    dottedCircle->m_TransformCom->Set_Scale(scale);

    return dottedCircle;
}

void CConstructCircle::Free()
{
    CBuildingBase::Free();
}
