#pragma once
#include "CMapObjectBase.h"

class CMapObject_Deco : public CMapObjectBase
{
private:
    explicit CMapObject_Deco(DEVICE graphicDev);
    explicit CMapObject_Deco(const CMapObjectBase& rhs);
    ~CMapObject_Deco() override;

public:
    HRESULT     Ready_GameObject() override;
    _int        Update_GameObject(const _float& timeDelta) override;
    void        LateUpdate_GameObject(const _float& timeDelta) override;
    void        Render_GameObject() override;
    
    MAPOBJTYPE  Get_Type() override;

    static CMapObject_Deco* Create(DEVICE graphicDev);

private:
    void Free() override;


};
