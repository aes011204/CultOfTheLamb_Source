#include "CRcTex.h"

CRcTex::CRcTex()
{ }

CRcTex::CRcTex(DEVICE graphicDev)
    : CVIBuffer(graphicDev), m_HalfWidth(1.f), m_HalfHeight(1.f), m_OffsetX(0), m_OffsetY(0), m_FlipX(false)
{
}

CRcTex::CRcTex(const CRcTex& rhs)
    : CVIBuffer(rhs), m_HalfWidth(1.f), m_HalfHeight(1.f), m_OffsetX(0), m_OffsetY(0), m_FlipX(false)
{
    Safe_Release(m_VB);
    Safe_Release(m_IB);

    m_HalfWidth = rhs.m_HalfWidth;
    m_HalfHeight = rhs.m_HalfHeight;

    if (FAILED(Ready_Buffer()))
    {
        MSG_BOX("RcTex Create Failed");
        return;
    }
}

CRcTex::~CRcTex()
{ }

HRESULT CRcTex::Ready_Buffer()
{
    m_VtxSize = sizeof(VTXTEX);
    m_VtxCnt  = 4;
    m_TriCnt  = 2;
    m_FVF     = FVF_TEX;

    m_IdxSize = sizeof(INDEX32);
    m_IdxFmt  = D3DFMT_INDEX32;

    if (FAILED(CVIBuffer::Ready_Buffer()))
        return E_FAIL;

    Apply_VertexSize();

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

void CRcTex::Render_Buffer()
{
    CVIBuffer::Render_Buffer();
}

void CRcTex::Set_UV(_float u0, _float v0, _float u1, _float v1)
{
    VTXTEX* vertex = nullptr;

    m_VB->Lock(0, 0, (void**)&vertex, 0);

    // 좌상단
    vertex[0].texUV.x = u0;
    vertex[0].texUV.y = v0;

    // 우상단
    vertex[1].texUV.x = u1;
    vertex[1].texUV.y = v0;

    // 우하단
    vertex[2].texUV.x = u1;
    vertex[2].texUV.y = v1;

    // 좌하단
    vertex[3].texUV.x = u0;
    vertex[3].texUV.y = v1;

    m_VB->Unlock();
}

void CRcTex::Set_Scale(_float width, _float height)
{
    m_HalfWidth = width;
    m_HalfHeight = height;

    Apply_VertexSize();
}

void CRcTex::Set_Offset(float x, float y)
{
    m_OffsetX = x;
    m_OffsetY = y;

    Apply_VertexSize();
}

void CRcTex::Set_FlipX(_bool flip)
{
    if (m_FlipX != flip)
    {
        m_FlipX = flip;
        Apply_VertexSize();
    }
}

void CRcTex::Apply_VertexSize()
{
    if (m_VB == nullptr)
        return;

    VTXTEX* vtxtex = nullptr;
    m_VB->Lock(0, 0, (void**)&vtxtex, 0);

    const float hw = m_HalfWidth;
    const float hh = m_HalfHeight;

    const float dx = -m_OffsetX;
    const float dy = m_OffsetY;

    _float u0 = m_FlipX ? 1.f : 0.f;
    _float u1 = m_FlipX ? 0.f : 1.f;

    // 좌상단
    vtxtex[0].position = { -hw + dx,  hh + dy, 0.f };
    vtxtex[0].normal = { 0.f, 0.f, -1.f };
    vtxtex[0].texUV = { u0, 0.f };

    // 우상단
    vtxtex[1].position = { hw + dx,  hh + dy, 0.f };
    vtxtex[1].normal = { 0.f, 0.f, -1.f };
    vtxtex[1].texUV = { u1, 0.f };

    // 우하단
    vtxtex[2].position = { hw + dx, -hh + dy, 0.f };
    vtxtex[2].normal = { 0.f, 0.f, -1.f };
    vtxtex[2].texUV = { u1, 1.f };

    // 좌하단
    vtxtex[3].position = { -hw + dx, -hh + dy, 0.f };
    vtxtex[3].normal = { 0.f, 0.f, -1.f };
    vtxtex[3].texUV = { u0, 1.f };

    m_VB->Unlock();
}

CRcTex* CRcTex::Create(DEVICE graphicDev)
{
    auto rcTex = new CRcTex(graphicDev);

    if (FAILED(rcTex->Ready_Buffer()))
    {
        Safe_Release(rcTex);
        MSG_BOX("RcTex Create Failed");
        return nullptr;
    }

    return rcTex;
}

CComponent* CRcTex::Clone()
{
    return new CRcTex(*this);
}

void CRcTex::Free()
{
    CVIBuffer::Free();
}
