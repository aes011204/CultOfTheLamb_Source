#include "pch.h"
#include "CRock.h"

#include "CBoxCollider.h"
#include "CCombatStat.h"
#include "CCreateHelper.h"
#include "CDInputMgr.h"
#include "CDropSystem.h"
#include "CHarvestableHerb.h"
#include "CInventory.h"
#include "CParticleHelper.h"
#include "CPlayer.h"
#include "CRcTex.h"
#include "CRenderer.h"
#include "CSpriteEff.h"
#include "CState.h"
#include "CTexture.h"
#include "CTransform.h"
#include "ItemInstance.h"

CRock::CRock(DEVICE graphicDev, ROCK rock)
    : CWorldInteractive(graphicDev),
      m_rock(rock),
      m_MovedPos(_vec3(0.f, 0.f, 0.f))
{ }

CRock::CRock(const CRock& rhs)
    : CWorldInteractive(rhs),
      m_MovedPos(rhs.m_MovedPos)
{ }

CRock::~CRock()
{ }

HRESULT CRock::Ready_GameObject()
{
    if (FAILED(CWorldInteractive::Ready_GameObject()))
        return E_FAIL;

    switch (m_rock)
    {
    case SMALLROCK:
        m_deadTexIndex = 999;
        m_TexIndex1 = 0;
        m_TexIndex2 = 999; // 안쓸예정

        m_ItemNum = 3;

        m_CombatStatCom->Set_MaxHp(10.f);
        m_TransformCom->Set_Scale(_vec3(1.f, 1.f, 1.f));
        break;
    case MIDDLEROCK:
        m_deadTexIndex = 999;
        m_TexIndex1 = 1;
        m_TexIndex2 = 999;

        m_ItemNum = 4;

        m_CombatStatCom->Set_MaxHp(16.f);
        m_TransformCom->Set_Scale(_vec3(1.f, 1.f, 1.f));
        break;
    case BIGROCK:
        m_deadTexIndex = 999;
        m_TexIndex1 = 2;
        m_TexIndex2 = 999;

        m_ItemNum = 6;

        m_CombatStatCom->Set_MaxHp(20.f);
        m_TransformCom->Set_Scale(_vec3(1.f, 1.f, 1.f));
        break;
    case END_ROCK:
        return E_FAIL;
        break;
    }

    // 밑은 공통
    m_Type = TYPE::ROCK;

    m_CurTexIndex = m_TexIndex1;

    m_CombatStatCom->Set_Hp(m_CombatStatCom->Get_MaxHp());

    m_TransformCom->Set_Pos(_vec3(30.f, 1.f, 30.f));// 밖에서 덮어 씌울예정

    if (m_BoxColCom)
    {
        m_BoxColCom->Set_CollisionLayer(COLLISION_LAYERTYPE::INTERACTIVEOBJ);
        m_BoxColCom->Set_Size({ 3.f, 3.f, 3.f });
    }

    m_InventoryCom->SetInvenSlotNum(7);// 기본셋팅 무조건 해야 하는거

    return S_OK;
}

void CRock::Initialize_GameObject()
{
    CWorldInteractive::Initialize_GameObject();
    // 인벤 셋팅 (동전 1001, 라즈베리 1002, 잔디 1003, 나무 1005, 돌 1006)

    ItemInstance Item = { 1006, -1, 0 };

    for (int i = 0; i < m_ItemNum; i++)
    {
        m_InventoryCom->AddItem(Item);
    }

    m_KockBackTimer = 0.1f;

    m_OrignPos = m_TransformCom->Get_Pos();
}

void CRock::OnInteractiveDamaged()
{
    CWorldInteractive::OnInteractiveDamaged();

    this->m_OnDamage = true;

    //CParticleHelper::Spawn_Particle({ m_TransformCom->Get_Pos().x, m_TransformCom->Get_Pos().y + 0.3f, m_TransformCom->Get_Pos().z },
    //                                L"HitEffect_Star",
    //                                14);

    CParticleHelper::Spawn_StaticBurstParticle(
        m_TransformCom->Get_Pos(),          // 위치
        COMPONENTTYPE::TEX_ROCK_PARTICLE,   // 텍스처 타입
        5,                                  // 생성할 개수
        0.1f, 0.12f,                        // 크기 범위
        2,                                  // 텍스처 랜덤으로 몇개 출력
        15.f,                               // 땅에 떨어지는 속도
        m_TransformCom->Get_Pos().y - 0.5f  // 바닥 Y (돌 아래쪽)
    );
}

void CRock::KnockBackEff(const _float& timeDelta)
{
    _vec3 backDir = m_CombatStatCom->Get_Dir();

    D3DXVec3Normalize(&backDir, &backDir);
    if (m_KockBackTimer > 0.f)
    {
        // 뒤로 밀리기
        m_KockBackTimer -= timeDelta;

        m_MovedPos += backDir * .8f * timeDelta;//1은 스피드
        m_TransformCom->Set_Pos(m_OrignPos + m_MovedPos);

        return;
    }
    else
        m_KockDone = true;

    if (m_KockDone == true && m_ReturnDone == false)
    {
        LerpTime += timeDelta * 5.f;

        if (LerpTime >= 0.f)
        {
            LerpTime     = 0;
            m_ReturnDone = true;

            return;
        }

        _vec3 cur = m_TransformCom->Get_Pos();

        D3DXVec3Lerp(&cur, &cur, &m_OrignPos, LerpTime);

        m_TransformCom->Set_Pos(cur);
    }

    if (m_ReturnDone)
    {
        m_TransformCom->Set_Pos(m_OrignPos);

        m_KockDone      = false;
        m_ReturnDone    = false;
        m_KockBackTimer = 0.1f;
        m_OnDamage      = false;
        LerpTime        = 0.f;
        m_MovedPos = { 0.f, 0.f, 0.f };
    }
}

_int CRock::Update_GameObject(const _float& timeDelta)
{
    _int exit = CWorldInteractive::Update_GameObject(timeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

    if (m_OnDamage == true)
        KnockBackEff(timeDelta);

    if (m_CombatStatCom->Is_Dead() == true)
    {
        // 텍스쳐 바꿈
        m_CurTexIndex = m_deadTexIndex;
        return 0;
    }

    return exit;
}

void CRock::LateUpdate_GameObject(const _float& timeDelta)
{
    CWorldInteractive::LateUpdate_GameObject(timeDelta);
}

void CRock::Render_GameObject()
{
    if (m_CurTexIndex == 999)
        return;
    m_TextureCom->Set_Texture(m_CurTexIndex);
    CWorldInteractive::Render_GameObject();
}

HRESULT CRock::Add_Component()
{
    if (FAILED(CWorldInteractive::Add_Component()))
        return E_FAIL;
    // texture
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_ROCK);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_ROCK, m_TextureCom });

    return S_OK;
}

void CRock::OnBeginOverlap(CCollider* self, CCollider* other)
{
    CWorldInteractive::OnBeginOverlap(self, other);
}

void CRock::OnEndOverlap(CCollider* self, CCollider* other)
{
    CWorldInteractive::OnEndOverlap(self, other);
}

CRock* CRock::Create(DEVICE graphicDev, ROCK rock)
{
    auto Rock = new CRock(graphicDev, rock);

    if (FAILED(Rock->Ready_GameObject()))
    {
        MSG_BOX("Rock Create Failed");
        Safe_Release(Rock);
        return nullptr;
    }

    return Rock;
}

void CRock::Free()
{
    CWorldInteractive::Free();
}
