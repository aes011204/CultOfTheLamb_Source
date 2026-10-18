#pragma once

#include "CComponent.h"

class CWallRenderer : public CComponent
{
private:
    explicit CWallRenderer(DEVICE graphicDev);
    explicit CWallRenderer(const CWallRenderer& rhs);
    virtual ~CWallRenderer() override;

public:
    HRESULT Ready_WallRenderer(DEVICE graphicDev);
    _int    Update_Component(const _float& timeDelta) override;
    void    LateUpdate_Component() override;
    void    Render_Editor() override;

public:
    void Initialize(_int sizeX, _int sizeZ, _float cellSize);
    void Resize(_int sizeX, _int sizeZ, _float cellSize);

    // 벽 하나만 배치
    void Set_WallPosition(_int x, _int z, _bool value);
    void Clear_All();

    _bool Has_Wall(_int x, _int z);
    _int Get_WallCount();
    _int Get_GridSizeX() { return m_GridSizeX; }
    _int Get_GridSizeZ() { return m_GridSizeZ; }

    HRESULT Save_Walls(ofstream& ofs) const;
    HRESULT Load_Walls(ifstream& ifs);

    void Set_EditMode(bool enabled) { m_EditMode = enabled; }
    bool Get_EditMode() const { return m_EditMode; }

public:
    static CWallRenderer* Create(DEVICE graphicDev);
    CComponent* Clone() override;

protected:
    void Free() override;

private:
    vector<_int> m_WallData;
    _int m_GridSizeX, m_GridSizeZ;

    _float m_CellSize;
    _bool m_EditMode;
};
