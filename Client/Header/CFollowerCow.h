#pragma once
#include "CNPCBase.h"

class CFollowerCow : public CNPCBase
{
private:
    explicit CFollowerCow(DEVICE graphicDev);
    explicit CFollowerCow(const CFollowerCow& rhs);
    ~CFollowerCow() override;

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
    static CFollowerCow* Create(DEVICE graphicDev);

private:
    void Free() override;
};
