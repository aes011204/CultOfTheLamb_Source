#include "pch.h"
#include "CUICookingCell.h"
#include "CCreateHelper.h"
#include "CRcTex.h"
#include "CTransform.h"
#include "CTexture.h"
#include "CUIIcon.h"
#include "CDInputMgr.h"
#include "CItemDB.h"
#include "CRecipeDB.h"
#include "CFontMgr.h"
#include "CUICookingMenu.h"
#include "CSoundMgr.h"

CUICookingCell::CUICookingCell(DEVICE graphicDev)
    : CUI(graphicDev),
    m_CookingMenu(nullptr),
    m_ItemIcon(nullptr),
    m_CardIcon(nullptr),
    m_CardItemIcon(nullptr),
    m_Highlight(nullptr),
    m_TextureIndex(0),
    m_RecipeId(-1),
    m_IsHovered(false),
    m_IsActive(nullptr)
{ }

CUICookingCell::CUICookingCell(const CUICookingCell& rhs)
    : CUI(rhs),
      m_CookingMenu(nullptr),
      m_ItemIcon(nullptr),
      m_CardIcon(nullptr),
      m_CardItemIcon(nullptr),
      m_Highlight(nullptr),
      m_TextureIndex(0),
      m_RecipeId(-1),
      m_IsHovered(false),
      m_IsActive(nullptr)
{}

CUICookingCell::~CUICookingCell() {}

HRESULT CUICookingCell::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    //셀 크기 지정
    m_TransformCom->Set_Scale(45.f, 45.f, 0.f);
    m_BaseScale = m_TransformCom->Get_Scale();
    m_TargetScale = m_BaseScale;

    //셀 아이템 그리기용 객체 생성
    m_ItemIcon = CUIIcon::Create(m_GraphicDev, COMPONENTTYPE::TEX_ITEM); //아이템 텍스처
    m_ItemIcon->SetCloneTexAspectScale({ .2f, .2f, 1 });
    m_ItemIcon->SetRegister(false); // 아이템 텍스처 렌더 그룹에 중복 등록되는 것 막기 위한 플래그
    m_ItemBaseScale=m_ItemIcon->Get_Transform()->Get_Scale();
    m_ItemTargetScale = m_ItemBaseScale;

    //설명 카드 그리기용 객체 생성
    m_CardIcon = CUIIcon::Create(m_GraphicDev, COMPONENTTYPE::TEX_UI_RITUALCARD);
    m_CardIcon->SetRegister(false);
    m_CardIcon->Get_Transform()->Set_Scale(250.f, 264, 0.f);
    m_CardIcon->Get_Transform()->Set_Pos(370.f, 0.f, 0.f);

    //카드 아이템 그리기용 객체 생성
    m_CardItemIcon = CUIIcon::Create(m_GraphicDev, COMPONENTTYPE::TEX_ITEM); //아이템 텍스처
    m_CardItemIcon->SetRegister(false);
    _vec3 cardPos = m_CardIcon->Get_Transform()->Get_Pos();
    _vec2 itemOffset = { 0.f, 100.f };
    m_CardItemIcon->SetCloneTexAspectScale({ .08f, .08f, 1 });
    m_CardItemIcon->Get_Transform()->Set_Pos(cardPos.x + itemOffset.x, cardPos.y + itemOffset.y, 0.f);

    //필요 재료 표시용 객체 생성
    m_IngredientIcon = CUIIcon::Create(m_GraphicDev, COMPONENTTYPE::TEX_ITEM);
    m_IngredientIcon->SetRegister(false);
    m_IngredientIcon->SetCloneTexAspectScale({ .12f, .12f, 1 });
    _vec2 ingredientOffset = { 35.f,170.f };
    m_IngredientIcon->Get_Transform()->Set_Pos(cardPos.x - ingredientOffset.x, cardPos.y - ingredientOffset.y, cardPos.z);

    //TODO 유림 업데이트 쪽에 폰트 위치 기준으로 옮기기

    //외곽선 표시용 객체 생성
    m_Highlight = CUIIcon::Create(m_GraphicDev, COMPONENTTYPE::TEX_UI_HIGHLIGHT);
    m_Highlight->SetRegister(false);
    m_Highlight->Get_Transform()->Set_Scale(_vec3(54.f, 54.f, 0.f));

    return CUI::Ready_GameObject();
}

_int CUICookingCell::Update_GameObject(const _float& timeDelta)
{
    m_TransformCom->Set_Scale(m_TargetScale);

    if (m_ItemIcon)
        m_ItemIcon->Update_GameObject(timeDelta);
         m_ItemIcon->Get_Transform()->Set_Scale(m_ItemTargetScale);

    if (m_CardIcon)
        m_CardIcon->Update_GameObject(timeDelta);

    if (m_CardItemIcon)
        m_CardItemIcon->Update_GameObject(timeDelta);

    if (m_IngredientIcon)
        m_IngredientIcon->Update_GameObject(timeDelta);

    if (m_Highlight)
    {
        _vec3 cellPos = m_TransformCom->Get_Pos();
        m_Highlight->Get_Transform()->Set_Pos(cellPos);
        m_Highlight->Update_GameObject(timeDelta);
    }

    if (m_IsHovered &&
        (CDInputMgr::GetInstance()->MouseDown(MOUSEKEYSTATE::DIM_LB)))
    {
        if (m_OnClick)
        {
            m_OnClick();
        }
    }

    int exit = CUI::Update_GameObject(timeDelta);

    return exit;
}

void CUICookingCell::LateUpdate_GameObject(const _float& timeDelta)
{
    CUI::LateUpdate_GameObject(timeDelta);
}

void CUICookingCell::Render_GameObject()
{
    CUI::Render_GameObject();

    m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());

    m_GraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(180, 70, 70, 70));

    m_GraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);   // 무시됨
    m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);

    //셀 그리기
    m_TextureCom->Set_Texture(0);
    m_BufferCom->Render_Buffer();

    m_GraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_GraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(255, 255, 255, 255));

    if (!(m_RecipeId == -1))
    {

        int TextureIndex = CItemDB::GetInstance()->GetIndexById(CRecipeDB::GetInstance()->GetRecipeById(m_RecipeId)->result);
        m_ItemIcon->Set_TextureIndex(TextureIndex);
        auto iconTransform = m_ItemIcon->Get_Transform();
        iconTransform->Set_Pos(_vec3(
            m_TransformCom->Get_Pos().x,
            m_TransformCom->Get_Pos().y,
            m_TransformCom->Get_Pos().z));

        if(m_IsActive()&&m_IsActive)
        {
            m_ItemIcon->Render_GameObject();
        }
        else
        {
        m_GraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
        m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
        m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);

        // 회색 계열 (RGB 동일하게)
        m_GraphicDev->SetRenderState(
            D3DRS_TEXTUREFACTOR,
            D3DCOLOR_ARGB(0, 0, 0, 0));
        m_ItemIcon->Render_GameObject();

        // 텍스처 그대로 사용
        m_GraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
        m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
        m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
        // TFACTOR 초기화
        m_GraphicDev->SetRenderState(
            D3DRS_TEXTUREFACTOR,
            D3DCOLOR_ARGB(255, 255, 255, 255));

        }

        if (m_IsHovered && m_CookingMenu->Get_IsOpen())
        {
            CSoundMgr::GetInstance()->Play_Once(L"UI_Change_Selection.wav",
                SOUND_EFFECT,
                1.f);

            m_TargetScale = m_BaseScale * 1.2;
            m_ItemTargetScale = m_ItemBaseScale * 1.2;

            //카드 그리기
            int TextureIndex = CItemDB::GetInstance()->GetIndexById(CRecipeDB::GetInstance()->GetRecipeById(m_RecipeId)->result);
            m_CardItemIcon->Get_Transform()->Set_Scale(_vec3(30.f, 30.f, 0.f));
            m_CardIcon->Set_TextureIndex(2);
            m_CardIcon->Render_GameObject();
            m_CardItemIcon->Set_TextureIndex(TextureIndex);
            m_CardItemIcon->Render_GameObject();

            _vec3 pos = m_CardIcon->Get_Transform()->Get_Pos();
            _vec2 descOffset = { 0.f, -10.f };
            _vec2 screen = { WINCX / 2 + pos.x, WINCY / 2 - pos.y + descOffset.y };
            _vec2 descOffset2 = { 0.f, 90.f };
            _vec2 screen2 = { WINCX / 2 + pos.x, WINCY / 2 - pos.y + descOffset2.y };
            _vec2 descOffset3 = { 0.f, 80.f };
            _vec2 screen3 = { WINCX / 2 + pos.x, WINCY / 2 - pos.y + descOffset3.y };
            _vec2 descOffset4 = { 0.f, 155.f };
            _vec2 screen4 = { WINCX / 2 + pos.x, WINCY / 2 - pos.y + descOffset4.y };
            _vec2 descOffset5 = { 30.f, 19.f };
            _vec2 screen5 = { screen4.x + descOffset5.x, screen4.y + descOffset5.y };
            //레시피 이름 출력
            Recipe* recipe = CRecipeDB::GetInstance()->GetRecipeById(m_RecipeId);
            CFontMgr::GetInstance()->Render_Font(L"아이템 이름",
                CRecipeDB::GetInstance()->Utf8ToWstring(recipe->name),
                &screen,
                D3DXCOLOR{ 1.f, 1.f, 1.f, 1.f });

            //레시피 메인 설명 출력
            CFontMgr::GetInstance()->Render_Font(L"카드 설명",
                CItemDB::GetInstance()->Utf8ToWstring(recipe->desc),
                &screen2,
                D3DXCOLOR{ 1.f, 1.f, 1.f, 1.f });

          //  std::wstring text =
          //      CItemDB::GetInstance()->Utf8ToWstring(recipe->good) +
          //      L"\n" +
          //      CItemDB::GetInstance()->Utf8ToWstring(recipe->bad);
          //
          //  //레시피 효과 설명 출력
          //  CFontMgr::GetInstance()->Render_Font(L"카드 설명",
          //      text.c_str(),
          //      &screen3,
          //      D3DXCOLOR{ 1.f, 1.f, 1.f, 1.f });
            if(m_GetCount&&(m_GetCount() >= recipe->ingredients.begin()->second)) //필요 재료 & 현재 보유 재료
            {
                std::wstring text2 =
                    L"필요한 재료\n\n" +
                    to_wstring(recipe->ingredients.begin()->second);

                    std::wstring text3 =
                        L"(" + to_wstring(m_GetCount()) + L")";

                    CFontMgr::GetInstance()->Render_Font(L"카드 설명",
                        text3,
                        &screen5,
                        D3DXCOLOR{ 0.3f, 0.7f, 0.1f, 1.f });


                CFontMgr::GetInstance()->Render_Font(L"카드 설명",
                    text2,
                    &screen4,
                    D3DXCOLOR{ 0.3f, 0.7f, 0.1f, 1.f });

            }
            else
            {
                std::wstring text2 =
                    L"필요한 재료\n\n" +
                    to_wstring(recipe->ingredients.begin()->second);

                //if (m_GetCount)
                //{
                    std::wstring text3 =
                        L"(" + to_wstring(m_GetCount()) + L")";

                    CFontMgr::GetInstance()->Render_Font(L"카드 설명",
                        text3,
                        &screen5,
                        D3DXCOLOR{ 0.8f, 0.2f, 0.2f, 1.f });

               // }

                CFontMgr::GetInstance()->Render_Font(L"카드 설명",
                    text2,
                    &screen4,
                    D3DXCOLOR{ 0.8f, 0.2f, 0.2f, 1.f });

            }

            //_vec2 ingredientOffset = { 35.f,170.f };
            //m_IngredientIcon->Get_Transform()->Set_Pos(pos.x - ingredientOffset.x, pos.y - ingredientOffset.y, pos.z);

            int IngredientTextureIndex = CItemDB::GetInstance()->GetIndexById((CRecipeDB::GetInstance()->GetRecipeById(m_RecipeId))->ingredients.begin()->first);
            m_IngredientIcon->Set_TextureIndex(IngredientTextureIndex);
            m_IngredientIcon->Render_GameObject();

            m_Highlight->Set_TextureIndex(0);
            //외곽선 바꾸기
            D3DCOLOR color = D3DCOLOR_ARGB(255, 160, 0, 0);

            m_GraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, color);

            m_GraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE); //색상을 곱하기 방식으로 처리하곘다는 뜻
            m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE); //첫번째 인자는 텍스처
            m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR); //두번째 인자는 색상

            m_Highlight->Render_GameObject();

            //RenderState 원상태로 돌리기
            m_GraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(255, 255, 255, 255));

            m_GraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
            m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
            m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

        }
        else
        {
            m_ItemTargetScale = m_ItemBaseScale;
            m_TargetScale = m_BaseScale;
         
        }
    }
}


void CUICookingCell::Render_Editor()
{
    CUI::Render_Editor();
}

HRESULT CUICookingCell::Add_Component()
{
    // buffer
    m_BufferCom = CreateProtoComponent<CRcTex>(this, COMPONENTTYPE::RC_TEX);
    NULL_CHECK_RETURN(m_BufferCom, E_FAIL);

    m_Components[ID_STATIC].insert({ COMPONENTTYPE::RC_TEX, m_BufferCom });

    // transform
    m_TransformCom = CreateProtoComponent<CTransform>(this, COMPONENTTYPE::TRANSFORM);
    NULL_CHECK_RETURN(m_TransformCom, E_FAIL);

    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::TRANSFORM, m_TransformCom });

    // cell texture
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_UI_CELL);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);

    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_UI_CELL, m_TextureCom });

    return S_OK;

}

CUICookingCell* CUICookingCell::Create(DEVICE graphicDev)
{
    auto cookingCell = new CUICookingCell(graphicDev);

    if (FAILED(cookingCell->Ready_GameObject()))
    {
        Safe_Release(cookingCell);
        MSG_BOX("UI_BuildMenu Create Failed");
        return nullptr;
    }
    return cookingCell;
    
}

void CUICookingCell::Free()
{
    Safe_Release(m_ItemIcon);
    Safe_Release(m_CardItemIcon);
    Safe_Release(m_Highlight);
    Safe_Release(m_CardIcon);
    Safe_Release(m_IngredientIcon);

    CUI::Free();
}
