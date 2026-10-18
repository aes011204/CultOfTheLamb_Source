#include "pch.h"
#include "CUICookingGame.h"
#include "CCombatStat.h"
#include "CCreateHelper.h"
#include "CTexture.h"
#include "CTransform.h"
#include "CRenderer.h"
#include "CDInputMgr.h"
#include "CRcTex.h"
#include "CUIIcon.h"
#include "CItemDB.h"
#include "CFontMgr.h"
#include "CRandomMgr.h"
#include "CSoundMgr.h"


CUICookingGame::CUICookingGame(DEVICE graphicDev, CGameObject* owner, RecipeInst recipeInst)
    : CUI(graphicDev), m_Owner(owner), m_recipeInst(recipeInst)
{
}

CUICookingGame::~CUICookingGame()
{
}

HRESULT CUICookingGame::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;



    // m_TransformCom->Set_RotationAxis(ROT_X, D3DXToRadian(45.f));
    // m_TransformCom->Set_Pos(m_Owner->Get_Transform()->Get_Pos().x, m_Owner->Get_Transform()->Get_Pos().y+2, m_Owner->Get_Transform()->Get_Pos().z);

    if (m_TransformCom)
    {
        m_TransformCom->Set_Pos(_vec3(0.f, 150.f, .5f));  // 화면 하단
        m_TransformCom->Set_Scale(_vec3(1.f, 1.f, 1.f));
    }

    return S_OK;
}

void CUICookingGame::Initialize_GameObject()
{
    CUI::Initialize_GameObject();

    if (m_Owner)
    {
        m_OwnerTransform = dynamic_cast<CTransform*>(
            m_Owner->Get_Component(COMPONENTID::ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));
    }

    //m_TransformCom->Set_Pos({ m_OwnerTransform->Get_Pos().x ,  m_OwnerTransform->Get_Pos().y + 3.f,  m_OwnerTransform->Get_Pos().z +2.f});

    {
        // 테스트
        const float BG_WIDTH = 45.f * 3.5f;
        const float BG_HEIGHT = 6.f * 3.5f;

        m_BaseRight = m_TransformCom->Get_Pos().x + (BG_WIDTH );
        m_BaseLeft = m_TransformCom->Get_Pos().x - (BG_WIDTH );

        m_BufferCom->Set_Scale(BG_WIDTH, BG_HEIGHT);
        m_BufferCom->Set_Offset(0.f, 0.f);
}
   // 결과물 이미지
    m_ItemIcon = CUIIcon::Create(m_GraphicDev, COMPONENTTYPE::TEX_ITEM);


    // 움직이는 화살표
    m_Curser = CUIIcon::Create(m_GraphicDev, COMPONENTTYPE::TEX_UI_COOK);
    if (m_ItemIcon)
    {
        //SpawnUIOnLayer(m_ItemIcon, OBJTYPE::UI);
        m_Curser->Set_TextureIndex(1);
        m_Curser->SetCloneTexAspectScale({ .01f, .2f, 1 });
    }

//영역 위치 랜덤으로
    m_StencilLength = CRandomMgr::GetInstance()->Get_Int_Random(80, 140);
    m_StencilPosX = (CRandomMgr::GetInstance()->Get_Int_Random((int)(m_BaseLeft + m_StencilLength*.5f),(int) (m_BaseRight - m_StencilLength*.5f)));
    m_StencilLeft = m_StencilPosX - m_StencilLength * .5f;
    m_StencilRight = m_StencilPosX + m_StencilLength * .5f;
    // 기본 버퍼 크기 생각 해야할듯
}

_int CUICookingGame::Update_GameObject(const _float& timeDelta)
{
    _int exit = CUI::Update_GameObject(timeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

    m_Curser->Update_GameObject(timeDelta);
    m_ItemIcon->Update_GameObject(timeDelta);

    if (CDInputMgr::GetInstance()->KeyDown(DIK_P) ||     //?? P로 한 건 실수?
        CDInputMgr::GetInstance()->MouseDown(DIM_LB))  
    {
        //커서포즈 멈춤
        OnClick = true;
    }

    if (OnClick == false)
    {
         m_CurserX += timeDelta * m_Speed;
        //커서포즈 계속 왓다 갓다
        if (m_CurserX <= m_BaseLeft || m_CurserX >= m_BaseRight)
        {
            m_Speed *= -1;
        }
        cout << "m_CurserX" << m_CurserX << endl;
    }

    if (OnClick == true)
    {
        DelayTime += timeDelta;
        //일단 부호 반대로 transform 이랑 retex 좌표계안맞는득
        if (m_CurserX >= -m_StencilRight && m_CurserX <= -m_StencilLeft)
        {
            m_result = GOOD; // 안탄거
            cout << "curser" << "true" << endl;
        }
        else
        {
            m_result = BAD; // 탄거
            cout << "curser" << "false" << endl;

        }


        if (DelayTime > 1.f)
        {
            cout << "m_StencilLeft" << m_StencilLeft << endl;
            cout << "m_StencilRight" << m_StencilRight << endl;
            cout << "m_StencilLength" << m_StencilLength << endl;
            cout << "m_StencilPosX" << m_StencilPosX << endl;


           
            //위치가 에리어 안이라면
           
           m_DoneEvent();
            m_ResultItemDrop(m_recipeInst.ResultItemID, m_result);
			CSoundMgr::GetInstance()->Play_Once(L"Cook/Meal_Cooked.wav", SOUNDTYPE::SOUND_UI, 1.f);
            Mark_As_PendingKill();
        }

    }


    if (!m_Owner)
        return exit;

    return exit;
}

void CUICookingGame::LateUpdate_GameObject(const _float& timeDelta)
{
    CUI::LateUpdate_GameObject(timeDelta);
    m_Curser->Get_Transform()->Set_Pos({ m_CurserX , m_TransformCom->Get_Pos().y, 1.f });
    m_Curser->LateUpdate_GameObject(timeDelta);
    m_ItemIcon->LateUpdate_GameObject(timeDelta);
}

void CUICookingGame::Render_GameObject()
{
    CUI::Render_GameObject();

    //m_GraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    //m_GraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    //m_GraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    //m_GraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    //m_GraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    //m_GraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
    //m_GraphicDev->SetRenderState(D3DRS_ALPHAREF, 0);


//빨간색 
    const _matrix& world = m_TransformCom->Get_World();

    {
        //const float BG_WIDTH = 45.f * 3.5f;
        //const float BG_HEIGHT = 6.f * 3.5f;

        //m_BaseRight = m_TransformCom->Get_Pos().x + (BG_WIDTH /** .5f*/);
        //m_BaseLeft = m_TransformCom->Get_Pos().x - (BG_WIDTH /** .5f*/);

        //m_BufferCom->Set_Scale(BG_WIDTH, BG_HEIGHT);
        //m_BufferCom->Set_Offset(0.f, 0.f);

        m_GraphicDev->SetTransform(D3DTS_WORLD, &world);
        m_TextureCom->Set_Texture(4);  // 레드
        m_BufferCom->Render_Buffer();
    }

    //메인 바 데코 

    {
        const float BG_WIDTH = 62.f * 3.5f;
        const float BG_HEIGHT = 16.f * 3.5f;

        m_Buffer_Deco->Set_Scale(BG_WIDTH, BG_HEIGHT);
        m_Buffer_Deco->Set_Offset(0.f, 5.f);

        m_GraphicDev->SetTransform(D3DTS_WORLD, &world);
        m_TextureCom->Set_Texture(2);  // 데코
        m_Buffer_Deco->Render_Buffer();
    }


    //배경 


    {
        const float BG_WIDTH = 110.f;
        const float BG_HEIGHT = 35.f;
        const float offX = 0.f;
        const float offY = 110.f;

        _vec2 partPos{ m_TransformCom->Get_Pos().x + offX,m_TransformCom->Get_Pos().y + offY };

        m_Buffer_Info->Set_Scale(BG_WIDTH, BG_HEIGHT);
        m_Buffer_Info->Set_Offset(offX, offY);

        m_GraphicDev->SetTransform(D3DTS_WORLD, &world);
        m_TextureCom->Set_Texture(5);  // 위 검은 텍스처
        m_Buffer_Info->Render_Buffer();

        // 배경 안 음식 아이콘

        m_ItemIcon->Set_TextureIndex(CItemDB::GetInstance()->GetIndexById(m_recipeInst.ResultItemID));
        m_ItemIcon->SetCloneTexAspectScale({ .2f, .2f, 1 });
        m_ItemIcon->Get_Transform()->Set_Pos({ partPos.x - 50.f, partPos.y + 5.f, 0.f });

        _vec3 center = { partPos.x + 30.f, partPos.y, 0.f };

        _vec2 rectLT{ center.x + WINCX * .5f, WINCY * 0.5f - center.y };

        wchar_t buffer[32];
        swprintf_s(buffer, L"%d/%d", m_recipeInst.StockNum, m_StockMax);

        CFontMgr::GetInstance()->Render_Font(L"NanumSquare_Big",
            buffer,
            &rectLT,
            D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
    }


    //배경 

    {
        const float BG_WIDTH = 90.f;
        const float BG_HEIGHT = 20.f;
        const float offX = 0.f;
        const float offY = -55.f;

        _vec2 partPos{ m_TransformCom->Get_Pos().x + offX,m_TransformCom->Get_Pos().y + offY };


        m_Buffer_keyInfo->Set_Scale(BG_WIDTH, BG_HEIGHT);
        m_Buffer_keyInfo->Set_Offset(offX, offY);

        m_GraphicDev->SetTransform(D3DTS_WORLD, &world);
        m_TextureCom->Set_Texture(5);  // 아래 키 텍스처
        m_Buffer_keyInfo->Render_Buffer();

        _vec3 center = { partPos.x , partPos.y,0.f };
        _vec2 rectLT{ center.x + WINCX * .5f, WINCY * 0.5f - center.y };

        CFontMgr::GetInstance()->Render_Font(L"NanumSquare_Regular",
            L"\"A\" 요리",
            &rectLT,
            D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
    }


    Stencil();


    //// 화살표 렌더 젤 마지막
m_Curser->Render_GameObject();
    ////


    //m_GraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    //m_GraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    //m_GraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CUICookingGame::Stencil()
{
    // 스텐실 초기화
    m_GraphicDev->SetRenderState(D3DRS_STENCILENABLE, TRUE);
    m_GraphicDev->Clear(0, NULL, D3DCLEAR_STENCIL, 0, 0, 0);
    m_GraphicDev->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
    m_GraphicDev->SetRenderState(D3DRS_STENCILREF,1);
    m_GraphicDev->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_REPLACE);
    m_GraphicDev->SetRenderState(D3DRS_COLORWRITEENABLE, 0); // 색상출력 끄기

    // 마스크 랜더
    //float localOffsetX = m_StencilPosX - m_TransformCom->Get_Pos().x;
    //m_MaskBuffer->Set_Offset(localOffsetX, 0);
    m_MaskBuffer->Set_Scale((int)m_StencilLength*.5f, 100);//기본 rctex가 2x2라서 length의 두배일거임
 m_MaskBuffer->Set_Offset(m_StencilPosX , 0);
    m_MaskBuffer->Render_Buffer();

    //
    //스텐실 안에서만 프린트 되야하는 이미지 출력
    // 준비
    m_GraphicDev->SetRenderState(D3DRS_COLORWRITEENABLE,
        D3DCOLORWRITEENABLE_RED |
        D3DCOLORWRITEENABLE_GREEN |
        D3DCOLORWRITEENABLE_BLUE |
        D3DCOLORWRITEENABLE_ALPHA
    );// 색상출력 켜기

    m_GraphicDev->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
    m_GraphicDev->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);
    m_GraphicDev->SetRenderState(D3DRS_STENCILREF, 1);

    // 버퍼 출력
    m_Buffer_blue->Set_Scale(45.f * 3.5f, 6.f * 3.5f);
    m_Buffer_blue->Set_Offset(0,0);
    m_TextureCom->Set_Texture(3);
    m_Buffer_blue->Render_Buffer();

    // 스텐실 종료
    m_GraphicDev->SetRenderState(D3DRS_STENCILENABLE, FALSE);
}

HRESULT CUICookingGame::Add_Component()
{
    // Texture
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_UI_COOK);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_UI_COOK, m_TextureCom });

    // Transform
    m_TransformCom = CreateProtoComponent<CTransform>(this, COMPONENTTYPE::TRANSFORM);
    NULL_CHECK_RETURN(m_TransformCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::TRANSFORM, m_TransformCom });

    m_BufferCom = CreateProtoComponent<CRcTex>(this, COMPONENTTYPE::RC_TEX);
    NULL_CHECK_RETURN(m_BufferCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::RC_TEX, m_BufferCom });

    m_Buffer_Deco = CreateProtoComponent<CRcTex>(this, COMPONENTTYPE::RC_TEX);
    NULL_CHECK_RETURN(m_Buffer_Deco, E_FAIL);
    //m_Components[ID_STATIC].insert({ COMPONENTTYPE::RC_TEX, m_Buffer_Deco });


    m_Buffer_keyInfo = CreateProtoComponent<CRcTex>(this, COMPONENTTYPE::RC_TEX);
    NULL_CHECK_RETURN(m_Buffer_keyInfo, E_FAIL);
    // m_Components[ID_STATIC].insert({ COMPONENTTYPE::RC_TEX, m_Buffer_keyInfo });

    m_Buffer_Info = CreateProtoComponent<CRcTex>(this, COMPONENTTYPE::RC_TEX);
    NULL_CHECK_RETURN(m_Buffer_Info, E_FAIL);
    //m_Components[ID_STATIC].insert({ COMPONENTTYPE::RC_TEX, m_Buffer_Info });

    m_Buffer_blue = CreateProtoComponent<CRcTex>(this, COMPONENTTYPE::RC_TEX);
    NULL_CHECK_RETURN(m_Buffer_blue, E_FAIL);
    //m_Components[ID_STATIC].insert({ COMPONENTTYPE::RC_TEX, m_Buffer_Info });

    m_MaskBuffer = CreateProtoComponent<CRcTex>(this, COMPONENTTYPE::RC_TEX);
    NULL_CHECK_RETURN(m_MaskBuffer, E_FAIL);
    //m_Components[ID_STATIC].insert({ COMPONENTTYPE::RC_TEX, m_Buffer_Info });

    return S_OK;
}

CUICookingGame* CUICookingGame::Create(DEVICE graphicDev, CGameObject* owner, RecipeInst recipeInst)
{
    CUICookingGame* CookingGame = new CUICookingGame(graphicDev, owner, recipeInst);

    if (FAILED(CookingGame->Ready_GameObject()))
    {
        Safe_Release(CookingGame);
        MSG_BOX("CookingGame Create Failed");
        return nullptr;
    }

    return CookingGame;
}

void CUICookingGame::Free()
{

    Safe_Release(m_Buffer_Deco);
    Safe_Release(m_Buffer_keyInfo);
    Safe_Release(m_Buffer_blue);
    Safe_Release(m_MaskBuffer);
    Safe_Release(m_Buffer_Info);

    Safe_Release(m_ItemIcon);
    Safe_Release(m_Curser);

    CUI::Free();
}
