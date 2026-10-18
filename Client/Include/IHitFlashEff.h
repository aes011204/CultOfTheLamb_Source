#pragma once
#include "Engine_Define.h"
class IHitFlashEff
{
public:
    virtual ~IHitFlashEff() = default;
    virtual void FlashRenderStart(DWORD color = D3DCOLOR_ARGB(255, 255, 255, 255)) = 0;
    virtual void FlashRenderEnd() = 0;
    virtual void AlphaRenderStart(float alpha = 0.8f) = 0;
    virtual void AlphaRenderEnd() = 0;
};
