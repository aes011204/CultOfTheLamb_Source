#include "CRenderer.h"

#include "CGameObject.h"
#include "CTransform.h"

IMPLEMENT_SINGLETON(CRenderer)

CRenderer::CRenderer()
{}

CRenderer::~CRenderer()
{
    CRenderer::Free();
}

void CRenderer::Add_RenderGroup(RENDERID type, CGameObject* gameObject)
{
    if (type < RENDER_PRIORITY || type >= RENDER_END || nullptr == gameObject)
        return;

    m_RenderGroup[type].push_back(gameObject);
    gameObject->AddRef();
}

void CRenderer::Render_GameObject(DEVICE& graphicDev)
{
    Render_Priority_UI(graphicDev);

    SetFogOn(graphicDev);
    Render_Priority(graphicDev);
    Render_Priority_Alpha(graphicDev);
    Render_NonAlpha(graphicDev);
    Render_Trail(graphicDev);
    Render_Alpha(graphicDev);
    Render_Particle(graphicDev);
    SetFogOff(graphicDev);

    Render_UI(graphicDev);

    Clear_RenderGroup();
}

void CRenderer::Clear_RenderGroup()
{
    for (size_t i = 0; i < RENDER_END; ++i)
    {
        for_each(m_RenderGroup[i].begin(), m_RenderGroup[i].end(), CDeleteObj());
        m_RenderGroup[i].clear();
    }
}

void CRenderer::Render_Priority(DEVICE& graphicDev)
{
    for (auto& obj : m_RenderGroup[RENDER_PRIORITY])
    {
        obj->Render_GameObject();
    }
}

void CRenderer::Render_Priority_Alpha(DEVICE& graphicDev)
{
    // 알파 블렌딩 켜기
    graphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    graphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    graphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    for (auto& obj : m_RenderGroup[RENDER_PRIORITY_ALPHA])
    {
        obj->Render_GameObject();
    }

    // 알파 블렌딩 끄기
    graphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}

void CRenderer::Render_NonAlpha(DEVICE& graphicDev)
{
    for (auto& obj : m_RenderGroup[RENDER_NONALPHA])
    {
        obj->Render_GameObject();
    }
}

void CRenderer::Render_Alpha(DEVICE& graphicDev)
{
    // 알파 정렬
    _matrix matView, matCam;
    graphicDev->GetTransform(D3DTS_VIEW, &matView);
    D3DXMatrixInverse(&matCam, nullptr, &matView);

    _vec3 camPos = *(_vec3*)&matCam.m[3][0];

    m_RenderGroup[RENDER_ALPHA].sort([&camPos](CGameObject* pObj1, CGameObject* pObj2)
    {
        CTransform* trans1 = pObj1->Get_Transform();
        CTransform* trans2 = pObj2->Get_Transform();

        if (!trans1 || !trans2)
            return false;

        _vec3 pivot1 = trans1->Get_Pos() - camPos;
        _vec3 pivot2 = trans2->Get_Pos() - camPos;

        _float dist1 = D3DXVec3LengthSq(&pivot1);
        _float dist2 = D3DXVec3LengthSq(&pivot2);

        return dist1 > dist2;
    });

    graphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

    for (auto& obj : m_RenderGroup[RENDER_ALPHA])
    {
        obj->Render_GameObject();
    }

    graphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}

void CRenderer::Render_Particle(DEVICE& graphicDev)
{
    // 알파 블렌딩 켜기
    graphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    graphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    graphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    for (auto& obj : m_RenderGroup[RENDER_PARTICLE])
    {
        obj->Render_GameObject();
    }

    // 알파 블렌딩 끄기
    graphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}

void CRenderer::Render_UI(DEVICE& graphicDev)
{
    if (m_RenderGroup[RENDER_UI].empty())
        return;

    _matrix prevView;
    _matrix prevProj;

    graphicDev->GetTransform(D3DTS_VIEW, &prevView);        // 쓰던 뷰 행렬 임시 저장
    graphicDev->GetTransform(D3DTS_PROJECTION, &prevProj);  // 쓰던 투영 행렬 임시 저장

    _matrix uiView;
    _matrix uiProj;

    D3DXMatrixIdentity(&uiView);    // UI용 뷰 행렬은 항등 행렬로 설정

    // UI 투영 행렬은 직교 투영으로 설정
    D3DXMatrixOrthoLH(
        &uiProj,
        static_cast<_float>(WINCX),
        static_cast<_float>(WINCY),
        0.f,
        1.f);

    graphicDev->SetTransform(D3DTS_VIEW, &uiView);          // UI 뷰 행렬 사용
    graphicDev->SetTransform(D3DTS_PROJECTION, &uiProj);    // UI 투영 행렬 사용

    // 깊이 끄기
    graphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
    graphicDev->SetRenderState(D3DRS_ZENABLE, FALSE);

    // 알파 블렌딩
    graphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    graphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    graphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    for (auto& obj : m_RenderGroup[RENDER_UI])
    {
        obj->Render_GameObject();
    }

    // 상태 복구
    graphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);

    graphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    graphicDev->SetRenderState(D3DRS_ZENABLE, TRUE);

    graphicDev->SetTransform(D3DTS_VIEW, &prevView);
    graphicDev->SetTransform(D3DTS_PROJECTION, &prevProj);
    graphicDev->SetRenderState(D3DRS_ZENABLE, TRUE);
}

void CRenderer::Render_Priority_UI(DEVICE& graphicDev)
{
    if (m_RenderGroup[RENDER_PRIORITY_UI].empty())
        return;

    _matrix prevView;
    _matrix prevProj;

    graphicDev->GetTransform(D3DTS_VIEW, &prevView);        // 쓰던 뷰 행렬 임시 저장
    graphicDev->GetTransform(D3DTS_PROJECTION, &prevProj);  // 쓰던 투영 행렬 임시 저장

    _matrix uiView;
    _matrix uiProj;

    D3DXMatrixIdentity(&uiView);    // UI용 뷰 행렬은 항등 행렬로 설정

    // UI 투영 행렬은 직교 투영으로 설정
    D3DXMatrixOrthoLH(
        &uiProj,
        static_cast<_float>(WINCX),
        static_cast<_float>(WINCY),
        0.f,
        1.f);

    graphicDev->SetTransform(D3DTS_VIEW, &uiView);          // UI 뷰 행렬 사용
    graphicDev->SetTransform(D3DTS_PROJECTION, &uiProj);    // UI 투영 행렬 사용

    // 깊이 끄기
    graphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
    graphicDev->SetRenderState(D3DRS_ZENABLE, FALSE);

    // 알파 블렌딩
    graphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    graphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    graphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    for (auto& obj : m_RenderGroup[RENDER_PRIORITY_UI])
    {
        obj->Render_GameObject();
    }

    // 상태 복구
    graphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);

    graphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    graphicDev->SetRenderState(D3DRS_ZENABLE, TRUE);

    graphicDev->SetTransform(D3DTS_VIEW, &prevView);
    graphicDev->SetTransform(D3DTS_PROJECTION, &prevProj);
    graphicDev->SetRenderState(D3DRS_ZENABLE, TRUE);
}

void CRenderer::Render_Trail(DEVICE& graphicDev)
{
    if (m_RenderGroup[RENDER_TRAIL].empty())
        return;

    //기존 상태저장
    IDirect3DStateBlock9* pStateBlock = nullptr;
    if (SUCCEEDED(graphicDev->CreateStateBlock(D3DSBT_ALL, &pStateBlock)) && pStateBlock)
        pStateBlock->Capture();

    DWORD prevFVF;
    graphicDev->GetFVF(&prevFVF);
    //
    // 트레일용 렌더 셋팅
    graphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    graphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    graphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    graphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    //

    for (auto& obj : m_RenderGroup[RENDER_TRAIL])
    {
        obj->Render_Trail();
    }

    // 렌더 상태 원복
    if (pStateBlock)
    {
        pStateBlock->Apply();
        pStateBlock->Release();
    }
}

void CRenderer::SetFogOn(LPDIRECT3DDEVICE9 graphicDev)
{
    if (m_UseFog == false)
        return;
    // Fog 켜기
    graphicDev->SetRenderState(D3DRS_FOGENABLE, TRUE);

    // Linear Fog (가장 직관적)
    graphicDev->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_LINEAR);
    // 만약 테이블 포그 안 먹으면 아래로 교체
    // graphicDev->SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR);

    // 안개 색 (멀리 있는 색)
    graphicDev->SetRenderState(
        D3DRS_FOGCOLOR,
        D3DCOLOR_XRGB(255, 240, 211)   // 메인 메뉴용 노랑이 색임
    );

    // 안개 시작/끝 거리
    float fogStart = 5.f;   // 이 거리 전까진 원래색
    float fogEnd   = 25.f;   // 이 거리 이후엔 거의 안개색

    graphicDev->SetRenderState(D3DRS_FOGSTART, *(DWORD*)&fogStart);
    graphicDev->SetRenderState(D3DRS_FOGEND, *(DWORD*)&fogEnd);
}

void CRenderer::SetFogOff(LPDIRECT3DDEVICE9 graphicDev)
{
    if (m_UseFog == false)
        return;

    graphicDev->SetRenderState(D3DRS_FOGENABLE, FALSE);
    graphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
}

void CRenderer::Free()
{
    Clear_RenderGroup();
}
