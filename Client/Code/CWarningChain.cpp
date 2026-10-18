#include "pch.h"
#include "CWarningChain.h"

#include "CAnimator.h"
#include "CAttackBox.h"
#include "CBoss2_Chain.h"
#include "CBossChain.h"
#include "CBossPhase1.h"
#include "CBossPhase2.h"
#include "CCombatStat.h"
#include "CCreateHelper.h"
#include "CRcTex.h"
#include "CRenderer.h"
#include "CTexture.h"
#include "CTransform.h"

CWarningChain::CWarningChain(DEVICE graphicDev, CGameObject* owner)
    : CGameObject(graphicDev),

      m_BufferCom(nullptr),
      m_TransformCom(nullptr),
      m_TextureCom(nullptr),
      m_BoxColCom(nullptr),
      m_CombatStatCom(nullptr),
      m_pOwner(owner)
{
    m_pOwner->AddRef();
}

CWarningChain::CWarningChain(const CWarningChain& rhs)
    : CGameObject(rhs),

      m_BufferCom(nullptr),
      m_TransformCom(nullptr),
      m_TextureCom(nullptr),
      m_BoxColCom(nullptr),
      m_CombatStatCom(nullptr),
      m_pOwner(nullptr)
{ }

CWarningChain::~CWarningChain()
{ }

HRESULT CWarningChain::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    SetTexAspectInit(m_TextureCom, m_TransformCom, 0);

    SetAspectScale(m_TransformCom, { .5f, .5f, 0.f });

    const auto ownerTransform =
        dynamic_cast<CTransform*>(m_pOwner->Get_Component(
            ID_DYNAMIC,
            COMPONENTTYPE::TRANSFORM));

    //TODO:: 나중에 땅의 위치를 알게 되면 수정 
    m_TransformCom->Set_Pos({ ownerTransform->Get_Pos().x, /*-ownerTransform->Get_Scale().y * .5f*/0.f, ownerTransform->Get_Pos().z });

    m_TransformCom->Set_Rotation({ D3DXToRadian(90), 0, 0 });

    return S_OK;
}

float CWarningChain::easeOutElastic(float x)
{
    // 진동 주기 조정 상수 (2π/3)
    const float period = (2 * D3DX_PI) / 3;

    // 0~1의 시작/끝 특별 처리
    if (x == 0.0f) return 0.0f;
    if (x == 1.0f) return 1.0f;

    // 🔹 1) 감쇠되는 진폭(amplitude decay)
    const float amplitude = powf(2, -10 * x);

    // 🔹 2) 진동(oscillation) 
    const float oscillation = sinf((x * 3 - 0.1) * period);

    // 🔹 3) 전체 결과값
    return amplitude * oscillation + 1;
}

_int CWarningChain::Update_GameObject(const _float& timeDelta)
{
    _int exit = CGameObject::Update_GameObject(timeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

    if (m_Sizeflag == false)
    {
        // 초기 사이즈 저장
        m_originSize = m_TransformCom->Get_Scale();
        m_Sizeflag   = true;
    }
    m_fElapsedTime += timeDelta;

    //if (eff == false)
    //{
    _float i       = Clamp(m_fElapsedTime, 0.f, 1.f);
    _float effSize = easeOutElastic(i);
    //effSize = Clamp(effSize, 0.5, 2);

    m_TransformCom->Set_Scale({ m_originSize.x * effSize, m_originSize.y * effSize, 0.f });
    // }

    if (m_fElapsedTime > 0.8f && m_flag == false)
    {
		CBossPhase1* boss1 = dynamic_cast<CBossPhase1*>(m_pOwner);
		CBossPhase2* boss2 = dynamic_cast<CBossPhase2*>(m_pOwner);

		if (boss1)
		{
			CBossChain* chain = CBossChain::Create(boss1->Get_GraphicDev(), boss1);

			if (chain)
			{
				CTransform* ChainTransform =
					dynamic_cast<CTransform*>(chain->Get_Component(
						ID_DYNAMIC,
						COMPONENTTYPE::TRANSFORM));
				_vec3 tmp = { 0.f, ChainTransform->Get_Scale().y, 0.f };
				_vec3 offset = m_TransformCom->Get_Pos() + tmp;
				ChainTransform->Set_Pos(offset);
				SpawnObjectOnLayer(chain);
			}
		}
		else if (boss2)
		{
			CBoss2_Chain* chain = CBoss2_Chain::Create(boss2->Get_GraphicDev(), boss2);

			if (chain)
			{
				CTransform* ChainTransform =
					dynamic_cast<CTransform*>(chain->Get_Component(
						ID_DYNAMIC,
						COMPONENTTYPE::TRANSFORM));

				if (ChainTransform)
				{
					_vec3 tmp = { 0.f, ChainTransform->Get_Scale().y, 0.f };
					_vec3 offset = m_TransformCom->Get_Pos() + tmp;

					ChainTransform->Set_Scale(0.8f, 3.f, 1.f);
					ChainTransform->Set_Pos(offset);
					SpawnObjectOnLayer(chain);
				}
                else
                {
					SpawnObjectOnLayer(chain);
                }
			}
		}

		m_fElapsedTime = 0;
		m_flag = true;
    }
    //
    //if (chain != nullptr)
    //{

    //    CAnimator* ChainAni =
    //        dynamic_cast<CAnimator*>(chain->Get_Component(COMPONENTID::ID_DYNAMIC, COMPONENTTYPE::ANIMATOR));
    //    if (ChainAni->Get_CurFrame() == ChainAni->Get_MaxFrame() - 2) // 혹시 사슬객체가 먼저 사라지는지 확인 필요
    //    {
    //        m_bFade = true;
    //    }
    //}

    if (m_fElapsedTime > 0.3f && m_flag == true)
        m_bFade = true;

    if (m_bFade == true)
    {
        m_fAlpha -= timeDelta * 1;
        if (m_fAlpha <= 0)
        {
            m_fAlpha = 0;
            this->Mark_As_PendingKill();
        }
    }
    // 해당 프레임이 끝나면 자기삭제

    return exit;
}

void CWarningChain::LateUpdate_GameObject(const _float& timeDelta)
{
    CGameObject::LateUpdate_GameObject(timeDelta);
}

void CWarningChain::Render_GameObject()
{
    m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());
    //

    m_GraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_GraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_GraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    m_GraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    m_GraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
    m_GraphicDev->SetRenderState(D3DRS_ALPHAREF, 0);
    //

    m_TextureCom->Set_Texture(0);

    m_GraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(static_cast<int>(m_fAlpha * 255), 255, 255, 255));
    //
    m_GraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR); // ← texturefactor.alpha 사용

    m_BufferCom->Render_Buffer();
    //
    m_GraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
    //
    m_GraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    m_GraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    //
    m_GraphicDev->SetRenderState(
        D3DRS_TEXTUREFACTOR,
        D3DCOLOR_ARGB(255, 255, 255, 255)); // 알파 1.0, 색 그대로
}

void CWarningChain::OnBeginOverlap(CCollider* self, CCollider* other)
{
    CGameObject::OnBeginOverlap(self, other);
}

void CWarningChain::OnEndOverlap(CCollider* self, CCollider* other)
{
    CGameObject::OnEndOverlap(self, other);
}

void CWarningChain::Animation_Setting()
{ }

HRESULT CWarningChain::Add_Component()
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
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_OUT);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_OUT, m_TextureCom });

    // CombatStat
    m_CombatStatCom = CreateProtoComponent<CCombatStat>(this, COMPONENTTYPE::COMBATSTAT);
    NULL_CHECK_RETURN(m_CombatStatCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::COMBATSTAT, m_CombatStatCom });

    return S_OK;
}

CWarningChain* CWarningChain::Create(DEVICE graphicDev, CGameObject* owner)
{
    auto WarningChain = new CWarningChain(graphicDev, owner);

    if (FAILED(WarningChain->Ready_GameObject()))
    {
        MSG_BOX("WarningChain Create Failed");
        Safe_Release(WarningChain);
        return nullptr;
    }

    return WarningChain;
}

void CWarningChain::Free()
{
    Safe_Release(m_pOwner);
    // Safe_Release(chain);
    CGameObject::Free();
}
