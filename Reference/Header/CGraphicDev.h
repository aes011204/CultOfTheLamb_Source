#pragma once
#include "CBase.h"
#include "Engine_Define.h"

BEGIN(Engine)
class ENGINE_DLL CGraphicDev : public CBase
{
    DECLARE_SINGLETON(CGraphicDev)

private:
    explicit CGraphicDev();
    ~CGraphicDev() override;

public:
    DEVICE Get_GraphicDev() { return m_GraphicDev; }

public:
    HRESULT Ready_GraphicDev(HWND          hWnd,
                             WINMODE       mode,
                             const _uint&  sizeX,
                             const _uint&  sizeY,
                             CGraphicDev** outGraphicDev);

    void Render_Begin(D3DXCOLOR color);
    void Render_End();

private:
    LPDIRECT3D9 m_SDK;           // 1. 그래픽 카드 성능 조사 객체
    DEVICE      m_GraphicDev;    // 2. 그리기 담당 Direct3D 디바이스 (COM 객체)

private:
    void Free() override;
};

END
