#pragma once
#include "CUI.h"
#include <d3dx9.h>

BEGIN(Engine)
class CTransform;
END

class  CUIButton : public CUI
{
protected:
    explicit CUIButton(DEVICE graphicDev, int texOn, int texOff);
    explicit CUIButton(const CUIButton& rhs, int texOn, int texOff);
    ~CUIButton() override;

public:
    HRESULT Ready_GameObject();
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;
    HRESULT Add_Component();

    bool    Get_Selected() { return m_Selected; }
	void    Set_Selected(bool tf);

    void OnHoverStart();
    void OnHoverEnd();

protected:
    bool m_Hovered = false;
    //bool m_bClicked = false;
    bool m_Selected = false;
    RECT m_Rect;
public:
    static CUIButton* Create(DEVICE graphicDev, int texOn, int texOff);

    void        Set_TextureIndex(int textureOn, int textureOff) { m_TextureOn = textureOn; m_TextureOff = textureOff;}
    CTransform* Get_Transform() const { return m_TransformCom; }
    bool Get_Hovered() { return m_Hovered; }
    void Set_Hovered(bool tf) { m_Hovered = tf; }
	void Set_HasText() { m_HasText = true; }
    void SetOnClick(std::function<void()> callback) { m_OnClick = callback; }
	void Set_Text(const wstring& text) { m_Text = text; }
    bool SetCloneTexAspectScale(_vec3 vec);
private:
    std::function<void()> m_OnClick;

    static int m_NextID;
    int        m_TextureIndex;

    int m_TextureOn;
    int m_TextureOff;

	_vec2 m_TextOffset;
	bool m_HasText;
	D3DXCOLOR m_D3dxColor;
	wstring	m_Text;
private:
    void Free() override;


};


