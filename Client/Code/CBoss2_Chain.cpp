#include "pch.h"
#include "CBoss2_Chain.h"
#include "CAnimator.h"
#include "CCombatStat.h"
#include "CCreateHelper.h"
#include "CRcTex.h"
#include "CRenderer.h"
#include "CTexture.h"
#include "CTransform.h"
#include "CAttackBuilder.h"

CBoss2_Chain::CBoss2_Chain(DEVICE graphicDev, CGameObject* owner)
    : CGameObject(graphicDev), m_pOwner(owner)
{
    m_pOwner->AddRef();

}

CBoss2_Chain::CBoss2_Chain(const CBoss2_Chain& rhs) : CGameObject(rhs)
{
}

CBoss2_Chain::~CBoss2_Chain()
{
}

HRESULT CBoss2_Chain::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    wstring key = L"Boss2_Chain";

    // 비율 맞춰주는거 생성할때 외부에서 SetAspectScale로 스케일 조정 
    SetTexAspectInit(m_TextureCom, m_TransformCom, 0, &key);

    SetAspectScale(m_TransformCom, { .2f,.2f,0.f });

    Animation_Setting();

    m_AnimatorCom->Play_Animation(L"Boss2_Chain", ANIMSTATE::ONCE);

    const auto ownerTransform =
        dynamic_cast<CTransform*>(m_pOwner->Get_Component(
            ID_DYNAMIC,
            COMPONENTTYPE::TRANSFORM));

     //TODO:: 나중에 땅의 위치를 알게 되면 수정 
     //m_TransformCom->Set_Pos({ ownerTransform->Get_Pos().x, -ownerTransform->Get_Scale().y * .5f,ownerTransform->Get_Pos().z });
     //m_TransformCom->Set_Pos
     //  if (m_BoxColCom)
     //      m_BoxColCom->Set_CollisionLayer(COLLISION_LAYERTYPE::BOSSPROJECTILE);

    return S_OK;
}

_int CBoss2_Chain::Update_GameObject(const _float& timeDelta)
{
    _int exit = CGameObject::Update_GameObject(timeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

    if (m_flag == false)
    {
        {
            CAttackBuilder(m_GraphicDev, this)
                .Box()
                .Damage(1.f)
                .Knockback(0.f)
                .Position(m_TransformCom->Get_Pos())
                .InstigatorPos()
                .Offset(_vec3(0.f, 0.f, 0.f))
                .LifeTime(.3f)
                .CollisionLayer(COLLISION_LAYERTYPE::BOSSPROJECTILE)
                .Scale({ m_TransformCom->Get_Scale().x, m_TransformCom->Get_Scale().y * .5f , m_TransformCom->Get_Scale().z * .5f })
                .Spawn();

        }
        m_flag = true;
    }

    // 해당 프레임이 끝나면 자기삭제
    if (m_AnimatorCom->Get_CurFrame() == m_AnimatorCom->Get_MaxFrame() - 1)
    {
        this->Mark_As_PendingKill();
    }

    return exit;
}

void CBoss2_Chain::LateUpdate_GameObject(const _float& timeDelta)
{
    CGameObject::LateUpdate_GameObject(timeDelta);
}

void CBoss2_Chain::Render_GameObject()
{
    m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());

    m_GraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_GraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_GraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    m_GraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    m_GraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
    m_GraphicDev->SetRenderState(D3DRS_ALPHAREF, 0);

    if (m_TextureCom && m_AnimatorCom)
    {
        const wstring& key = m_AnimatorCom->Get_CurKey();
        _int           frame = m_AnimatorCom->Get_CurFrame();

        m_TextureCom->Set_Texture(key, frame);
    }

    m_BufferCom->Render_Buffer();

    m_GraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    m_GraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

    m_GraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);


}

void CBoss2_Chain::OnBeginOverlap(CCollider* self, CCollider* other)
{
    CGameObject::OnBeginOverlap(self, other);
}

void CBoss2_Chain::OnEndOverlap(CCollider* self, CCollider* other)
{
    CGameObject::OnEndOverlap(self, other);
}

CBoss2_Chain* CBoss2_Chain::Create(DEVICE graphicDev, CGameObject* owner)
{
    auto BossChain = new CBoss2_Chain(graphicDev, owner);

    if (FAILED(BossChain->Ready_GameObject()))
    {
        MSG_BOX("BossChain Create Failed");
        Safe_Release(BossChain);
        return nullptr;
    }

    return BossChain;
}

void CBoss2_Chain::Animation_Setting()
{
    m_AnimatorCom->Create_Animation(L"Boss2_Chain", 34, 0.03f);

}

HRESULT CBoss2_Chain::Add_Component()
{
    // buffer
    m_BufferCom = CreateProtoComponent<CRcTex>(this, COMPONENTTYPE::RC_TEX);
    NULL_CHECK_RETURN(m_BufferCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::RC_TEX, m_BufferCom });

    // transform
    m_TransformCom = CreateProtoComponent<CTransform>(this, COMPONENTTYPE::TRANSFORM);
    NULL_CHECK_RETURN(m_TransformCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::TRANSFORM, m_TransformCom });

    // texture
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_BOSS2_PHASE2);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_BOSS2_PHASE2, m_TextureCom });

    // Animator
    m_AnimatorCom = CreateProtoComponent<CAnimator>(this, COMPONENTTYPE::ANIMATOR);
    NULL_CHECK_RETURN(m_AnimatorCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::ANIMATOR, m_AnimatorCom });

    m_AnimatorCom->Set_TextureType(COMPONENTTYPE::TEX_BOSS2_PHASE2);

    // CombatStat
    m_CombatStatCom = CreateProtoComponent<CCombatStat>(this, COMPONENTTYPE::COMBATSTAT);
    NULL_CHECK_RETURN(m_CombatStatCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::COMBATSTAT, m_CombatStatCom });

    return S_OK;
}

void CBoss2_Chain::Free()
{
    Safe_Release(m_pOwner);

    CGameObject::Free();
}
