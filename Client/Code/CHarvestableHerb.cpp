#include "pch.h"
#include "CHarvestableHerb.h"
#include "CParticleHelper.h"
#include "CBoxCollider.h"
#include "CCombatStat.h"
#include "CCreateHelper.h"
#include "CDInputMgr.h"
#include "CDropSystem.h"
#include "CInventory.h"
#include "CPlayer.h"
#include "CRcTex.h"
#include "CRenderer.h"
#include "CSpriteEff.h"
#include "CState.h"
#include "CTexture.h"
#include "CTransform.h"
#include "ItemInstance.h"

CHarvestableHerb::CHarvestableHerb(DEVICE graphicDev, BERRY m_Berry)
    : CWorldInteractive(graphicDev),
      m_Berry(m_Berry)
{ }

CHarvestableHerb::CHarvestableHerb(const CHarvestableHerb& rhs)
    : CWorldInteractive(rhs)
{ }

CHarvestableHerb::~CHarvestableHerb()
{ }

HRESULT CHarvestableHerb::Ready_GameObject()
{
    if (FAILED(CWorldInteractive::Ready_GameObject()))
        return E_FAIL;

    switch (m_Berry)
    {
    case SMALLBERRY:
        m_deadTexIndex = 3;
        m_TexIndex1 = 2;
        m_TexIndex2 = 999; // 안쓸예정

        m_ItemNum = 3;

        m_CombatStatCom->Set_MaxHp(10.f);
        m_TransformCom->Set_Scale(_vec3(0.8f, 0.8f, 0.8f));
        break;
    case BIGBERRY:
        m_deadTexIndex = 1;
        m_TexIndex1 = 6;
        m_TexIndex2 = 0;

        m_ItemNum = 5;

        m_CombatStatCom->Set_MaxHp(16.f);
        m_TransformCom->Set_Scale(_vec3(1.f, 1.f, 1.f));
        break;
    case END_BERRY:
        return E_FAIL;
        break;
    }

    // 밑은 공통
    m_Type = TYPE::BERRY;

    m_CurTexIndex = m_TexIndex1;

    m_CombatStatCom->Set_Hp(m_CombatStatCom->Get_MaxHp());

    m_TransformCom->Set_Pos(_vec3(30.f, 1.f, 30.f));// 밖에서 덮어 씌울예정

    if (m_BoxColCom)
    {
        m_BoxColCom->Set_CollisionLayer(COLLISION_LAYERTYPE::INTERACTIVEOBJ);
        m_BoxColCom->Set_Size({ 3.f, 3.f, 3.f });
    }

    m_InventoryCom->SetInvenSlotNum(5);// 기본셋팅 무조건 해야 하는거

    return S_OK;
}

void CHarvestableHerb::Initialize_GameObject()
{
    CWorldInteractive::Initialize_GameObject();

    // 인벤 셋팅 (동전 1001, 라즈베리 1002, 잔디 1003, 나무 1005, 돌 1006)
    ItemInstance Item = { 1002, -1, 0 };

    for (int i = 0; i < m_ItemNum; i++)
    {
        m_InventoryCom->AddItem(Item);
    }
}

_int CHarvestableHerb::Update_GameObject(const _float& timeDelta)
{
    _int exit = CWorldInteractive::Update_GameObject(timeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

    if (m_CombatStatCom->Is_Dead() == true)
    {
        // 텍스쳐 바꿈
        m_CurTexIndex = m_deadTexIndex;
        return 0;
    }

    return exit;
}

void CHarvestableHerb::LateUpdate_GameObject(const _float& timeDelta)
{
    CWorldInteractive::LateUpdate_GameObject(timeDelta);
}

void CHarvestableHerb::Render_GameObject()
{
    m_TextureCom->Set_Texture(m_CurTexIndex);

    CWorldInteractive::Render_GameObject();
}

void CHarvestableHerb::OnInteractiveDamaged()
{
    CWorldInteractive::OnInteractiveDamaged();

    CParticleHelper::Spawn_StaticBurstParticle(
        m_TransformCom->Get_Pos(),          // 위치
        COMPONENTTYPE::TEX_GRASS_PARTICLE,  // 텍스처 타입
        5,                                  // 생성할 개수
        0.1f, 0.12f,                 // 크기 범위
        6,                                  // 텍스처 랜덤으로 몇개 출력
        15.f,                               // 땅에 떨어지는 속도
        m_TransformCom->Get_Pos().y - 0.5f  // 바닥 Y (돌 아래쪽)
    );
}

HRESULT CHarvestableHerb::Add_Component()
{
    if (FAILED(CWorldInteractive::Add_Component()))
        return E_FAIL;
    // texture
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_BERRY);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_BERRY, m_TextureCom });

    return S_OK;
}

void CHarvestableHerb::OnBeginOverlap(CCollider* self, CCollider* other)
{
    CWorldInteractive::OnBeginOverlap(self, other);
}

void CHarvestableHerb::OnEndOverlap(CCollider* self, CCollider* other)
{
    CWorldInteractive::OnEndOverlap(self, other);
}

CHarvestableHerb* CHarvestableHerb::Create(DEVICE graphicDev, BERRY berryType)
{
    auto HarvestableHerb = new CHarvestableHerb(graphicDev, berryType);

    if (FAILED(HarvestableHerb->Ready_GameObject()))
    {
        MSG_BOX("HarvestableHerb Create Failed");
        Safe_Release(HarvestableHerb);
        return nullptr;
    }

    return HarvestableHerb;
}

void CHarvestableHerb::Free()
{
    CWorldInteractive::Free();
}
