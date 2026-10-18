#include "pch.h"
#include "CGameObject.h"
#include "CUIItemGetManager.h"
#include "CUIItemGet.h"
#include "CInventory.h"
#include "CRenderer.h"
#include "CTransform.h"

CUIItemGetManager::CUIItemGetManager(DEVICE graphicDev)
    : CGameObject(graphicDev),
    m_Inventory(nullptr),
    m_IsAnyShifting(false),
    m_NeedsReindex(false)
{
}

CUIItemGetManager::CUIItemGetManager(const CUIItemGetManager& rhs)
    : CGameObject(rhs),
    m_Inventory(nullptr),
    m_IsAnyShifting(false),
    m_NeedsReindex(false)
{
}

CUIItemGetManager::~CUIItemGetManager()
{

}

HRESULT CUIItemGetManager::Ready_GameObject()
{


    return S_OK;
}
void CUIItemGetManager::Initialize()
{
    m_Inventory->SetOnItemAdded([this](ItemInstance* itemInstance/*, int itemCount*/) {this->Add_UIItemGet(itemInstance, 1); }); // 아이템은 1 개씩 처리 로직이라 기본값으로 1
}
_int CUIItemGetManager::Update_GameObject(const _float& timeDelta)
{
    int exit = CGameObject::Update_GameObject(timeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

    m_IsAnyShifting = false;

    if (m_UIItemGets.empty())
        return 0;

    for (auto iter = m_UIItemGets.begin(); iter != m_UIItemGets.end();)
    {

        if ((*iter)->Get_IsShifting())
        {
            m_IsAnyShifting = true;
        }

        if ((*iter)->Get_IsDead())
        {
            Safe_Release(*iter);
            iter = m_UIItemGets.erase(iter);
        }
        else
        {
            (*iter)->Update_GameObject(timeDelta);
            iter++;
        }

    }

    int index = 0;

    if (!m_IsAnyShifting)
    {

        for (auto iter = m_UIItemGets.begin(); iter != m_UIItemGets.end(); iter++)
        {
            (*iter)->Set_Index(index);
            index++;
        }
    }
    cout << m_UIItemGets.size() << endl;


    return exit;
}
void CUIItemGetManager::LateUpdate_GameObject(const _float& timeDelta)
{
    CGameObject::LateUpdate_GameObject(timeDelta);

    if (!m_UIItemGets.empty())
    {
        for (auto iter = m_UIItemGets.begin(); iter != m_UIItemGets.end();)
        {
            (*iter)->LateUpdate_GameObject(timeDelta);
            iter++;
        }
    }
}

void CUIItemGetManager::Render_GameObject()
{

    CGameObject::Render_GameObject();

    if (!m_UIItemGets.empty())
    {
        for (auto iter = m_UIItemGets.begin(); iter != m_UIItemGets.end();)
        {

            (*iter)->Render_GameObject();
            iter++;
        }
    }

}

void CUIItemGetManager::Add_UIItemGet(ItemInstance* itemInstance, int itemCount)

{
    for (auto iter = m_UIItemGets.begin(); iter != m_UIItemGets.end(); iter++)
    {
        
        if ((*iter)->Get_UIItem().itemId == itemInstance->itemId)
        {
            (*iter)->Set_UIItem(itemInstance->itemId, itemCount);
            return; 
        }
    }

    CUIItemGet* m_UIItemGet = CUIItemGet::Create(m_GraphicDev);

    if (m_UIItemGet)
    {
        m_UIItemGet->Set_UIItem(itemInstance->itemId, itemCount);
        m_UIItemGet->Set_Index(m_UIItemGets.size());
        m_UIItemGets.push_back(m_UIItemGet);
    }

}


CUIItemGetManager* CUIItemGetManager::Create(DEVICE graphicDev)
{
    CUIItemGetManager* uiItemGetManager = new CUIItemGetManager(graphicDev);

    if (FAILED(uiItemGetManager->Ready_GameObject()))
    {
        Safe_Release(uiItemGetManager);
        MSG_BOX("UI_ItemGetManager Create Failed");
        return nullptr;
    }
    return uiItemGetManager;
}

void CUIItemGetManager::Free()
{
    if (!m_UIItemGets.empty())
    {
        for_each(m_UIItemGets.begin(), m_UIItemGets.end(), CDeleteObj());
   
    }

    CGameObject::Free();

}
