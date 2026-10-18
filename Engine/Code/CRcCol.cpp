#include "CRcCol.h"

CRcCol::CRcCol()
{ }

CRcCol::CRcCol(DEVICE graphicDev)
    : CVIBuffer(graphicDev)
{ }

CRcCol::CRcCol(const CRcCol& rhs)
    : CVIBuffer(rhs)
{ }

CRcCol::~CRcCol()
{ }

HRESULT CRcCol::Ready_Buffer()
{
    m_VtxSize = sizeof(VTXCOL);
    m_VtxCnt  = 4;
    m_TriCnt  = 2;
    m_FVF     = FVF_COL;

    m_IdxSize = sizeof(INDEX32);
    m_IdxFmt  = D3DFMT_INDEX32;

    if (FAILED(CVIBuffer::Ready_Buffer()))
        return E_FAIL;

    VTXCOL* vertex = nullptr;

    m_VB->Lock(0, 0, (void**)&vertex, 0);

    vertex[0].position = { -1.f, 1.f, 0.f };
    vertex[0].color    = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);

    vertex[1].position = { 1.f, 1.f, 0.f };
    vertex[1].color    = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);

    vertex[2].position = { 1.f, -1.f, 0.f };
    vertex[2].color    = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);

    vertex[3].position = { -1.f, -1.f, 0.f };
    vertex[3].color    = D3DXCOLOR(0.f, 1.f, 0.f, 1.f);

    m_VB->Unlock();

    INDEX32* index = nullptr;

    m_IB->Lock(0, 0, (void**)&index, 0);

    // 오른쪽 위
    index[0]._0 = 0;
    index[0]._1 = 1;
    index[0]._2 = 2;

    // 왼쪽 아래
    index[1]._0 = 0;
    index[1]._1 = 2;
    index[1]._2 = 3;

    m_IB->Unlock();

    return S_OK;
}

void CRcCol::Render_Buffer()
{
    CVIBuffer::Render_Buffer();
}

CRcCol* CRcCol::Create(DEVICE graphicDev)
{
    auto* rcCol = new CRcCol(graphicDev);

    if (FAILED(rcCol->Ready_Buffer()))
    {
        Safe_Release(rcCol);
        MSG_BOX("RcCol Create Failed");
        return nullptr;
    }

    return rcCol;
}

CComponent* CRcCol::Clone()
{
    return new CRcCol(*this);
}

void CRcCol::Free()
{
    CVIBuffer::Free();
}
