#include "CTerrainTex.h"

CTerrainTex::CTerrainTex(DEVICE graphicDev)
    : CVIBuffer(graphicDev), m_Pos(nullptr), m_CntX(0), m_CntZ(0)
{
}

CTerrainTex::CTerrainTex(const CTerrainTex& rhs)
    : CVIBuffer(rhs), m_Pos(nullptr), m_CntX(rhs.m_CntX), m_CntZ(rhs.m_CntZ)
{
    // 깊은 복사
    if (rhs.m_Pos != nullptr && m_VtxCnt > 0)
    {
        m_Pos = new _vec3[m_VtxCnt];

        memcpy(m_Pos, rhs.m_Pos, sizeof(_vec3) * m_VtxCnt);
    }
}

CTerrainTex::~CTerrainTex()
{

}

HRESULT CTerrainTex::Ready_Buffer(const _ulong& CntX,
    const _ulong& CntZ, const _ulong& VtxItv)
{
    const _ulong iCellCntX = CntX - 1;
    const _ulong iCellCntZ = CntZ - 1;

    m_VtxSize = sizeof(VTXTEX);
    m_VtxCnt = iCellCntX * iCellCntZ * 4; 
    m_TriCnt = iCellCntX * iCellCntZ * 2; 
    m_FVF = FVF_TEX;

    m_CntX = CntX;
    m_CntZ = CntZ;

    m_Pos = new _vec3[m_VtxCnt];

    m_IdxSize = sizeof(INDEX32);
    m_IdxFmt = D3DFMT_INDEX32;

    if (FAILED(CVIBuffer::Ready_Buffer()))
        return E_FAIL;

    VTXTEX* vertex = NULL;
    m_VB->Lock(0, 0, (void**)&vertex, 0);

    _ulong index = 0;

    for (_ulong z = 0; z < iCellCntZ; ++z)
    {
        for (_ulong x = 0; x < iCellCntX; ++x)
        {
            index = (z * iCellCntX + x) * 4;

            float xPos = (_float)(x * VtxItv);
            float zPos = (_float)(z * VtxItv);
            float xPosNext = (_float)((x + 1) * VtxItv);
            float zPosNext = (_float)((z + 1) * VtxItv);

            // 0: 좌상
            vertex[index + 0].position = { xPos, 0.f, zPos };
            vertex[index + 0].texUV = { 0.f, 0.f };

            // 1: 우상
            vertex[index + 1].position = { xPosNext, 0.f, zPos };
            vertex[index + 1].texUV = { 1.f, 0.f };

            // 2: 좌하
            vertex[index + 2].position = { xPos, 0.f, zPosNext };
            vertex[index + 2].texUV = { 0.f, 1.f };

            // 3: 우하
            vertex[index + 3].position = { xPosNext, 0.f, zPosNext };
            vertex[index + 3].texUV = { 1.f, 1.f };

            // m_Pos에도 저장 (Picking 등을 위해)
            m_Pos[index + 0] = vertex[index + 0].position;
            m_Pos[index + 1] = vertex[index + 1].position;
            m_Pos[index + 2] = vertex[index + 2].position;
            m_Pos[index + 3] = vertex[index + 3].position;
        }
    }

    m_VB->Unlock();

    INDEX32* pIndex = NULL;
    m_IB->Lock(0, 0, (void**)&pIndex, 0);

    _ulong dwTriCnt = 0;

    for (_ulong z = 0; z < iCellCntZ; ++z)
    {
        for (_ulong x = 0; x < iCellCntX; ++x)
        {
            index = (z * iCellCntX + x) * 4;

            // 삼각형 1
            // (좌상 - 우상 - 좌하) -> (0, 1, 2)
            pIndex[dwTriCnt]._0 = index + 0;
            pIndex[dwTriCnt]._1 = index + 1;
            pIndex[dwTriCnt]._2 = index + 2;
            dwTriCnt++;

            // 삼각형 2
            // (우상 - 우하 - 좌하) -> (1, 3, 2)
            pIndex[dwTriCnt]._0 = index + 1;
            pIndex[dwTriCnt]._1 = index + 3;
            pIndex[dwTriCnt]._2 = index + 2;
            dwTriCnt++;
        }
    }

    m_IB->Unlock();

    return S_OK;
}

void CTerrainTex::Render_Buffer()
{
    //m_GraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

    CVIBuffer::Render_Buffer();

    m_GraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

}

void CTerrainTex::ApplyTileSetting(const vector<_int>& tileIndices, _int atlasCols, _int atlasRows, _float texWidth, _float texHeight)
{
    if (m_VB == nullptr || m_CntX < 2 || m_CntZ < 2)
        return;

    const _ulong cellCountX = m_CntX - 1;
    const _ulong cellCountZ = m_CntZ - 1;

    if (tileIndices.size() < cellCountX * cellCountZ)
        return;

    VTXTEX* vertex = nullptr;
    if (FAILED(m_VB->Lock(0, 0, (void**)&vertex, 0)))
        return;

    const float du = 1.f / (float)atlasCols;
    const float dv = 1.f / (float)atlasRows;

    const float epsU = 0.5f / texWidth;   
    const float epsV = 0.5f / texHeight;  

    for (_ulong z = 0; z < cellCountZ; ++z)
    {
        for (_ulong x = 0; x < cellCountX; ++x)
        {
            const _ulong cellIndex = z * cellCountX + x;
            _int tileIndex = tileIndices[cellIndex];

            const _ulong baseIndex = (z * cellCountX + x) * 4;

            if (tileIndex < 0)
            {
                vertex[baseIndex + 0].position = { 0.f, 0.f, 0.f };
                vertex[baseIndex + 1].position = { 0.f, 0.f, 0.f };
                vertex[baseIndex + 2].position = { 0.f, 0.f, 0.f };
                vertex[baseIndex + 3].position = { 0.f, 0.f, 0.f };
                continue;
            }

            vertex[baseIndex + 0].position = m_Pos[baseIndex + 0];
            vertex[baseIndex + 1].position = m_Pos[baseIndex + 1];
            vertex[baseIndex + 2].position = m_Pos[baseIndex + 2];
            vertex[baseIndex + 3].position = m_Pos[baseIndex + 3];

            const _int tileX = tileIndex % atlasCols;
            const _int tileY = tileIndex / atlasCols;

            float u0 = tileX * du;
            float u1 = u0 + du;
            float v0 = tileY * dv;
            float v1 = v0 + dv;

            // 경계에서 한 픽셀 안쪽으로
            const float insetPixels = 2.0f;  
            const float insetU = insetPixels / texWidth;
            const float insetV = insetPixels / texHeight;
            u0 += insetU;  u1 -= insetU;
            v0 += insetV;  v1 -= insetV;

            vertex[baseIndex + 0].texUV = { u0, v1 }; // 좌상
            vertex[baseIndex + 1].texUV = { u1, v1 }; // 우상
            vertex[baseIndex + 2].texUV = { u0, v0 }; // 좌하 
            vertex[baseIndex + 3].texUV = { u1, v0 }; // 우하
        }
    }

    m_VB->Unlock();
}

CTerrainTex* CTerrainTex::Create(LPDIRECT3DDEVICE9 graphicDev, const _ulong& CntX, const _ulong& CntZ,
                                 const _ulong& VtxItv)
{
    CTerrainTex* pTerainTex = new CTerrainTex(graphicDev);

    if (FAILED(pTerainTex->Ready_Buffer(CntX, CntZ, VtxItv)))
    {
        Safe_Release(pTerainTex);
        MSG_BOX("pTerainTex Create Failed");
        return nullptr;
    }

    return pTerainTex;
}

CComponent* CTerrainTex::Clone()
{
    return new CTerrainTex(*this);
}

void CTerrainTex::Free()
{
    //if (m_IsClone == false)
    Safe_Delete_Array(m_Pos);
    m_Pos = nullptr;

    CVIBuffer::Free();
}
