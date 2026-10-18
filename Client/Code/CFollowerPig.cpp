#include "pch.h"
#include "CFollowerPig.h"

#include "CAnimator.h"
#include "CCreateHelper.h"
#include "CDInputMgr.h"
#include "CNPCController.h"
#include "CTexture.h"
#include "CTransform.h"

CFollowerPig::CFollowerPig(DEVICE graphicDev)
    : CNPCBase(graphicDev)
{ }

CFollowerPig::CFollowerPig(const CFollowerPig& rhs)
    : CNPCBase(rhs)

{ }

CFollowerPig::~CFollowerPig()
{ }

HRESULT CFollowerPig::Ready_GameObject()
{
    Set_Name(L"FollowerPig");

    if (FAILED(CNPCBase::Ready_GameObject()))
        return E_FAIL;

    // 초기 생성 위치
    m_TransformCom->Set_Pos(23.5f, 1.f, 24.5f);

    m_NPCController->Set_Work(CNPCController::NPC_WORK::PRAY);
    m_NPCController->Set_PrayTargetPos(_vec3(20.f, 1.f, 22.5f));
    m_NPCController->Set_AnimKeyPrefix(L"Pig");

    return S_OK;
}

void CFollowerPig::Initialize_GameObject()
{
    CNPCBase::Initialize_GameObject();
}

_int CFollowerPig::Update_GameObject(const _float& timeDelta)
{
    _int exit = CNPCBase::Update_GameObject(timeDelta);

    return exit;
}

void CFollowerPig::LateUpdate_GameObject(const _float& timeDelta)
{
    CNPCBase::LateUpdate_GameObject(timeDelta);
}

void CFollowerPig::Render_GameObject()
{
    CNPCBase::Render_GameObject();
}

HRESULT CFollowerPig::Add_Component()
{
    if (FAILED(CNPCBase::Add_Component()))
        return E_FAIL;

    // Texture
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_PIG);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_PIG, m_TextureCom });

    m_AnimatorCom->Set_TextureType(COMPONENTTYPE::TEX_PIG);

    // 디버그: 텍스처 컴포넌트 추가 성공 여부 확인
    if (!m_TextureCom)
        MSG_BOX("FollowerPig: Texture Component Missing in Add_Component");

    return S_OK;
}

void CFollowerPig::Animation_Setting()
{
    m_AnimatorCom->Create_Animation(L"PigIdle",
                                    49,
                                    0.02f,
                                    _vec2(0.75f, 0.67f),
                                    _vec2(0.f, -0.13f));

    m_AnimatorCom->Create_Animation(L"PigRun",
                                    29,
                                    0.02f,
                                    _vec2(0.7f, 0.7f),
                                    _vec2(0.f, -0.13f));

    m_AnimatorCom->Create_Animation(L"PigWalkPast",
                                    86,
                                    0.02f,
                                    _vec2(0.7f, 0.7f),
                                    _vec2(0.f, -0.13f));

    m_AnimatorCom->Create_Animation(L"PigBuild",
                                    53,
                                    0.02f,
                                    _vec2(0.8f, 0.8f),
                                    _vec2(0.f, -0.13f));

    m_AnimatorCom->Create_Animation(L"PigChopWood",
                                    81,
                                    0.02f,
                                    _vec2(1.1f, 1.1f),
                                    _vec2(0.f, -0.13f));

    m_AnimatorCom->Create_Animation(L"PigMining",
                                    48,
                                    0.02f,
                                    _vec2(1.1f, 1.1f),
                                    _vec2(0.f, -0.13f));

    m_AnimatorCom->Create_Animation(L"PigPray",
                                    62,
                                    0.04f,
                                    _vec2(0.75f, 0.67f),
                                    _vec2(0.f, -0.13f));

    m_AnimatorCom->Create_Animation(L"PigPrayPeek",
                                    62,
                                    0.04f,
                                    _vec2(0.75f, 0.67f),
                                    _vec2(0.f, -0.13f));

    m_AnimatorCom->Create_Animation(L"PigEat",
                                    50,
                                    0.05f,
                                    _vec2(0.72f, 0.65f),
                                    _vec2(0.f, -0.12f));

    m_AnimatorCom->Create_Animation(L"PigSpawn",
                                    172,
                                    0.04f,
                                    _vec2(1.5f, 1.5f),
                                    _vec2(0.f, 0.2f));

    m_AnimatorCom->Create_Animation(L"PigTransform1",
                                    94,
                                    0.04f,
                                    _vec2(1.5f, 2.4f),
                                    _vec2(0.f, 1.2f));

    m_AnimatorCom->Create_Animation(L"PigTransform2",
                                    26,
                                    0.04f,
                                    _vec2(1.5f, 2.4f),
                                    _vec2(0.f, 1.2f));

    m_AnimatorCom->Create_Animation(L"PigTransform3",
                                    89,
                                    0.04f,
                                    _vec2(1.5f, 2.4f),
                                    _vec2(0.f, 1.2f));
}

CFollowerPig* CFollowerPig::Create(DEVICE graphicDev)
{
    auto instance = new CFollowerPig(graphicDev);

    if (FAILED(instance->Ready_GameObject()))
    {
        MSG_BOX("Follower Pig Create Failed");
        Safe_Release(instance);
        return nullptr;
    }

    return instance;
}

void CFollowerPig::Free()
{
    CNPCBase::Free();
}
