#include "pch.h"
#include "CInteractionChecker.h"

#include "CCombatStat.h"
#include "CConstructCircle.h"
#include "CDroppedItem.h"
#include "CDropSystem.h"
#include "CItemDB.h"
#include "CLayer.h"
#include "CManagement.h"
#include "CScene.h"
#include "CTransform.h"
#include "CWorldInteractive.h"

CInteractionChecker::CInteractionChecker(DEVICE graphicDev)
    : CComponent(graphicDev)
{ }

CInteractionChecker::CInteractionChecker(const CInteractionChecker& rhs)
    : CComponent(rhs)
{ }

CInteractionChecker::~CInteractionChecker()
{ }

HRESULT CInteractionChecker::Ready_InteractionChecker()
{
    // 컴포넌트 장착 전 Create (프로토타입 생성) 과정에서 해야할 것들
    return S_OK;
}

void CInteractionChecker::Initialize_Component()
{
    // 컴포넌트의 초기 동작
}

_int CInteractionChecker::Update_Component(const _float& timeDelta)
{
    return CComponent::Update_Component(timeDelta);
}

void CInteractionChecker::LateUpdate_Component()
{
    CComponent::LateUpdate_Component();
}

CConstructCircle* CInteractionChecker::Get_NearestConstruction(const _vec3& srcPos)
{
    CScene* curScene = CManagement::GetInstance()->Get_Scene();
    if (!curScene) return nullptr;

    CLayer* layer = curScene->Get_Layer(LAYERTYPE::GAMELOGIC);
    if (!layer) return nullptr;

    const auto& mapObj = layer->Get_Objects();
    auto        iter   = mapObj.find(OBJTYPE::CONSTRUCTION_SITE);
    if (iter == mapObj.end()) return nullptr;

    _float            minDist = FLT_MAX;
    CConstructCircle* target  = nullptr;

    for (auto& obj : iter->second)
    {
        auto constructCircle = dynamic_cast<CConstructCircle*>(obj);
        if (!constructCircle) continue;

        // 거리 체크
        auto targetTransform = dynamic_cast<CTransform*>(
            constructCircle->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));
        if (!targetTransform) continue;

        _vec3  toTarget = targetTransform->Get_Pos() - srcPos;
        _float dist     = D3DXVec3Length(&toTarget);

        if (dist < minDist)
        {
            minDist = dist;
            target  = constructCircle;
        }
    }

    return target;
}

CWorldInteractive* CInteractionChecker::Get_NearestTree(const _vec3& srcPos)
{
    CScene* curScene = CManagement::GetInstance()->Get_Scene();

    if (!curScene) return nullptr;

    CLayer* layer = curScene->Get_Layer(LAYERTYPE::GAMELOGIC);

    if (!layer) return nullptr;

    const auto& mapObj = layer->Get_Objects();
    auto        iter   = mapObj.find(OBJTYPE::INTERACTIVE);

    if (iter == mapObj.end()) return nullptr;

    _float             minDist = FLT_MAX;
    CWorldInteractive* target  = nullptr;

    for (auto& obj : iter->second)
    {
        auto interactive = dynamic_cast<CWorldInteractive*>(obj);

        if (!interactive) continue;

        if (interactive->GetInteractiveType() != CWorldInteractive::TYPE::WOOD)
            continue;

        // 체력 0 이하 오브젝트 무시
        auto combatStat = dynamic_cast<CCombatStat*>(interactive->Get_Component(ID_DYNAMIC, COMPONENTTYPE::COMBATSTAT));
        if (combatStat && combatStat->Get_Hp() <= 0.f)
            continue;

        // 거리 체크
        auto targetTransform = dynamic_cast<CTransform*>(
            interactive->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));

        if (!targetTransform) continue;

        _vec3  toTarget = targetTransform->Get_Pos() - srcPos;
        _float dist     = D3DXVec3Length(&toTarget);

        if (dist < minDist)
        {
            minDist = dist;
            target  = interactive;
        }
    }

    return target;
}

CWorldInteractive* CInteractionChecker::Get_NearestRock(const _vec3& srcPos)
{
    CScene* curScene = CManagement::GetInstance()->Get_Scene();

    if (!curScene) return nullptr;

    CLayer* layer = curScene->Get_Layer(LAYERTYPE::GAMELOGIC);

    if (!layer) return nullptr;

    const auto& mapObj = layer->Get_Objects();
    auto        iter   = mapObj.find(OBJTYPE::INTERACTIVE);

    if (iter == mapObj.end()) return nullptr;

    _float             minDist = FLT_MAX;
    CWorldInteractive* target  = nullptr;

    for (auto& obj : iter->second)
    {
        auto interactive = dynamic_cast<CWorldInteractive*>(obj);
        if (!interactive) continue;

        if (interactive->GetInteractiveType() != CWorldInteractive::TYPE::ROCK)
            continue;

        // 체력 0 이하 오브젝트 무시
        auto combatStat = dynamic_cast<CCombatStat*>(interactive->Get_Component(ID_DYNAMIC, COMPONENTTYPE::COMBATSTAT));
        if (combatStat && combatStat->Get_Hp() <= 0.f)
            continue;

        // 거리 체크
        auto targetTransform = dynamic_cast<CTransform*>(
            interactive->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));

        if (!targetTransform) continue;

        _vec3  toTarget = targetTransform->Get_Pos() - srcPos;
        _float dist     = D3DXVec3Length(&toTarget);

        if (dist < minDist)
        {
            minDist = dist;
            target  = interactive;
        }
    }

    return target;
}

CDroppedItem* CInteractionChecker::Get_NearestFood(const _vec3& srcPos)
{
    auto&         droppedItems = CDropSystem::GetInstance()->Get_DroppedItems();
    CDroppedItem* pNearestFood = nullptr;
    _float        fMinDist     = 9999.f;

    for (auto& pItem : droppedItems)
    {
        if (!pItem || pItem->Is_PendingDestroy())
            continue;

        const ItemInstance& itemInst  = pItem->Get_ItemInst();
        Item*               pItemData = CItemDB::GetInstance()->GetItemById(itemInst.itemId);
        if (!pItemData) continue;

        if (pItemData->type == ItemType::Food)
        {
            auto targetTransform = dynamic_cast<CTransform*>(pItem->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));
            if (!targetTransform) continue;

            _vec3  itemPos = targetTransform->Get_Pos();
            _vec3  toItem  = itemPos - srcPos;
            _float dist    = D3DXVec3Length(&toItem);

            if (dist < fMinDist)
            {
                fMinDist     = dist;
                pNearestFood = pItem;
            }
        }
    }

    return pNearestFood;
}

CInteractionChecker* CInteractionChecker::Create(DEVICE graphicDev)
{
    auto instance = new CInteractionChecker(graphicDev);

    if (FAILED(instance->Ready_InteractionChecker()))
    {
        Safe_Release(instance);
        MSG_BOX("Interaction Checker Create Failed");
        return nullptr;
    }

    return instance;
}

CComponent* CInteractionChecker::Clone()
{
    return new CInteractionChecker(*this);
}

void CInteractionChecker::Free()
{
    CComponent::Free();
}
