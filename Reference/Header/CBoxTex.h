#pragma once
#include "CVIBuffer.h"
#include "Engine_Define.h"

BEGIN(Engine)
/**
 * \brief Render에서 WireFrame으로 설정 중. Box 텍스쳐를 쓰지 않을거같음
 * CubeTex와 동일한 구조
 */

class ENGINE_DLL CBoxTex : public CVIBuffer
{
private:
    explicit CBoxTex();
    explicit CBoxTex(DEVICE pGraphicDev);
    explicit CBoxTex(const CBoxTex& rhs);
    ~CBoxTex() override;

public:
    HRESULT Ready_Buffer() override;
    void    Render_Buffer() override;

public:
    static CBoxTex* Create(DEVICE pGraphicDev);
    CComponent*     Clone() override;
private:
    void Free() override;
};

END
