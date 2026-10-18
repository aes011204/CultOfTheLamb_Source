#include "CGraphicDev.h"

IMPLEMENT_SINGLETON(CGraphicDev)

CGraphicDev::CGraphicDev()
    : m_SDK(nullptr), m_GraphicDev(nullptr)
{ }

CGraphicDev::~CGraphicDev()
{
    CGraphicDev::Free();
}

HRESULT CGraphicDev::Ready_GraphicDev(HWND          hWnd,
                                      WINMODE       mode,
                                      const _uint&  sizeX,
                                      const _uint&  sizeY,
                                      CGraphicDev** outGraphicDev)
{
    NULL_CHECK_RETURN_MSG(
        outGraphicDev,
        E_POINTER,
        L"Ready_GraphicDev failed: ppGraphicDev is null (invalid output pointer)")

    // 1. 장치 조사를 위한 Direct3D SDK 객체 생성
    LPDIRECT3D9 sdk = Direct3DCreate9(D3D_SDK_VERSION);
    if (nullptr == sdk)
        return E_FAIL;

    // 2. 장치 성능 조사 : 하드웨어가 지원하는 기능을 확인한다.
    D3DCAPS9 deviceCaps;
    ZeroMemory(&deviceCaps, sizeof(D3DCAPS9));

    if (FAILED(sdk->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &deviceCaps)))
    {
        Safe_Release(sdk);
        return E_FAIL;
    }

    _ulong flag(0);

    // vp = 정점 변환 + 조명(명암) 연산
    // 하드웨어가 T&L을 지원하면 GPU에서 처리하고, 아니면 CPU(소프트웨어)에서 처리한다.
    if (deviceCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
        flag |= D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED;
    else
        flag |= D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED;

    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory(&d3dpp, sizeof(D3DPRESENT_PARAMETERS));

    // 후면 버퍼 설정: 화면 크기 및 포맷 지정
    d3dpp.BackBufferWidth  = sizeX;
    d3dpp.BackBufferHeight = sizeY;
    d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
    d3dpp.BackBufferCount  = 1;

    // 멀티 샘플링 (= 안티 앨리어싱)
    // 계단 현상을 방지하는 기능. 외곽선을 부드럽게 만들어준다. 사용시 리소스 사용 증가.
    // 일단은 사용하지 않을 것이므로 NONE으로 설정
    d3dpp.MultiSampleType    = D3DMULTISAMPLE_NONE;
    d3dpp.MultiSampleQuality = 0;

    // D3DSWAPEFFECT_DISCARD : 스왑 체인. 최적의 방식을 선택하므로 이 설정을 권장한다.
    // D3DSWAPEFFECT_FLIP	 : 하나의 버퍼를 뒤집어가며 사용한다.
    // D3DSWAPEFFECT_COPY	 : 더블 버퍼링과 유사한 유형. 백버퍼 -> 프론트버퍼를 복사
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;

    d3dpp.hDeviceWindow = hWnd;
    d3dpp.Windowed      = mode; // 창 모드 또는 전체 화면 선택

    // 깊이 버퍼, 스텐실 버퍼 : 3D용 버퍼
    // 스텐실 버퍼 : 거울이나 수면 반사, 디테일한 그림자 표현에 쓰인다
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;

    d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
    d3dpp.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;

    DEVICE graphicDev = nullptr;

    // 3. 그리기 담당 객체 생성 : 앞서 준비한 파라미터로 Direct3D 디바이스를 만든다.
    if (FAILED(sdk->CreateDevice(
        D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, flag, &d3dpp, &graphicDev)))
    {
        Safe_Release(graphicDev);
        Safe_Release(sdk);

        return E_FAIL;
    }

    // 초기화에 성공한 경우 기존 멤버를 정리하고 새로 획득한 포인터를 보관한다.
    Safe_Release(m_GraphicDev);
    Safe_Release(m_SDK);
    m_SDK        = sdk;
    m_GraphicDev = graphicDev;

    *outGraphicDev = this;

    return S_OK;
}

// 후면 버퍼 동작 원리
// 1. 지우기
// 2. 다음화면에 그릴것 준비
void CGraphicDev::Render_Begin(D3DXCOLOR color)
{
    m_GraphicDev->Clear(0,          // 렉트의 개수
                        nullptr,    // 렉트의 주소
                        D3DCLEAR_TARGET | D3DCLEAR_STENCIL | D3DCLEAR_ZBUFFER,
                        color,      // 후면 버퍼 텍스처 색상
                        1.f,        // 깊이(z)버퍼의 초기화 값
                        0);         // 스텐실 버퍼 초기화 값

    m_GraphicDev->BeginScene();
}

void CGraphicDev::Render_End()
{
    m_GraphicDev->EndScene();
    m_GraphicDev->Present(nullptr, nullptr, nullptr, nullptr);

    // 1, 2 매개 변수 : 렉트의 주소
    // 3 매개 변수 : 핸들(기본 창)
    // 1, 2, 4 매개 변수 : D3DSWAPEFFECT_COPY일 때만 사용 가능
    // 화면 분할을 위해서는 GraphicDev를 여러 개 만들고, 핸들 값 또한 다르게 넣어줘야 한다
}

void CGraphicDev::Free()
{
    Safe_Release(m_GraphicDev);
    Safe_Release(m_SDK);
}
