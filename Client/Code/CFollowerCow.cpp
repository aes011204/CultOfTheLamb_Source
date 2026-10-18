#include "pch.h"
#include "CFollowerCow.h"

#include "CAnimator.h"
#include "CCreateHelper.h"
#include "CDInputMgr.h"
#include "CNPCController.h"
#include "CTexture.h"
#include "CTransform.h"

CFollowerCow::CFollowerCow(DEVICE graphicDev)
    : CNPCBase(graphicDev)
{ }

CFollowerCow::CFollowerCow(const CFollowerCow& rhs)
    : CNPCBase(rhs)

{ }

CFollowerCow::~CFollowerCow()
{ }

HRESULT CFollowerCow::Ready_GameObject()
{
    Set_Name(L"FollowerCow");

    if (FAILED(CNPCBase::Ready_GameObject()))
        return E_FAIL;

    // 초기 생성 위치
    m_TransformCom->Set_Pos(23.5f, 1.f, 24.5f);

    m_NPCController->Set_Work(CNPCController::NPC_WORK::CHOPWOOD);
    m_NPCController->Set_PrayTargetPos(_vec3(18.f, 1.f, 21.5f));
    m_NPCController->Set_AnimKeyPrefix(L"Cow");

    return S_OK;
}

void CFollowerCow::Initialize_GameObject()
{
    CNPCBase::Initialize_GameObject();
}

_int CFollowerCow::Update_GameObject(const _float& timeDelta)
{
    _int exit = CNPCBase::Update_GameObject(timeDelta);

    return exit;
}

void CFollowerCow::LateUpdate_GameObject(const _float& timeDelta)
{
    CNPCBase::LateUpdate_GameObject(timeDelta);
}

void CFollowerCow::Render_GameObject()
{
    CNPCBase::Render_GameObject();
}

HRESULT CFollowerCow::Add_Component()
{
    if (FAILED(CNPCBase::Add_Component()))
        return E_FAIL;

    // Texture
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_COW);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_COW, m_TextureCom });

    m_AnimatorCom->Set_TextureType(COMPONENTTYPE::TEX_COW);

    // 디버그: 텍스처 컴포넌트 추가 성공 여부 확인
    if (!m_TextureCom)
        MSG_BOX("FollowerCow: Texture Component Missing in Add_Component");

    return S_OK;
}

void CFollowerCow::Animation_Setting()
{
    m_AnimatorCom->Create_Animation(L"CowIdle",
                                    49,
                                    0.02f,
                                    _vec2(0.75f, 0.67f),
                                    _vec2(0.f, -0.13f));

    m_AnimatorCom->Create_Animation(L"CowRun",
                                    29,
                                    0.02f,
                                    _vec2(0.7f, 0.7f),
                                    _vec2(0.f, -0.13f));

    m_AnimatorCom->Create_Animation(L"CowWalkPast",
                                    86,
                                    0.02f,
                                    _vec2(0.7f, 0.7f),
                                    _vec2(0.f, -0.13f));

    m_AnimatorCom->Create_Animation(L"CowBuild",
                                    53,
                                    0.02f,
                                    _vec2(0.8f, 0.8f),
                                    _vec2(0.f, -0.13f));

    m_AnimatorCom->Create_Animation(L"CowChopWood",
                                    81,
                                    0.02f,
                                    _vec2(1.1f, 1.1f),
                                    _vec2(0.f, -0.13f));

    m_AnimatorCom->Create_Animation(L"CowMining",
                                    48,
                                    0.02f,
                                    _vec2(1.1f, 1.1f),
                                    _vec2(0.f, -0.13f));

    m_AnimatorCom->Create_Animation(L"CowPray",
                                    62,
                                    0.04f,
                                    _vec2(0.75f, 0.67f),
                                    _vec2(0.f, -0.13f));

    m_AnimatorCom->Create_Animation(L"CowPrayPeek",
                                    62,
                                    0.04f,
                                    _vec2(0.75f, 0.67f),
                                    _vec2(0.f, -0.13f));

    m_AnimatorCom->Create_Animation(L"CowEat",
                                    50,
                                    0.05f,
                                    _vec2(0.72f, 0.65f),
                                    _vec2(0.f, -0.12f));

    m_AnimatorCom->Create_Animation(L"CowSpawn",
                                    172,
                                    0.04f,
                                    _vec2(1.5f, 1.5f),
                                    _vec2(0.f, 0.2f));

    m_AnimatorCom->Create_Animation(L"CowTransform1",
                                    94,
                                    0.04f,
                                    _vec2(1.5f, 2.4f),
                                    _vec2(0.f, 1.2f));

    m_AnimatorCom->Create_Animation(L"CowTransform2",
                                    26,
                                    0.04f,
                                    _vec2(1.5f, 2.4f),
                                    _vec2(0.f, 1.2f));

    m_AnimatorCom->Create_Animation(L"CowTransform3",
                                    89,
                                    0.04f,
                                    _vec2(1.5f, 2.4f),
                                    _vec2(0.f, 1.2f));
}

CFollowerCow* CFollowerCow::Create(DEVICE graphicDev)
{
    auto instance = new CFollowerCow(graphicDev);

    if (FAILED(instance->Ready_GameObject()))
    {
        MSG_BOX("Follower Cow Create Failed");
        Safe_Release(instance);
        return nullptr;
    }

    return instance;
}

void CFollowerCow::Free()
{
    CNPCBase::Free();
}
