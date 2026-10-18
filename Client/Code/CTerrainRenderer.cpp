#include "pch.h"
#include "CTerrainRenderer.h"
#include "CLayer.h"
#include "CObjectSpawner.h"
#include "CTransform.h"
#include "CTexture.h"
#include "CDInputMgr.h"
#include "CFogMgr.h"
#include "CGameObject.h"
#include "CMapObjectMgr.h"
#include "CTerrainTex.h"
#include "CPickingHelper.h"
#include "CTerrain.h"
#include "CWallRenderer.h"

CTerrainRenderer::CTerrainRenderer(DEVICE graphicDev)
    : CComponent(graphicDev),
      m_TransformCom(nullptr),
      m_BufferCom(nullptr),
      m_TextureCom284(nullptr), m_TextureCom1024(nullptr), m_TextureComOutline(nullptr), m_TextureComOutline2(nullptr),
      m_SelectedTile(0), m_SelectedOutline(0), m_SelectedOutline2(0),
      m_PaintX(0),
      m_PaintZ(0),
      m_MapName{"Terrain_"},
      m_SelectedMapIndex(0), m_SizeX(TILE_CNT_X), m_SizeZ(TILE_CNT_Z), m_CellSize(1.f), m_ShowTile(false),
      m_TileTypeIndex(0)
{
}

CTerrainRenderer::CTerrainRenderer(const CTerrainRenderer& rhs)
    : CComponent(rhs),
      m_TransformCom(nullptr),
      m_BufferCom(nullptr),
      m_TextureCom284(nullptr), m_TextureCom1024(nullptr), m_TextureComOutline(nullptr), m_TextureComOutline2(nullptr),
      m_TileIndices284(rhs.m_TileIndices284),
      m_SelectedTile(rhs.m_SelectedTile), m_SelectedOutline(0), m_SelectedOutline2(0),
      m_PaintX(rhs.m_PaintX),
      m_PaintZ(rhs.m_PaintZ),
      m_SelectedMapIndex(rhs.m_SelectedMapIndex), m_SizeX(TILE_CNT_X), m_SizeZ(TILE_CNT_Z), m_CellSize(rhs.m_CellSize),
      m_ShowTile(false), m_TileTypeIndex(0),
      m_MapList(rhs.m_MapList)
{
    memcpy(m_MapName, rhs.m_MapName, sizeof(m_MapName));
}

CTerrainRenderer::~CTerrainRenderer()
{
}

HRESULT CTerrainRenderer::Ready_TerrainRenderer()
{
    if (m_Owner)
    {
        m_TransformCom = dynamic_cast<CTransform*>(
            m_Owner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));

        m_BufferCom = dynamic_cast<CTerrainTex*>(
            m_Owner->Get_Component(ID_STATIC, COMPONENTTYPE::TERRAIN_TEX));

        m_TextureCom284 = dynamic_cast<CTexture*>(
            m_Owner->Get_Component(ID_STATIC, COMPONENTTYPE::TEX_TILE_284));

        m_TextureCom1024 = dynamic_cast<CTexture*>(
            m_Owner->Get_Component(ID_STATIC, COMPONENTTYPE::TEX_TILE_1024));

        m_TextureComOutline = dynamic_cast<CTexture*>(
            m_Owner->Get_Component(ID_STATIC, COMPONENTTYPE::TEX_TILE_OUTLINE));

        m_TextureComOutline2 = dynamic_cast<CTexture*>(
            m_Owner->Get_Component(ID_STATIC, COMPONENTTYPE::TEX_TILE_OUTLINE2));
    }

    NULL_CHECK_RETURN(m_TransformCom, E_FAIL);
    NULL_CHECK_RETURN(m_BufferCom, E_FAIL);
    NULL_CHECK_RETURN(m_TextureCom284, E_FAIL);
    NULL_CHECK_RETURN(m_TextureCom1024, E_FAIL);
    NULL_CHECK_RETURN(m_TextureComOutline, E_FAIL);
    NULL_CHECK_RETURN(m_TextureComOutline2, E_FAIL);

    m_BufferCom->AddRef();

    const int cellCntX = m_SizeX - 1;
    const int cellCntZ = m_SizeZ - 1;
    const int cellCount = cellCntX * cellCntZ;

    m_TileIndices284.assign(cellCount, 0);
    m_TileIndices1024.assign(cellCount, -1);
    m_TileIndicesOutline.assign(cellCount, -1);
    m_TileIndicesOutline2.assign(cellCount, -1);

    // 초기에 맵 리스트 로드 한번 진행
    Refresh_MapFileList();

    return S_OK;
}

_int CTerrainRenderer::Update_Component(const _float& timeDelta)
{
    _int exit = CComponent::Update_Component(timeDelta);

    // ImGui에 마우스 입력 중일 시에는 에디터 입력 무시
    if (!ImGui::GetIO().WantCaptureMouse)
    {
        int cellX, cellZ;
        if (PickCellMousePos(cellX, cellZ))
        {
            const int iCellCntX = m_SizeX - 1;
            const int index = cellZ * iCellCntX + cellX;

            if (m_ShowTile && CDInputMgr::GetInstance()->MousePress(DIM_LB))
            {
                m_PaintX = cellX;
                m_PaintZ = cellZ;

                const _int cellCount = (m_SizeX - 1) * (m_SizeZ - 1);
                if (index < 0 || index >= cellCount)
                {
                    return exit;
                }

                if (m_TileTypeIndex == static_cast<_int>(TileType::TILE_284))
                {
                    m_TileIndices284[index] = m_SelectedTile;
                    m_TileIndices1024[index] = -1;
                }
                else if (m_TileTypeIndex == static_cast<_int>(TileType::TILE_1024))
                {
                    m_TileIndices1024[index] = m_SelectedTile;
                    m_TileIndices284[index] = -1;
                }
                else if (m_TileTypeIndex == static_cast<_int>(TileType::TILE_Outline))
                {
                    _int combinedIndex = m_SelectedOutline * ATLAS_COLS_OUTLINE + m_SelectedTile;
                    m_TileIndicesOutline[index] = combinedIndex;
                }
                else if (m_TileTypeIndex == static_cast<_int>(TileType::TILE_Outline2))
                {
                    _int combinedIndex = m_SelectedOutline2 * ATLAS_COLS_OUTLINE2 + m_SelectedTile;
                    m_TileIndicesOutline2[index] = combinedIndex;
                }
            }
        }
    }

    return exit;
}

void CTerrainRenderer::LateUpdate_Component()
{
    CComponent::LateUpdate_Component();
}

void CTerrainRenderer::Render_TerrainRenderer()
{
    if (m_BufferCom == nullptr || m_TextureCom284 == nullptr || m_TransformCom == nullptr)
        return;

    m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());
    m_GraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_GraphicDev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
    m_GraphicDev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
    m_GraphicDev->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
    m_GraphicDev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
    m_GraphicDev->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

    Set_Material();

    // 알파블렌딩 안켜주면 투명타일 검은색으로 나옴
    m_GraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_GraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_GraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    const _int cellCntX = m_SizeX - 1;
    const _int cellCntZ = m_SizeZ - 1;
    const _int cellCount = cellCntX * cellCntZ;

    // 284
    if (m_TextureCom284 && m_TileIndices284.size() == cellCount)
    {
        m_BufferCom->ApplyTileSetting(m_TileIndices284,
            ATLAS_COLS_284, ATLAS_ROWS_284, 1704.f, 284.f);
        m_TextureCom284->Set_Texture(0);
        m_BufferCom->Render_Buffer();
    }

    // 1024
    if (m_TextureCom1024 && m_TileIndices1024.size() == cellCount)
    {
        m_BufferCom->ApplyTileSetting(m_TileIndices1024,
            ATLAS_COLS_1024, ATLAS_ROWS_1024, 1024.f, 1024.f);
        m_TextureCom1024->Set_Texture(0);
        m_BufferCom->Render_Buffer();
    }

    // Outline
    for (int outlineIdx = 0; outlineIdx < 3; ++outlineIdx)
    {
        if (m_TextureComOutline && m_TileIndicesOutline.size() == cellCount)
        {
            vector<_int> indicesOutline = m_TileIndicesOutline;
            for (size_t i = 0; i < indicesOutline.size(); ++i)
            {
                _int value = indicesOutline[i];
                if (value < 0 || value / ATLAS_COLS_OUTLINE != outlineIdx)
                {
                    indicesOutline[i] = -1;
                }
                else
                {
                    indicesOutline[i] = value % ATLAS_COLS_OUTLINE;
                }
            }

            m_BufferCom->ApplyTileSetting(indicesOutline,
            ATLAS_COLS_OUTLINE, ATLAS_ROWS_OUTLINE, 530.f, 84.f);

            m_TextureComOutline->Set_Texture(outlineIdx);
            m_BufferCom->Render_Buffer();
        }

        if (m_TextureComOutline2 && m_TileIndicesOutline2.size() == cellCount)
        {
            m_BufferCom->ApplyTileSetting(m_TileIndicesOutline2,
            ATLAS_COLS_OUTLINE2, ATLAS_ROWS_OUTLINE2, 530.f, 84.f);  
            m_TextureComOutline2->Set_Texture(0);
            m_BufferCom->Render_Buffer();
        }
    }

    m_GraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    m_GraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
}

void CTerrainRenderer::Render_Editor()
{
    CComponent::Render_Editor();

    if (ImGui::Selectable(u8"Tile << 선택"))
    {
        m_ShowTile = !m_ShowTile;
    }

    Render_TileWindow();

    ImGui::NewLine();
	ImGui::Separator();
    // --- Save / Load ---
    ImGui::Text(u8"저장할 맵 이름 : ");
    ImGui::SameLine();

    ImGui::SetNextItemWidth(120.0f);
    ImGui::InputText("##MapName", m_MapName, IM_ARRAYSIZE(m_MapName));

    ImGui::SameLine();
    if (ImGui::Button(u8"Save"))
    {
        string fullPath = "../Bin/Resource/Data/Terrain/";
        fullPath += m_MapName;
        fullPath += ".map";

        if (SUCCEEDED(SaveMap(fullPath.c_str())))
            Refresh_MapFileList();
    }

    ImGui::Separator();

    if (ImGui::CollapsingHeader(u8"저장된 맵 리스트", ImGuiTreeNodeFlags_DefaultOpen))
    {
        if (!m_MapList.empty())
        {
            for (_int i = 0; i < (_int)m_MapList.size(); ++i)
            {
                bool isSelected = (m_SelectedMapIndex == i);

                if (ImGui::Selectable(m_MapList[i].c_str(), isSelected))
                {
                    m_SelectedMapIndex = i;

                    string fullPath = "../Bin/Resource/Data/Terrain/";
                    fullPath += m_MapList[i];

                    LoadMap(fullPath.c_str());
                }
            }
        }
        else
        {
            ImGui::TextDisabled(u8"[맵 파일 없음]");
        }
    }
    
}

void CTerrainRenderer::Render_TileWindow()
{
	if (!m_ShowTile)
		return;

	ImGui::PushID("Tile_Render");
	{
		if (ImGui::Begin("Tile", &m_ShowTile))
		{
			const char* tileItems[] = { "Tile_284", "Tile_1024", "Tile_Outline", "Tile_Outline2"};

            _int prevIndex = m_TileTypeIndex;
            ImGui::Combo("##Combo Tile", &m_TileTypeIndex, tileItems, IM_ARRAYSIZE(tileItems));

            if (prevIndex != m_TileTypeIndex)
            {
                m_SelectedTile = 0;
                m_SelectedOutline = 0;
            }

			ImGui::Separator();

			if (m_TileTypeIndex == 0)
			{
				Render_Tile284();
			}
			else if (m_TileTypeIndex == 1)
			{
				Render_Tile1024();
			}
            else if (m_TileTypeIndex == 2)
            {
                Render_Outline();
            }
            else if (m_TileTypeIndex == 3)
            {
                Render_Outline2();
            }
		}

		ImGui::End();
	}
	ImGui::PopID();

}

void CTerrainRenderer::Render_Tile284()
{
	ImGui::Text(u8"선택된 타일 : %d", m_SelectedTile + 1);
	ImGui::Separator();

	// --- 타일 버튼 ---
	if (m_TextureCom284)
	{
		const _uint  tileCount = 6;
		const _uint  tilePerRow = 6;
		const ImVec2 tileSize(48.f, 48.f);

		IDirect3DBaseTexture9* baseTexture = m_TextureCom284->Get_BaseTexture(0);
		if (baseTexture)
		{
			ImTextureID texID = (ImTextureID)baseTexture;

			for (_uint i = 0; i < tileCount; i++)
			{
				if (i > 0 && (i % tilePerRow) != 0)
					ImGui::SameLine();

				_int tileX = i;
				_int tileY = 0;

				_float du = 1.f / (float)ATLAS_COLS_284;
				_float dv = 1.f / (float)ATLAS_ROWS_284;

				ImVec2 uv0(tileX * du, tileY * dv);
				ImVec2 uv1((tileX + 1) * du, (tileY + 1) * dv);

				ImGui::PushID((int)i);

				const _bool isSelected = (i == (_uint)m_SelectedTile);

				if (isSelected)
				{
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.7f, 0.2f, 1.f));
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8f, 0.8f, 0.3f, 1.0f));
				}

				if (ImGui::ImageButton("Tile284", texID, tileSize, uv0, uv1))
				{
					m_SelectedTile = (int)i;
				}

				if (isSelected)
					ImGui::PopStyleColor(2);

				ImGui::PopID();
			}
		}

		ImGui::Separator();
		if (ImGui::Button(u8"전체 타일을 선택된 타일로"))
		{
			for (size_t i = 0; i < m_TileIndices284.size(); ++i)
			{
                m_TileIndices284[i] = m_SelectedTile;
                m_TileIndices1024[i] = -1;
			}
		}
	}
}

void CTerrainRenderer::Render_Tile1024()
{
    ImGui::Text(u8"선택된 타일 : %d", m_SelectedTile + 1);
    ImGui::Separator();

    // --- 타일 버튼 ---
    if (m_TextureCom1024)
    {
        const _uint  tileCount = ATLAS_COLS_1024 * ATLAS_ROWS_1024;
        const _uint  tilePerRow = ATLAS_COLS_1024;
        const ImVec2 tileSize(48.f, 48.f);

        IDirect3DBaseTexture9* baseTexture = m_TextureCom1024->Get_BaseTexture(0);
        if (baseTexture)
        {
            ImTextureID texID = (ImTextureID)baseTexture;

            for (_uint i = 0; i < tileCount; i++)
            {
                if (i > 0 && (i % tilePerRow) != 0)
                    ImGui::SameLine();

                int tileX = i % ATLAS_COLS_1024;  
                _int tileY = i / ATLAS_COLS_1024; 

                _float du = 1.f / (float)ATLAS_COLS_1024;
                _float dv = 1.f / (float)ATLAS_ROWS_1024;

                ImVec2 uv0(tileX * du, tileY * dv);
                ImVec2 uv1((tileX + 1) * du, (tileY + 1) * dv);

                ImGui::PushID((int)i);

                const _bool isSelected = (i == (_uint)m_SelectedTile);

                if (isSelected)
                {
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.7f, 0.2f, 1.f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8f, 0.8f, 0.3f, 1.0f));
                }

                if (ImGui::ImageButton("Tile1024", texID, tileSize, uv0, uv1))
                {
                    m_SelectedTile = (int)i;
                }

                if (isSelected)
                    ImGui::PopStyleColor(2);

                ImGui::PopID();
            }
        }

        ImGui::Separator();
        if (ImGui::Button(u8"전체 타일을 선택된 타일로"))
        {
            for (size_t i = 0; i < m_TileIndices1024.size(); ++i)
            {
                m_TileIndices1024[i] = m_SelectedTile;
                m_TileIndices284[i] = -1;
            }
        }
    }
}

void CTerrainRenderer::Render_Outline()
{
    if (!m_TextureComOutline)
        return;

    ImGui::PushID("Outline_Render");

    const char* outlineItems[] = { "Outline 1", "Outline 2", "Outline 3" };
    ImGui::Combo("##Select Outline", &m_SelectedOutline, outlineItems, 3);

    ImGui::Separator();

    IDirect3DBaseTexture9* tex = m_TextureComOutline->Get_BaseTexture(m_SelectedOutline);
    if (tex)
    {
        const ImVec2 tileSize(48.f, 48.f);

        for (int i = 0; i < 5; ++i)
        {
            if (i > 0) ImGui::SameLine();

            float u0 = i * 0.2f;
            float u1 = (i + 1) * 0.2f;
            ImVec2 uv0(u0, 0.f);
            ImVec2 uv1(u1, 1.f);

            ImGui::PushID(i);
            bool isSelected = (i == m_SelectedTile);

            if (isSelected)
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.7f, 0.2f, 1.f));

            if (ImGui::ImageButton("OutlineTile", (ImTextureID)tex, tileSize, uv0, uv1))
            {
                m_SelectedTile = i; 
            }

            if (isSelected)
                ImGui::PopStyleColor();

            ImGui::PopID();
        }
    }
    ImGui::PopID();
}

void CTerrainRenderer::Render_Outline2()
{
    if (!m_TextureComOutline2)
        return;

    ImGui::PushID("Outline2_Render");
    ImGui::Text(u8"선택된 타일 : %d", m_SelectedTile + 1);
    ImGui::Separator();

    IDirect3DBaseTexture9* tex = m_TextureComOutline2->Get_BaseTexture(0);

    if (tex)
    {
        const ImVec2 tileSize(48.f, 48.f);
        for (int i = 0; i < ATLAS_COLS_OUTLINE2; ++i)
        {
            if (i > 0) ImGui::SameLine();

            _float du = 1.f / (_float)ATLAS_COLS_OUTLINE2;

            ImVec2 uv0(i * du, 0.f);
            ImVec2 uv1((i + 1) * du, 1.f);

            ImGui::PushID(i);

            _bool isSelected = (i == m_SelectedTile);
            if (isSelected)
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.7f, 0.2f, 1.f));

            if (ImGui::ImageButton("Outline2Tile", (ImTextureID)tex, tileSize, uv0, uv1))
            {
                m_SelectedTile = i;
            }

            if (isSelected)
                ImGui::PopStyleColor();

            ImGui::PopID();
        }
    }
    ImGui::PopID();
}

HRESULT CTerrainRenderer::SaveMap(const char* filePath)
{
    ofstream ofs(filePath, ios_base::binary);
    if (!ofs.is_open())
        return E_FAIL;

    uint32_t magic = 0x54494C45; // 'TILE'
    uint32_t version = 9;

    uint32_t cellCntX = m_SizeX - 1;
    uint32_t cellCntZ = m_SizeZ - 1;
    uint32_t tileSize = static_cast<uint32_t>(m_TileIndices284.size());
    _float cellSize = m_CellSize;

    ofs.write(reinterpret_cast<char*>(&magic), sizeof(magic));
    ofs.write(reinterpret_cast<char*>(&version), sizeof(version));
    ofs.write(reinterpret_cast<char*>(&cellCntX), sizeof(cellCntX));
    ofs.write(reinterpret_cast<char*>(&cellCntZ), sizeof(cellCntZ));
    ofs.write(reinterpret_cast<char*>(&cellSize), sizeof(cellSize));

    if (tileSize > 0)
    {
        ofs.write(reinterpret_cast<const char*>(m_TileIndices284.data()), sizeof(_int) * tileSize);
        ofs.write(reinterpret_cast<const char*>(m_TileIndices1024.data()), sizeof(_int) * tileSize);
        ofs.write(reinterpret_cast<const char*>(m_TileIndicesOutline.data()), sizeof(_int) * tileSize);
        ofs.write(reinterpret_cast<const char*>(m_TileIndicesOutline2.data()), sizeof(_int) * tileSize);
    }

    if (m_TransformCom && m_TransformCom->Get_Owner())
    {
        CTerrain* owner = dynamic_cast<CTerrain*>(m_TransformCom->Get_Owner());
        if (owner)
        {
            // 벽 위치 저장
            CWallRenderer* wallRenderer = owner->Get_WallRenderer();
            if (wallRenderer)
            {
                wallRenderer->Save_Walls(ofs);
            }
        }
    }

    if (version >= 8)
    {
        CMapObjectMgr::GetInstance()->Save_ToStream(ofs);

        // 몬스터 위치 저장
        if (m_TransformCom && m_TransformCom->Get_Owner())
        {
            CTerrain* owner = dynamic_cast<CTerrain*>(m_TransformCom->Get_Owner());
            if (owner)
            {
                CObjectSpawner* monsterSpawner = dynamic_cast<CObjectSpawner*>(
                    owner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::OBJECT_SPAWNER));

                if (monsterSpawner)
                {
                    monsterSpawner->Save_Objects(ofs);
                }
            }
        }

        // 안개 저장
        CFogMgr::GetInstance()->Save_ToStream(ofs);
    }

    ofs.close();

    return S_OK;
}

HRESULT CTerrainRenderer::LoadMap(const char* filePath)
{
    ifstream ifs(filePath, ios_base::binary);
    if (!ifs.is_open())
        return E_FAIL;

    uint32_t magic = 0;
    uint32_t version = 0;
    uint32_t cellCntX = 0;
    uint32_t cellCntZ = 0;

    ifs.read(reinterpret_cast<char*>(&magic), sizeof(magic));
    ifs.read(reinterpret_cast<char*>(&version), sizeof(version));
    ifs.read(reinterpret_cast<char*>(&cellCntX), sizeof(cellCntX));
    ifs.read(reinterpret_cast<char*>(&cellCntZ), sizeof(cellCntZ));

    // 파일 포맷 체크
    if (magic != 0x54494C45)
    {
        ifs.close();
        return E_FAIL;
    }

    float cellSize = 1.f;  // 기본값

    if (version >= 2)
    {
        ifs.read(reinterpret_cast<char*>(&cellSize), sizeof(cellSize));
    }

    if (cellCntX + 1 != m_SizeX || cellCntZ + 1 != m_SizeZ || cellSize != m_CellSize)
    {
        Resize_Terrain(cellCntX + 1, cellCntZ + 1, cellSize);
    }

    const size_t tileSize = static_cast<size_t>(cellCntX) * cellCntZ;
    m_TileIndices284.resize(tileSize);
    m_TileIndices1024.resize(tileSize);
    m_TileIndicesOutline.resize(tileSize);
    m_TileIndicesOutline2.resize(tileSize);


    if (tileSize > 0)
    {
        ifs.read(reinterpret_cast<char*>(m_TileIndices284.data()), sizeof(_int) * tileSize);

        if (version >= 5)
            ifs.read(reinterpret_cast<char*>(m_TileIndices1024.data()), sizeof(_int) * tileSize);

        if (version >= 6)
            ifs.read(reinterpret_cast<char*>(m_TileIndicesOutline.data()),sizeof(_int) * tileSize);

        if (version >= 9)
            ifs.read(reinterpret_cast<char*>(m_TileIndicesOutline2.data()), sizeof(_int) * tileSize);
    }

    if (version >= 7)
    {
        if (m_TransformCom && m_TransformCom->Get_Owner())
        {
            CTerrain* owner = dynamic_cast<CTerrain*>(m_TransformCom->Get_Owner());
            if (owner)
            {
                // 벽 소환
                CWallRenderer* wallRenderer = owner->Get_WallRenderer();
                if (wallRenderer)
                {
                    wallRenderer->Clear_All();

                    wallRenderer->Load_Walls(ifs);
                }

            }
        }
    }

    if (version >= 8)
    {
        _vec3 terrainPos(0.f, 0.f, 0.f);

        if (m_TransformCom)
            terrainPos = m_TransformCom->Get_Pos();

        // 맵 오브젝트 소환
        CMapObjectMgr::GetInstance()->Load_FromStream(ifs, terrainPos);

        // 오브젝트 소환
        if (m_TransformCom && m_TransformCom->Get_Owner())
        {
            CTerrain* owner = dynamic_cast<CTerrain*>(m_TransformCom->Get_Owner());
            if (owner)
            {
                CObjectSpawner* monsterSpawner = dynamic_cast<CObjectSpawner*>(
                    owner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::OBJECT_SPAWNER));

                if (monsterSpawner)
                {
                    monsterSpawner->Load_Objects(ifs);
                }
            }
        }

        // 안개 소환
        CFogMgr::GetInstance()->Load_FromStream(ifs, terrainPos);
    }

    if (version >= 9)
        ifs.read(reinterpret_cast<char*>(m_TileIndicesOutline2.data()), sizeof(_int) * tileSize);


    ifs.close();
    return S_OK;
}

_vec3 CTerrainRenderer::GetCellCenterWorld(_uint cellX, _uint cellZ) const
{
    const _float vtxItv = m_CellSize;

    _vec3 localPos(
        cellX * vtxItv + vtxItv * 0.5f,
        1.f,
        cellZ * vtxItv + vtxItv * 0.5f);

    if (m_TransformCom)
    {
        _vec3 pos, scale;
        m_TransformCom->Get_Info(INFO_POS, &pos);
        scale = m_TransformCom->Get_Scale();

        _vec3 worldPos;
        worldPos.x = localPos.x * scale.x + pos.x;
        worldPos.y = localPos.y * scale.y + pos.y;
        worldPos.z = localPos.z * scale.z + pos.z;

        return worldPos;
    }

    return localPos;
}

bool CTerrainRenderer::PickCellMousePos(_int& outX, _int& outZ)
{
    _vec3 terrainPos;
    m_TransformCom->Get_Info(INFO_POS, &terrainPos);
    float planeY = terrainPos.y;

	_vec3 hitWorld;
	if (PickPlaneY(m_GraphicDev, g_hWnd, terrainPos.y, hitWorld) == false)
		return false;

	_matrix matWorld = m_TransformCom->Get_World();
	_matrix matInvWorld;
	D3DXMatrixInverse(&matInvWorld, nullptr, &matWorld);

	_vec3 hitLocal;
	D3DXVec3TransformCoord(&hitLocal, &hitWorld, &matInvWorld);

    const float cellSize = m_CellSize;  
    _int cellX = (_int)floorf(hitLocal.x / cellSize);
    _int cellZ = (_int)floorf(hitLocal.z / cellSize);

    if (cellX < 0 || cellX >= m_SizeX - 1 || 
        cellZ < 0 || cellZ >= m_SizeZ - 1)   
        return false;

	outX = cellX;
	outZ = cellZ;
	return true;
}

HRESULT CTerrainRenderer::Refresh_MapFileList()
{
    m_MapList.clear();

    const char* mapDir = "../Bin/Resource/Data/Terrain";

    filesystem::path dirPath(mapDir);
    if (!filesystem::exists(dirPath) || !filesystem::is_directory(dirPath))
        return E_FAIL;

    for (const auto& entry : filesystem::directory_iterator(dirPath))
    {
        if (!entry.is_regular_file())
            continue;

        const filesystem::path& p = entry.path();
        if (p.extension() == ".map")
        {
            // 파일명만 유지 (경로는 제외)
            m_MapList.push_back(p.filename().string());
        }
    }

    // 정렬 한번 후 출력
    sort(m_MapList.begin(), m_MapList.end());

    if (m_MapList.empty())
        m_SelectedMapIndex = -1;

    else if (m_SelectedMapIndex >= (_int)m_MapList.size())
        m_SelectedMapIndex = (_int)m_MapList.size() - 1;

    return S_OK;
}

void CTerrainRenderer::Resize_Terrain(_int sizeX, _int sizeZ, _float cellSize)
{
    m_SizeX = sizeX;
    m_SizeZ = sizeZ;

    m_CellSize = cellSize;

    // 기존 버퍼 메모리 해제
    Safe_Release(m_BufferCom);

    _ulong vtxItv = static_cast<_ulong>(cellSize + 0.5f);
    m_CellSize = static_cast<_float>(vtxItv);

    m_BufferCom = CTerrainTex::Create(m_GraphicDev, sizeX, sizeZ, vtxItv);

    const int cellCntX = sizeX - 1;
    const int cellCntZ = sizeZ - 1;
    const int cellCount = cellCntX * cellCntZ;

    m_TileIndices284.assign(cellCount, 0);     
    m_TileIndices1024.assign(cellCount, -1);   
    m_TileIndicesOutline.assign(cellCount, -1);
    m_TileIndicesOutline2.assign(cellCount, -1);
}

CTerrainRenderer* CTerrainRenderer::Create(DEVICE graphicDev)
{
    CTerrainRenderer* renderer = new CTerrainRenderer(graphicDev);

    if (renderer == nullptr)
        return nullptr;

    /*if (FAILED(renderer->Ready_TerrainRenderer()))
    {
        Safe_Release(renderer);
        MSG_BOX("Terrain_Renderer Create Faield");
        return nullptr;
    }*/

    return renderer;
}

CComponent* CTerrainRenderer::Clone()
{
    return new CTerrainRenderer(*this);
}

void CTerrainRenderer::Free()
{
    Safe_Release(m_BufferCom);

    CComponent::Free();
}

void CTerrainRenderer::Set_Material()
{
    m_GraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);

    D3DMATERIAL9 material;
    ZeroMemory(&material, sizeof(D3DMATERIAL9));

    material.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
    material.Ambient = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
    material.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);

    m_GraphicDev->SetMaterial(&material);
}
