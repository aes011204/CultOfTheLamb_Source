#ifndef Engine_Struct_h__
#define Engine_Struct_h__

namespace Engine
{
// 색상 정보를 포함하는 정점 구조체
typedef struct tagVertexColor
{
    _vec3  position;
    _ulong color;
} VTXCOL;

constexpr _ulong FVF_COL = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX0;

// 기본 UV 좌표 및 조명 계산용 법선을 가지는 텍스처 정점 구조체
typedef struct tagVertexTexture
{
    _vec3 position;
    _vec3 normal;
    _vec2 texUV;
} VTXTEX;

constexpr _ulong FVF_TEX = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;

// 큐브 맵 샘플링을 위한 3차원 텍스처 좌표를 가진 정점 구조체
typedef struct tagVertexCubeTexture
{
    _vec3 position;
    _vec3 texUV;
} VTXCUBE;

constexpr _ulong FVF_CUBE = D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE3(0);
// D3DFVF_TEXCOORDSIZE3(0) :
// 텍스처의 UV 좌표 값을 FLOAT형 3개로 표현하겠다는 매크로
// 괄호 안의 숫자 0의 의미는 본래 버텍스에 텍스쳐 UV값이 여러개가 올 수 있는데
// 그중 0번째 값을 지정하겠다는 의미

typedef struct tagIndex16
{
    _ushort _0;
    _ushort _1;
    _ushort _2;
} INDEX16;

typedef struct tagIndex32
{
    _ulong _0;
    _ulong _1;
    _ulong _2;
} INDEX32;

typedef struct tagSprite
{
    _uint  index;           // 스프라이트 인덱스
    _float duration;        // 프레임 전환 속도
} SPRITE;

typedef struct tagParticle
{
    std::wstring key;      // 애니메이션 키
    _uint   frames;        // 프레임 수
    _float  interval;      // 간격
    _vec3   scale;         // 크기
    _vec3   offset;        // 위치 오프셋

    tagParticle()
        : key(L""),
        frames(10),
        interval(0.05f),
        scale(_vec3(1.f, 1.f, 1.f)),
        offset(_vec3(0.f, 0.f, 0.f))
    {
    }

    // 파티클 정보가 있는지 체크
    bool IsValid() const { return !key.empty(); }
} PARTICLEDATA;

}



#endif // Engine_Struct_h__
