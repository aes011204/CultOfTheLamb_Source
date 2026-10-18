#pragma once
#include "CBase.h"
#include "Engine_Define.h"

BEGIN(Engine)
class CLight;

class ENGINE_DLL CLightMgr : public CBase
{
    DECLARE_SINGLETON(CLightMgr)

private:
    explicit CLightMgr();
    ~CLightMgr() override;

public:
    HRESULT Ready_Light(DEVICE           graphicDev,
                        const D3DLIGHT9* lightInfo,
                        const _uint&     index);

    void Clear_Lights();

private:
    list<CLight*> m_LightList;  // 리스트를 통한 광원 관리

private:
    void Free() override;
};

END
