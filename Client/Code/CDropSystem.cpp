#include "pch.h"
#include "CDropSystem.h"

#include "CDroppedItem.h"
#include "CGameObject.h"
#include "CInventory.h"
#include "CStage.h"
#include "CTransform.h"
#include "Client_Struct.h"
#include "CRandomMgr.h"

IMPLEMENT_SINGLETON(CDropSystem)

CDropSystem::CDropSystem()
{ }

CDropSystem::~CDropSystem()
{
    CDropSystem::Free();
}

// 이건 그냥 바닥에 드롭 시킬떄 새로 생성해서 줌

CDroppedItem* CDropSystem::SpawnDrop(DEVICE pGraphicDev, const ItemInstance& item, const _vec3& worldPos,
    std::optional<_vec3> scale, std::optional<SpriteBounceDesc> bounceDesc)
{
    CDroppedItem* pDrop = CDroppedItem::Create(pGraphicDev, item, worldPos);

    // Scale 설정
    if (scale.has_value())
    {
        auto transform = pDrop->Get_Transform();
        if (transform)
        {
            transform->Set_Scale(scale.value());
            transform->Set_RotationAxis(ROT_X, D3DXToRadian(45.f));
        }
    }

    if (bounceDesc.has_value())
    {
        pDrop->SetBounceDesc(*bounceDesc);
    }

    tmpItem.push_back(pDrop);

    m_nowScene->AddObjectOnLayer(LAYERTYPE::GAMELOGIC, pDrop, OBJTYPE::ITEM);

    return pDrop;
}

void CDropSystem::SpawnAllFromInven(DEVICE pGraphicDev, CInventory* Inven, _vec3 dropDir, int offset)
{
    CGameObject* obj = Inven->Get_Owner();
    if (obj == nullptr)
        return;

    auto transfom = dynamic_cast<CTransform*>(obj->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));
    _vec3 ownerPos = transfom->Get_Pos();

    for (int i = 0; i < Inven->GetVector().size(); i++)
    {
        const InventorySlot& slot     = Inven->GetVector()[i];
        ItemInstance         itemInst = slot.itemInst;

        if (itemInst.itemId != -1 && Inven->GetVector()[i].count >= 1)
        {
            int dropCount = slot.count;
            for (int j = 0; j < dropCount; j++)
            {

                // 플레이어 잉떄
                _vec3 tmp0Dir = { 0,0,0 };
                if (dropDir == tmp0Dir && offset == 0)
                {
                int angle = 25*i+ j*13;

                SpriteBounceDesc d;
                _vec3 tmpvec = { cos(D3DXToRadian(angle)),1,sin(D3DXToRadian(angle))};
                D3DXVec3Normalize(&tmpvec, &tmpvec);

                d.pos = ownerPos;
                // d.velocity
                d.velocity = tmpvec * 30;
                d.groundY = 1.f;
                // 중력
                d.gravity = -400.f;
                d.restitution = 0.7f;// 튕김 계수
                d.damping = 0.7f;// 충돌 감쇠(마찰)
                d.m_DampingPerSecond = 0.9f;// 공기 저항
                    _vec3 tmppos = { ownerPos.x, ownerPos.y + 1.5f, ownerPos.z };

                SpawnDrop(pGraphicDev, itemInst, tmppos, _vec3(0.5f, 0.5f, 0.5f), d);

                Inven->RemoveItem(itemInst.itemId, 1);
                }
                else // 다른 오브젝트 일떄  )
                {
                   // int index =  i + (j+1);

                    SpriteBounceDesc d;
                    _vec3 tmpvec = dropDir;
                   // tmpvec.y = 0.f;
                    D3DXVec3Normalize(&tmpvec, &tmpvec);

                   int RandOffSet = CRandomMgr::GetInstance()->Get_Int_Random(-offset, offset); // y 축 회전

                    int PitchAngle = 60;

                    //D3DXMATRIX matRotX, matRotY, matFinal;

                    //D3DXMatrixRotationX(&matRotX, D3DXToRadian(PitchAngle));// 위 아래 회전 
                    //D3DXMatrixRotationY(&matRotY, D3DXToRadian(RandOffSet)); // offset 으로 양 옆 회전

                    //matFinal = matRotY * matRotX;

                    //D3DXVec3TransformNormal(&tmpvec, &tmpvec, &matFinal);
                    //D3DXVec3Normalize(&tmpvec, &tmpvec);
                   
        

                    // 월드 기준 Up 방향
                    _vec3 worldUp = { 0.f, 1.f, 0.f };

                    // 1) Yaw 랜덤 적용
                    D3DXMATRIX matYaw;
                    D3DXMatrixRotationY(&matYaw, D3DXToRadian(RandOffSet));
                    D3DXVec3TransformNormal(&tmpvec, &tmpvec, &matYaw);

                    // 2) Pitch(X 축 회전이 아니라! 벡터와 worldUp의 교차축을 찾아서 회전)
                    _vec3 right;
                    D3DXVec3Cross(&right, &worldUp, &tmpvec);
                    D3DXVec3Normalize(&right, &right);

                    D3DXMATRIX matPitch;
                    D3DXMatrixRotationAxis(&matPitch, &right, D3DXToRadian(PitchAngle));
                    D3DXMatrixRotationAxis(&matPitch, &right, D3DXToRadian(PitchAngle));

                    // 최종 적용
                    D3DXVec3TransformNormal(&tmpvec, &tmpvec, &matPitch);
                    D3DXVec3Normalize(&tmpvec, &tmpvec);

                    float horizontal = 10.f;
                    float vertical = 40.f;


                    _vec3 Startpos = { ownerPos.x, 0.5f, ownerPos.z };
                    //d.pos = ownerPos;
                    d.pos = Startpos;
              
                    d.velocity.x = tmpvec.x * horizontal;
                    d.velocity.z = tmpvec.z * horizontal;
                    d.velocity.y = vertical;

                    d.groundY = .5f;
                    
                    d.gravity = -300.f;
                    d.restitution = 0.8f;// 튕김 계수
                    d.damping = 0.7f;// 충돌 감쇠(마찰)
                    d.m_DampingPerSecond = 0.9f;// 공기 저항


                    SpawnDrop(pGraphicDev, itemInst, Startpos, _vec3(0.5f, 0.5f, 0.5f), d);

                    Inven->RemoveItem(itemInst.itemId, 1);
                }

            }
        }
    }
    return ;
}

void CDropSystem::Remove_Item(CDroppedItem* item)
{
    for (auto it = tmpItem.begin(); it != tmpItem.end(); )
    {
        if (*it == item)
        {
            it = tmpItem.erase(it);
            return;
        }
        else
            ++it;
    }
}

void CDropSystem::Clear_Item()
{
    for (auto& it : tmpItem)
    {
        if (it != nullptr && (*it).Is_PendingDestroy() != true)
        {
            it->Mark_As_PendingKill();
            
        }
    }
    tmpItem.clear();
}

void CDropSystem::Free()
{
    Clear_Item();
}
