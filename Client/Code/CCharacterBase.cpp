#include "pch.h"
#include "CCharacterBase.h"

#include "CAnimator.h"
#include "CBoxCollider.h"
#include "CCombatStat.h"
#include "CCreateHelper.h"
#include "CRcTex.h"
#include "CRenderer.h"
#include "CState.h"
#include "CTexture.h"
#include "CTransform.h"

CCharacterBase::CCharacterBase(DEVICE graphicDev)
    : CRenderObject(graphicDev),
      m_AnimatorCom(nullptr),
      m_StateCom(nullptr),
      m_CombatStatCom(nullptr),
      m_BoxColCom(nullptr)
{ }

CCharacterBase::CCharacterBase(const CCharacterBase& rhs)
    : CRenderObject(rhs),
      m_AnimatorCom(nullptr),
      m_StateCom(nullptr),
      m_CombatStatCom(nullptr),
      m_BoxColCom(nullptr)
{ }

CCharacterBase::~CCharacterBase()
{ }

HRESULT CCharacterBase::Ready_GameObject()
{
    if (FAILED(CRenderObject::Ready_GameObject()))
        return E_FAIL;

    if (m_StateCom)
    {
        m_StateCom->Change_State(ACTORSTATE::IDLE);
        m_StateCom->Change_Dir(ACTORDIR::LEFT);
    }

    // 자식이 애니메이션 설정
    Animation_Setting();

    return S_OK;
}

void CCharacterBase::Initialize_GameObject()
{
    CRenderObject::Initialize_GameObject();
}

_int CCharacterBase::Update_GameObject(const _float& timeDelta)
{
    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

    _int exit = CRenderObject::Update_GameObject(timeDelta);

    return exit;
}

void CCharacterBase::LateUpdate_GameObject(const _float& timeDelta)
{
    CRenderObject::LateUpdate_GameObject(timeDelta);
}

void CCharacterBase::Render_GameObject()
{
    // Transform 설정
    if (m_TransformCom)
        m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());

    Render_Setting();

    if (m_TextureCom && m_AnimatorCom)
    {
        const wstring& key   = m_AnimatorCom->Get_CurKey();
        _int           frame = m_AnimatorCom->Get_CurFrame();

        m_TextureCom->Set_Texture(key, frame);
    }

    if (m_BufferCom)
        m_BufferCom->Render_Buffer();

    Render_Reset();

    // 충돌체 렌더 (디버그용)
    if (m_BoxColCom)
        m_BoxColCom->Render();
}

void CCharacterBase::OnBeginOverlap(CCollider* self, CCollider* other)
{
    CGameObject::OnBeginOverlap(self, other);
}

void CCharacterBase::OnEndOverlap(CCollider* self, CCollider* other)
{
    CGameObject::OnEndOverlap(self, other);
}

HRESULT CCharacterBase::Add_Component()
{
    if (FAILED(CRenderObject::Add_Component()))
        return E_FAIL;

    // Animator
    m_AnimatorCom = CreateProtoComponent<CAnimator>(this, COMPONENTTYPE::ANIMATOR);
    NULL_CHECK_RETURN(m_AnimatorCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::ANIMATOR, m_AnimatorCom });

    // State
    m_StateCom = CreateProtoComponent<CState>(this, COMPONENTTYPE::STATE);
    NULL_CHECK_RETURN(m_StateCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::STATE, m_StateCom });

    // CombatStat
    m_CombatStatCom = CreateProtoComponent<CCombatStat>(this, COMPONENTTYPE::COMBATSTAT);
    NULL_CHECK_RETURN(m_CombatStatCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::COMBATSTAT, m_CombatStatCom });

    // BoxCollider
    m_BoxColCom = CreateProtoComponent<CBoxCollider>(this, COMPONENTTYPE::BOX_COLL);
    NULL_CHECK_RETURN(m_BoxColCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::BOX_COLL, m_BoxColCom });

    return S_OK;
}

void CCharacterBase::Render_Setting()
{
    CRenderObject::Render_Setting();

}

void CCharacterBase::Free()
{
    CRenderObject::Free();
}
