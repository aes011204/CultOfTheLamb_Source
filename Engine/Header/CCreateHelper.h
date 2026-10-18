#pragma once
#include "CEnumHelper.h"
#include "CManagement.h"
#include "CProtoMgr.h"
#include "CScene.h"

BEGIN(Engine)
class CGameObject;

template <typename T>
static T* CreateProtoComponent(CGameObject* Owner, COMPONENTTYPE eComponentType)
{
    if (Owner == nullptr)
        return nullptr;

    auto proto = Engine::CProtoMgr::GetInstance()->Clone_Prototype(eComponentType);
    if (proto == nullptr)
        return nullptr;

    T* component = dynamic_cast<T*>(proto);

    if (component == nullptr)
    {
        Safe_Release(proto);
        return nullptr;
    }

    component->Set_Owner(Owner);
    component->Set_Type(eComponentType);

    const char* tempChar = ToString(eComponentType);
    wstring componentName = CharToWString(tempChar);

    component->Set_Name(componentName);

    return component;
}

static void SpawnObjectOnLayer(CGameObject* obj, OBJTYPE objType = OBJTYPE::ATTACK_COL)
{
    auto mgr = CManagement::GetInstance();
    auto scene = mgr ? mgr->Get_Scene() : nullptr;

    if (scene == nullptr)
        return;

    scene->AddObjectOnLayer(LAYERTYPE::GAMELOGIC, obj, objType);
}

static void SpawnUIOnLayer(CGameObject* obj, OBJTYPE objType = OBJTYPE::UI)
{
    auto mgr = CManagement::GetInstance();
    auto scene = mgr ? mgr->Get_Scene() : nullptr;

    if (!scene || !obj)
        return;

    scene->AddObjectOnLayer(LAYERTYPE::UI, obj, objType);
}

static void DrawDebugBox(DEVICE graphicDv, const _vec3& center, const _vec3& size, DWORD color)
{
    if (!graphicDv) return;

    _vec3 vMin = center - (size * 0.5f);
    _vec3 vMax = center + (size * 0.5f);

    DWORD dwPrevFillMode = 0;
    graphicDv->GetRenderState(D3DRS_FILLMODE, &dwPrevFillMode);

    BOOL bPrevLighting = FALSE;
    graphicDv->GetRenderState(D3DRS_LIGHTING, (DWORD*)&bPrevLighting);

    D3DXMATRIX matPrevWorld;
    graphicDv->GetTransform(D3DTS_WORLD, &matPrevWorld);

    D3DXMATRIX matIdentity;
    D3DXMatrixIdentity(&matIdentity);
    graphicDv->SetTransform(D3DTS_WORLD, &matIdentity);

    // 렌더 상태 설정
    graphicDv->SetRenderState(D3DRS_LIGHTING, FALSE);
    graphicDv->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
    graphicDv->SetTexture(0, nullptr);
    graphicDv->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);

    // 버텍스 구조체
    struct DebugVtx { _vec3 v; DWORD color; };

    // 박스의 8개 꼭지점
    DebugVtx v[8];
    v[0].v = { vMin.x, vMin.y, vMin.z }; v[0].color = color;
    v[1].v = { vMin.x, vMax.y, vMin.z }; v[1].color = color;
    v[2].v = { vMax.x, vMax.y, vMin.z }; v[2].color = color;
    v[3].v = { vMax.x, vMin.y, vMin.z }; v[3].color = color;

    v[4].v = { vMin.x, vMin.y, vMax.z }; v[4].color = color;
    v[5].v = { vMin.x, vMax.y, vMax.z }; v[5].color = color;
    v[6].v = { vMax.x, vMax.y, vMax.z }; v[6].color = color;
    v[7].v = { vMax.x, vMin.y, vMax.z }; v[7].color = color;

    // 12개의 선 (24개 버텍스)
    DebugVtx lines[24];

    // 앞면 사각형
    lines[0] = v[0]; lines[1] = v[1];
    lines[2] = v[1]; lines[3] = v[2];
    lines[4] = v[2]; lines[5] = v[3];
    lines[6] = v[3]; lines[7] = v[0];

    // 뒷면 사각형
    lines[8] = v[4]; lines[9] = v[5];
    lines[10] = v[5]; lines[11] = v[6];
    lines[12] = v[6]; lines[13] = v[7];
    lines[14] = v[7]; lines[15] = v[4];

    // 앞뒤 연결선
    lines[16] = v[0]; lines[17] = v[4];
    lines[18] = v[1]; lines[19] = v[5];
    lines[20] = v[2]; lines[21] = v[6];
    lines[22] = v[3]; lines[23] = v[7];

    // 그리기
    graphicDv->DrawPrimitiveUP(
        D3DPT_LINELIST,
        12,  // 12개의 선
        lines,
        sizeof(DebugVtx));

    // 렌더 상태 복원
    graphicDv->SetTransform(D3DTS_WORLD, &matPrevWorld);
    graphicDv->SetRenderState(D3DRS_FILLMODE, dwPrevFillMode);
    graphicDv->SetRenderState(D3DRS_LIGHTING, bPrevLighting);
}

static void DrawDebugSphere(DEVICE graphicDev, const _vec3& center, _float radius, DWORD color, _int segments = 32)
{
    if (!graphicDev) return;

    // 렌더 상태 백업
    DWORD prevFillMode = 0;
    graphicDev->GetRenderState(D3DRS_FILLMODE, &prevFillMode);

    BOOL prevLighting = FALSE;
    graphicDev->GetRenderState(D3DRS_LIGHTING, (DWORD*)&prevLighting);

    D3DXMATRIX prevWorld;
    graphicDev->GetTransform(D3DTS_WORLD, &prevWorld);

    // 항등 행렬 설정
    D3DXMATRIX matIdentity;
    D3DXMatrixIdentity(&matIdentity);
    graphicDev->SetTransform(D3DTS_WORLD, &matIdentity);

    // 렌더 상태 설정
    graphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
    graphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
    graphicDev->SetTexture(0, nullptr);
    graphicDev->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);

    struct DebugVtx { _vec3 v; DWORD color; };

    auto DrawCircle = [&](_int plane)
        {
            DebugVtx* v = new DebugVtx[segments + 1];

            for (int i = 0; i <= segments; ++i)
            {
                float t = (2.f * D3DX_PI * i) / segments;
                float ct = cosf(t);
                float st = sinf(t);

                switch (plane)
                {
                case 0: // XZ 평면 (바닥)
                    v[i].v = { center.x + ct * radius, center.y, center.z + st * radius };
                    break;

                case 1: // XY 평면 (정면)
                    v[i].v = { center.x + ct * radius, center.y + st * radius, center.z };
                    break;

                case 2: // YZ 평면 (측면)
                    v[i].v = { center.x, center.y + ct * radius, center.z + st * radius };
                    break;
                }

                v[i].color = color;
            }

            graphicDev->DrawPrimitiveUP(D3DPT_LINESTRIP, segments, v, sizeof(DebugVtx));

            delete[] v;
        };

    DrawCircle(0); // XZ
    DrawCircle(1); // XY
    DrawCircle(2); // YZ

    graphicDev->SetTransform(D3DTS_WORLD, &prevWorld);
    graphicDev->SetRenderState(D3DRS_FILLMODE, prevFillMode);
    graphicDev->SetRenderState(D3DRS_LIGHTING, prevLighting);
}

END
