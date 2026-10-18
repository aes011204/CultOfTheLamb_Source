#include "pch.h"
#include "CBoss_SlaveElephant.h"
#include "CCreateHelper.h"
#include "CTexture.h"
#include "CTransform.h"
#include "CRenderer.h"
#include "CAnimator.h"
#include "CBoxCollider.h"

CBoss_SlaveElephant::CBoss_SlaveElephant(DEVICE graphicDev)
    : CCharacterBase(graphicDev)
{

}

CBoss_SlaveElephant::CBoss_SlaveElephant(const CBoss_SlaveElephant& rhs)
    : CCharacterBase(rhs)
{

}

CBoss_SlaveElephant::~CBoss_SlaveElephant()
{

}

HRESULT CBoss_SlaveElephant::Ready_GameObject()
{
    if (FAILED(CCharacterBase::Ready_GameObject()))
        return E_FAIL;

    m_TransformCom->Set_Scale(_vec3(1.8f, 3.8f, 1.f));
    m_TransformCom->Set_RotationAxis(ROT_X, D3DXToRadian(25.f));

    m_BoxColCom->Set_CollisionActive(false);

    return S_OK;
}

_int CBoss_SlaveElephant::Update_GameObject(const _float& timeDelta)
{
    _int exit = CCharacterBase::Update_GameObject(timeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDERID::RENDER_ALPHA, this);

    switch (m_SlaveAnimState)
    {
    case Appear:
        if (m_AnimatorCom->Is_AnimationFinished())
        {
            m_SlaveAnimState = Slave_AnimState::Idle;

            m_AnimatorCom->Play_Animation(L"NPC_Elephant_Idle", ANIMSTATE::LOOP);
        }
        break;

    case Idle:
        // 특정 조건 후 Freed
        break;

    case Freed:

        break;
    }

    return exit;
}

void CBoss_SlaveElephant::LateUpdate_GameObject(const _float& timeDelta)
{
    CCharacterBase::LateUpdate_GameObject(timeDelta);

}

void CBoss_SlaveElephant::Render_GameObject()
{
    CCharacterBase::Render_GameObject();

}

void CBoss_SlaveElephant::Animation_Setting()
{
    m_AnimatorCom->Create_Animation(L"NPC_Elephant_Appear", 140, 0.02f);
    m_AnimatorCom->Create_Animation(L"NPC_Elephant_Idle", 50, 0.02f);
    //m_AnimatorCom->Create_Animation(L"NPC_Elephant_Freed", 102, 0.02f);

    m_AnimatorCom->Play_Animation(L"NPC_Elephant_Appear", ANIMSTATE::ONCE);
}

HRESULT CBoss_SlaveElephant::Add_Component()
{
    if (FAILED(CCharacterBase::Add_Component()))
        return E_FAIL;

    COMPONENTTYPE texType = COMPONENTTYPE::TEX_SLAVE_TEST;

    m_TextureCom = CreateProtoComponent<CTexture>(this, texType);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);
    m_Components[ID_STATIC].insert({ texType, m_TextureCom });

    m_AnimatorCom->Set_TextureType(texType);


    return S_OK;
}

CBoss_SlaveElephant* CBoss_SlaveElephant::Create(DEVICE graphicDev)
{
    auto slave = new CBoss_SlaveElephant(graphicDev);

    if (FAILED(slave->Ready_GameObject()))
    {
        MSG_BOX("Boss_Slave Create Failed");
        Safe_Release(slave);
        return nullptr;
    }

    return slave;
}

void CBoss_SlaveElephant::Free()
{
    CCharacterBase::Free();
}
