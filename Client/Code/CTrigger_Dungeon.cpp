#include "pch.h"
#include "CTrigger_Dungeon.h"
#include "CTransform.h"
#include "CBoxCollider.h"
#include "CPlayer.h"

CTrigger_Dungeon::CTrigger_Dungeon(DEVICE graphicDev)
    : CTrigger(graphicDev)
{
    

}

CTrigger_Dungeon::CTrigger_Dungeon(const CTrigger_Dungeon& rhs)
    : CTrigger(rhs)
{
    
}

CTrigger_Dungeon::~CTrigger_Dungeon()
{
}

HRESULT CTrigger_Dungeon::Ready_GameObject()
{
    return CTrigger::Ready_GameObject();
}

void CTrigger_Dungeon::Render_GameObject()
{
    CTrigger::Render_GameObject();

    // Transform 설정
    if (m_TransformCom)
        m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());

    // 버퍼 렌더링
    if (m_ColliderCom)
        m_ColliderCom->Render();
}

void CTrigger_Dungeon::Render_Editor()
{
    ImGui::Text("=== Dungeon Trigger ===");

   
}

void CTrigger_Dungeon::OnBeginOverlap(CCollider* self, CCollider* other)
{
    CTrigger::OnBeginOverlap(self, other);
}

CTrigger_Dungeon* CTrigger_Dungeon::Create(DEVICE graphicDev)
{
    auto trigger = new CTrigger_Dungeon(graphicDev);

    if (FAILED(trigger->Ready_GameObject()))
    {
        Safe_Release(trigger);
        MSG_BOX("CTrigger_Dungeon Create Failed");
        return nullptr;
    }

    return trigger;

}

void CTrigger_Dungeon::Free()
{
    CTrigger::Free();

}
