#pragma once
#include "CBase.h"
#include "Engine_Define.h"
#include <random>

class CRandomMgr :
    public CBase
{

DECLARE_SINGLETON(CRandomMgr)
private:
    CRandomMgr();
    ~CRandomMgr();
private:
    std::mt19937 m_gen;
public:
    mt19937& Get_Gen() { return m_gen; }

    template<typename T>
    T Get_Int_Random(T min, T max)
    {
        uniform_int_distribution<T> dist((int)min, (int)max);
        return dist(m_gen);
    }

public:
    void Free();
};

