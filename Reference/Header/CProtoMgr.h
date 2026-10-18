#pragma once
#include "CBase.h"
#include "Engine_Define.h"

BEGIN(Engine)
class CComponent;

class ENGINE_DLL CProtoMgr : public CBase
{
    DECLARE_SINGLETON(CProtoMgr)

private:
    CProtoMgr();
    ~CProtoMgr() override;

public:
    HRESULT     Ready_Prototype(COMPONENTTYPE componentType, CComponent* component);
    CComponent* Clone_Prototype(COMPONENTTYPE componentType);

private:
    CComponent* Find_Prototype(COMPONENTTYPE componentType);

private:
    map<COMPONENTTYPE, CComponent*> m_Prototypes;
    CRITICAL_SECTION m_CriticalSection;

private:
    void Free() override;
};

END
