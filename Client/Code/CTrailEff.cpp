#include "pch.h"
#include "CTrailEff.h"
#include "CTransform.h"
#include "CTexture.h"
#include "CManagement.h"
#include "CScene.h"
#include "CMainCamera.h"
#include "CRandomMgr.h"

CTrailEff::CTrailEff()
{}

CTrailEff::CTrailEff(DEVICE graphicDev)
    : CEffectBaseCom(graphicDev)
{}

CTrailEff::CTrailEff(const CTrailEff& rhs)
    : CEffectBaseCom(rhs)
{}

CTrailEff::~CTrailEff()
{}

HRESULT CTrailEff::Ready_Effect()
{
    return S_OK;
}

void CTrailEff::Initialize_Component()
{
    m_CameraTransform = dynamic_cast<CTransform*>(CManagement::GetInstance()->Get_Scene()->Get_Component(ID_DYNAMIC,LAYERTYPE::GAMELOGIC, OBJTYPE::CAMERA,COMPONENTTYPE::TRANSFORM));
}

_int CTrailEff::Update_Component(const _float& timeDelta)
{
    if (m_TransformCom == nullptr || m_CameraTransform == nullptr)
        return 0; // 혹은 -1 (오브젝트 삭제 요청)



    _vec3 curPos = m_TransformCom->Get_Pos();

    _vec3 tmp  = curPos - m_LastPos;
    float dist = D3DXVec3Length(&tmp);

    if (m_Node.empty() || dist >= m_NodeIntervalDist)
    {
        TrailNode node;
        node.pos     = curPos;
        node.life    = 0.f;
        node.maxLife = m_Time;
        node.width = m_TrailWidth;
        node.alpha = 1.f;// 이건 임시 밑에서 실제값넣을거임

        float tmpOffset = (CRandomMgr::GetInstance()->
            Get_Int_Random(int(-(node.width * 0.09f * 100)), int(node.width * 0.09f * 100))) / 100.f;

        node.offset = tmpOffset;
   
        //// 빌보드???

       
        ///
        m_Node.push_back(node);
        m_LastPos = curPos;
    }

    

    for (auto it = m_Node.begin();it < m_Node.end();)
    {
        it->life += timeDelta;

        it->width = m_TrailWidth * (1.0f - it->life / (it->maxLife *2.f));
        it->alpha = (.7f - (it->life/ it->maxLife * .7f));

        if (it->life >= it->maxLife)
            it = m_Node.erase(it);
        else
            ++it;

        
    }

    return 0;
}

void CTrailEff::Bill()
{
    //_vec3 forward = curPos - m_LastPos
}

void CTrailEff::LateUpdate_Component()
{}

void CTrailEff::Render_Effect(CTexture* textureCom, int Num)
{

    if (!m_GraphicDev || !textureCom || !m_CameraTransform)
        return;

    if (m_Node.size() < 2)
    {
        return;
    }

    //렌더러에서 해줌
    //IDirect3DStateBlock9* pStateBlock = nullptr;
    //if (SUCCEEDED(m_GraphicDev->CreateStateBlock(D3DSBT_ALL, &pStateBlock)) && pStateBlock)
    //    pStateBlock->Capture();

    //DWORD prevFVF;
    //m_GraphicDev->GetFVF(&prevFVF);




    D3DXMATRIX matWorld;
    D3DXMatrixIdentity(&matWorld);
    m_GraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

    // 렌더러에서 해줌
    //m_GraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    //m_GraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    //m_GraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    //m_GraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);


    m_GraphicDev->SetFVF(D3DFVF_XYZ | D3DFVF_TEX1);
    textureCom->Set_Texture(Num);

    for (int i = 0; i < m_Node.size() - 1; i++)
    {
        {
            //알파
            m_GraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB((int)(m_Node[i].alpha * 255), 255, 255, 255));
            //
            m_GraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
            m_GraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
            m_GraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR); // ← texturefactor.alpha 사용

        }


        TrailNode& A = m_Node[i]; //prev
        TrailNode& B = m_Node[i + 1]; //cur


        _vec3 side = {};
        {
            // 1. 진행 방향 (Forward) 계산
            _vec3 forward = B.pos - A.pos;
            float fLength = D3DXVec3Length(&forward);
            if (fLength < 0.0001f)
            {
                continue;
            }
            D3DXVec3Normalize(&forward, &forward);

            // 2. 안정적인 Up 벡터 (월드 Up 사용)
            //_vec3 Up = { 0.f, 1.f, 0.f };

            _vec3  ToCam = (m_CameraTransform->Get_Pos() - B.pos);
            D3DXVec3Normalize(&ToCam, &ToCam);


            // 3. 트레일 폭을 결정할 수직 벡터 (Side) 계산
            D3DXVec3Cross(&side, &forward, &ToCam);
            D3DXVec3Normalize(&side, &side);

            // 4. 버텍스 위치 계산
        }

        _vec3 offfsetA =  A.pos + side * A.offset;
        _vec3 offfsetB = B.pos + side * B.offset;

        _vec3 A_Left = offfsetA + side * (A.width * 0.5f);
        _vec3 A_Right = offfsetA - side * (A.width * 0.5f);

        _vec3 B_Left = offfsetB + side * (B.width * 0.5f);
        _vec3 B_Right = offfsetB - side * (B.width * 0.5f);

        TRAILVERTEX v[4];
        v[0] = { A_Left.x,  A_Left.y,  A_Left.z, 0.f, 0.f };
        v[1] = { A_Right.x, A_Right.y, A_Right.z, 1.f, 0.f };
        v[2] = { B_Left.x,  B_Left.y,  B_Left.z, 0.f, 1.f };
        v[3] = { B_Right.x, B_Right.y, B_Right.z, 1.f, 1.f };

        m_GraphicDev->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, v, sizeof(TRAILVERTEX));



    }



    ///렌더러에서 해줌
  /*  if (pStateBlock)
    {
        pStateBlock->Apply();
        pStateBlock->Release();
    }*/


}

CTrailEff* CTrailEff::Create(DEVICE graphicDev)
{
    auto TrailEff = new CTrailEff(graphicDev);

    if (FAILED(TrailEff->Ready_Effect()))
    {
        MSG_BOX("TrailEff Create Failed");
        Safe_Release(TrailEff);
        return nullptr;
    }

    return TrailEff;
}

void CTrailEff::RessetRenderSate(DEVICE graphicDev)
{
    m_GraphicDev->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1);

    // 텍스처 스테이지 리셋
    m_GraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

    m_GraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);

    // 알파 블렌딩 OFF
    m_GraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    
    // 컬링 복구
    m_GraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

    m_GraphicDev->SetVertexShader(nullptr);
    m_GraphicDev->SetPixelShader(nullptr);


}
CComponent* CTrailEff::Clone()
{
    return new CTrailEff(*this);
}

void CTrailEff::Free()
{
   

    CComponent::Free();
}
