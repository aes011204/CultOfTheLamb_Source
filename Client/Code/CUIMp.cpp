#include "pch.h"
#include "CUIMp.h"

#include <CCombatStat.h>

#include "CCreateHelper.h"
#include "CRcTex.h"
#include "CRenderer.h"
#include "CTexture.h"
#include "CTransform.h"

int CUIMp::m_nextID = 0;

CUIMp::CUIMp(DEVICE pGraphicDev) : CUI(pGraphicDev)
{

    m_ID = ++m_nextID;
}

CUIMp::CUIMp(const CUIMp& rhs) : CUI(rhs)
{
    m_ID = ++m_nextID;
}

CUIMp::~CUIMp()
{
}

HRESULT CUIMp::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    m_TransformCom->Set_Pos(_vec3(-540.f, 260.f, 0.f));
    m_TransformCom->Set_Scale(_vec3(54.f, 54.f, 0.f));
    return S_OK;
}

_int CUIMp::Update_GameObject(const _float& timeDelta)
{
    _int exit = CUI::Update_GameObject(timeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

    return exit;
}

void CUIMp::LateUpdate_GameObject(const _float& timeDelta)
{
    CUI::LateUpdate_GameObject(timeDelta);
}

void CUIMp::Render_GameObject()
{
    // 조명안받게
    //m_GraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

    m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());
    m_TextureCom->Set_Texture(0);

    _vec3 vPos;
    m_TransformCom->Get_Info(INFO_POS, &vPos);

    //UI 좌표계와 스크린 좌표계가 다르므로 전환
    float sx = vPos.x + WINCX / 2.f;
    float sy = WINCY / 2.f - vPos.y;

    _vec3 vSize = m_TransformCom->Get_Scale();

    RECT rcFull;
    rcFull.left    = LONG(sx - vSize.x);
    rcFull.top     = LONG(sy - vSize.y);          // RcTex 기본 크기가 2이기 때문에 Transform Scale은 반 사이즈
    rcFull.right   = LONG(sx + vSize.x);          // 따라서 여기서  vSize *0.5 하면 실제 크기가 반으로 줄어듦
    rcFull.bottom  = LONG(sy + vSize.y);

    float currMp = m_CombatStatCom->Get_Mp();
    float maxMp = m_CombatStatCom->Get_MaxMp();

    float ratio = currMp / maxMp;                //현재 MP의 비율

    int fullHeight = rcFull.bottom - rcFull.top; //MP가 가득 찼을 때의 높이
    int fillHeight = int(fullHeight * ratio);    //높이는 MP에 비례

    rcFull.top = rcFull.bottom - fillHeight;     //MP가 늘어날수록 화면에 그려지는 rcFull의 top은 점점 작아진다

    m_GraphicDev->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);
    m_GraphicDev->SetScissorRect(&rcFull);      //ScissorRect 안에 들어오는 픽셀만 그리고 나머지는 자르겠다는 뜻 

    m_BufferCom->Render_Buffer();

    m_GraphicDev->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
    //m_GraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);

}

HRESULT CUIMp::Add_Component()
{

    // buffer
    m_BufferCom = CreateProtoComponent<CRcTex>(this, COMPONENTTYPE::RC_TEX);
    NULL_CHECK_RETURN(m_BufferCom, E_FAIL);

    m_Components[ID_STATIC].insert({ COMPONENTTYPE::RC_TEX, m_BufferCom });

    // transform
    m_TransformCom = CreateProtoComponent<CTransform>(this, COMPONENTTYPE::TRANSFORM);
    NULL_CHECK_RETURN(m_TransformCom, E_FAIL);

    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::TRANSFORM, m_TransformCom });

    // texture
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_UI_COLOR);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);

    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_UI_COLOR, m_TextureCom });

    return S_OK;
}

CUIMp* CUIMp::Create(DEVICE graphicDev)
{
    auto circle = new CUIMp(graphicDev);
    if (FAILED(circle->Ready_GameObject()))
    {
        Safe_Release(circle);
        MSG_BOX("UIXp Created Failed");
        return nullptr;
    }
    return circle;
}

void CUIMp::Free()
{

    CUI::Free();
}
