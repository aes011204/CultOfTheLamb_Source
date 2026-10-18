#pragma once
#include "CMapObjectBase.h"

class CMapObject_Rubble : public CMapObjectBase
{
private:
    explicit CMapObject_Rubble(DEVICE graphicDev);
    explicit CMapObject_Rubble(const CMapObjectBase& rhs);
    ~CMapObject_Rubble() override;

public:
    HRESULT     Ready_GameObject() override;
    _int        Update_GameObject(const _float& timeDelta) override;
    void        LateUpdate_GameObject(const _float& timeDelta) override;
    void        Render_GameObject() override;
    
    MAPOBJTYPE  Get_Type() override;

    static CMapObject_Rubble* Create(DEVICE graphicDev);

private:
    void Free() override;


};
