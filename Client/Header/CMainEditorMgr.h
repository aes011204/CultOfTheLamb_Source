#pragma once
#include "CBase.h"

class CCameraController;
class CMapEditorPlacer;
class CMapObjectBase;
class CMapObjectGrass;
class CTerrainRenderer;
class CWallRenderer;
class CTerrain;

BEGIN(Engine)
class CEditContext;
class CGameObject;
END

class CMainEditorMgr : public CBase
{
    enum class MAPTOOL_VIEW
    {
        NONE,
        TERRAIN,
        MAP_OBJECT,
        WALL,
        MONSTER_SPAWNER,
        FOG_RENDERER,

        MAPTOOL_END
    };

    DECLARE_SINGLETON(CMainEditorMgr)

private:
    explicit CMainEditorMgr() = default;
    virtual ~CMainEditorMgr();

public:
    void Ready_MainEditor(DEVICE graphicDev, CEditContext* ctx);

    // 매 프레임 ImGui로 메인 에디터 창 그리는 부분
    void Update_MainEditor(const _float& timeDelta);
    void Render_MainEditor();

    void Set_MainCamera(CCameraController* cam) { m_MainCameraController = cam; }
    CCameraController* Get_MainCameraControl() { return m_MainCameraController; }

    _bool IsPlaying() const { return m_IsPlaying; }

    _float Get_TimeScale() const { return m_TimeScale; }
    void   Set_TimeScale(_float timeScale) { m_TimeScale = timeScale; }


    _bool IsGuiVisible() const { return m_ShowImGui; }
    void  Toggle_ImGui() { m_ShowImGui = !m_ShowImGui; }

private:
    // Context 세팅
    void Set_EditContext(CEditContext* ctx);

    void Render_SceneTab();
    void Render_MapToolTab();
    void Render_SceneChangeTab();

    // Editor Render
    void Render_ToolBar();
    void Render_Hierarchy();
    void Render_Inspector();

    void Render_DynamicComponents(CGameObject* obj);
    void Render_StaticComponents(CGameObject* obj);

    void Render_SelectComponent();

    CTerrainRenderer* Find_TerrainRenderer();
    CWallRenderer*    Find_WallRenderer();
    CTerrain*         Find_Terrain();

protected:
    virtual void Free() override;

private:
    DEVICE             m_GraphicDev = nullptr;
    CEditContext*      m_Context    = nullptr;
    CCameraController* m_MainCameraController = nullptr;

    _bool              m_IsPlaying = true;
    _bool              m_ShowImGui = true;

    MAPTOOL_VIEW m_MapToolView = MAPTOOL_VIEW::TERRAIN;

    _float             m_TimeScale = 1.f;

};

