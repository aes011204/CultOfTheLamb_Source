#pragma once
#include "CEffectBaseCom.h"
#include <deque>

namespace Engine
{
    class CTexture;
    class CTransform;
}

struct TRAILVERTEX
{
    float x, y, z;


    float u;
    float v;
};

struct TrailNode
{
    _vec3 pos;  // 월드 위치
    float life; // 현제 남은 수명
    float maxLife;//초기 최대 수명
    float width;
    _vec3 side;// 빌보드후 사이즈 저장
    float alpha; //알파값
    float offset; //알파값
};


class CTrailEff :
    public CEffectBaseCom
{
protected:
    explicit CTrailEff();
    explicit CTrailEff(DEVICE graphicDev);
    explicit CTrailEff(const CTrailEff& rhs);
    ~CTrailEff() override;

public:
    HRESULT Ready_Effect() override;
    void      Initialize_Component()override;
    _int      Update_Component(const _float& timeDelta)override;
    void      LateUpdate_Component()override;

    void Render_Effect(CTexture* textureCom, int Num);

    void Set_CamTransform(CTransform* camT) { m_CameraTransform = camT; }
public:
    static CTrailEff* Create(DEVICE graphicDev);
    void RessetRenderSate(DEVICE graphicDev);
    CComponent* Clone();

    void Bill();
    deque<TrailNode> m_Node;

    float m_NodeIntervalDist =0.01f; // 노드끼리의 간격
    float m_Time = 0.23f; //노드 유지시간
    float m_TrailWidth = .9f; // 노드 두께

    _vec3 m_LastPos{0.f,0.f,0.f}; // 마지막으로 찍힌 노드의 위치ㅏㅒ

    //오너 프레스폼은 있음

    CTransform* m_CameraTransform = nullptr; 
protected:
    void Free() override;
};

