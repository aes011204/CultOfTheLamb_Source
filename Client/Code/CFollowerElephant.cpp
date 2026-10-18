#include "pch.h"
#include "CFollowerElephant.h"

#include "CAnimator.h"
#include "CCreateHelper.h"
#include "CDInputMgr.h"
#include "CNPCController.h"
#include "CTexture.h"
#include "CTransform.h"

CFollowerElephant::CFollowerElephant(DEVICE graphicDev)
    : CNPCBase(graphicDev)
{ }

CFollowerElephant::CFollowerElephant(const CFollowerElephant& rhs)
    : CNPCBase(rhs)

{ }

CFollowerElephant::~CFollowerElephant()
{ }

HRESULT CFollowerElephant::Ready_GameObject()
{
    Set_Name(L"FollowerElephant");

    if (FAILED(CNPCBase::Ready_GameObject()))
        return E_FAIL;

    // 초기 생성 위치
    m_TransformCom->Set_Pos(23.5f, 1.f, 24.5f);

    m_NPCController->Set_Work(CNPCController::NPC_WORK::MINING);
    m_NPCController->Set_PrayTargetPos(_vec3(15.5f, 1.f, 22.5f));
    m_NPCController->Set_AnimKeyPrefix(L"Elephant");

    return S_OK;
}

void CFollowerElephant::Initialize_GameObject()
{
    CNPCBase::Initialize_GameObject();
}

_int CFollowerElephant::Update_GameObject(const _float& timeDelta)
{
    _int exit = CNPCBase::Update_GameObject(timeDelta);

    return exit;
}

void CFollowerElephant::LateUpdate_GameObject(const _float& timeDelta)
{
    CNPCBase::LateUpdate_GameObject(timeDelta);
}

void CFollowerElephant::Render_GameObject()
{
    CNPCBase::Render_GameObject();
}

HRESULT CFollowerElephant::Add_Component()
{
    if (FAILED(CNPCBase::Add_Component()))
        return E_FAIL;

    // Texture
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_ELEPHANT);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_ELEPHANT, m_TextureCom });

    m_AnimatorCom->Set_TextureType(COMPONENTTYPE::TEX_ELEPHANT);

    // 디버그: 텍스처 컴포넌트 추가 성공 여부 확인
    if (!m_TextureCom)
        MSG_BOX("FollowerElephant: Texture Component Missing in Add_Component");

    return S_OK;
}

void CFollowerElephant::Animation_Setting()
{
    m_AnimatorCom->Create_Animation(L"ElephantIdle",
                                    49,
                                    0.02f,
                                    _vec2(0.75f, 0.67f),
                                    _vec2(0.f, -0.13f));

    m_AnimatorCom->Create_Animation(L"ElephantRun",
                                    29,
                                    0.02f,
                                    _vec2(0.7f, 0.7f),
                                    _vec2(0.f, -0.13f));

    m_AnimatorCom->Create_Animation(L"ElephantWalkPast",
                                    86,
                                    0.02f,
                                    _vec2(0.7f, 0.7f),
                                    _vec2(0.f, -0.13f));

    m_AnimatorCom->Create_Animation(L"ElephantBuild",
                                    53,
                                    0.02f,
                                    _vec2(0.8f, 0.8f),
                                    _vec2(0.f, -0.13f));

    m_AnimatorCom->Create_Animation(L"ElephantChopWood",
                                    81,
                                    0.02f,
                                    _vec2(1.1f, 1.1f),
                                    _vec2(0.f, -0.13f));

    m_AnimatorCom->Create_Animation(L"ElephantMining",
                                    48,
                                    0.02f,
                                    _vec2(1.1f, 1.1f),
                                    _vec2(0.f, -0.13f));

    m_AnimatorCom->Create_Animation(L"ElephantPray",
                                    62,
                                    0.04f,
                                    _vec2(0.75f, 0.67f),
                                    _vec2(0.f, -0.13f));

    m_AnimatorCom->Create_Animation(L"ElephantPrayPeek",
                                    62,
                                    0.04f,
                                    _vec2(0.75f, 0.67f),
                                    _vec2(0.f, -0.13f));

    m_AnimatorCom->Create_Animation(L"ElephantEat",
                                    50,
                                    0.05f,
                                    _vec2(0.72f, 0.65f),
                                    _vec2(0.f, -0.12f));

    m_AnimatorCom->Create_Animation(L"ElephantSpawn",
                                    172,
                                    0.04f,
                                    _vec2(1.5f, 1.5f),
                                    _vec2(0.f, 0.2f));

    m_AnimatorCom->Create_Animation(L"ElephantTransform1",
                                    94,
                                    0.04f,
                                    _vec2(1.5f, 2.4f),
                                    _vec2(0.f, 1.2f));

    m_AnimatorCom->Create_Animation(L"ElephantTransform2",
                                    26,
                                    0.04f,
                                    _vec2(1.5f, 2.4f),
                                    _vec2(0.f, 1.2f));

    m_AnimatorCom->Create_Animation(L"ElephantTransform3",
                                    89,
                                    0.04f,
                                    _vec2(1.5f, 2.4f),
                                    _vec2(0.f, 1.2f));
}

CFollowerElephant* CFollowerElephant::Create(DEVICE graphicDev)
{
    auto instance = new CFollowerElephant(graphicDev);

    if (FAILED(instance->Ready_GameObject()))
    {
        MSG_BOX("Follower Elephant Create Failed");
        Safe_Release(instance);
        return nullptr;
    }

    return instance;
}

void CFollowerElephant::Free()
{
    CNPCBase::Free();
}
