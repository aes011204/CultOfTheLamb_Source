#include "CTriCol.h"

CTriCol::CTriCol()
{
    m_ComponentName = L"TriCol";
}

CTriCol::CTriCol(DEVICE graphicDev)
    : CVIBuffer(graphicDev)
{
    m_ComponentName = L"TriCol";
}

CTriCol::CTriCol(const CTriCol& rhs)
    : CVIBuffer(rhs)
{
    m_ComponentName = L"TriCol";
}

CTriCol::~CTriCol()
{ }

HRESULT CTriCol::Ready_Buffer()
{
    m_VtxSize = sizeof(VTXCOL);
    m_VtxCnt  = 3;
    m_TriCnt  = 1;
    m_FVF     = FVF_COL;

    m_IdxSize = sizeof(INDEX32);
    m_IdxFmt  = D3DFMT_INDEX32;

    if (FAILED(CVIBuffer::Ready_Buffer()))
        return E_FAIL;

    VTXCOL* vertex = nullptr;

    // 3 매개 변수 : m_pVB에 보관된 정점들 중 첫 번째 정점의 주소를 얻어오는 것

    m_VB->Lock(0, 0, (void**)&vertex, 0);

    vertex[0].position = { 0.f, 1.f, 0.f };
    vertex[0].color    = D3DXCOLOR(0.f, 1.f, 0.f, 1.f);

    vertex[1].position = { 1.f, -1.f, 0.f };
    vertex[1].color    = D3DXCOLOR(0.f, 1.f, 0.f, 1.f);

    vertex[2].position = { -1.f, -1.f, 0.f };
    vertex[2].color    = D3DXCOLOR(0.f, 1.f, 0.f, 1.f);

    m_VB->Unlock();

    INDEX32* index = nullptr;

    m_IB->Lock(0, 0, (void**)&index, 0);

    index[0]._0 = 0;
    index[0]._1 = 1;
    index[0]._2 = 2;

    m_IB->Unlock();

    return S_OK;
}

void CTriCol::Render_Buffer()
{
    CVIBuffer::Render_Buffer();
}

CTriCol* CTriCol::Create(DEVICE graphicDev)
{
    auto triCol = new CTriCol(graphicDev);

    if (FAILED(triCol->Ready_Buffer()))
    {
        Safe_Release(triCol);
        MSG_BOX("TriCol Create Failed");
        return nullptr;
    }

    return triCol;
}

CComponent* CTriCol::Clone()
{
    return new CTriCol(*this);
}

void CTriCol::Free()
{
    CVIBuffer::Free();
}
