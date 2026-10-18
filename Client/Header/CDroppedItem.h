#pragma once
#include "CGameObject.h"
#include "ItemInstance.h"
#include "Client_Struct.h"

class CUIWeaponExplain;
class CPlayer;

BEGIN(Engine)
class CTransform;
class CBoxCollider;
class CRcTex;
class CTexture;
END

class CSpriteEff;
struct SpriteBounceDesc;

class CDroppedItem : public CGameObject
{
public:

private:
    explicit CDroppedItem(DEVICE graphicDev, ItemInstance itemInst);
    explicit CDroppedItem(const CDroppedItem& rhs);
    ~CDroppedItem() override;

public:
    HRESULT Ready_GameObject() override;
    HRESULT Ready_GameObject(_vec3 pos);
    void    Initialize_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;

    void OnBeginOverlap(CCollider* self, CCollider* other) override;
    void OnStayOverlap(CCollider* self, CCollider* other) override;
    void OnEndOverlap(CCollider* self, CCollider* other) override;

    void DisappearEff();


    void SetFoodQuality(FOODQUALITY quality) { m_foodQuality = quality; }
    static void Reset_WeaponExplainUI() { m_WeaponExplainUI = nullptr; }

    // 추가 : NPC가 음식 오브젝트를 인식하기 위해 필요한 ItemInstance 반환
    const ItemInstance& Get_ItemInst() const { return m_itemInst; }


private:
    HRESULT Add_Component();

private:
    Engine::CRcTex*             m_BufferCom;
    Engine::CTransform*         m_TransformCom;
    Engine::CTexture*           m_TextureCom;
    Engine::CBoxCollider*       m_BoxColCom;
    //Engine::CAnimator*       m_AnimationCom;
    static CUIWeaponExplain*    m_WeaponExplainUI;

    CSpriteEff* m_SpriteEff;
    // 추가
    Engine::CRcTex* m_Buffer_Cook = nullptr;
    Engine::CTexture* m_TextureComCook = nullptr;
public:
    static CDroppedItem* Create(DEVICE graphicDev, ItemInstance itemInst, _vec3 pos);

    void SetBounceDesc(SpriteBounceDesc& d) { m_BounceDesc = d; is_SetBounceDesc = true; } // 바닥완성 후 추가 작업 예정

private:
    void Player_ZoomIn(CPlayer* ply);

private:
    SpriteBounceDesc m_BounceDesc;
    bool is_SetBounceDesc = false;

    ItemInstance m_itemInst;
    int          m_index = 0;

    FOODQUALITY m_foodQuality = FOODQUALITY::NO_FOOD;

    float m_ignoreTime = 0;


private:
    void Free() override;
};
