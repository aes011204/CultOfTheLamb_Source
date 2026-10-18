#include "CVIBuffer.h"

CVIBuffer::CVIBuffer()
    : m_VB(nullptr), m_VtxSize(0), m_VtxCnt(0), m_TriCnt(0), m_FVF(0),
      m_IB(nullptr), m_IdxSize(0), m_IdxFmt(D3DFMT_INDEX32)
{ }

CVIBuffer::CVIBuffer(DEVICE graphicDev)
    : CComponent(graphicDev),
      m_VB(nullptr), m_VtxSize(0), m_VtxCnt(0), m_TriCnt(0), m_FVF(0),
      m_IB(nullptr), m_IdxSize(0), m_IdxFmt(D3DFMT_INDEX32)
{ }

CVIBuffer::CVIBuffer(const CVIBuffer& rhs)
    : CComponent(rhs),
      m_VB(rhs.m_VB), m_VtxSize(rhs.m_VtxSize), m_VtxCnt(rhs.m_VtxCnt),
      m_TriCnt(rhs.m_TriCnt), m_FVF(rhs.m_FVF),
      m_IB(rhs.m_IB), m_IdxSize(rhs.m_IdxSize), m_IdxFmt(rhs.m_IdxFmt)
{
    m_VB->AddRef();
    m_IB->AddRef();
}

CVIBuffer::~CVIBuffer()
{ }

HRESULT CVIBuffer::Ready_Buffer()
{
    if (FAILED(m_GraphicDev->CreateVertexBuffer(
        m_VtxCnt * m_VtxSize,   // 버텍스 버퍼 전체 크기
        0,                      // 버퍼의 속성(0인 경우 정적 버퍼, D3DUSAGE_DYNAMIC인 경우 동적 버퍼)
        m_FVF,                  // 버텍스 속성
        D3DPOOL_MANAGED,        // 메모리 풀 타입. 정적 버퍼인 경우 MANAGED만 지원
        &m_VB,                  // 버텍스 버퍼 포인터
        NULL)))                 // 공유할 일 없음
    {
        MSG_BOX("Ready_Buffer: Failed to create VB");
        return E_FAIL;
    }

    if (FAILED(m_GraphicDev->CreateIndexBuffer(
        m_IdxSize * m_TriCnt,   // 인덱스 버퍼 전체 크기 (삼각형당 바이트 * 삼각형 개수)
        0,                      // 버퍼의 종류(속성) -> D3DUSAGE_DYNAMIC
        m_IdxFmt,               // 인덱스 속성
        D3DPOOL_MANAGED,        // 메모리 풀
        &m_IB,                  // 인덱스 버퍼 포인터
        NULL)))
    {
        MSG_BOX("Ready_Buffer: Failed to create IB");
        return E_FAIL;
    }

    // 동적 버퍼 :
    // 파티클 이펙트에서 동적으로 나타났다 사라졌다 하는 데 사용

    // 메모리 풀의 종류
    // - 빠름 -
    // D3DPOOL_DEFAULT      = 0,    // 그래픽 메모리 저장 (빨라서 종종 사용)
    // D3DPOOL_MANAGED      = 1,    // 그래픽 메모리 저장, 메인 메모리에 백업 (안전해서 일반적으로 사용)
    // - 느림 -
    // D3DPOOL_SYSTEMMEM    = 2,    // 메인 메모리 저장 (CPU가 접근하게 만들 때)
    // D3DPOOL_SCRATCH      = 3,    // 메인 메모리 저장 (DX 장치로 접근 불가) (거의 안씀)

    return S_OK;
}

void CVIBuffer::Render_Buffer()
{
    m_GraphicDev->SetStreamSource(0, m_VB, 0, m_VtxSize);

    m_GraphicDev->SetFVF(m_FVF);

    if (m_IB)  // 인덱스 버퍼가 있을 경우 DrawIndexedPrimitive
    {
        m_GraphicDev->SetIndices(m_IB);
        m_GraphicDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_VtxCnt, 0, m_TriCnt);
    }
    else        // 인덱스 버퍼가 없을 경우 DrawPrimitive
        m_GraphicDev->DrawPrimitive(D3DPT_TRIANGLELIST, 0, m_TriCnt);

    // D3D 그리기 종류
    // D3DPT_POINTLIST      = 1,    // 점 단위로 그리기 (파티클 렌더 용)
    // D3DPT_LINELIST       = 2,    // 두 점씩 묶어 선 그리기
    // D3DPT_LINESTRIP      = 3,    // 연속적으로 선 그리기
    // D3DPT_TRIANGLELIST   = 4,    // 세 점씩 묶어 삼각형 그리기
    // D3DPT_TRIANGLESTRIP  = 5,    // 연속적으로 삼각형 그리기
    // D3DPT_TRIANGLEFAN    = 6,    // 첫 번째 정점을 기준으로 부채꼴 모양 그리기
}

void CVIBuffer::Free()
{
    Safe_Release(m_VB);
    Safe_Release(m_IB);

    CComponent::Free();
}
