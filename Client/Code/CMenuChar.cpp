#include "pch.h"
#include "CMenuChar.h"

#include "CAnimator.h"
#include "CCreateHelper.h"
#include "CProtoMgr.h"
#include "CRcTex.h"
#include "CRenderer.h"
#include "CState.h"
#include "CTexture.h"
#include "CTransform.h"
#include "CTransform.h"

CMenuChar::CMenuChar(DEVICE graphicDev) 
    : CGameObject(graphicDev),
      m_BufferCom(nullptr),
      m_TransformCom(nullptr),
      m_TextureCom(nullptr),
      m_AnimatorCom(nullptr),
      m_StateCom(nullptr)
{ }

CMenuChar::CMenuChar(const CMenuChar& rhs)
    : CGameObject(rhs),
      m_BufferCom(nullptr),
      m_TransformCom(nullptr),
      m_TextureCom(nullptr),
      m_AnimatorCom(nullptr),
      m_StateCom(nullptr)
{ }

CMenuChar::~CMenuChar()
{ }

HRESULT CMenuChar::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    Animation_Setting();

    m_TransformCom->Set_Pos({ 0.f, 1.1f, 1.f });
    m_TransformCom->Set_Scale(2.1f, 2.5f, 2.5f);

    wstring key = L"MenuChar";
    if (false == SetTexAspectInit(m_TextureCom, m_TransformCom, 1, &key))
        return E_FAIL;




    return S_OK;
}

_int CMenuChar::Update_GameObject(const _float& timeDelta)
{
    _int exit = CGameObject::Update_GameObject(timeDelta);
    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

    if (m_TransformComPattern)
    {
        m_TransformComPattern->Update_Component(timeDelta);
    }

    _vec3 mainPos = m_TransformCom->Get_Pos();
    _vec3 mainSize = m_TransformCom->Get_Scale();
    m_TransformComPattern->Set_Pos({ mainPos.x,mainPos.y -mainSize.y, mainPos.z+0.2f });
    m_TransformComPattern->Set_Scale(2.5f, 2.5f, 2.5f);
    m_TransformComPattern->Set_Rotation({ D3DXToRadian(90),0.f,0.f });


    return exit;
}

void CMenuChar::LateUpdate_GameObject(const _float& timeDelta)
{
    CGameObject::LateUpdate_GameObject(timeDelta);
}

void CMenuChar::Render_GameObject()
{
    //_GraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
    m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());

    Render_Setting();

    if (m_TextureCom && m_AnimatorCom)
    {
        const wstring& key   = m_AnimatorCom->Get_CurKey();
        _int           frame = m_AnimatorCom->Get_CurFrame();

        m_TextureCom->Set_Texture(key, frame);
    }

    m_BufferCom->Render_Buffer();
    //

    m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformComPattern->Get_World());
    m_TextureComPattern->Set_Texture(1);
    m_BufferComPattern->Render_Buffer();

    Render_Reset();
    //m_GraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
}

void CMenuChar::Animation_Setting()
{
    // 애니메이션 생성
    m_AnimatorCom->Create_Animation(L"MenuChar", 100, 0.02f);

    // State -> Animation 연동
    m_StateCom->Set_AnimInfo(ACTORSTATE::IDLE, L"MenuChar", ANIMSTATE::LOOP);
}

HRESULT CMenuChar::Add_Component()
{
    //buffer
    m_BufferCom = CreateProtoComponent<CRcTex>(this, COMPONENTTYPE::RC_TEX);
    NULL_CHECK_RETURN(m_BufferCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::RC_TEX, m_BufferCom });

    //transform
    m_TransformCom = CreateProtoComponent<CTransform>(this, COMPONENTTYPE::TRANSFORM);
    NULL_CHECK_RETURN(m_TransformCom, E_FAIL);

    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::TRANSFORM, m_TransformCom });

    // texture
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_MENU_CHAR);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);

    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_MENU_CHAR, m_TextureCom });

    // Animator
    m_AnimatorCom = CreateProtoComponent<CAnimator>(this, COMPONENTTYPE::ANIMATOR);
    NULL_CHECK_RETURN(m_AnimatorCom, E_FAIL);

    m_AnimatorCom->Set_TextureType(COMPONENTTYPE::TEX_MENU_CHAR);

    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::ANIMATOR, m_AnimatorCom });

    m_StateCom = CreateProtoComponent<CState>(this, COMPONENTTYPE::STATE);
    NULL_CHECK_RETURN(m_StateCom, E_FAIL);

    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::STATE, m_StateCom });


    // 바닥용 레이어에 안넣고 자신이 세이프릴리즈 할거임
    //buffer
    m_BufferComPattern = CreateProtoComponent<CRcTex>(this, COMPONENTTYPE::RC_TEX);
    NULL_CHECK_RETURN(m_BufferComPattern, E_FAIL);
   // m_Components[ID_STATIC].insert({ COMPONENTTYPE::RC_TEX, m_BufferComPattern });

    //transform
    m_TransformComPattern = CreateProtoComponent<CTransform>(this, COMPONENTTYPE::TRANSFORM);
    NULL_CHECK_RETURN(m_TransformComPattern, E_FAIL);
   // m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::TRANSFORM, m_TransformComPattern });

    m_TextureComPattern = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_MAINBG);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);
    //m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_MENU_CHAR, m_TextureCom });


    return S_OK;
}

void CMenuChar::Render_Setting()
{
    m_GraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_GraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_GraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_GraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    m_GraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    m_GraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
    m_GraphicDev->SetRenderState(D3DRS_ALPHAREF, 0);
}

void CMenuChar::Render_Reset()
{
    m_GraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    m_GraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    m_GraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CMenuChar::OnBeginOverlap(CCollider* self, CCollider* other)
{ }

void CMenuChar::OnEndOverlap(CCollider* self, CCollider* other)
{ }

CMenuChar* CMenuChar::Create(DEVICE graphicDev)
{
    auto MenuChar = new CMenuChar(graphicDev);

    if (FAILED(MenuChar->Ready_GameObject()))
    {
        MSG_BOX("MenuChar Create Failed");
        Safe_Release(MenuChar);
        return nullptr;
    }

    return MenuChar;
    //return nullptr;
}

void CMenuChar::Free()
{
    Safe_Release(m_BufferComPattern);
    Safe_Release(m_TextureComPattern);
    Safe_Release(m_TransformComPattern);
    CGameObject::Free();
}
