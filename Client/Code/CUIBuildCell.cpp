#include "pch.h"
#include "CUIBuildCell.h"
#include "CCreateHelper.h"
#include "CDInputMgr.h"
#include "CEnumHelper.h"
#include "CFontMgr.h"
#include "CItemDB.h"
#include "CRcTex.h"
#include "CRecipeDB.h"
#include "CRenderer.h"
#include "CTexture.h"
#include "CTransform.h"
#include "CUIIcon.h"
#include "ItemData.h"
#include "CUIBuildMenu.h"
#include "CSoundMgr.h"

CUIBuildCell::CUIBuildCell(DEVICE graphicDev)
    : CUI(graphicDev),
    m_ItemIcon(nullptr),
    m_CardIcon(nullptr),
    m_CardItemIcon(nullptr),
    m_Highlight(nullptr),
    m_IsHovered(false)
{
}

CUIBuildCell::CUIBuildCell(const CUIBuildCell& rhs)
    : CUI(rhs),
    m_ItemIcon(nullptr),
    m_CardIcon(nullptr),
    m_CardItemIcon(nullptr),
    m_Highlight(nullptr),
    m_IsHovered(rhs.m_IsHovered)
{
}

CUIBuildCell::~CUIBuildCell()
{
}

void CUIBuildCell::OnBeginOverlap(CCollider* self, CCollider* other)
{
    CUI::OnBeginOverlap(self, other);
}

void CUIBuildCell::OnEndOverlap(CCollider* self, CCollider* other)
{
    CUI::OnEndOverlap(self, other);
}

void CUIBuildCell::Render_Editor()
{
    CUI::Render_Editor();
}

HRESULT CUIBuildCell::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    //셀 크기 지정
    m_TransformCom->Set_Scale(60.f, 60.f, 0.f);
    m_BaseScale = m_TransformCom->Get_Scale();
    m_TargetScale = m_BaseScale;

    //셀 아이템 그리기용 객체 생성
    m_ItemIcon = CUIIcon::Create(m_GraphicDev, COMPONENTTYPE::TEX_UI_BUILD); //아이템 텍스처
    m_ItemIcon->Get_Transform()->Set_Scale(_vec3(40.f, 40.f, 0.f));
    m_ItemIcon->SetRegister(false); // 아이템 텍스처 렌더 그룹에 중복 등록되는 것 막기 위한 플래그

    //설명 카드 그리기용 객체 생성
    m_CardIcon = CUIIcon::Create(m_GraphicDev, COMPONENTTYPE::TEX_UI_RITUALCARD);
    m_CardIcon->SetRegister(false);
    m_CardIcon->Get_Transform()->Set_Scale(250.f, 180.f, 0.f);
    m_CardIcon->Get_Transform()->Set_Pos(370.f, 0.f, 0.f);

    //카드 아이템 그리기용 객체 생성
    m_CardItemIcon = CUIIcon::Create(m_GraphicDev, COMPONENTTYPE::TEX_UI_BUILD); //아이템 텍스처
    m_CardItemIcon->SetRegister(false);
    _vec3 cardPos = m_CardIcon->Get_Transform()->Get_Pos();
    _vec2 itemOffset = { 0.f, 70.f };
    m_CardItemIcon->Get_Transform()->Set_Scale(_vec3(40.f, 40.f, 0.f));
    m_CardItemIcon->Get_Transform()->Set_Pos(cardPos.x + itemOffset.x, cardPos.y + itemOffset.y, 0.f);

    //외곽선용 객체 생성
    m_Highlight = CUIIcon::Create(m_GraphicDev, COMPONENTTYPE::TEX_UI_HIGHLIGHT);
    m_Highlight->SetRegister(false);
    m_Highlight->Get_Transform()->Set_Scale(_vec3(78.f, 78.f, 0.f));

    //재료 표시용 아이콘
    m_LogIcon = CUIIcon::Create(m_GraphicDev, COMPONENTTYPE::TEX_ITEM);
    m_LogIcon->SetRegister(false);
    int logIndex = CItemDB::GetInstance()->GetIndexById(1005);
    m_LogIcon->Set_TextureIndex(logIndex);
    m_LogIcon->Get_Transform()->Set_Scale(20.f,20.f,0.f);

    m_CoinIcon = CUIIcon::Create(m_GraphicDev, COMPONENTTYPE::TEX_ITEM);
    m_CoinIcon->SetRegister(false);
    int coinIndex = CItemDB::GetInstance()->GetIndexById(1001);
    m_CoinIcon->Set_TextureIndex(coinIndex);
    m_CoinIcon->Get_Transform()->Set_Scale(20.f, 20.f, 0);

    return S_OK;
}

_int CUIBuildCell::Update_GameObject(const _float& timeDelta)
{

    m_TransformCom->Set_Scale(m_TargetScale);

    if (m_ItemIcon)
        m_ItemIcon->Update_GameObject(timeDelta);
        m_ItemIcon->Get_Transform()->Set_Scale(m_TargetScale);

    if (m_CardIcon)
        m_CardIcon->Update_GameObject(timeDelta);

    if (m_CardItemIcon)
        m_CardItemIcon->Update_GameObject(timeDelta);

    if (m_Highlight)
    {
        _vec3 cellPos = m_TransformCom->Get_Pos();
        m_Highlight->Get_Transform()->Set_Pos(cellPos);
        m_Highlight->Update_GameObject(timeDelta);
    }

    if (m_LogIcon)
        m_LogIcon->Update_GameObject(timeDelta);

    if (m_CoinIcon)
        m_CoinIcon->Update_GameObject(timeDelta);

    if (m_IsHovered &&
        (CDInputMgr::GetInstance()->Get_DIMouseState(MOUSEKEYSTATE::DIM_LB) & 0x80))
    {
        // cout << "CUIBuildCell::Update_GameObject: Click detected!" << endl;
        if (m_OnClick)
        {
			CSoundMgr::GetInstance()->Play_Once(L"UI/UI_Confirm_Selection.wav",
				SOUND_EFFECT,
				0.7f);
            // cout << "CUIBuildCell::Update_GameObject: Executing OnClick" << endl;
            m_OnClick();
        }
    }

    int exit = CUI::Update_GameObject(timeDelta);

    return exit;
}

void CUIBuildCell::LateUpdate_GameObject(const _float& timeDelta)
{
    CUI::LateUpdate_GameObject(timeDelta);
}

void CUIBuildCell::Render_GameObject()
{
    CUI::Render_GameObject();

    m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());

    if(m_IsActive())
    {
        m_TextureCom->Set_Texture(1);
        m_BufferCom->Render_Buffer();
    }
    else
    {
        m_TextureCom->Set_Texture(2);
        m_BufferCom->Render_Buffer();
    }

    m_ItemIcon->Set_TextureIndex(m_TextureIndex);
    m_CardItemIcon->Set_TextureIndex(m_TextureIndex);

    _vec3 cellPos = m_TransformCom->Get_Pos();
    _vec2 logOffset = { -45.f, -100.f };
    _vec2 coinOffset = { 20.f, -100.f };

    m_ItemIcon->Get_Transform()->Set_Pos(cellPos);
    m_LogIcon->Get_Transform()->Set_Pos(cellPos.x+logOffset.x, cellPos.y+logOffset.y, cellPos.z);
    m_CoinIcon->Get_Transform()->Set_Pos(cellPos.x + coinOffset.x, cellPos.y + coinOffset.y, cellPos.z);

    m_ItemIcon->Render_GameObject();
    m_LogIcon->Render_GameObject();
    m_CoinIcon->Render_GameObject();

    _vec3 logPos = m_LogIcon->Get_Transform()->Get_Pos();
    float logOffsetX = 35.f;
    _vec2 logScreen = {WINCX / 2 + logPos.x + logOffsetX,WINCY / 2 - logPos.y};
    Recipe* recipe1 =CRecipeDB::GetInstance()->GetRecipeById(m_RecipeId);
    CFontMgr::GetInstance()->Render_Font(L"NanumSquare_Small",
        to_wstring(recipe1->ingredients[1005]),
        &logScreen,
        D3DXCOLOR{ 1.f, 1.f, 1.f, 1.f });

    _vec3 coinPos = m_CoinIcon->Get_Transform()->Get_Pos();
    float coinOffsetX = 35.f;
    _vec2 coinScreen = { WINCX / 2 + coinPos.x + coinOffsetX,WINCY / 2 - coinPos.y };
    Recipe* recipe2 = CRecipeDB::GetInstance()->GetRecipeById(m_RecipeId);
    CFontMgr::GetInstance()->Render_Font(L"NanumSquare_Small",
        to_wstring(recipe2->ingredients[1001]),
        &coinScreen,
        D3DXCOLOR{ 1.f, 1.f, 1.f, 1.f });

    //셀 선택시 외곽선 그리기
    if (m_IsHovered && m_IsActive() &&m_BuildMenu->Get_IsOpen())
    {
		CSoundMgr::GetInstance()->Play_Once(L"UI_Change_Selection.wav",
			SOUND_EFFECT,
			1.f);

        m_TargetScale = m_BaseScale * 1.2;

        m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());
        m_Highlight->Set_TextureIndex(0);

        //카드 그리기
        m_CardIcon->Set_TextureIndex(1);
        m_CardIcon->Render_GameObject();
        m_CardItemIcon->Render_GameObject();

        //TODO 유림 : 이미지 바꿀건지 결정하기
        _vec3 pos = m_CardIcon->Get_Transform()->Get_Pos();
        _vec2 descOffset = { 0.f, 15.f };
        _vec2 screen = { WINCX / 2 + pos.x, WINCY / 2 - pos.y + descOffset.y };
        _vec2 descOffset2 = { 0.f, 95.f };
        _vec2 screen2 = { WINCX / 2 + pos.x, WINCY / 2 - pos.y + descOffset2.y };

        //아이템 이름 출력
        Recipe* recipe = CRecipeDB::GetInstance()->GetRecipeById(m_RecipeId);
        CFontMgr::GetInstance()->Render_Font(L"아이템 이름",
            CRecipeDB::GetInstance()->Utf8ToWstring(recipe->name),
            &screen,
            D3DXCOLOR{ 1.f, 1.f, 1.f, 1.f });

        //아이템 메인 설명 출력
        CFontMgr::GetInstance()->Render_Font(L"카드 설명",
            CItemDB::GetInstance()->Utf8ToWstring(recipe->desc),
            &screen2,
            D3DXCOLOR{ 1.f, 1.f, 1.f, 1.f });

        //외곽선 바꾸기
        D3DCOLOR color = D3DCOLOR_ARGB(255, 255, 255, 255);

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
        m_TargetScale = m_BaseScale;
    }


}

HRESULT CUIBuildCell::Add_Component()
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

CUIBuildCell* CUIBuildCell::Create(DEVICE graphicDev)
{
    auto cell = new CUIBuildCell(graphicDev);
    if (FAILED(cell->Ready_GameObject()))
    {
        Safe_Release(cell);
        MSG_BOX("InventoryCell Created Failed");
        return nullptr;
    }
    return cell;
}

void CUIBuildCell::Free()
{
    Safe_Release(m_ItemIcon);
    Safe_Release(m_Highlight);
    Safe_Release(m_CardIcon);
    Safe_Release(m_CardItemIcon);
    Safe_Release(m_LogIcon);
    Safe_Release(m_CoinIcon);

    CUI::Free();
}
