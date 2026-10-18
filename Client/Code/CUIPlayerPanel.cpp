#include "pch.h"
#include "CUIPlayerPanel.h"

#include <CCreateHelper.h>
#include "CEnumHelper.h"
#include "CCombatStat.h"
#include "CPlayer.h"
#include "CRenderer.h"
#include "CTransform.h"
#include "CUIHeartBar.h"
#include "CUIPanel.h"

CUIPlayerPanel::CUIPlayerPanel(DEVICE graphicDev)
    : CUIPanel(graphicDev),
    m_CombatStatCom(nullptr)
{
    this->Set_Name(L"PlayerPanel");
}

CUIPlayerPanel::CUIPlayerPanel(const CUIPlayerPanel& rhs)
    : CUIPanel(rhs),
    m_CombatStatCom(nullptr)
{
    this->Set_Name(L"PlayerPanel");
}

CUIPlayerPanel::~CUIPlayerPanel()
{
}

HRESULT CUIPlayerPanel::Ready_GameObject()
{
    CUIPanel::Ready_GameObject();

    if (FAILED(Add_Component()))
        return E_FAIL;


    // CUIPlayerPanel (부모 패널)의 월드 위치 및 크기 설정
    // 예시: 화면 좌상단 근처에 배치
    m_TransformCom->Set_Pos(_vec3(-395.f,  280.f, 0.f));
    m_TransformCom->Set_Scale(_vec3(150.f, 50.f, 1.f));

    // 하트바 생성
    auto heart1 = CUIHeartBar::Create(m_GraphicDev);
    NULL_CHECK_RETURN(heart1, E_FAIL)
        AddChild(heart1);

    auto heart2 = CUIHeartBar::Create(m_GraphicDev);
    NULL_CHECK_RETURN(heart2, E_FAIL)
        AddChild(heart2);

    auto heart3 = CUIHeartBar::Create(m_GraphicDev);
    NULL_CHECK_RETURN(heart3, E_FAIL)
        AddChild(heart3);

    return S_OK;
}

_int CUIPlayerPanel::Update_GameObject(const _float& timeDelta)
{
    // ----------------------------------------------------
    // 🛑 하트바 위치 갱신 로직 (PlayerPanel이 매 프레임 제어)
    // ----------------------------------------------------

    // 1. 부모(PlayerPanel)의 월드 위치를 가져옵니다.
    const _vec3& parent_world_pos = m_TransformCom->Get_Pos();

    // 2. 하트바 배치를 위한 로컬 오프셋 설정
    const float fStartOffsetX = -40.f; // Panel 중심에서 왼쪽으로 오프셋 시작
    const float fOffsetY = 0.f;        // Panel Y 중심
    const float fHeartSpacing = 40.f;  // 하트 간격 (하트 크기 + 여백)

    int i = 0;
    for (auto child : m_Children)
    {
        // 3. 하트바 트랜스폼 가져오기
        CUIHeartBar* heartBar = dynamic_cast<CUIHeartBar*>(child);
        if (!heartBar) continue;

        CTransform* heart_transform = dynamic_cast<CTransform*>(heartBar->Get_Component(COMPONENTID::ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));

        if (heart_transform)
        {
            // 4. 하트바의 로컬 오프셋 계산 (부모 중심 기준)
            float local_x = fStartOffsetX + (float)i * fHeartSpacing;

            // 5. [수정된 스타일] 최종 월드 위치를 계산하는 오프셋 벡터 정의
            const _vec3 heartOffset = { local_x, fOffsetY, 0.f };

            // 6. 하트바의 트랜스폼에 최종 월드 위치를 설정합니다. (부모 월드 위치 + 오프셋)
            heart_transform->Set_Pos(parent_world_pos + heartOffset);
        }

        // ----------------------------------------------------
        // (기존 HP 체크 및 Set_State 로직 유지)
        // ----------------------------------------------------

        float hp = m_CombatStatCom->Get_Hp();
        float maxHp = m_CombatStatCom->Get_MaxHp();
        float ratio = (maxHp > 0.f) ? (hp / maxHp) : 0.f;
        float start = i * 2.0f;//0,2,4
        float end = start + 2.0f;//2,4,6
        int heartCount = static_cast<int>(m_Children.size());
        float filledHalfUnits = ratio * (2.0f * heartCount);
        HEARTSTATE state;

        if (filledHalfUnits >= end)
            state = HEARTSTATE::FULL;

        else if (filledHalfUnits > start)
            state = HEARTSTATE::HALF;

        else
            state = HEARTSTATE::EMPTY;

        heartBar->Set_State(state);

        i++;
    }
    // ----------------------------------------------------

    _int exit = CUIPanel::Update_GameObject(timeDelta);
    CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

    return exit;
}

void CUIPlayerPanel::LateUpdate_GameObject(const _float& timeDelta)
{
    CUIPanel::LateUpdate_GameObject(timeDelta);
}

void CUIPlayerPanel::Render_GameObject()
{
    CUIPanel::Render_GameObject();

}

void CUIPlayerPanel::Render_Editor()
{
    // 기존 코드 유지
    CUIPanel::Render_Editor();

    if (ImGui::Begin("UI_Heart Inspector"))
    {
        for (auto child : m_Children)
        {
            CUIHeartBar* heartBar = dynamic_cast<CUIHeartBar*>(child);

            if (!heartBar)
                continue;

            ImGui::PushID(heartBar->Get_ID());

            CTransform* m_HeartTransformCom = dynamic_cast<CTransform*>(heartBar->Get_Component(COMPONENTID::ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));

            // TransformComponent
            if (m_HeartTransformCom && ImGui::CollapsingHeader(("Transform Component" + to_string(heartBar->Get_ID())).c_str(),
                ImGuiTreeNodeFlags_DefaultOpen))
            {
                // NOTE: Get_Pos()의 반환값을 const 참조로 받으면 InputFloat에 바로 사용할 수 없습니다.
                // 임시 변수에 복사하여 수정 후 Set_Pos로 다시 설정해야 합니다.
                _vec3 pos = m_HeartTransformCom->Get_Pos();

                ImGui::Text("Position");

                ImGui::Text("X :");
                ImGui::SameLine();
                ImGui::InputFloat("##HeartX", &pos.x); // &pos.x로 수정

                ImGui::Text("Y :");
                ImGui::SameLine();
                ImGui::InputFloat("##HeartY", &pos.y); // &pos.y로 수정

                ImGui::Text("Z :");
                ImGui::SameLine();
                ImGui::InputFloat("##HeartZ", &pos.z); // &pos.z로 수정

                m_HeartTransformCom->Set_Pos(pos);

                // NOTE: Scale도 마찬가지로 임시 변수에 복사하여 수정 후 Set_Scale로 다시 설정해야 합니다.
                _vec3 scale = m_HeartTransformCom->Get_Scale();

                ImGui::InputFloat("##ScaleX", &scale.x);
                ImGui::InputFloat("##ScaleY", &scale.y);

                m_HeartTransformCom->Set_Scale(scale);
            }

            ImGui::PopID();
        }
    }
    ImGui::End();
}

HRESULT CUIPlayerPanel::Add_Component()
{
    // transform
    m_TransformCom = CreateProtoComponent<CTransform>(this, COMPONENTTYPE::TRANSFORM);
    NULL_CHECK_RETURN(m_TransformCom, E_FAIL);

    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::TRANSFORM, m_TransformCom });

}

CUIPlayerPanel* CUIPlayerPanel::Create(DEVICE graphicDev)
{
    auto mainmenuPanel = new CUIPlayerPanel(graphicDev);

    if (FAILED(mainmenuPanel->Ready_GameObject()))
    {
        MSG_BOX("MainMenu Create Failed");
        Safe_Release(mainmenuPanel);
        return nullptr;
    }

    return mainmenuPanel;
}

void CUIPlayerPanel::Free()
{
    Engine::CUIPanel::Free();
}
