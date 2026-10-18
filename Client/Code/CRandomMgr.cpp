#include "pch.h"
#include "CRandomMgr.h"

IMPLEMENT_SINGLETON(CRandomMgr)

CRandomMgr::CRandomMgr()
{
    std::random_device rd;
    m_gen.seed(rd());
}

CRandomMgr::~CRandomMgr()
{
}

void CRandomMgr::Free()
{
}
