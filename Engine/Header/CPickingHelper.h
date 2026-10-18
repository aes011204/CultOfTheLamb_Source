#pragma once
#include "Engine_Define.h"

// 마우스 좌표 월드 변환 헬퍼

BEGIN(Engine)

// 현재 마우스에서 나가는 Ray (월드 기준임)
static bool GetMouseRay(DEVICE graphicDev, HWND hWnd, _vec3& outOrigin, _vec3& outDir)
{
    if (graphicDev == nullptr)
        return false;

    POINT pt;
    GetCursorPos(&pt);
    ScreenToClient(hWnd, &pt);

    RECT rc;
    GetClientRect(hWnd, &rc);

    D3DVIEWPORT9 viewport;
    graphicDev->GetViewport(&viewport);

    if (rc.right > 0 && rc.bottom > 0)
    {
        pt.x = static_cast<LONG>(pt.x * (viewport.Width / static_cast<float>(rc.right)));
        pt.y = static_cast<LONG>(pt.y * (viewport.Height / static_cast<float>(rc.bottom)));
    }

    _matrix matView, matProj, matWorld;
    graphicDev->GetTransform(D3DTS_VIEW, &matView);
    graphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
    D3DXMatrixIdentity(&matWorld);

    // near 매크로 때문에 변수명 변경
    _vec3 vNear = _vec3(static_cast<float>(pt.x), static_cast<float>(pt.y), 0.f);
    _vec3 vFar = _vec3(static_cast<float>(pt.x), static_cast<float>(pt.y), 1.f);

    D3DXVec3Unproject(&vNear, &vNear, &viewport, &matProj, &matView, &matWorld);
    D3DXVec3Unproject(&vFar, &vFar, &viewport, &matProj, &matView, &matWorld);

    _vec3 dir = vFar - vNear;
    D3DXVec3Normalize(&dir, &dir);

    outOrigin = vNear;
    outDir = dir;

    return true;
}

// y = planeY 평면과 교차점 (월드 좌표)
static bool PickPlaneY(DEVICE graphicDev, HWND hWnd, float planeY, _vec3& outHitWorld)
{
    _vec3 origin, dir;

    if (!GetMouseRay(graphicDev, hWnd, origin, dir))
        return false;

    if (fabsf(dir.y) < 1e-6f)
        return false;

    float time = (planeY - origin.y) / dir.y;

    // 뒷면은 무시하기
    if (time < 0.f)
        return false;

    outHitWorld = origin + dir * time;

    return true;
}

END
