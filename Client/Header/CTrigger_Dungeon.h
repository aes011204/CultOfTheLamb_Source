#pragma once

#include "CTrigger.h"

class CTrigger_Dungeon : public CTrigger
{
private:
    explicit CTrigger_Dungeon(DEVICE graphicDev);
    explicit CTrigger_Dungeon(const CTrigger_Dungeon& rhs);
    virtual ~CTrigger_Dungeon();

public:
    HRESULT Ready_GameObject() override;
    void    Render_GameObject() override;
    void    Render_Editor() override;

    void OnBeginOverlap(CCollider* self, CCollider* other) override;

public:
    static CTrigger_Dungeon* Create(DEVICE graphicDev);

private:
    void Free() override;


};

