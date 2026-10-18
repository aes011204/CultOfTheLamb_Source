#pragma once
#include "CBase.h"
#include "Engine_Define.h"

class CMainCamera;

BEGIN(Engine)
class CComponent;
class CLayer;
class CGameObject;

/// @brief 게임의 한 화면/상태를 표현하며 다수의 레이어를 관리하는 상위 클래스
/// @details
/// - 하나의 Scene 안의 레이어/오브젝트/컴포넌트까지의 업데이트 순서를 책임진다.
/// - 그래픽 디바이스를 공유하여 렌더링 자원 사용을 통합 관리한다.
class ENGINE_DLL CScene : public CBase
{
protected:
    explicit CScene(DEVICE graphicDev);
    ~CScene() override;

public:
    CComponent* Get_Component(COMPONENTID   componentID,
                              LAYERTYPE     layerType,
                              OBJTYPE       objType,
                              COMPONENTTYPE componentType);

public:
    virtual HRESULT Ready_Scene();
    virtual void    Initialize_Scene();
    virtual _int    Update_Scene(const _float& timeDelta);
    virtual void    LateUpdate_Scene(const _float& timeDelta);
    virtual void    Render_Scene() PURE;
    virtual HRESULT Ready_Light();

    void            AddObjectOnLayer(LAYERTYPE layerType, CGameObject* obj, OBJTYPE objType);

    // Editor
    const map<LAYERTYPE, CLayer*>& Get_Layers() const { return m_Layers; }
    CLayer*                        Get_Layer(LAYERTYPE type);

public:
    virtual void OnMonsterDeath(CGameObject* obj) {}
    _int         Get_KillCount() { return m_KillCount; }

protected:
    map<LAYERTYPE, CLayer*> m_Layers;
    DEVICE                  m_GraphicDev;

    _int                    m_KillCount = 0;

protected:
    void Free() override;
};

END
