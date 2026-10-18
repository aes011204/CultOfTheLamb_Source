#pragma once

CBase::CBase()
    : m_RefCnt(0)
{ }

unsigned long CBase::AddRef() noexcept
{
    return ++m_RefCnt;
}

unsigned long CBase::Release() noexcept
{
    if (0 == m_RefCnt)
    {
        Free();
        delete this;

        return 0;
    }

    return m_RefCnt--;
}
