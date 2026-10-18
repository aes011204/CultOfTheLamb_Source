#pragma once
#include "CUI.h"


enum class HEARTSTATE { EMPTY, HALF, FULL };

class CPlayer;

BEGIN(Engine)
class CRcTex;
class CTexture;
class CTransform;

class CUIHeartBar : public CUI
{
public:
    explicit CUIHeartBar(DEVICE graphicDev);
    explicit CUIHeartBar(const CUIHeartBar& rhs);
    ~CUIHeartBar() override;

public:
    HRESULT Ready_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;

    void    Render_GameObject() override;

    HRESULT Add_Component();

public:
    static CUIHeartBar* Create(DEVICE graphicDev);
    
    void Set_State(HEARTSTATE state) { m_HeartState = state; }
    void Set_Hp(float Hp, float maxHp) { m_Hp = Hp, m_maxHp = maxHp; }

private:
   CRcTex*             m_BufferCom;
   CTexture*           m_TextureCom;
   CTransform*         m_TransformCom;
   CPlayer*            m_Player;
   float              m_Hp;
   float               m_maxHp;

   static int          m_nextID;
   HEARTSTATE         m_HeartState;

protected:
    void Free() override;
};

END
