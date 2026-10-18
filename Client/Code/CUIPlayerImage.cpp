#include "pch.h"
#include "CUI.h"
#include "CUIPlayerImage.h"
#include "CState.h"
#include "CTransform.h"
#include "CTexture.h"
#include "CAnimator.h"
#include "CCreateHelper.h"
#include "CRcTex.h"
#include "Engine_Macro.h"

CUIPlayerImage::CUIPlayerImage(DEVICE pGraphicDev) : CUI(pGraphicDev)
{
    
}

CUIPlayerImage::CUIPlayerImage(const CUI& rhs) : CUI(rhs)
{

}

HRESULT CUIPlayerImage::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    m_AnimatorCom->Create_Animation(L"PlayerIdle", 150, 0.02f);

    m_StateCom->Set_AnimInfo(ACTORSTATE::IDLE, L"PlayerIdle", ANIMSTATE::LOOP);

    m_AnimatorCom->Play_Animation(L"PlayerIdle", ANIMSTATE::LOOP, false);
    return S_OK;
}

_int CUIPlayerImage::Update_GameObject(const _float& timeDelta)
{
    int exit = CUI::Update_GameObject(timeDelta);

    return exit;
}

void CUIPlayerImage::LateUpdate_GameObject(const _float& timeDelta)
{
    CUI::LateUpdate_GameObject(timeDelta);
}


void CUIPlayerImage::Render_GameObject()
{
    CUI::Render_GameObject();

    m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());

    if (m_TextureCom && m_AnimatorCom)
    {
        const wstring& key = m_AnimatorCom->Get_CurKey();
        _int           frame = m_AnimatorCom->Get_CurFrame();

        m_TextureCom->Set_Texture(key, frame);
    }

    m_BufferCom->Render_Buffer();

   // cout << m_TransformCom->Get_Pos().x << "\t" << m_TransformCom->Get_Pos().y << endl;
}

HRESULT CUIPlayerImage::Add_Component()
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
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_PLAYER);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_PLAYER, m_TextureCom });

    // Animator
    m_AnimatorCom = CreateProtoComponent<CAnimator>(this, COMPONENTTYPE::ANIMATOR);
    NULL_CHECK_RETURN(m_AnimatorCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::ANIMATOR, m_AnimatorCom });

    m_AnimatorCom->Set_TextureType(COMPONENTTYPE::TEX_PLAYER);

    // Stat
    m_StateCom = CreateProtoComponent<CState>(this, COMPONENTTYPE::STATE);
    NULL_CHECK_RETURN(m_StateCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::STATE, m_StateCom });
}

CUIPlayerImage* CUIPlayerImage::Create(DEVICE graphicDev)
{
    CUIPlayerImage* uiPlayerImage = new CUIPlayerImage(graphicDev);

    if (FAILED(uiPlayerImage->Ready_GameObject()))
    {
        Safe_Release(uiPlayerImage);
        MSG_BOX("uiPlayerPage Created Failed");
        return nullptr;
    }
    return uiPlayerImage;

}

void CUIPlayerImage::Free()
{
    CUI::Free();
}
