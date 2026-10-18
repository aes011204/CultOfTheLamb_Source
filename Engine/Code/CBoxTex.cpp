#include "CBoxTex.h"

CBoxTex::CBoxTex()
{}

CBoxTex::CBoxTex(DEVICE pGraphicDev)
    : CVIBuffer(pGraphicDev)
{}

CBoxTex::CBoxTex(const CBoxTex& rhs)
    : CVIBuffer(rhs)
{}

CBoxTex::~CBoxTex()
{}

HRESULT CBoxTex::Ready_Buffer()
{
    m_VtxSize = sizeof(VTXCUBE);
    m_VtxCnt  = 8;
    m_TriCnt  = 12;
    m_FVF     = FVF_CUBE;

    m_IdxSize = sizeof(INDEX32);
    m_IdxFmt  = D3DFMT_INDEX32;

    if (FAILED(CVIBuffer::Ready_Buffer()))
        return E_FAIL;

    VTXCUBE* pVertex = nullptr;

    m_VB->Lock(0, 0, (void**)&pVertex, 0);

    // 전면
    pVertex[0].position = { -0.5f, 0.5f, -0.5f };
    pVertex[0].texUV    = pVertex[0].position;

    pVertex[1].position = { 0.5f, 0.5f, -0.5f };
    pVertex[1].texUV    = pVertex[1].position;

    pVertex[2].position = { 0.5f, -0.5f, -0.5f };
    pVertex[2].texUV    = pVertex[2].position;

    pVertex[3].position = { -0.5f, -0.5f, -0.5f };
    pVertex[3].texUV    = pVertex[3].position;

    // 후면
    pVertex[4].position = { -0.5f, 0.5f, 0.5f };
    pVertex[4].texUV    = pVertex[4].position;

    pVertex[5].position = { 0.5f, 0.5f, 0.5f };
    pVertex[5].texUV    = pVertex[5].position;

    pVertex[6].position = { 0.5f, -0.5f, 0.5f };
    pVertex[6].texUV    = pVertex[6].position;

    pVertex[7].position = { -0.5f, -0.5f, 0.5f };
    pVertex[7].texUV    = pVertex[7].position;

    m_VB->Unlock();

    INDEX32* pIndex = nullptr;

    m_IB->Lock(0, 0, (void**)&pIndex, 0);

    // X+
    // 오른쪽 위
    pIndex[0]._0 = 1;
    pIndex[0]._1 = 5;
    pIndex[0]._2 = 6;

    // 왼쪽 아래
    pIndex[1]._0 = 1;
    pIndex[1]._1 = 6;
    pIndex[1]._2 = 2;

    // X-
    // 오른쪽 위
    pIndex[2]._0 = 4;
    pIndex[2]._1 = 0;
    pIndex[2]._2 = 3;

    // 왼쪽 아래
    pIndex[3]._0 = 4;
    pIndex[3]._1 = 3;
    pIndex[3]._2 = 7;

    // Y+
    // 오른쪽 위
    pIndex[4]._0 = 4;
    pIndex[4]._1 = 5;
    pIndex[4]._2 = 1;

    // 왼쪽 아래
    pIndex[5]._0 = 4;
    pIndex[5]._1 = 1;
    pIndex[5]._2 = 0;

    // Y-
    // 오른쪽 위
    pIndex[6]._0 = 3;
    pIndex[6]._1 = 2;
    pIndex[6]._2 = 6;

    // 왼쪽 아래
    pIndex[7]._0 = 3;
    pIndex[7]._1 = 6;
    pIndex[7]._2 = 7;

    // Z+
    // 오른쪽 위
    pIndex[8]._0 = 7;
    pIndex[8]._1 = 6;
    pIndex[8]._2 = 5;

    // 왼쪽 아래
    pIndex[9]._0 = 7;
    pIndex[9]._1 = 5;
    pIndex[9]._2 = 4;

    // Z-
    // 오른쪽 위
    pIndex[10]._0 = 0;
    pIndex[10]._1 = 1;
    pIndex[10]._2 = 2;

    // 왼쪽 아래
    pIndex[11]._0 = 0;
    pIndex[11]._1 = 2;
    pIndex[11]._2 = 3;

    m_IB->Unlock();

    return S_OK;
}

void CBoxTex::Render_Buffer()
{
    m_GraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

    CVIBuffer::Render_Buffer();

    m_GraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
}

CBoxTex* CBoxTex::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CBoxTex* boxCol = new CBoxTex(pGraphicDev);

    if (FAILED(boxCol->Ready_Buffer()))
    {
        Safe_Release(boxCol);
        MSG_BOX("BoxCol Create Failed");
        return nullptr;
    }

    return boxCol;
}

CComponent* CBoxTex::Clone()
{
    return new CBoxTex(*this);
}

void CBoxTex::Free()
{
    CVIBuffer::Free();
}
