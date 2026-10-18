#include "pch.h"
#include "CUIPlayerPage.h"
#include "CManagement.h"
#include "CCombatStat.h"
#include "CUIPlayerImage.h"
#include "CCreateHelper.h"
#include "CTransform.h"
#include "CRcTex.h"
#include "CTexture.h"
#include "CUIIcon.h"
#include "CFontMgr.h"
#include "CUIPlayerPanel.h"
#include "CPlayer.h"
#include "CUI.h"
#include "CUITabMenu.h"
#include "CWeaponEquip.h"

CUIPlayerPage::CUIPlayerPage(DEVICE pGraphicDev, CUI* owner)
    : CUI(pGraphicDev),
      m_PlayerUI(nullptr),
      m_PlayerBG(nullptr),
      m_LockUI(nullptr),
      m_UnlockUI(nullptr),
      m_OutfitUI(nullptr),
      m_WeaponUI(nullptr),
      m_PlayerPanel(nullptr)
{
    m_Owner = owner;
}

CUIPlayerPage::CUIPlayerPage(const CUIPlayerPage& rhs, CUI* owner)
    : CUI(rhs),
      m_PlayerUI(nullptr),
      m_PlayerBG(nullptr),
      m_LockUI(nullptr),
      m_UnlockUI(nullptr),
      m_OutfitUI(nullptr),
      m_WeaponUI(nullptr),
      m_PlayerPanel(nullptr)
{
    m_Owner = owner;
}

CUIPlayerPage::~CUIPlayerPage()
{}

HRESULT CUIPlayerPage::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    m_TransformCom->Set_Scale(450.f, 400.f, 1.f);
    _vec3 pos = m_TransformCom->Get_Pos();

    //플레이어 UI
    m_PlayerUI = CUIPlayerImage::Create(m_GraphicDev);
    dynamic_cast<CTransform*>(m_PlayerUI->Get_Component(COMPONENTID::ID_DYNAMIC, COMPONENTTYPE::TRANSFORM))->Set_Scale(80.f, 80.f, 0.f);

    //BG UI
    m_PlayerBG = CUIIcon::Create(m_GraphicDev, COMPONENTTYPE::TEX_UI_ICON);
    dynamic_cast<CTransform*>(m_PlayerBG->Get_Component(COMPONENTID::ID_DYNAMIC, COMPONENTTYPE::TRANSFORM))->Set_Scale(200.f, 200.f, 0.f);
    m_PlayerBG->Set_TextureIndex(2);
    m_PlayerBG->SetRegister(false);

    //Lock UI
    m_LockUI = CUIIcon::Create(m_GraphicDev, COMPONENTTYPE::TEX_UI_ICON);
    dynamic_cast<CTransform*>(m_LockUI->Get_Component(COMPONENTID::ID_DYNAMIC, COMPONENTTYPE::TRANSFORM))->Set_Scale(35.f, 35.f, 0.f);
    m_LockUI->SetRegister(false);

    //Cloth UI
    m_OutfitUI = CUIIcon::Create(m_GraphicDev, COMPONENTTYPE::TEX_UI_ICON);
    dynamic_cast<CTransform*>(m_OutfitUI->Get_Component(COMPONENTID::ID_DYNAMIC, COMPONENTTYPE::TRANSFORM))->Set_Scale(35.f, 35.f, 0.f);
    m_OutfitUI->SetRegister(false);

    //Unlock UI
    m_UnlockUI = CUIIcon::Create(m_GraphicDev, COMPONENTTYPE::TEX_UI_ICON);
    dynamic_cast<CTransform*>(m_UnlockUI->Get_Component(COMPONENTID::ID_DYNAMIC, COMPONENTTYPE::TRANSFORM))->Set_Scale(35.f, 35.f, 0.f);
    m_UnlockUI->SetRegister(false);

    //Weapon UI
    m_WeaponUI = CUIIcon::Create(m_GraphicDev, COMPONENTTYPE::TEX_UI_ICON);
    dynamic_cast<CTransform*>(m_WeaponUI->Get_Component(COMPONENTID::ID_DYNAMIC, COMPONENTTYPE::TRANSFORM))->Set_Scale(40.f, 40.f, 0.f);
    m_WeaponUI->SetRegister(false);

    return S_OK;
}

_int CUIPlayerPage::Update_GameObject(const _float& timeDelta)
{
    int exit = CUI::Update_GameObject(timeDelta);

    _vec3 pos              = m_TransformCom->Get_Pos();
    _vec2 backgroundOffset = { 0.f, -80.f };
    _vec2 playerOffset     = { 0.f, -80.f };
    _vec2 LockOffset       = { 0.f, -270.f };
    _vec2 UnlockOffset     = { 100.f, -270.f };
    _vec2 OufitOffset      = { -100.f, -270.f };
    _vec2 HeartOffset      = { 0.f, 100.f };

    dynamic_cast<CTransform*>(m_PlayerUI->Get_Component(COMPONENTID::ID_DYNAMIC, COMPONENTTYPE::TRANSFORM))->Set_Pos(
        pos.x,
        pos.y + backgroundOffset.y,
        0.f);
    m_PlayerUI->Update_GameObject(timeDelta);
    dynamic_cast<CTransform*>(m_PlayerBG->Get_Component(COMPONENTID::ID_DYNAMIC, COMPONENTTYPE::TRANSFORM))->Set_Pos(
        pos.x,
        pos.y + playerOffset.y,
        0.f);
    m_PlayerBG->Update_GameObject(timeDelta);
    dynamic_cast<CTransform*>(m_LockUI->Get_Component(COMPONENTID::ID_DYNAMIC, COMPONENTTYPE::TRANSFORM))->Set_Pos(pos.x, pos.y + LockOffset.y, 0.f);
    m_LockUI->Update_GameObject(timeDelta);
    dynamic_cast<CTransform*>(m_PlayerPanel->Get_Component(COMPONENTID::ID_DYNAMIC, COMPONENTTYPE::TRANSFORM))->Set_Pos(
        pos.x,
        pos.y + HeartOffset.y,
        0.f);
    m_PlayerPanel->Update_GameObject(timeDelta);
    dynamic_cast<CTransform*>(m_OutfitUI->Get_Component(COMPONENTID::ID_DYNAMIC, COMPONENTTYPE::TRANSFORM))->Set_Pos(
        pos.x + OufitOffset.x,
        pos.y + OufitOffset.y,
        0.f);
    m_OutfitUI->Update_GameObject(timeDelta);
    dynamic_cast<CTransform*>(m_UnlockUI->Get_Component(COMPONENTID::ID_DYNAMIC, COMPONENTTYPE::TRANSFORM))->Set_Pos(
        pos.x + UnlockOffset.x,
        pos.y + UnlockOffset.y,
        0.f);
    m_UnlockUI->Update_GameObject(timeDelta);
    dynamic_cast<CTransform*>(m_WeaponUI->Get_Component(COMPONENTID::ID_DYNAMIC, COMPONENTTYPE::TRANSFORM))->Set_Pos(
        pos.x + UnlockOffset.x,
        pos.y + UnlockOffset.y,
        0.f);
    m_WeaponUI->Update_GameObject(timeDelta);

    dynamic_cast<CTransform*>(m_WeaponUI->Get_Component(COMPONENTID::ID_DYNAMIC, COMPONENTTYPE::TRANSFORM))->Set_Scale(38.f, 35.f, 0.f);
    return exit;
}

void CUIPlayerPage::LateUpdate_GameObject(const _float& timeDelta)
{
    CUI::LateUpdate_GameObject(timeDelta);
}

void CUIPlayerPage::Render_GameObject()
{
    CUI::Render_GameObject();

    m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());

    //인벤토리 배경 그리기
    m_TextureCom->Set_Texture(0);
    m_BufferCom->Render_Buffer();

    //플레이어 뒷배경 그리기
    m_PlayerBG->Render_GameObject();

    //플레이어 그리기
    m_PlayerUI->Render_GameObject();

    //잠금 UI 그리기
    m_LockUI->Set_TextureIndex(3);
    m_LockUI->Render_GameObject();

    //옷 UI 그리기
    m_OutfitUI->Set_TextureIndex(4);
    m_OutfitUI->Render_GameObject();

    //Unlock UI 그리기
    m_UnlockUI->Set_TextureIndex(5);
    m_UnlockUI->Render_GameObject();

    //Weapon UI :
    CScene*       scene       = CManagement::GetInstance()->Get_Scene();
    CWeaponEquip* weaponEquip = dynamic_cast<CWeaponEquip*>(scene->Get_Component(COMPONENTID::ID_STATIC,
                                                                                 LAYERTYPE::GAMELOGIC,
                                                                                 OBJTYPE::PLAYER,
                                                                                 COMPONENTTYPE::WEAPON_EQUIP));

    if (weaponEquip->Get_WeaponEquipped())
    {
        switch (weaponEquip->Get_CurrentWeaponID())
        {
        case 3001:
            m_WeaponUI->Set_TextureIndex(6);
            m_WeaponUI->Render_GameObject();
            break;
        case 3002:
            m_WeaponUI->Set_TextureIndex(7);
            m_WeaponUI->Render_GameObject();
            break;
        }
    }

    //폰트 그리기
    _vec2 invenPos   = { m_TransformCom->Get_Pos().x, m_TransformCom->Get_Pos().y };
    _vec2 fontOffset = { 0.f, - 150.f };
    _vec2 screen     = { WINCX / 2 + invenPos.x + fontOffset.x, WINCY / 2 - invenPos.y + fontOffset.y }; //폰트 출력을 위해 스크린 좌표로 변환

    CFontMgr::GetInstance()->Render_Font(L"NanumSquare_Heavy",
                                         L"쥬신이",
                                         &screen,
                                         D3DXCOLOR(1.f, 1.f, 1.f, 1.f));

    //하트 그리기
    m_PlayerPanel->Render_GameObject();
}

void CUIPlayerPage::Initialize()
{
    //플레이어 하트 UI
    m_PlayerPanel                 = CUIPlayerPanel::Create(m_GraphicDev);
    CCombatStat* playerCombatstat = dynamic_cast<CUITabMenu*>(m_Owner)->Get_PlayerCombatStat();
    if (!playerCombatstat)
        return;
    m_PlayerPanel->Set_PlayerCombatStat(playerCombatstat);
    m_PlayerPanel->Set_Resister(false);
    dynamic_cast<CTransform*>(m_PlayerPanel->Get_Component(COMPONENTID::ID_DYNAMIC, COMPONENTTYPE::TRANSFORM))->Set_Pos(-390.f, 200.f, 0.f);

    return;
}

HRESULT CUIPlayerPage::Add_Component()
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

CUIPlayerPage* CUIPlayerPage::Create(DEVICE graphicDev, CUI* owner)
{
    CUIPlayerPage* uiPlayerPage = new CUIPlayerPage(graphicDev, owner);

    if (FAILED(uiPlayerPage->Ready_GameObject()))
    {
        Safe_Release(uiPlayerPage);
        MSG_BOX("uiPlayerPage Created Failed");
        return nullptr;
    }

    return uiPlayerPage;
}

void CUIPlayerPage::Free()
{
    Safe_Release(m_PlayerUI);
    Safe_Release(m_PlayerBG);
    Safe_Release(m_LockUI);
    Safe_Release(m_UnlockUI);
    Safe_Release(m_OutfitUI);
    Safe_Release(m_WeaponUI);
    Safe_Release(m_PlayerPanel);
    CUI::Free();
}
