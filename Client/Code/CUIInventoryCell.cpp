#include "pch.h"
#include "CUIInventoryCell.h"

#include "CCreateHelper.h"
#include "CDInputMgr.h"
#include "CEnumHelper.h"
#include "CFontMgr.h"
#include "CItemDB.h"
#include "CRcTex.h"
#include "CRenderer.h"
#include "CTexture.h"
#include "CTransform.h"
#include "CUIIcon.h"
#include "ItemData.h"


CUIInventoryCell::CUIInventoryCell(DEVICE pGraphicDev)
    : CUI(pGraphicDev),
      m_ItemIcon(nullptr),
      m_CardIcon(nullptr),
      m_CardItemIcon(nullptr),
      m_HighlightTexCom(nullptr),
      m_Count(0),
      m_IsHovered(false)
{ }

CUIInventoryCell::CUIInventoryCell(const CUIInventoryCell& rhs)
    : CUI(rhs),
      m_ItemIcon(nullptr),
      m_CardIcon(nullptr),
      m_CardItemIcon(nullptr),
      m_HighlightTexCom(nullptr),
      m_Count(0),
      m_IsHovered(rhs.m_IsHovered)
{ }

CUIInventoryCell::~CUIInventoryCell()
{ }

void CUIInventoryCell::OnBeginOverlap(CCollider* self, CCollider* other)
{
    CUI::OnBeginOverlap(self, other);
}

void CUIInventoryCell::OnEndOverlap(CCollider* self, CCollider* other)
{
    CUI::OnEndOverlap(self, other);
}

void CUIInventoryCell::Render_Editor()
{
    CUI::Render_Editor();
}

HRESULT CUIInventoryCell::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    //인벤토리 아이템 그리기용 객체 생성
    m_TransformCom->Set_Scale(45.f, 45.f, 0.f);

    m_ItemIcon = CUIIcon::Create(m_GraphicDev, COMPONENTTYPE::TEX_ITEM); //아이템 텍스처
    m_ItemIcon->SetRegister(false); // 아이템 텍스처 렌더 그룹에 중복 등록되는 것 막기 위한 플래그

    //카드 그리기용 객체 생성
    m_CardIcon = CUIIcon::Create(m_GraphicDev, COMPONENTTYPE::TEX_UI_RITUALCARD);
    m_CardIcon->SetRegister(false);
    m_CardIcon->Get_Transform()->Set_Scale(250.f, 180.f, 0.f);
    m_CardIcon->Get_Transform()->Set_Pos(370.f, 0.f, 0.f);

    //카드 아이템 그리기용 객체 생성
    m_CardItemIcon = CUIIcon::Create(m_GraphicDev, COMPONENTTYPE::TEX_ITEM); //아이템 텍스처
    m_CardItemIcon->SetRegister(false);
    _vec3 cardPos    = m_CardIcon->Get_Transform()->Get_Pos();
    _vec2 itemOffset = { 0.f, 70.f };
    m_CardItemIcon->Get_Transform()->Set_Pos(cardPos.x + itemOffset.x, cardPos.y + itemOffset.y, 0.f);
    return S_OK;
}

_int CUIInventoryCell::Update_GameObject(const _float& timeDelta)
{

    if (m_ItemIcon)
        m_ItemIcon->Update_GameObject(timeDelta);

    if (m_CardIcon)
        m_CardIcon->Update_GameObject(timeDelta);

    if (m_CardItemIcon)
        m_CardItemIcon->Update_GameObject(timeDelta);

    int exit = CUI::Update_GameObject(timeDelta);
    return exit;
}

void CUIInventoryCell::LateUpdate_GameObject(const _float& timeDelta)
{
    CUI::LateUpdate_GameObject(timeDelta);
}

void CUIInventoryCell::Render_GameObject()
{
    CUI::Render_GameObject();

    m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());

    m_GraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(180, 100, 100, 100));

    m_GraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);   // 무시됨
    m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);

    m_TextureCom->Set_Texture(0);
    m_BufferCom->Render_Buffer();

    m_GraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_GraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(255, 255, 255, 255));

    //아이템이 있으면 아이템 아이콘 렌더링
    if (!(m_ItemInst.itemId == -1))
    {
        int TextureIndex = CItemDB::GetInstance()->GetIndexById(m_ItemInst.itemId);
        m_ItemIcon->Set_TextureIndex(TextureIndex);
        m_CardItemIcon->Set_TextureIndex(TextureIndex);
        auto iconTransform = m_ItemIcon->Get_Transform();
        iconTransform->Set_Pos(_vec3(
            m_TransformCom->Get_Pos().x,
            m_TransformCom->Get_Pos().y,
            m_TransformCom->Get_Pos().z));
        m_ItemIcon->Get_Transform()->Set_Scale(_vec3(30.f, 30.f, 0.f));
        m_CardItemIcon->Get_Transform()->Set_Scale(_vec3(30.f, 30.f, 0.f));

        m_ItemIcon->Render_GameObject();
    }
    float numberOffsetX = 30.f;
    float numberOffsetY = 25.f;

    //숫자가 0 이상이면 숫자 렌더링
    if (m_Count > 0)
    {
        _vec2 cellPos = { m_TransformCom->Get_Pos().x + WINCX / 2, WINCY / 2 - m_TransformCom->Get_Pos().y };

        _vec2 number = { cellPos.x + numberOffsetX, cellPos.y + numberOffsetY };
        CFontMgr::GetInstance()->Render_FontOutlined(L"Ghana_Heavy",
                                                     to_wstring(m_Count),
                                                     &number,
                                                     D3DXCOLOR(1.f, 1.f, 1.f, 1.f),
                                                     D3DXCOLOR(0.f, 0.f, 0.f, 1.f),
                                                     3);
    }

    //셀 선택시 외곽선 그리기
    if (m_IsHovered)
    {
    

        m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());
        m_HighlightTexCom->Set_Texture(1);

        //외곽선 빨강으로 바꾸기
        D3DCOLOR color = D3DCOLOR_ARGB(255, 160, 0, 0);

        m_GraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, color);

        m_GraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE); //색상을 곱하기 방식으로 처리하곘다는 뜻
        m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE); //첫번째 인자는 텍스처
        m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR); //두번째 인자는 색상
        m_BufferCom->Render_Buffer();

        //RenderState 원상태로 돌리기
        m_GraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(255, 255, 255, 255));

        m_GraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
        m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
        m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

        if (!(m_ItemInst.itemId == -1))
        {
            //카드 그리기
            m_CardIcon->Set_TextureIndex(0);
            m_CardIcon->Render_GameObject();
            m_CardItemIcon->Render_GameObject();

            //아이템 이름 출력
            Item* item       = CItemDB::GetInstance()->GetItemById(m_ItemInst.itemId);
            _vec3 pos        = m_CardIcon->Get_Transform()->Get_Pos();
            _vec2 descOffset = { 0.f, 0.f };
            _vec2 screen     = { WINCX / 2 + pos.x, WINCY / 2 - pos.y + descOffset.y };
            CFontMgr::GetInstance()->Render_Font(L"아이템 이름",
                                                 CItemDB::GetInstance()->Utf8ToWstring(item->name),
                                                 &screen,
                                                 D3DXCOLOR{ 1.f, 1.f, 1.f, 1.f });

            //아이템 메인 설명 출력
            descOffset = { 0.f, 95.f };
            screen     = { WINCX / 2 + pos.x, WINCY / 2 - pos.y + descOffset.y };
            CFontMgr::GetInstance()->Render_Font(L"카드 설명",
                                                 CItemDB::GetInstance()->Utf8ToWstring(item->desc),
                                                 &screen,
                                                 D3DXCOLOR{ 1.f, 1.f, 1.f, 1.f });

            //아이템 부연 설명 출력
            descOffset = { 0.f, 25.f };
            screen     = { WINCX / 2 + pos.x, WINCY / 2 - pos.y + descOffset.y };
            CFontMgr::GetInstance()->Render_Font(L"부연 설명",
                                                 CItemDB::GetInstance()->Utf8ToWstring(item->additionalDesc),
                                                 &screen,
                                                 D3DXCOLOR{ 0.6f, 0.6f, 0.6f, 1.f });
        }
    }
}

HRESULT CUIInventoryCell::Add_Component()
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

    //highlight texture
    m_HighlightTexCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_UI_HIGHLIGHT);
    NULL_CHECK_RETURN(m_HighlightTexCom, E_FAIL);

    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_UI_HIGHLIGHT, m_HighlightTexCom });

    return S_OK;
}

CUIInventoryCell* CUIInventoryCell::Create(DEVICE graphicDev)
{
    auto cell = new CUIInventoryCell(graphicDev);
    if (FAILED(cell->Ready_GameObject()))
    {
        Safe_Release(cell);
        MSG_BOX("InventoryCell Created Failed");
        return nullptr;
    }
    return cell;
}

void CUIInventoryCell::Free()
{
    Safe_Release(m_ItemIcon);
    Safe_Release(m_HighlightTexCom);
    Safe_Release(m_CardIcon);
    Safe_Release(m_CardItemIcon);

    CUI::Free();
}
