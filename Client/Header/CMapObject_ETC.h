#pragma once
#include "CMapObjectBase.h"

class CMapObject_ETC : public CMapObjectBase
{
private:
    explicit CMapObject_ETC(DEVICE graphicDev);
    explicit CMapObject_ETC(const CMapObjectBase& rhs);
    ~CMapObject_ETC() override;

public:
    HRESULT     Ready_GameObject() override;
    _int        Update_GameObject(const _float& timeDelta) override;
    void        LateUpdate_GameObject(const _float& timeDelta) override;
    void        Render_GameObject() override;
    
    MAPOBJTYPE  Get_Type() override;

    static CMapObject_ETC* Create(DEVICE graphicDev);

private:
    void Free() override;


};
