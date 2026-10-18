#include "pch.h"
#include "CFogMgr.h"
#include "CFog.h"
#include "CDInputMgr.h"
#include "CManagement.h"
#include "CScene.h"
#include "CLayer.h"
#include "CPickingHelper.h"
#include "CTerrain.h"
#include "CTransform.h"

IMPLEMENT_SINGLETON(CFogMgr)

CFogMgr::CFogMgr()
{
}

CFogMgr::~CFogMgr()
{
    CFogMgr::Free();
}

void CFogMgr::Ready_FogMgr(DEVICE graphicDev)
{
    m_GraphicDev = graphicDev;
    m_GraphicDev->AddRef();

}

void CFogMgr::Update_FogMgr(const _float& timeDelta)
{
    if (!m_IsPlacing || !m_PreviewFog)
        return;

    _vec3 worldPos;
    if (PickPlaneY(m_GraphicDev, g_hWnd, 0.f, worldPos))
    {
        worldPos.y += m_YOffset;

        m_PreviewFog->Set_OriginalPos(worldPos);
        m_PreviewFog->Get_Transform()->Set_Scale(m_FogScale);
        m_PreviewFog->Set_SwaySpeed(m_SwaySpeed);
        m_PreviewFog->Set_SwayAmount(m_SwayAmount);
        m_PreviewFog->Set_Alpha(m_Alpha);
        m_PreviewFog->Set_Color(m_FogColor[0], m_FogColor[1], m_FogColor[2]);
        m_PreviewFog->Get_Transform()->Set_Rotation(m_FogRotation);
    }

    m_PreviewFog->Update_GameObject(timeDelta);

    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse)
        return;

    // MapObject와 마찬가지로 조절 가능하도록

    auto input = CDInputMgr::GetInstance();

    _long wheelDelta = input->Get_DIMouseMove(DIMS_Z);
    if (wheelDelta != 0)
    {
        if (input->KeyPress(DIK_LCONTROL) || input->KeyPress(DIK_RCONTROL))
        {
            _float rotChange = (wheelDelta > 0 ? D3DX_PI / 50.f : -D3DX_PI / 50.f);
            m_FogRotation.x += rotChange;
            m_FogRotation.x = fmodf(m_FogRotation.x, D3DX_PI * 2.f);
        }
        else if (input->KeyPress(DIK_LSHIFT) || input->KeyPress(DIK_RSHIFT))
        {
            _float posChange = (wheelDelta > 0 ? 0.1f : -0.1f);
            m_YOffset += posChange;
        }
        else
        {
            _float scaleChange = (wheelDelta > 0 ? 0.5f : -0.5f);
            m_FogScale.x += scaleChange;
            m_FogScale.y += scaleChange;
            m_FogScale.x = max(0.1f, min(50.f, m_FogScale.x));
            m_FogScale.y = max(0.1f, min(50.f, m_FogScale.y));
        }
    }

    // 배치
    if (input->MouseDown(DIM_LB))
        Place_Fog();

    //제거 
    if (input->MouseDown(DIM_RB))
        Remove_LastFog();

}

void CFogMgr::Render_FogWindow()
{
    if (ImGui::CollapsingHeader("Fog Editor"))
    {
        if (ImGui::Checkbox("Place Mode", &m_IsPlacing))
        {
            if (m_IsPlacing)
                Start_Place();
        }

        ImGui::SeparatorText("Fog Settings");

        ImGui::DragFloat3("Scale", &m_FogScale.x, 0.5f, 0.1f, 50.f);
        ImGui::DragFloat("Y Offset", &m_YOffset, 0.1f, 0.f, 20.f);
        ImGui::DragFloat("Sway Speed", &m_SwaySpeed, 0.1f, 0.f, 5.f);
        ImGui::DragFloat("Sway Amount", &m_SwayAmount, 0.1f, 0.f, 10.f
        );
        ImGui::SliderFloat("Alpha", &m_Alpha, 0.f, 1.f);
        ImGui::ColorEdit3("Fog Color", m_FogColor);

        ImGui::SeparatorText("Actions");

        ImGui::Text("Placed Fogs: %d", (_int)m_PlacedFogs.size());

        if (ImGui::Button("Undo Last"))
            Remove_LastFog();

        ImGui::SameLine();

        if (ImGui::Button("Clear All"))
            Clear_AllFog();
    }

}

void CFogMgr::Render_Preview()
{
    if (m_IsPlacing && m_PreviewFog)
    {
        m_PreviewFog->Render_GameObject();
    }
}

void CFogMgr::Start_Place()
{
    m_IsPlacing = true;

    // 프리뷰가 없으면 생성
    if (!m_PreviewFog)
    {
        m_PreviewFog = CFog::Create(m_GraphicDev);
        if (m_PreviewFog)
            m_PreviewFog->Set_Alpha(1.f);
    }
}

void CFogMgr::Stop_Place()
{
    m_IsPlacing = false;
}

void CFogMgr::Place_Fog()
{
    if (!m_PreviewFog) return;

    CFog* newFog = CFog::Create(m_GraphicDev);

    if (!newFog) return;

    _vec3 pos = m_PreviewFog->Get_Transform()->Get_Pos();

    newFog->Set_OriginalPos(pos);

    newFog->Get_Transform()->Set_Scale(m_FogScale);
    newFog->Get_Transform()->Set_Rotation(m_FogRotation);

    newFog->Set_SwaySpeed(m_SwaySpeed);
    newFog->Set_SwayAmount(m_SwayAmount);

    newFog->Set_Alpha(m_Alpha);
    newFog->Set_Color(m_FogColor[0], m_FogColor[1], m_FogColor[2]);

    auto scene = CManagement::GetInstance()->Get_Scene();
    auto layer = scene->Get_Layer(LAYERTYPE::ENVIRONMENT);

    layer->Add_GameObject(OBJTYPE::FOG, newFog);

    m_PlacedFogs.push_back(newFog);
}

void CFogMgr::Remove_LastFog()
{
    if (m_PlacedFogs.empty()) return;

    CFog* lastFog = m_PlacedFogs.back();
    m_PlacedFogs.pop_back();

    lastFog->Mark_As_PendingKill();
}

void CFogMgr::Clear_AllFog()
{
    for (auto fog : m_PlacedFogs)
    {
        fog->Mark_As_PendingKill();
    }

    m_PlacedFogs.clear();
}

HRESULT CFogMgr::Save_ToStream(std::ofstream& ofs)
{
    size_t count = m_PlacedFogs.size();
    ofs.write((char*)&count, sizeof(size_t));

    for (auto fog : m_PlacedFogs)
    {
        _vec3 pos = fog->Get_Transform()->Get_Pos();
        _vec3 scale = fog->Get_Transform()->Get_Scale();
        _vec3 rotation = fog->Get_Transform()->Get_Rotation();
        _float speed = fog->Get_SwaySpeed();
        _float amount = fog->Get_SwayAmount();
        _float alpha = fog->Get_Alpha();

        COLOR color = fog->Get_Color();

        ofs.write((char*)&pos, sizeof(_vec3));
        ofs.write((char*)&scale, sizeof(_vec3));
        ofs.write((char*)&rotation, sizeof(_vec3));

        ofs.write((char*)&speed, sizeof(_float));
        ofs.write((char*)&amount, sizeof(_float));
        ofs.write((char*)&alpha, sizeof(_float));
        ofs.write((char*)&color, sizeof(COLOR));
    }

    return S_OK;
}
HRESULT CFogMgr::Load_FromStream(std::ifstream& ifs, _vec3 offset)
{
    Clear_AllFog();

    size_t count = 0;
    ifs.read((char*)&count, sizeof(size_t));

    auto scene = CManagement::GetInstance()->Get_Scene();
    auto layer = scene->Get_Layer(LAYERTYPE::ENVIRONMENT);

    for (size_t i = 0; i < count; ++i)
    {
        _vec3 pos, scale, rotation;
        _float speed, amount, alpha;
        COLOR color;

        ifs.read((char*)&pos, sizeof(_vec3));
        ifs.read((char*)&scale, sizeof(_vec3));
        ifs.read((char*)&rotation, sizeof(_vec3));

        ifs.read((char*)&speed, sizeof(_float));
        ifs.read((char*)&amount, sizeof(_float));
        ifs.read((char*)&alpha, sizeof(_float));
        ifs.read((char*)&color, sizeof(COLOR));

        CFog* fog = CFog::Create(m_GraphicDev);

        fog->Set_OriginalPos(pos + offset);
        fog->Get_Transform()->Set_Scale(scale);

        fog->Get_Transform()->Set_Rotation(_vec3(-rotation.x, rotation.y, -rotation.z));

        fog->Set_SwaySpeed(speed);
        fog->Set_SwayAmount(amount);
        fog->Set_Alpha(alpha);
        fog->Set_Color(color);

        layer->Add_GameObject(OBJTYPE::FOG, fog);
        m_PlacedFogs.push_back(fog);
    }

    return S_OK;
}

void CFogMgr::Free()
{
    m_PlacedFogs.clear();

    Safe_Release(m_PreviewFog);
    Safe_Release(m_GraphicDev);
}
