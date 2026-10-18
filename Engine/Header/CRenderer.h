#pragma once
#include "CBase.h"
#include "Engine_Define.h"

BEGIN(Engine)
class CGameObject;

class ENGINE_DLL CRenderer : public CBase
{
    DECLARE_SINGLETON(CRenderer)

private:
    explicit CRenderer();
    ~CRenderer() override;

public:
    void Add_RenderGroup(RENDERID type, CGameObject* gameObject);
    void Render_GameObject(DEVICE& graphicDev);
    void Clear_RenderGroup();

    void SetFog(bool useFog) { m_UseFog = useFog; }
 
private:
    void Render_Priority(DEVICE& graphicDev);
    void Render_Priority_Alpha(DEVICE& graphicDev);
    void Render_NonAlpha(DEVICE& graphicDev);
    void Render_Alpha(DEVICE& graphicDev);
    void Render_Particle(DEVICE& graphicDev);
    void Render_UI(DEVICE& graphicDev);
    // 추가 (백 ui , 트레일용)
    void Render_Priority_UI(DEVICE& graphicDev);
    void Render_Trail(DEVICE& graphicDev);


    void SetFogOn(LPDIRECT3DDEVICE9 graphicDev);

    void SetFogOff(LPDIRECT3DDEVICE9 graphicDev);

private:
    list<CGameObject*> m_RenderGroup[RENDER_END];

    bool m_UseFog = false;
private:
    void Free() override;
};

END
