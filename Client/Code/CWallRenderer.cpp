#include "pch.h"
#include "CWallRenderer.h"
#include "CCreateHelper.h"
#include "CTerrain.h"

CWallRenderer::CWallRenderer(DEVICE graphicDev)
    : CComponent(graphicDev)
    , m_GridSizeX(0)
    , m_GridSizeZ(0)
    , m_CellSize(1.f)
    , m_EditMode(false)
{
}

CWallRenderer::CWallRenderer(const CWallRenderer& rhs)
    : CComponent(rhs)
    , m_WallData(rhs.m_WallData)
    , m_GridSizeX(rhs.m_GridSizeX)
    , m_GridSizeZ(rhs.m_GridSizeZ)
    , m_CellSize(rhs.m_CellSize)
    , m_EditMode(false)
{
}

CWallRenderer::~CWallRenderer()
{
}

HRESULT CWallRenderer::Ready_WallRenderer(DEVICE graphicDev)
{


    return S_OK;
}

_int CWallRenderer::Update_Component(const _float& timeDelta)
{
    _int exit = CComponent::Update_Component(timeDelta);



    return exit;
}

void CWallRenderer::LateUpdate_Component()
{
    CComponent::LateUpdate_Component();

}

void CWallRenderer::Render_Editor()
{
    CComponent::Render_Editor();

    ImGui::PushID("WallRenderer");
    {
        ImGui::SeparatorText("Wall Renderer");

        // 편집 모드로 전환
        ImGui::Checkbox("Edit Mode", &m_EditMode);

        if (m_EditMode)
        {
            ImGui::TextColored(ImVec4(1, 1, 0, 1), "LMB: Place | RMB: Remove");
        }

        ImGui::Separator();

        _int wallCount = this->Get_WallCount();
        ImGui::Text("Total Walls : %d", wallCount);
        ImGui::Text("Grid : %d %d", m_GridSizeX - 1, m_GridSizeZ - 1);

        if (ImGui::Button("Remove All"))
        {
            Clear_All();
        }

        if (m_Owner)
        {
            CTerrain* terrain = dynamic_cast<CTerrain*>(m_Owner);
            if (terrain)
            {
                CTerrain* terrain = dynamic_cast<CTerrain*>(m_Owner);
                if (terrain)
                {
                    _float cellSize = terrain->Get_CellSize();
                    DWORD color = D3DCOLOR_ARGB(255, 255, 255, 255);

                    for (int z = 0; z < m_GridSizeZ - 1; z++)
                    {
                        for (int x = 0; x < m_GridSizeX - 1; x++)
                        {
                            if (Has_Wall(x, z))
                            {
                                _vec3 wallPos = terrain->Get_CellCenterWorld(x, z);
                                Engine::DrawDebugBox(m_GraphicDev, wallPos,
                                    _vec3(cellSize, 5.f, cellSize), color);
                            }
                        }
                    }
                }
            }
        }

    }
    ImGui::PopID();
}

void CWallRenderer::Initialize(_int sizeX, _int sizeZ, _float cellSize)
{
    m_GridSizeX = sizeX; m_GridSizeZ = sizeZ;
    m_CellSize = cellSize;

    const _int cellCount = (sizeX - 1) * (sizeZ - 1);
    m_WallData.assign(cellCount, 0);
}

void CWallRenderer::Resize(_int sizeX, _int sizeZ, _float cellSize)
{
    m_GridSizeX = sizeX; m_GridSizeZ = sizeZ;
    m_CellSize = cellSize;

    const _int cellCount = (sizeX - 1) * (sizeZ - 1);
    m_WallData.assign(cellCount, 0);
}

void CWallRenderer::Set_WallPosition(_int x, _int z, _bool value)
{
    const _int cellCntX = m_GridSizeX - 1;
    const _int cellCntZ = m_GridSizeZ - 1;

    if (x < 0 || x >= cellCntX || z < 0 || z >= cellCntZ)
        return;

    const _int index = z * cellCntX + x;

    if (index < m_WallData.size())
    {
        m_WallData[index] = value ? 1 : 0;
    }
}

void CWallRenderer::Clear_All()
{
    fill(m_WallData.begin(), m_WallData.end(), 0);
}

_bool CWallRenderer::Has_Wall(_int x, _int z)
{
    const _int cellCntX = m_GridSizeX - 1;
    const _int cellCntZ = m_GridSizeZ - 1;

    if (x < 0 || x >= cellCntX || z < 0 || z >= cellCntZ)
        return false;

    const _int index = z * cellCntX + x;
    if (index >= m_WallData.size())
        return false;

    return m_WallData[index] == 1;
}

_int CWallRenderer::Get_WallCount()
{
    return count(m_WallData.begin(), m_WallData.end(), 1); // 이터레이터 순회해서 1찾기
}

HRESULT CWallRenderer::Save_Walls(ofstream& ofs) const
{
    if (!ofs.is_open())
        return E_FAIL;

    const uint32_t dataSize = static_cast<uint32_t>(m_WallData.size());

    if (dataSize > 0)
    {
        ofs.write(reinterpret_cast<const char*>(m_WallData.data()),
            sizeof(_int) * dataSize);
    }

    return S_OK;
}

HRESULT CWallRenderer::Load_Walls(ifstream& ifs)
{
    if (!ifs.is_open())
        return E_FAIL;

    const size_t dataSize = m_WallData.size();

    if (dataSize > 0)
    {
        ifs.read(reinterpret_cast<char*>(m_WallData.data()),
            sizeof(_int) * dataSize);
    }

    return S_OK;
}

CWallRenderer* CWallRenderer::Create(DEVICE graphicDev)
{
    CWallRenderer* wall = new CWallRenderer(graphicDev);

    if (FAILED(wall->Ready_WallRenderer(graphicDev)))
    {
        Safe_Release(wall);
        MSG_BOX("Create WallRenderer Failed");
        return nullptr;
    }

    return wall;
}

CComponent* CWallRenderer::Clone()
{
    return new CWallRenderer(*this);
}

void CWallRenderer::Free()
{
    CComponent::Free();

}
