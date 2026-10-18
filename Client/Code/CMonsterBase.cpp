#include "pch.h"
#include "CMonsterBase.h"
#include "CScene.h"
#include "CManagement.h"
#include "CAnimator.h"
#include "CBoxCollider.h"
#include "CCombatStat.h"
#include "CRcTex.h"
#include "CTexture.h"
#include "CTransform.h"
#include "CStage_Dungeon.h";

namespace Engine
{
class CScene;
}

CMonsterBase::CMonsterBase(DEVICE graphicDev)
    : CCharacterBase(graphicDev),
      m_Target(nullptr),
      m_TargetTransform(nullptr),
      m_StoppingDistance(2.f),
      m_AttackRange(2.5f),
      m_ChaseRange(10.f),
      m_AttackCooldown(0),
      m_AttackTimer(0),
      m_CanAttack(false)
{ }

CMonsterBase::CMonsterBase(const CMonsterBase& rhs)
    : CCharacterBase(rhs),
      m_Target(nullptr),
      m_TargetTransform(nullptr),
      m_StoppingDistance(rhs.m_StoppingDistance),
      m_AttackRange(rhs.m_AttackRange),
      m_ChaseRange(rhs.m_ChaseRange),
      m_AttackCooldown(rhs.m_AttackCooldown),
      m_AttackTimer(rhs.m_AttackTimer),
      m_CanAttack(false)
{ }

CMonsterBase::~CMonsterBase()
{ }

HRESULT CMonsterBase::Ready_GameObject()
{
    if (FAILED(CCharacterBase::Ready_GameObject()))
        return E_FAIL;

    m_CombatStatCom->Set_Speed(3.f);

    if (m_BoxColCom)
        m_BoxColCom->Set_CollisionLayer(COLLISION_LAYERTYPE::ENEMY);

    return S_OK;
}

void CMonsterBase::Initialize_GameObject()
{
    CCharacterBase::Initialize_GameObject();
}

_int CMonsterBase::Update_GameObject(const _float& timeDelta)
{

    _int exit = CCharacterBase::Update_GameObject(timeDelta);
 
    if (m_Target) // 타겟의 방향 설정
    {
        _vec3 pos       = m_TransformCom->Get_Pos();
        _vec3 targetPos = m_TargetTransform->Get_Pos();
        m_TargetDir     = targetPos - pos;
    }

    if (m_bHurt == true)
    {
        HurtTime += timeDelta;

        if (HurtTime > flashTime)
        {
            m_bHurt  = false;
            HurtTime = 0;
        }
    }

    return exit;
}

void CMonsterBase::LateUpdate_GameObject(const _float& timeDelta)
{
    CCharacterBase::LateUpdate_GameObject(timeDelta);
}

void CMonsterBase::Render_GameObject()
{
    if (m_TransformCom)
        m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());

    Render_Setting();

    // 4) 텍스처 바인딩
    if (m_TextureCom && m_AnimatorCom)
    {
        const wstring& key   = m_AnimatorCom->Get_CurKey();
        int            frame = m_AnimatorCom->Get_CurFrame();
        m_TextureCom->Set_Texture(key, frame);
    }

    //  5) 기본 초기화 (선택사항)
    m_GraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

    if (m_bHurt)
    {
        // 현제 값은 초기 값세팅을 한거 나중에 변경 하고 싶으면 인자 넣으면 됩니다 색,투명도
        FlashRenderStart();//색 (ARGB)
        AlphaRenderStart();//투명도(0~1)
    }

    // 7) 버텍스 버퍼 렌더
    if (m_BufferCom)
        m_BufferCom->Render_Buffer();

    // 초기화
    if (m_bHurt)
    {
        FlashRenderEnd();
        AlphaRenderEnd();
    }
    if (m_BoxColCom)
        m_BoxColCom->Render();
}

_float CMonsterBase::Get_DistanceToTarget()
{
    if (!m_TargetTransform || !m_TransformCom)
        return 9999.f;

    _vec3 targetPos  = m_TargetTransform->Get_Pos();
    _vec3 monsterPos = m_TransformCom->Get_Pos();

    _vec3 diff = monsterPos - targetPos;

    return D3DXVec3Length(&diff);
}

_bool CMonsterBase::Is_InAttackRange() const
{
    if (!m_TargetTransform || !m_TransformCom)
        return false;

    _vec3 pos       = m_TransformCom->Get_Pos();
    _vec3 targetPos = m_TargetTransform->Get_Pos();
    _vec3 diff      = targetPos - pos;

    _float distance = D3DXVec3Length(&diff);

    return distance <= m_AttackRange;
}

_bool CMonsterBase::Is_InStoppingRange() const
{
    if (!m_TargetTransform || !m_TransformCom)
        return false;

    _vec3 pos       = m_TransformCom->Get_Pos();
    _vec3 targetPos = m_TargetTransform->Get_Pos();
    _vec3 diff      = targetPos - pos;

    _float distance = D3DXVec3Length(&diff);

    return distance <= m_StoppingDistance;
}

_bool CMonsterBase::Is_InChaseRange() const
{
    if (!m_TargetTransform || !m_TransformCom)
        return false;

    _vec3 pos       = m_TransformCom->Get_Pos();
    _vec3 targetPos = m_TargetTransform->Get_Pos();
    _vec3 diff      = targetPos - pos;

    _float distance = D3DXVec3Length(&diff);

    return distance <= m_ChaseRange;
}

_bool CMonsterBase::Is_OutofChaseRange() const
{
    return !Is_InChaseRange();
}

void CMonsterBase::OnBeginOverlap(CCollider* self, CCollider* other)
{
    m_bHurt = true;
}

void CMonsterBase::OnEndOverlap(CCollider* self, CCollider* other)
{ }

void CMonsterBase::FlashRenderStart(DWORD color)
{
    m_GraphicDev->SetRenderState(
        D3DRS_TEXTUREFACTOR,
        D3DCOLOR_ARGB(255, 255, 255, 255)
    );

    // TEXTURE * TFACTOR 로 변경하면 Tint 색 정확히 나옴
    m_GraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_ADD);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
}

void CMonsterBase::FlashRenderEnd()
{
    // Hurt tint가 남지 않게 기본 연산도 복구
    m_GraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
}

void CMonsterBase::AlphaRenderStart(float alpha)
{
    //알파
    m_GraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(static_cast<int>(alpha * 255), 255, 255, 255));
    //
    m_GraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR); // ← texturefactor.alpha 사용
}

void CMonsterBase::AlphaRenderEnd()
{
    // 알파 리셋
    m_GraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
    m_GraphicDev->SetRenderState(
        D3DRS_TEXTUREFACTOR,
        D3DCOLOR_ARGB(255, 255, 255, 255)); // 알파 1.0, 색 그대로
}

void CMonsterBase::Trigger_OnDeath()
{
    if (m_OnDeath)
        m_OnDeath();

    auto scene = CManagement::GetInstance()->Get_Scene();
    if (scene)
        scene->OnMonsterDeath(this);
}

void CMonsterBase::Free()
{
    CCharacterBase::Free();
}
