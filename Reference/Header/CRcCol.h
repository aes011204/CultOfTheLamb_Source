#pragma once
#include "CVIBuffer.h"

BEGIN(Engine)
class ENGINE_DLL CRcCol : public CVIBuffer
{
private:
    explicit CRcCol();
    explicit CRcCol(DEVICE graphicDev);
    explicit CRcCol(const CRcCol& rhs);
    ~CRcCol() override;

public:
    HRESULT Ready_Buffer() override;
    void    Render_Buffer() override;

public:
    static CRcCol* Create(DEVICE graphicDev);
    CComponent*    Clone() override;

private:
    void Free() override;
};

END
