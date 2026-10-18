#include "pch.h"
#include "CUICookedFoodCell.h"
#include "CCreateHelper.h"
#include "CItemDB.h"
#include "CRcTex.h"
#include "CRecipeDB.h"
#include "CTexture.h"
#include "CTransform.h"
#include "CUIIcon.h"

CUICookedFoodCell::CUICookedFoodCell(DEVICE graphicDev)
    : CUI(graphicDev),
      m_ItemIcon(nullptr),
      m_RecipeId(-1),
      m_RecipeInst() {}

CUICookedFoodCell::CUICookedFoodCell(const CUICookedFoodCell& rhs)
    : CUI(rhs),
      m_ItemIcon(nullptr),
      m_RecipeId(-1),
      m_RecipeInst() {}

CUICookedFoodCell::~CUICookedFoodCell() {}

HRESULT CUICookedFoodCell::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    //셀 크기 지정
    m_TransformCom->Set_Scale(70.f, 70.f, 0.f);

    //셀 아이템 그리기용 객체 생성
    m_ItemIcon = CUIIcon::Create(m_GraphicDev, COMPONENTTYPE::TEX_ITEM); //아이템 텍스처
	m_ItemIcon->SetCloneTexAspectScale({ .2f, .2f, 1 });
    m_ItemIcon->SetRegister(false); // 아이템 텍스처 렌더 그룹에 중복 등록되는 것 막기 위한 플래그
	m_ItemIcon->Get_Transform()->Set_Pos(-500.f, -500.f, 0.f);
    return S_OK;
}

_int CUICookedFoodCell::Update_GameObject(const _float& timeDelta)
{
    int exit = CUI::Update_GameObject(timeDelta);

    if (m_ItemIcon)
        m_ItemIcon->Update_GameObject(timeDelta);


    return exit;
}

void CUICookedFoodCell::LateUpdate_GameObject(const _float& timeDelta)
{
    CUI::LateUpdate_GameObject(timeDelta);
}

void CUICookedFoodCell::Render_GameObject()
{
    CUI::Render_GameObject();

    m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());

    //m_GraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(180, 100, 100, 100));
	//
    //m_GraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
    //m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);   // 무시됨
    //m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
	//
    m_TextureCom->Set_Texture(4);
    m_BufferCom->Render_Buffer();

    //m_GraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    //m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    //m_GraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(255, 255, 255, 255));
	//
    if (!(m_RecipeId == -1))
    {
        int TextureIndex = CItemDB::GetInstance()->GetIndexById(CRecipeDB::GetInstance()->GetRecipeById(m_RecipeId)->result);// 여기서 터졌었습
        m_ItemIcon->Set_TextureIndex(TextureIndex);
        auto iconTransform = m_ItemIcon->Get_Transform();
        iconTransform->Set_Pos(_vec3(
            m_TransformCom->Get_Pos().x,
            m_TransformCom->Get_Pos().y,
            m_TransformCom->Get_Pos().z));
        m_ItemIcon->Get_Transform()->Set_Scale(_vec3(30.f, 30.f, 0.f));
        m_ItemIcon->Render_GameObject();
    }
}

void CUICookedFoodCell::Render_Editor()
{
    CUI::Render_Editor();

    
}

HRESULT CUICookedFoodCell::Add_Component()
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

CUICookedFoodCell* CUICookedFoodCell::Create(DEVICE graphicDev)
{
    auto cookedFoodCell = new CUICookedFoodCell(graphicDev);

    if (FAILED(cookedFoodCell->Ready_GameObject()))
    {
        Safe_Release(cookedFoodCell);
        MSG_BOX("UI_BuildMenu Create Failed");
        return nullptr;
    }
    return cookedFoodCell;
}

void CUICookedFoodCell::Free()
{
    Safe_Release(m_ItemIcon);
    CUI::Free();
}
