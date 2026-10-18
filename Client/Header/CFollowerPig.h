#pragma once
#include "CNPCBase.h"

class CFollowerPig : public CNPCBase
{
private:
    explicit CFollowerPig(DEVICE graphicDev);
    explicit CFollowerPig(const CFollowerPig& rhs);
    ~CFollowerPig() override;

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
    static CFollowerPig* Create(DEVICE graphicDev);

private:
    void Free() override;
};
