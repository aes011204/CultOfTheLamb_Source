#pragma once
#include "CNPCBase.h"

class CFollowerElephant : public CNPCBase
{
private:
    explicit CFollowerElephant(DEVICE graphicDev);
    explicit CFollowerElephant(const CFollowerElephant& rhs);
    ~CFollowerElephant() override;

public:
    HRESULT Ready_GameObject() override;
    void    Initialize_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;

private:
    HRESULT Add_Component() override;
    void    Animation_Setting() override;

public:
    static CFollowerElephant* Create(DEVICE graphicDev);

private:
    void Free() override;
};
