#ifndef Engine_Function_h__
#define Engine_Function_h__

namespace Engine
{
// ========================================
// template Function
// ========================================

// 단일 객체를 안전하게 삭제
template <typename T>
void Safe_Delete(T& pointer)
{
    if (nullptr != pointer)
    {
        delete pointer;
        pointer = nullptr;
    }
}

// 배열로 할당된 객체를 안전하게 삭제
template <typename T>
void Safe_Delete_Array(T& pointer)
{
    if (nullptr != pointer)
    {
        delete[] pointer;
        pointer = nullptr;
    }
}

// COM 객체의 참조 카운트를 감소시키고 0이 되면 포인터를 NULL로 초기화
template <typename T>
unsigned long Safe_Release(T& instance)
{
    unsigned long refCnt = 0;

    if (nullptr != instance)
    {
        refCnt = instance->Release();

        if (0 == refCnt)
            instance = NULL;
    }

    return refCnt;
}

// ========================================
// Functor
// ========================================

// STL 컨테이너에서 문자열로 이루어진 태그를 찾기 위한 Functor
class CTag_Finder
{
public:
    explicit CTag_Finder(std::wstring tag)
        : m_TargetTag(std::move(tag))
    { }

    ~CTag_Finder(void)
    { }

public:
    template <typename T>
    _bool operator()(const T& pair)
    {
        return m_TargetTag == pair.first;
    }

private:
    std::wstring m_TargetTag;
};

// Release 함수를 제공하는 객체를 안전하게 삭제하기 위한 Functor
class CDeleteObj
{
public:
    explicit CDeleteObj(void)
    { }

    ~CDeleteObj(void)
    { }

public:
    template <typename T>
    void operator ()(T& instance)
    {
        _ulong refCnt = 0;

        refCnt = instance->Release();

        if (0 == refCnt)
            instance = nullptr;
    }
};

// 연관 컨테이너(map, unordered_map 등)의 second 값을 Release 하기 위한 Functor
class CDeleteMap
{
public:
    explicit CDeleteMap(void)
    { }

    ~CDeleteMap(void)
    { }

public:
    template <typename T>
    void operator ()(T& pair)
    {
        _ulong refCnt = 0;

        refCnt = pair.second->Release();

        if (0 == refCnt)
            pair.second = NULL;
    }
};
}

#endif // Engine_Function_h__
