#pragma once
#include "CVIBuffer.h"
#include "Engine_Define.h"

BEGIN(Engine)
class ENGINE_DLL CBoxCol : public CVIBuffer
{
private:
    explicit CBoxCol();
    explicit CBoxCol(DEVICE pGraphicDev);
    explicit CBoxCol(const CBoxCol& rhs);
    ~CBoxCol() override;

public:
    HRESULT Ready_Buffer() override;
    void    Render_Buffer() override;

public:
    static CBoxCol* Create(DEVICE pGraphicDev);
    CComponent*     Clone() override;
private:
    void Free() override;
};

END
