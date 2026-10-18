#pragma once

#include "CVIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CTerrainTex : public CVIBuffer
{
private:
    explicit CTerrainTex(DEVICE graphicDev);
    explicit CTerrainTex(const CTerrainTex& rhs);
    ~CTerrainTex() override;

public:
    virtual HRESULT Ready_Buffer(
        const _ulong& CntX,
        const _ulong& CntZ,
        const _ulong& VtxItv);

    void Render_Buffer() override;

public:
    void ApplyTileSetting(
        const vector<_int>& tileIndices,
        _int atlasCols,
        _int atlasRows,
        _float texWidth,
        _float texHeight);

    const _vec3* Get_VtxPos() { return m_Pos; }

private:
    _vec3* m_Pos;

    _uint m_CntX;
    _uint m_CntZ;

    _ulong m_CellX = 0;
    _ulong m_CellZ = 0;

public:
    static CTerrainTex* Create(LPDIRECT3DDEVICE9 graphicDev,
                               const _ulong&     CntX   = TILE_CNT_X,
                               const _ulong&     CntZ   = TILE_CNT_Z,
                               const _ulong&     VtxItv = VTX_IV);

    CComponent* Clone() override;
private:
    void Free() override;
};

END
