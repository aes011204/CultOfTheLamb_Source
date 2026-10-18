#pragma once
#include "CComponent.h"

// 이펙트 베이스 컴포넌트 : 이펙트 생명주기/활성/기본 실행 인터페이스만 통일하기 위해
class CEffectBaseCom :
    public CComponent
{
protected:
    explicit CEffectBaseCom() {}
    explicit CEffectBaseCom(DEVICE graphicDev) :CComponent(graphicDev) {}
    explicit CEffectBaseCom(const CEffectBaseCom& rhs) : CComponent(rhs) {}
    ~CEffectBaseCom() {}

public:
    virtual HRESULT     Ready_Effect() { return S_OK; }

    virtual void      Initialize_Component() {}
    virtual _int      Update_Component(const _float& timeDelta) { return 0; }
    virtual void      LateUpdate_Component() {}

    virtual void        Render_Effect() {}
    // Render_Component는 “내가 직접 그리는 함수”가 아니라, 오너(Renderer)가 호출할 수 있는 공통 인터페이스
public:
    //CEffectBaseCom* Create(DEVICE graphicDev);
    virtual CComponent* Clone() PURE;// 필요 없는데 이건 순수 가상이라서 일단 만듬
    //이 클래스를 직접 만들일 없으니까 생성X
protected:
    void Free() { CComponent::Free(); }
};

