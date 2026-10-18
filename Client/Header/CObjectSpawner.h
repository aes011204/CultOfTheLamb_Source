#pragma once

#include "CComponent.h"

enum class SPAWN_OBJ_TYPE : _int
{
    HUMAN_MONSTER = 0,
    FLOATER_MONSTER,

    NPC_TARO,
    NPC_SLAVE,

    // 종류가 추가된다면 여기에도 추가해주기

    SPAWN_TYPE_END
};

class CObjectSpawner : public CComponent
{
private:
    struct SpawnData
    {
        _int cellX, cellZ;
        SPAWN_OBJ_TYPE objectType;
    };

private:
    explicit CObjectSpawner(DEVICE graphicDev);
    explicit CObjectSpawner(const CObjectSpawner& rhs);
    virtual ~CObjectSpawner() override;

public:
    HRESULT Ready_MonsterSpawner(DEVICE graphicDev);
    _int    Update_Component(const _float& timeDelta) override;
    void    LateUpdate_Component() override;
    void    Render_Editor() override;

public:
    void Initialize(_int sizeX, _int sizeZ, _float cellSize);

    // 몬스터 배치/제거
    void Add_SpawnPoint(_int x, _int z, SPAWN_OBJ_TYPE type);
    void Remove_SpawnPoint(_int x, _int z);
    void Clear_All();

    // 조회
    _bool   Has_ObjPoint(_int x, _int z);
    _int    Get_ObjectCount() { return static_cast<_int>(m_SpawnPoints.size()); }
    const vector<SpawnData>& Get_SpawnPoints() const { return m_SpawnPoints; }

    // 저장/로드
    HRESULT Save_Objects(ofstream& ofs) const;
    HRESULT Load_Objects(ifstream& ifs);

    // 에디터
    void Set_EditMode(bool enabled) { m_EditMode = enabled; }
    bool Get_EditMode() const { return m_EditMode; }
    SPAWN_OBJ_TYPE Get_SelectedType() const { return m_SelectedType; }

public:
    static CObjectSpawner* Create(DEVICE graphicDev);
    CComponent* Clone() override;

protected:
    void Free() override;

private:
    vector<SpawnData>           m_SpawnPoints;
    _int                        m_GridSizeX, m_GridSizeZ;
    _float                      m_CellSize;
    _bool                       m_EditMode;
    SPAWN_OBJ_TYPE              m_SelectedType;

};

