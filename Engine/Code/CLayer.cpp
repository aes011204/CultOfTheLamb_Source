#include "CLayer.h"

#include "CGameObject.h"
#include "CEnumhelper.h"
#include "CUI.h"

CLayer::CLayer()
{ }

CLayer::~CLayer()
{ }

CComponent* CLayer::Get_Component(COMPONENTID componentID, OBJTYPE objType, COMPONENTTYPE componentType)
{
    auto iter = m_Objects.find(objType);

    if (iter == m_Objects.end())
        return nullptr;

    for (auto obj : iter->second)
    {
        CComponent* comp = obj->Get_Component(componentID, componentType);
        if (comp) return comp;
    }

    return nullptr;
}

HRESULT CLayer::Add_GameObject(OBJTYPE objType, CGameObject* gameObject)
{
    if (nullptr == gameObject)
        return E_FAIL;

    Object_NameSetting(objType, gameObject);

    m_Objects[objType].push_back(gameObject);

    return S_OK;
}

HRESULT CLayer::Ready_Layer()
{
    return S_OK;
}

void CLayer::Initialize_Layer()
{
    for (auto& pair : m_Objects)
    {
        for (auto& obj : pair.second)
        {
            obj->Initialize_GameObject();
        }
    }
}

_int CLayer::Update_Layer(const _float& timeDelta)
{
	_int result = 0;

	for (auto& pair : m_Objects)
	{
		vector<CGameObject*> objectsCopy = pair.second;

		for (auto* obj : objectsCopy)
		{

			if (obj->Is_PendingDestroy())
				continue;

			result = obj->Update_GameObject(timeDelta);

			if (result & 0x80000000)
				return result;
		}
	}

	return result;
}

void CLayer::LateUpdate_Layer(const _float& timeDelta)
{
    for (auto mapIter = m_Objects.begin(); mapIter != m_Objects.end(); ++mapIter)
    {
        auto& objList = mapIter->second;

        for (auto iter = objList.begin(); iter != objList.end(); )
        {
            CGameObject* obj = *iter;

            if (obj == nullptr)
            {
                iter = objList.erase(iter);
                continue;
            }

            obj->LateUpdate_GameObject(timeDelta);

            if (obj->Is_PendingDestroy())
            {
                // 참고카운트 줄이고, 삭제
                Safe_Release(obj);
                iter = objList.erase(iter);
            }
            else
            {
                ++iter;
            }
        }
    }
}

void CLayer::Object_NameSetting(OBJTYPE objType, CGameObject* gameObject)
{
    // 타입별로 번호 세팅
    // Ex) Monster, Monster2, Monster3..

    if (!gameObject)
        return;

    // UI용 예외처리. UI는 생성자에서 직접 세팅을 해줘야함
    // PlayerPanel 생성자 참고하기
    if (gameObject->Get_Name() != L"Default_GameObject")
        return;

    const char* baseName = ::ToString(objType);
    wstring wBaseName = CharToWString(baseName);

    _uint index = ++m_NameCounter[objType];

    wstring finalName = L"";

    if (index == 1)
        finalName = wBaseName;

    else
        finalName = wBaseName + L"_" + to_wstring(index);

    gameObject->Set_Name(finalName);
}

CLayer* CLayer::Create()
{
    auto layer = new CLayer;

    if (FAILED(layer->Ready_Layer()))
    {
        MSG_BOX("Layer Create Failed");
        Safe_Release(layer);
        return nullptr;
    }

    return layer;
}

void CLayer::Free()
{
    for (auto& pair : m_Objects)
    {
        for (auto* obj : pair.second)
        {
            Safe_Release(obj);
        }
    }
    m_Objects.clear();
}
