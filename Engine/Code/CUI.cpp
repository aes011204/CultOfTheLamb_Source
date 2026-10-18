#include "CUI.h"
#include "CDInputMgr.h"

CUI::CUI(DEVICE graphicDev)
    : CGameObject(graphicDev),
      m_TransformCom(nullptr),
      m_TextureCom(nullptr),
      m_BufferCom(nullptr), m_Invisible(false),
      m_ID(0)
{ }

CUI::CUI(const CUI& rhs)
    : CGameObject(rhs),
      m_TransformCom(nullptr),
      m_TextureCom(nullptr),
      m_BufferCom(nullptr), m_Invisible(false),
      m_ID(0)
{ }

CUI::~CUI()
{ }

HRESULT CUI::Ready_GameObject()
{
    CGameObject::Ready_GameObject();

    return S_OK;
}

_int CUI::Update_GameObject(const _float& timeDelta)
{

    CGameObject::Update_GameObject(timeDelta);

    return S_OK;
}

void CUI::LateUpdate_GameObject(const _float& timeDelta)
{
    CGameObject::LateUpdate_GameObject(timeDelta);
    
}

void CUI::Render_GameObject()
{
    CGameObject::Render_GameObject();

    
}

void CUI::Free()
{
    CGameObject::Free();
}
