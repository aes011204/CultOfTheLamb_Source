#include "CCollisionManager.h"
#include "CTransform.h"
#include "CCollider.h"
#include "CGameObject.h"
#include "CBoxCollider.h"

IMPLEMENT_SINGLETON(CCollisionManager)

CCollisionManager::CCollisionManager()
{
    Initialize_CollisionMatrix();
}

CCollisionManager::~CCollisionManager()
{
    Free();
}

void CCollisionManager::Update()
{
    auto&       colliders = m_Colliders;
    const _uint count     = static_cast<_uint>(colliders.size());

    for (_uint i = 0; i < count; i++)
    {
        CCollider* src = colliders[i];
        if (src == nullptr)
            continue;

        CGameObject* ownerSrc = src->Get_Owner();
        if (ownerSrc == nullptr || ownerSrc->Is_PendingDestroy()) // 소멸 예정인 애들은 충돌 X
            continue;

        if (src->IsCollisionActive() == false) // 충돌체 Active가 꺼져있다면 무시
            continue;

        for (_uint j = i + 1; j < count; j++)
        {
            CCollider* dst = colliders[j];
            if (dst == nullptr)
                continue;

            CGameObject* ownerDst = dst->Get_Owner();
            if (ownerDst == nullptr || ownerDst->Is_PendingDestroy()) // 소멸 예정인 애들은 충돌 X
                continue;

            if (dst->IsCollisionActive() == false) // 충돌체 Active가 꺼져있다면 무시
                continue;

            // Owner가 같으면 무시
            if (ownerSrc == ownerDst)
                continue;

            // Insigator 체크
            CGameObject* instigatorSrc = src->Get_Instigator();
            if (instigatorSrc != nullptr && instigatorSrc == ownerDst)
                continue;

            CGameObject* instigatorDst = dst->Get_Instigator();
            if (instigatorDst != nullptr && instigatorDst == ownerSrc)
                continue;

            // 레이어 확인
            if (!Can_Collide(src->Get_CollisionLayer(), dst->Get_CollisionLayer()))
                continue;

            bool isColliding = src->CheckCollision(dst);

            if (isColliding)
            {
                // 이번 프레임에 새로 겹치기 시작하면
                if (!src->IsOverlapping(dst))
                {
                    // Owner에게 이벤트 전달, 비교가 아닌 대입임
                    if (CGameObject* ownerSrc = src->Get_Owner())
                        ownerSrc->OnBeginOverlap(src, dst);

                    if (CGameObject* ownerDst = dst->Get_Owner())
                        ownerDst->OnBeginOverlap(dst, src);

                    // 충돌 상태 추가
                    src->AddOverlap(dst);
                    dst->AddOverlap(src);
                }
                // 이미 겹쳐 있을 때
                else
                {
                    if (CGameObject* ownerSrc = src->Get_Owner())
                        ownerSrc->OnStayOverlap(src, dst);

                    if (CGameObject* ownerDst = dst->Get_Owner())
                        ownerDst->OnStayOverlap(dst, src);
                }
            }

            else
            {
                // 이전 프레임까지 겹쳐 있었는데, 이제 떨어졌으면
                if (src->IsOverlapping(dst))
                {
                    if (CGameObject* ownerSrc = src->Get_Owner())
                        ownerSrc->OnEndOverlap(src, dst);

                    if (CGameObject* ownerDst = dst->Get_Owner())
                        ownerDst->OnEndOverlap(dst, src);

                    // 충돌 상태 제거
                    src->RemoveOverlap(dst);
                    dst->RemoveOverlap(src);
                }
            }
        }
    }
}

// 충돌체 추가
void CCollisionManager::AddCollider(CCollider* collider)
{
    if (collider == nullptr)
        return;

    m_Colliders.push_back(collider);
}

void CCollisionManager::RemoveCollider(CCollider* collider)
{
    if (collider == nullptr)
        return;

    for (CCollider* other : m_Colliders)
    {
        if (other && other != collider)
        {
            if (other->IsOverlapping(collider))
            {
                other->RemoveOverlap(collider);

                if (CGameObject* owner = other->Get_Owner())
                    owner->OnEndOverlap(other, collider);
            }
        }
    }

    // 리스트에서 제거
    auto iter = remove(m_Colliders.begin(), m_Colliders.end(), collider);
    m_Colliders.erase(iter, m_Colliders.end());
}

_bool CCollisionManager::Can_Collide(COLLISION_LAYERTYPE layer1, COLLISION_LAYERTYPE layer2)
{
    _int index1 = static_cast<int>(layer1);
    _int index2 = static_cast<int>(layer2);

    return m_CollisionMatrix[index1][index2];
}

void CCollisionManager::Render_Debug(bool& flag)
{
    for (const auto& collider : m_Colliders)
    {
        collider->Set_ShowDebug(flag);
    }
}

_bool CCollisionManager::Check_PointOverlap(const _vec3& pos, COLLISION_LAYERTYPE layer, _float radius)
{
    for (CCollider* col : m_Colliders)
    {
        if (!col)
            continue;

        // 원하는 레이어만 체크
        if (col->Get_CollisionLayer() != layer)
            continue;

        CGameObject* owner = col->Get_Owner();
        if (!owner)
            continue;

        CTransform* transform = dynamic_cast<CTransform*>(
            owner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));

        if (!transform)
            continue;

        _vec3 colPos = transform->Get_Pos();

        _float dx = pos.x - colPos.x;
        _float dz = pos.z - colPos.z;
        _float dist = sqrtf(dx * dx + dz * dz);

        // 충돌 반경 체크
        CBoxCollider* boxCol = dynamic_cast<CBoxCollider*>(col);
        _float colRadius = 1.f; 

        if (boxCol)
        {
            _vec3 size = boxCol->Get_Size();
            colRadius = (size.x + size.z) * 0.25f;  
        }

        if (dist < radius + colRadius)
            return true;
    }

    return false;
}

void CCollisionManager::Initialize_CollisionMatrix()
{
    // 일단 모든 레이어 충돌 가능하게 설정
    _int colLayerEnd = static_cast<_int>(COLLISION_LAYERTYPE::COL_LAYER_END);

    for (_int i = 0; i < colLayerEnd; i++)
    {
        for (_int j = 0; j < colLayerEnd; j++)
        {
            m_CollisionMatrix[i][j] = true;
        }
    }

    _int player_Index           = static_cast<_int>(COLLISION_LAYERTYPE::PLAYER);
    _int enemy_Index            = static_cast<_int>(COLLISION_LAYERTYPE::ENEMY);
    _int npc_Index              = static_cast<_int>(COLLISION_LAYERTYPE::NPC);
    _int wall_Index             = static_cast<_int>(COLLISION_LAYERTYPE::WALL);
    _int playerprojectile_Index = static_cast<_int>(COLLISION_LAYERTYPE::PLAYERPROJECTILE);
    _int projectile_Index       = static_cast<_int>(COLLISION_LAYERTYPE::BOSSPROJECTILE);
    _int item_Index             = static_cast<_int>(COLLISION_LAYERTYPE::ITEM);
    _int attack_Index           = static_cast<_int>(COLLISION_LAYERTYPE::ATTACK);
    _int trigger_Index          = static_cast<_int>(COLLISION_LAYERTYPE::TRIGGER);
    _int interactive_Index      = static_cast<_int>(COLLISION_LAYERTYPE::INTERACTIVEOBJ);
    _int workbench_Index        = static_cast<_int>(COLLISION_LAYERTYPE::WORKBENCH);
    _int kitchen_Index          = static_cast<_int>(COLLISION_LAYERTYPE::KITCHEN);
    _int site_Index             = static_cast<_int>(COLLISION_LAYERTYPE::CONSTRUCTION_SITE);

    // 벽끼리는 충돌하지 않도록 설정
    m_CollisionMatrix[wall_Index][wall_Index] = false;

    // 적끼리도 충돌 X
    m_CollisionMatrix[enemy_Index][enemy_Index] = false;

    // 아이템과 벽은 충돌 ㄴㄴ
    m_CollisionMatrix[wall_Index][item_Index] = false;
    m_CollisionMatrix[item_Index][wall_Index] = false;

    // 공격이랑 아이템 충돌 ㄴㄴ
    m_CollisionMatrix[attack_Index][item_Index] = false;
    m_CollisionMatrix[item_Index][attack_Index] = false;

    // 아이템, 적 충돌 ㄴㄴ
    m_CollisionMatrix[item_Index][enemy_Index] = false;
    m_CollisionMatrix[enemy_Index][item_Index] = false;

    // TODO : 다른 충돌 상태 판정 추가하기

    // 투사체끼리도 충돌 X, 투사체랑 보스랑도 충돌X
    m_CollisionMatrix[projectile_Index][projectile_Index] = false;
    m_CollisionMatrix[projectile_Index][enemy_Index]      = false;
    m_CollisionMatrix[enemy_Index][projectile_Index]      = false;

    //플레이어 투사체와 아이템 충돌 X, 플레이어 투사체와 플레이어 충돌 X
    m_CollisionMatrix[playerprojectile_Index][playerprojectile_Index] = false;
    m_CollisionMatrix[playerprojectile_Index][item_Index]             = false;
    m_CollisionMatrix[item_Index][playerprojectile_Index]             = false;
    //m_CollisionMatrix[playerprojectile_Index][player_Index] = false;
    //m_CollisionMatrix[player_Index][playerprojectile_Index] = false;

    // 플레이어, 몬스터 몸박 ㄴㄴ
    m_CollisionMatrix[enemy_Index][player_Index] = false;
    m_CollisionMatrix[player_Index][enemy_Index] = false;

    // 트리거와 충돌 판정 : 플레이어, NPC가 가능
    m_CollisionMatrix[trigger_Index][trigger_Index] = false;

    for (_int i = 0; i < colLayerEnd; i++)
    {
        if (i != player_Index && i != npc_Index && i != trigger_Index)
        {
            m_CollisionMatrix[trigger_Index][i] = false;
            m_CollisionMatrix[i][trigger_Index] = false;
        }
    }

    // 제작대는 플레이어와만 충돌 가능
    for (_int i = 0; i < colLayerEnd; i++)
    {
        if (i != player_Index && i != workbench_Index)
        {
            m_CollisionMatrix[workbench_Index][i] = false;
            m_CollisionMatrix[i][workbench_Index] = false;
        }
    }

    // 주방은 플레이어와만 충돌 가능
    for (_int i = 0; i < colLayerEnd; i++)
    {
        if (i != player_Index && i != kitchen_Index)
        {
            m_CollisionMatrix[kitchen_Index][i] = false;
            m_CollisionMatrix[i][kitchen_Index] = false;
        }
    }

    // 채집 오브젝트와 건축 부지 사이 충돌 off
    m_CollisionMatrix[interactive_Index][site_Index] = false;
    m_CollisionMatrix[site_Index][interactive_Index] = false;
}

void CCollisionManager::Free()
{
    m_Colliders.clear();
}
