#pragma once

#include "CBase.h"
#include "Engine_Define.h"

BEGIN(Engine)

class CScene;
class CGameObject;
class CComponent;

class ENGINE_DLL CEditContext : public CBase
{
private:
    explicit CEditContext();
    virtual  ~CEditContext();

public:
    CScene*         Get_ActiveScene() const { return m_ActiveScene; }
    CGameObject*    Get_SelectedObject() const { return m_SelectedObject; }
    CComponent*     Get_SelectedComponent() const { return m_SelectedComponent; }

public:
    void Set_ActiveScene(CScene* scene) { m_ActiveScene = scene; }
    void Select_Object(CGameObject* obj);
    void Select_Component(CComponent* comp);

    static CEditContext* Create();

private:
    virtual void Free() override;

private:
    CScene*         m_ActiveScene;
    CGameObject*    m_SelectedObject;
    CComponent*     m_SelectedComponent;

};

END
