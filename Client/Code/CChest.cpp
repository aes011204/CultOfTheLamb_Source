#include "pch.h"
#include "CChest.h"
#include "CCreateHelper.h"
#include "CTexture.h"
#include "CAnimator.h"
#include "CRcTex.h"
#include "CTransform.h"
#include "CRenderer.h"
#include "CInventory.h"
#include "CBoxCollider.h"
#include "CPlayer.h"
#include "CDropSystem.h"

CChest::CChest(DEVICE graphicDev, _vec3 pos) : CRenderObject(graphicDev), m_Pos(pos)
{
}

CChest::CChest(const CChest& rhs) : CRenderObject(rhs)
{
}

CChest::~CChest()
{
}

HRESULT CChest::Ready_GameObject()
{
    if (FAILED(CRenderObject::Ready_GameObject()))
        return E_FAIL;

    m_TransformCom->Set_Rotation({ D3DXToRadian(40), 0, 0 });
    m_TransformCom->Set_Pos(m_Pos);// 생성할때 셋팅
    m_TransformCom->Set_Scale({ 2.f,2.f,2.f });// 생성할때 셋팅

    m_InventoryCom->SetInvenSlotNum(7);// 기본셋팅 무조건 해야 하는거

    if (m_BoxColCom)
    {
        m_BoxColCom->Set_CollisionLayer(COLLISION_LAYERTYPE::INTERACTIVEOBJ);
        m_BoxColCom->Set_Size({ 2.f, 2.f, 2.f });
    }

    Animation_Setting();

    return S_OK;
}

void CChest::Initialize_GameObject()
{
    CRenderObject::Initialize_GameObject();

    m_AnimatorCom->Play_Animation(L"Chest-reveal", ANIMSTATE::ONCE);
    m_chest = CHESTSTATE::REVEAL;
}

bool CChest::AddItem(int id, int Num)
{
    ItemInstance Item = { id, -1, 0 };

    for (int i = 0; i < Num; i++)
    {
        
        if (m_InventoryCom->AddItem(Item) == false)
            return false;
    }
    return true;
}

_int CChest::Update_GameObject(const _float& timeDelta)
{
    _int exit = CRenderObject::Update_GameObject(timeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

    switch (m_chest)
    {
        case CHESTSTATE::REVEAL:
        {
            if (m_AnimatorCom->Get_CurFrame() == m_AnimatorCom->Get_MaxFrame() - 1)
            {
                m_RevealDone = true;
               
            }
           if( m_bPlayerOverlap == true && m_RevealDone == true)
           {
               m_chest = CHESTSTATE::OPEN;
           }
        }
        break;
        case CHESTSTATE::OPEN:
        {
            
                m_AnimatorCom->Play_Animation(L"Chest-open", ANIMSTATE::ONCE);

                if (m_AnimatorCom->Get_CurFrame() == 20)
                {
                    // 드롭 아이템
                    CDropSystem::GetInstance()->SpawnAllFromInven(m_GraphicDev, m_InventoryCom, { 1,0,1 }, 360);
                }

                if (m_AnimatorCom->Get_CurFrame() == m_AnimatorCom->Get_MaxFrame() - 1)
                {
                    Mark_As_PendingKill();
                }

            
        }
        break;

    }
         
   

    return exit;
}

void CChest::LateUpdate_GameObject(const _float& timeDelta)
{
    CRenderObject::LateUpdate_GameObject(timeDelta);

}

void CChest::Render_GameObject()
{
    // Transform 설정
    if (m_TransformCom)
        m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());

    Render_Setting();

    if (m_TextureCom && m_AnimatorCom)
    {
        const wstring& key = m_AnimatorCom->Get_CurKey();
        _int frame = m_AnimatorCom->Get_CurFrame();
        m_TextureCom->Set_Texture(key, frame);
    }

    // 버퍼 렌더링
    if (m_BufferCom)
        m_BufferCom->Render_Buffer();

    Render_Reset();

    // 충돌체 렌더 (디버그용)
    if (m_BoxColCom)
        m_BoxColCom->Render();
}

void CChest::Animation_Setting()
{
    // 애니메이션 생성
    m_AnimatorCom->Create_Animation(L"Chest-reveal", 57, 0.02f);
    m_AnimatorCom->Create_Animation(L"Chest-open", 50, 0.02f);

    //// State -> Animation 연동
    //m_StateCom->Set_AnimInfo(ACTORSTATE::IDLE, L"HumanMonsterIdle", ANIMSTATE::LOOP);
    //m_StateCom->Set_AnimInfo(ACTORSTATE::RUN, L"HumanMonsterRun", ANIMSTATE::LOOP);
    //m_StateCom->Set_AnimInfo(ACTORSTATE::HIT, L"HumanMonsterHurt", ANIMSTATE::ONCE);
    //m_StateCom->Set_AnimInfo(ACTORSTATE::ATTACK, L"HumanMonsterAttack", ANIMSTATE::ONCE);
}

HRESULT CChest::Add_Component()
{
    if (FAILED(CRenderObject::Add_Component()))
        return E_FAIL;

    // Texture
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_CHEST);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_CHEST, m_TextureCom });

    // Animator
    m_AnimatorCom = CreateProtoComponent<CAnimator>(this, COMPONENTTYPE::ANIMATOR);
    NULL_CHECK_RETURN(m_AnimatorCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::ANIMATOR, m_AnimatorCom });

    m_AnimatorCom->Set_TextureType(COMPONENTTYPE::TEX_CHEST);

    // Inventory
    m_InventoryCom = CreateProtoComponent<CInventory>(this, COMPONENTTYPE::INVENTORY);
    NULL_CHECK_RETURN(m_InventoryCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::INVENTORY, m_InventoryCom });

    // boxcoll
    m_BoxColCom = CreateProtoComponent<CBoxCollider>(this, COMPONENTTYPE::BOX_COLL);
    NULL_CHECK_RETURN(m_BoxColCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::BOX_COLL, m_BoxColCom });


    return S_OK;
}

void CChest::OnBeginOverlap(CCollider* self, CCollider* other)
{
    CRenderObject::OnBeginOverlap(self, other);

    CPlayer* ply = dynamic_cast<CPlayer*>(other->Get_Owner());
    if (ply)
        m_bPlayerOverlap = true;

}

void CChest::OnEndOverlap(CCollider* self, CCollider* other)
{
    CRenderObject::OnEndOverlap(self, other);


}




CChest* CChest::Create(DEVICE graphicDev, _vec3 pos)
{
    auto Chest = new CChest(graphicDev, pos);

    if (FAILED(Chest->Ready_GameObject()))
    {
        MSG_BOX("Chest Create Failed");
        Safe_Release(Chest);
        return nullptr;
    }

    return Chest;
}

void CChest::Free()
{
    CRenderObject::Free();
}
