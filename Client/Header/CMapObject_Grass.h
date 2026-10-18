#pragma once
#include "CMapObjectBase.h"

class CMapObject_Grass : public CMapObjectBase
{
private:
    explicit CMapObject_Grass(DEVICE graphicDev);
    explicit CMapObject_Grass(const CMapObjectBase& rhs);
    ~CMapObject_Grass() override;

public:
    HRESULT     Ready_GameObject() override;
    _int        Update_GameObject(const _float& timeDelta) override;
    void        LateUpdate_GameObject(const _float& timeDelta) override;
    void        Render_GameObject() override;
    
    MAPOBJTYPE  Get_Type() override;

    static CMapObject_Grass* Create(DEVICE graphicDev);

private:
    void Free() override;


};
