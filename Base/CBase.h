#pragma once

class __declspec(dllexport) CBase
{
protected:
    inline explicit CBase();
    virtual         ~CBase() = default;

public:
    // RefCnt의 의도하지 않은 증감을 막기 위해 암시적인 복사/이동 금지
    // 자식에서 명시적으로 복사 생성자를 만들어 쓰면서 RefCnt를 관리하면 문제 없음
    CBase(const CBase&)            = delete;	// 복사 생성자 삭제
    CBase& operator=(const CBase&) = delete;	// 복사 대입 연산자 삭제
    CBase(CBase&&)                 = delete;	// 이동 생성자 삭제
    CBase& operator=(CBase&&)      = delete;	// 이동 대입 연산자 삭제

public:
    inline unsigned long AddRef() noexcept;
    inline unsigned long Release() noexcept;

private:
    unsigned long m_RefCnt;

public:
    inline virtual void Free() = 0;
};

#include "CBase.inl"
