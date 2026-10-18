#pragma once
#include "CMapObjectBase.h"

class CMapObject_DungeonDeco : public CMapObjectBase
{
private:
    explicit CMapObject_DungeonDeco(DEVICE graphicDev);
    explicit CMapObject_DungeonDeco(const CMapObjectBase& rhs);
    ~CMapObject_DungeonDeco() override;

public:
    HRESULT     Ready_GameObject() override;
    _int        Update_GameObject(const _float& timeDelta) override;
    void        LateUpdate_GameObject(const _float& timeDelta) override;
    void        Render_GameObject() override;
    
    MAPOBJTYPE  Get_Type() override;

    static CMapObject_DungeonDeco* Create(DEVICE graphicDev);

private:
    void Free() override;


};
