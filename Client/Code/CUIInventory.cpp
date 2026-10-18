#include "pch.h"
#include "CUIInventory.h"
#include "CCreateHelper.h"
#include "CDInputMgr.h"
#include "CFontMgr.h"
#include "CInventory.h"
#include "CRcTex.h"
#include "CRenderer.h"
#include "CTexture.h"
#include "CTransform.h"
#include "CUIIcon.h"
#include "CFontMgr.h"
#include "CTimerMgr.h"
#include "CUIButton.h"
#include "CUIInventoryCell.h"
#include "CSoundMgr.h"
#include "Engine_Enum.h"

CUIInventory::CUIInventory(DEVICE graphicDev)
    : CUI(graphicDev),
      m_Cols(6),
      m_CellWidth(100.f),
      m_CellHeight(150.f),
      m_Inventory(nullptr)
{

}

CUIInventory::CUIInventory(const CUIInventory& rhs)
    : CUI(rhs),
      m_Cols(6),
      m_CellWidth(100.f),
      m_CellHeight(150.f),
      m_Inventory(nullptr)
{}

CUIInventory::~CUIInventory()
{}

HRESULT CUIInventory::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    m_TransformCom->Set_Scale(450.f, 400.f, 1.f);

    return S_OK;
}

_int CUIInventory::Update_GameObject(const _float& timeDelta)
{

    SetPositions(); //인벤토리 생성 & 이동시 셀 위치 재계산


    const vector<InventorySlot>& slots = m_Inventory->GetVector();
    for (int i = 0; i < slots.size(); i++) //슬롯 사이즈는 항상 12 (아이템이 없을 때도 -1값 저장됨)
    {
        auto& slot = slots[i];

        m_Cells[i]->SetInventoryCell(slot.itemInst, slot.count); //셀마다 정보 셋팅
        m_Cells[i]->Update_GameObject(timeDelta);
    }

    Check_Hovered();

    int exit = CUI::Update_GameObject(timeDelta);
    return exit;
}

void CUIInventory::LateUpdate_GameObject(const _float& timeDelta)
{
    CUI::LateUpdate_GameObject(timeDelta);

}

void CUIInventory::Render_GameObject()
{
    CUI::Render_GameObject();

    m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());

    _vec2 invenPos = { m_TransformCom->Get_Pos().x, m_TransformCom->Get_Pos().y };


    float materialOffsetX = -250.f;
    float materialOffsetY = 130.f;
    float foodOffsetX = -250.f;
    float foodOffsetY = -40.f;

    _vec2 material = { invenPos.x + materialOffsetX, invenPos.y + materialOffsetY };
    _vec2 food = { invenPos.x + foodOffsetX , invenPos.y + foodOffsetY };

    _vec2 screenMaterial = { WINCX / 2 + material.x, WINCY/2 - material.y }; //폰트 출력을 위해 스크린 좌표로 변환
    _vec2 screenFood = { WINCX / 2 + food.x , WINCY/2 - food.y };

    //인벤토리 배경 그리기
     m_TextureCom->Set_Texture(0);
     m_BufferCom->Render_Buffer();

    //폰트 그리기
     CFontMgr::GetInstance()->Render_Font(L"NanumSquare_Regular",
         L"재화",
         &screenMaterial,
         D3DXCOLOR(0.7f, 0.7f, 0.7f, 1.f));

     CFontMgr::GetInstance()->Render_Font(L"NanumSquare_Regular",
            L"음식",
            &screenFood,
            D3DXCOLOR(0.7f, 0.7f, 0.7f, 1.f));

    //셀 그리기
      const vector<InventorySlot>& slots = m_Inventory->GetVector();
      int slotCount = slots.size();
      for (int i = 0; i < slotCount; i++)
      {
          m_Cells[i]->Render_GameObject();
      }



}

void CUIInventory::SetInventory(CInventory* inventory)
{
     m_Inventory = inventory;

     if (!m_Inventory)
         return;

    CreateCells();

    SetPositions();
}

void CUIInventory::CreateCells()
{
    int rows = 2;
    int cols = 6;

    for (int i = 0; i < rows*cols; i++)
    {
        CUIInventoryCell* cell = CUIInventoryCell::Create(m_GraphicDev);
        m_Cells.push_back(cell);
    }
}


void CUIInventory::SetPositions()
{
    const vector<InventorySlot>& slots = m_Inventory->GetVector();
    _vec2 invenPos ={ m_TransformCom->Get_Pos().x, m_TransformCom->Get_Pos().y };
    int slotCount = slots.size();

    int materialRenderCount = 0;

    for (int i = 0; i < slotCount / 2; i++)
    {
            int col = materialRenderCount % m_Cols;
            float startOffset = -230.f;
            float materialOffsetX = startOffset +col * m_CellWidth;
            CTransform* transform = dynamic_cast<CTransform*>(m_Cells[i]->Get_Component(COMPONENTID::ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));

            transform->Set_Pos(invenPos.x + materialOffsetX, invenPos.y + 60, 0.f);

            materialRenderCount++;
    }
    int foodRenderCount = 0;

    for (int i = slotCount / 2; i < slotCount; i++)
    {
            int col = foodRenderCount % m_Cols;
            float startOffset = -230.f;
            float foodOffsetX = startOffset + col * m_CellWidth;
            CTransform* transform = dynamic_cast<CTransform*>(m_Cells[i]->Get_Component(COMPONENTID::ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));

            transform->Set_Pos(invenPos.x+foodOffsetX, invenPos.y-110.f, 0.f);

            foodRenderCount++;
    }


}

HRESULT CUIInventory::Add_Component()
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
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_UI_INVEN);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);

    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_UI_INVEN, m_TextureCom });

    return S_OK;
}

void CUIInventory::Check_Hovered() //셀 선택 체크는 인벤토리 UI 클래스에서 하고, 상태 변화에 따른 변경은 셀 클래스에서 함
{
 
    const vector<InventorySlot>& slots = m_Inventory->GetVector();
    for(int i = 0; i < slots.size();i++)
    {
       _vec3 cellPos = dynamic_cast<CTransform*>(m_Cells[i]->Get_Component(COMPONENTID::ID_DYNAMIC, COMPONENTTYPE::TRANSFORM))->Get_Pos();
       _vec3 cellSize = dynamic_cast<CTransform*>(m_Cells[i]->Get_Component(COMPONENTID::ID_DYNAMIC, COMPONENTTYPE::TRANSFORM))->Get_Scale();

       float screenX = cellPos.x + WINCX * 0.5f;
       float screenY = WINCY * 0.5f - cellPos.y;

       m_tRect.left = (LONG)(screenX - cellSize.x);
       m_tRect.top = (LONG)(screenY - cellSize.y);
       m_tRect.right = (LONG)(screenX + cellSize.x);
       m_tRect.bottom = (LONG)(screenY + cellSize.y);

       POINT pt;
       GetCursorPos(&pt);
       ScreenToClient(CDInputMgr::GetInstance()->Get_HWND(), &pt);
       bool hoveredNow = PtInRect(&m_tRect, pt) != 0;

        if(hoveredNow !=m_Cells[i]->Get_IsHovered())
        {
            if (hoveredNow)
            {
                CSoundMgr::GetInstance()->Play_Once(L"ui/ui_pointeron.wav",
                    SOUNDTYPE::SOUND_UI,
                    5.f);
            };
            m_Cells[i]->Set_IsHovered(hoveredNow);
        }

    }
}

void CUIInventory::Render_Editor()
{
    CUI::Render_Editor();
}

CUIInventory* CUIInventory::Create(DEVICE graphicDev)
{
    CUIInventory* inventoryUI = new CUIInventory(graphicDev);

    if (FAILED(inventoryUI->Ready_GameObject()))
    {
        Safe_Release(inventoryUI);
        MSG_BOX("inventoryUI Created Failed");
        return nullptr;
    }
    return inventoryUI;
}

void CUIInventory::Free()
{
    for (auto& cell : m_Cells)
    {
        Safe_Release(cell);
    }

    m_Cells.clear();

    CUI::Free();
}
