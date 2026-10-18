#include "CProtoMgr.h"

#include "CComponent.h"

IMPLEMENT_SINGLETON(CProtoMgr)

CProtoMgr::CProtoMgr()
{
    InitializeCriticalSection(&m_CriticalSection);

}

CProtoMgr::~CProtoMgr()
{
    CProtoMgr::Free();
}

HRESULT CProtoMgr::Ready_Prototype(COMPONENTTYPE componentType, CComponent* component)
{
    EnterCriticalSection(&m_CriticalSection); // 동기화 시작

    CComponent* instance = Find_Prototype(componentType);

    if (nullptr != instance)
    {
        LeaveCriticalSection(&m_CriticalSection); // 잠금 해제
        return E_FAIL;
    }

    m_Prototypes.insert({ componentType, component });

    LeaveCriticalSection(&m_CriticalSection); // 동기화 완료

    return S_OK;
}

CComponent* CProtoMgr::Clone_Prototype(COMPONENTTYPE componentType)
{
    EnterCriticalSection(&m_CriticalSection);

    CComponent* component = Find_Prototype(componentType);

    if (nullptr == component)
    {
        LeaveCriticalSection(&m_CriticalSection);
        return nullptr;
    }

    CComponent* cloned = component->Clone();

    LeaveCriticalSection(&m_CriticalSection);

    return cloned;
}

CComponent* CProtoMgr::Find_Prototype(COMPONENTTYPE componentType)
{
    auto iter = m_Prototypes.find(componentType);

    if (iter == m_Prototypes.end())
        return nullptr;

    return iter->second;
}

void CProtoMgr::Free()
{
    for_each(m_Prototypes.begin(), m_Prototypes.end(), CDeleteMap());
    m_Prototypes.clear();

    DeleteCriticalSection(&m_CriticalSection); // 크리티컬 섹션 삭제
}
