#pragma once
#include "CUI.h"
#include "Client_Struct.h"
#include "Client_Enum.h"
#include <functional>

class CUIIcon;

class CUICookingGame :
    public CUI
{
private:
    explicit CUICookingGame(DEVICE graphicDev,CGameObject* owner, RecipeInst recipeInst);
    explicit CUICookingGame(const CUICookingGame& rhs) = delete;
    virtual ~CUICookingGame();

public:
    HRESULT Ready_GameObject() override;
    void    Initialize_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;

public:
    void SetStockMax(int maxNum) { m_StockMax = maxNum; }
    void SetDoneEvent(function<void()> doneEvent) { m_DoneEvent = doneEvent; }
    void SetResultItemDrop(function<void(int , FOODQUALITY)> DropDish) { m_ResultItemDrop = DropDish; }

    void Stencil();
private:
    HRESULT Add_Component();

    CGameObject* m_Owner = nullptr;
    CTransform* m_OwnerTransform = nullptr;


    /// 렌더ui

    CRcTex* m_Buffer_Deco = nullptr;
    CRcTex* m_Buffer_Info = nullptr;
    CRcTex* m_Buffer_keyInfo = nullptr;
    CRcTex* m_Buffer_blue = nullptr;

    RecipeInst  m_recipeInst;
    function<void()> m_DoneEvent;
    function<void(int, FOODQUALITY)> m_ResultItemDrop;

    CUIIcon* m_ItemIcon = nullptr;
    CUIIcon* m_Curser = nullptr;

    FOODQUALITY m_result = NO_FOOD;
    int m_StockMax = 0;
    _vec3 m_CurserPos;

    /// 미니게임 시스템
    float m_BaseLeft = 0;
    float m_BaseRight = 0;

    float m_StencilLeft = 0;
    float m_StencilRight = 0;

    float m_CurserX = 0;
    float m_Speed = 500;

    bool OnClick = false;
    float DelayTime = 0;

    // 스텐실
    int m_StencilLength = 0;
    int m_StencilPosX = 0;
    CRcTex* m_MaskBuffer = nullptr;

public:
    static CUICookingGame* Create(DEVICE graphicDev, CGameObject* owner, RecipeInst recipeInst);

protected:
    void Free() override;
};

