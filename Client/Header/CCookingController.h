#pragma once
#include <queue>
#include "CComponent.h"
#include "Client_Struct.h"
#include "RecipeData.h"

class CUIButton;
class CInventory;

class CCookingController : public CComponent
{
private:
    explicit CCookingController(DEVICE graphicDev);

    explicit CCookingController(const CCookingController& rhs);
    ~CCookingController() override;

public:
    virtual HRESULT Ready_Inventory();
    void            Initialize_Component() override;
    _int            Update_Component(const _float& timeDelta) override;
    void            LateUpdate_Component() override;

    //UI
    void SetUIRecipeSlot(/*Slot* slot, */int id, CUIButton* quit, CUIButton* startCook); //데이터 설정(itemId) + 콜백 연결(onClick)만 해주는 초기화 함수
    void Set_Start(bool start) { m_bStart = start; }

    bool CanCook(int recipeID, CInventory* playerInven);// 이게 맞나..///
    bool AddRecipe(int id, CInventory* playerInven);

    bool RemoveAllRecipe(CInventory* playerInven);
    bool IsCooking() { return m_EnterFlag || !m_vecTmpRecipeID.empty() || !m_queRecipeInst.empty(); }

    //bool RemoveRecipe(int slotNum);// 이건 해야할까 말아야 할까
    vector<int> Get_Vector() { return m_vecTmpRecipeID; }

    //미니 게임
    bool ReloadForMiniGame();
    void DropDish(int dishID, FOODQUALITY quality);
    void Quit();

    bool m_EnterFlag = false;
    bool m_bStart    = false;
    // bool m_OnGame = false;
    bool m_bDone = false;

    //const vector<InventorySlot>& GetVector() const { return m_vecInven; }

public:
    static CCookingController* Create(DEVICE graphicDev);
    CComponent*                Clone() override;
private:
    //vector<Recipe*> RecipeShop;
    // 선택후
    vector<int>       m_vecTmpRecipeID;// 대기열
    queue<RecipeInst> m_queRecipeInst; // 주소로 안하는게 낫데 메모리 관리 신경 쓸 필요 없음

    CGameObject* m_pPlayerInven;

    int invenSlotNum = 12;
    int m_StockMax   = 0;

public:
    void Free() override;
};
