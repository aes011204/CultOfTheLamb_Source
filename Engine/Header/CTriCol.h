#pragma once
#include "CVIBuffer.h"

BEGIN(Engine)
class ENGINE_DLL CTriCol : public CVIBuffer
{
private:
    explicit CTriCol();
    explicit CTriCol(DEVICE graphicDev);
    explicit CTriCol(const CTriCol& rhs);
    ~CTriCol() override;

public:
    HRESULT Ready_Buffer() override;
    void    Render_Buffer() override;

public:
    static CTriCol* Create(DEVICE graphicDev);
    CComponent*     Clone() override;
private:
    void Free() override;
};

END
