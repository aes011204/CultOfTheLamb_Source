#pragma once
#include "CComponent.h"

BEGIN(Engine)
class ENGINE_DLL CVIBuffer : public CComponent
{
protected:
    explicit CVIBuffer();
    explicit CVIBuffer(DEVICE graphicDev);
    explicit CVIBuffer(const CVIBuffer& rhs);
    ~CVIBuffer() override;

public:
    virtual HRESULT Ready_Buffer();
    virtual void    Render_Buffer();

protected:
    LPDIRECT3DVERTEXBUFFER9 m_VB;

    _ulong m_VtxSize;	        // 단일 정점 구조체의 바이트 크기
    _ulong m_VtxCnt;		    // 버퍼에 포함된 정점 수
    _ulong m_TriCnt;		    // 렌더링할 삼각형 수
    _ulong m_FVF;		        // 버텍스 포맷을 지정하는 FVF 플래그 값

    LPDIRECT3DINDEXBUFFER9 m_IB;

    _ulong    m_IdxSize;        // 한 개의 삼각형 인덱스가 차지하는 바이트 크기
    D3DFORMAT m_IdxFmt;         // 인덱스 데이터 포맷

public:
    void Free() override;
};

END
