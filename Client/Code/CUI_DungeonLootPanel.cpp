#include "pch.h"
#include "CUI_DungeonLootPanel.h"
#include "CTexture.h"
#include "CCreateHelper.h"
#include "CUIIcon.h"
#include "CUIInventoryCell.h"
#include "CRcTex.h"
#include "CTransform.h"
#include "CFontMgr.h"
#include "CDInputMgr.h"
#include "CRenderer.h"
#include "CStage_Dungeon.h"
#include "CSpriteEff.h"
#include "CSoundMgr.h"

class CSpriteEff;
class CStage_Dungeon;
struct Slot;

struct InventorySlot;
CUI_DungeonLootPanel::CUI_DungeonLootPanel(DEVICE graphicDev)
    : CUI(graphicDev),
      m_Cols(12),
      m_CellWidth(100),
      m_CellHeight(130),
      m_ItemIcon(nullptr),
      m_RunLootLog(nullptr) {}

CUI_DungeonLootPanel::CUI_DungeonLootPanel(const CUI_DungeonLootPanel& rhs)
    : CUI(rhs),
      m_Cols(12),
      m_CellWidth(100),
      m_CellHeight(130),
      m_ItemIcon(nullptr),
      m_RunLootLog(nullptr) {}

CUI_DungeonLootPanel::~CUI_DungeonLootPanel() {}

void CUI_DungeonLootPanel::Initialize_GameObject()
{
    CUI::Initialize_GameObject();
    CreateCells();
}

void CUI_DungeonLootPanel::SetPositions()
{
    const vector<CRunLootLog::Slot>& slots = m_RunLootLog->Get_Vector();
    int slotCount = slots.size();

    for (int i = 0; i < slotCount ; i++)
    {
        int col = i % m_Cols;
        int row = i / m_Cols;
        float startOffset = -((slotCount -1 )* m_CellWidth / 2);
        _vec2 pos = { m_TransformCom->Get_Pos().x, m_TransformCom->Get_Pos().y };
        float offsetX = col * m_CellWidth;
        float offsetY = row * m_CellHeight;
        CTransform* transform = dynamic_cast<CTransform*>(m_Cells[i]->Get_Component(COMPONENTID::ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));
        transform->Set_Pos(pos.x + offsetX + startOffset, -offsetY+ pos.y, 0.f);
    }

}

HRESULT CUI_DungeonLootPanel::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    m_DecoIcon1 = CUIIcon::Create(m_GraphicDev, COMPONENTTYPE::TEX_UI_DECO);
    m_DecoIcon1->SetRegister(false);
    m_DecoIcon1->Get_Transform()->Set_Scale(84.f, 36.f, 0.f);
    m_DecoIcon1->Get_Transform()->Set_Pos(-250.f, 200.f, 0.f);
    m_DecoIcon1->Set_TextureIndex(0);
 
    m_DecoIcon2 = CUIIcon::Create(m_GraphicDev, COMPONENTTYPE::TEX_UI_DECO);
    m_DecoIcon2->SetRegister(false);
    m_DecoIcon2->Get_Transform()->Set_Scale(84.f, 36.f, 0.f);
    m_DecoIcon2->Get_Transform()->Set_Pos(250.f, 200.f, 0.f);
    m_DecoIcon2->Set_TextureIndex(1);
 

    m_DecoIcon3 = CUIIcon::Create(m_GraphicDev, COMPONENTTYPE::TEX_UI_DECO);
    m_DecoIcon3->SetRegister(false);
    m_DecoIcon3->Get_Transform()->Set_Scale(20.f, 20.f, 0.f);
    m_DecoIcon3->Get_Transform()->Set_Pos(200.f, -200.f, 0.f);
    m_DecoIcon3->Set_TextureIndex(2);
    
    m_TimerIcon = CUIIcon::Create(m_GraphicDev, COMPONENTTYPE::TEX_UI_DECO);
    m_TimerIcon->SetRegister(false);
    m_TimerIcon->Get_Transform()->Set_Scale(20.f, 20.f, 0.f);
    m_TimerIcon->Get_Transform()->Set_Pos(-220.f, -200.f, 0.f);
    m_TimerIcon->Set_TextureIndex(3);


    m_TransformCom->Set_Pos(0.f, 0.f, 0.f);
    m_TransformCom->Set_Scale(640.f, 380.f, 0.f);

    m_Active = false;


    return S_OK;
}

_int CUI_DungeonLootPanel::Update_GameObject(const _float& timeDelta)
{
    CScene* scene = CManagement::GetInstance()->Get_Scene();
    CStage_Dungeon* dungeonStage = dynamic_cast<CStage_Dungeon*>(scene);

    if(dungeonStage->IsCleared()&&!m_Active)
    {
        if (!m_BGMStarted)
        {
            CSoundMgr::GetInstance()->Play_BGM(L"UI/UI_DungeonClear.wav",
                SOUND_EFFECT,
                0.3f);
            m_BGMStarted = true;
        }
        m_Active = true;
        m_Alpha = 0.f;
    }

    if (!m_Active)
        return 0;
    
    m_Alpha += m_FadeInSpeed * timeDelta;

    m_Alpha = clamp(m_Alpha, 0.f, 1.f);

    if( m_Active == true && CDInputMgr::GetInstance()->KeyDown(DIK_M))
    {
        m_Active = false;
    }
    
    if( m_Active== true && CDInputMgr::GetInstance()->KeyDown(DIK_K))
    {
        m_Alpha = 0.f;
    }

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

    int exit = CUI::Update_GameObject(timeDelta);

    const vector<CRunLootLog::Slot>& slots = m_RunLootLog->Get_Vector();

    for (int i = 0; i < slots.size(); i++)
    {
        auto& slot = slots[i];

        m_Cells[i]->SetInventoryCell(slot.itemInst, slot.count); //셀마다 정보 셋팅
        m_Cells[i]->Update_GameObject(timeDelta);
    }

    if (m_DecoIcon1)
    {
        m_DecoIcon1->Update_GameObject(timeDelta);
        m_DecoIcon1->Set_Alpha(m_Alpha);
       
    }
    if (m_DecoIcon2)
    {
        m_DecoIcon2->Update_GameObject(timeDelta);
        m_DecoIcon2->Set_Alpha(m_Alpha);
    }
    if (m_DecoIcon3)
    {
        m_DecoIcon3->Update_GameObject(timeDelta);
        m_DecoIcon3->Set_Alpha(m_Alpha);
    }
    if (m_TimerIcon)
    {
        m_TimerIcon->Update_GameObject(timeDelta);
        m_TimerIcon->Set_Alpha(m_Alpha);
    }
    SetPositions();

    return exit;
}

void CUI_DungeonLootPanel::LateUpdate_GameObject(const _float& timeDelta)
{
    CUI::LateUpdate_GameObject(timeDelta);

}
void CUI_DungeonLootPanel::Render_GameObject()
{
    if (!m_Active)
        return;

    CUI::Render_GameObject();

    m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());

    //m_SpriteEff->AlphaRenderStart(m_Alpha);
    m_TextureCom->Set_Texture(0);
    m_BufferCom->Render_Buffer();
 
    const vector<CRunLootLog::Slot>& slots = m_RunLootLog->Get_Vector();
    int slotCount = slots.size();
    for (int i = 0; i < slotCount; i++)
    {
        m_Cells[i]->Render_GameObject();
    }

    _vec2 pos = { m_TransformCom->Get_Pos().x, m_TransformCom->Get_Pos().y };
    _vec2 offset = { 0.f,200.f };
    _vec2 screen = { WINCX / 2 + pos.x + offset.x , WINCY / 2 - pos.y - offset.y };
  
    CFontMgr::GetInstance()->Render_Font(L"NanumSquare_Heavy2",
        L"쓰러뜨린 이교도",
        &screen,
        D3DXCOLOR(242.f / 255.f, 237.f / 255.f, 221.f / 255.f, m_Alpha));

    _vec2 offset1 = { 0.f,130.f };
    _vec2 screen1 = { WINCX / 2 + pos.x + offset1.x , WINCY / 2 - pos.y - offset1.y };


    CFontMgr::GetInstance()->Render_Font(L"NanumSquare_Big",
        L"정화한 불신자들",
        &screen1,
        D3DXCOLOR(242.f / 255.f, 237.f / 255.f, 221.f / 255.f, m_Alpha * 0.6f));

    CScene* scene = CManagement::GetInstance()->Get_Scene();
    CStage_Dungeon* dungeonStage = dynamic_cast<CStage_Dungeon*>(scene);
    _vec3 countPos = m_DecoIcon3->Get_Transform()->Get_Pos();
    _vec2 offset2 = { 50.f,3.f };
    _vec2 screen2 = { WINCX / 2 + countPos.x + offset2.x , WINCY / 2 - countPos.y + offset2.y};
    CFontMgr::GetInstance()->Render_Font(L"NanumSquare_Big",
        to_wstring(dungeonStage->Get_KillCount()),
        &screen2,
        D3DXCOLOR(242.f / 255.f, 237.f / 255.f, 221.f / 255.f, m_Alpha));

    _float time = dungeonStage->Get_Timer();
    _vec3 timerIconPos = m_TimerIcon->Get_Transform()->Get_Pos();
    _vec2 offset3 = { 90.f,3.f };
    _vec2 screen3 = { WINCX / 2 + timerIconPos.x + offset3.x , WINCY / 2 - timerIconPos.y + offset3.y };
    CFontMgr::GetInstance()->Render_Font(L"NanumSquare_Big",
        FormatTime(time),
        &screen3,
        D3DXCOLOR(242.f / 255.f, 237.f / 255.f, 221.f / 255.f, m_Alpha));


    if (m_DecoIcon1)
        m_DecoIcon1->Render_GameObject();

    if (m_DecoIcon2)
        m_DecoIcon2->Render_GameObject();

    if (m_DecoIcon3)
        m_DecoIcon3->Render_GameObject();

    if (m_TimerIcon)
        m_TimerIcon->Render_GameObject();

   // m_SpriteEff->AlphaRenderEnd();
}

void CUI_DungeonLootPanel::CreateCells()
{
    int rows = 2;
    int cols = 6;

    for (int i = 0; i < rows * cols; i++)
    {
        CUIInventoryCell* cell = CUIInventoryCell::Create(m_GraphicDev);
        m_Cells.push_back(cell);
    }
}

HRESULT CUI_DungeonLootPanel::Add_Component()
{
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_UI_CLEAR);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_UI_CLEAR, m_TextureCom });

    m_BufferCom = CreateProtoComponent<CRcTex>(this, COMPONENTTYPE::RC_TEX);
    NULL_CHECK_RETURN(m_BufferCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::RC_TEX, m_BufferCom });

    m_TransformCom = CreateProtoComponent<CTransform>(this, COMPONENTTYPE::TRANSFORM);
    NULL_CHECK_RETURN(m_TransformCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::TRANSFORM, m_TransformCom });

    m_SpriteEff = CreateProtoComponent<CSpriteEff>(this, COMPONENTTYPE::SPRITEEFFECT);
    NULL_CHECK_RETURN(m_SpriteEff, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::SPRITEEFFECT, m_SpriteEff });
    return S_OK;
}
CUI_DungeonLootPanel* CUI_DungeonLootPanel::Create(DEVICE graphicDev)
{
    auto panel = new CUI_DungeonLootPanel(graphicDev);
    if (FAILED(panel->Ready_GameObject()))
    {
        Safe_Release(panel);
        MSG_BOX("CUI_DungeonLootPanel Created Failed");
        return nullptr;
    }
    return panel;
}

void CUI_DungeonLootPanel::Free()
{
    for (auto& cell : m_Cells)
    {
        Safe_Release(cell);
    }

    m_Cells.clear();
    Safe_Release(m_DecoIcon1);
    Safe_Release(m_DecoIcon2);
    Safe_Release(m_DecoIcon3);
    Safe_Release(m_ItemIcon);
    Safe_Release(m_TimerIcon);
    CUI::Free();
}
