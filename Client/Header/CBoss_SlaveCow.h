#pragma once

#include "CCharacterBase.h"

BEGIN(Engine)
class CTexture;
class CAnimator;
END

class CBoss_SlaveCow : public CCharacterBase
{
private:
    enum Slave_AnimState
    {
        Appear, Idle, Freed, End
    };

private:
    explicit CBoss_SlaveCow(DEVICE graphicDev);
    explicit CBoss_SlaveCow(const CBoss_SlaveCow& rhs);
    ~CBoss_SlaveCow() override;

public:
    HRESULT Ready_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;

private:
    void    Animation_Setting() override;
    HRESULT Add_Component()  override;

public:
    static CBoss_SlaveCow* Create(DEVICE graphicDev);

private:
    void Free() override;

    Slave_AnimState m_SlaveAnimState = Slave_AnimState::Appear;

};

