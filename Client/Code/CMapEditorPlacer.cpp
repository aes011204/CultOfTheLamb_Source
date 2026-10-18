#include "pch.h"
#include "CMapEditorPlacer.h"
#include "CMapObject_Helper.h"
#include "CDInputMgr.h"
#include "CGameObject.h"
#include "CLayer.h"
#include "CManagement.h"
#include "CMapObjectMgr.h"
#include "CPickingHelper.h"
#include "CScene.h"
#include "CTransform.h"

CMapEditorPlacer::CMapEditorPlacer(DEVICE graphicDev)
    : m_GraphicDev(graphicDev),
    m_PreviewObj(nullptr),
    m_SelectedType(MAPOBJTYPE::MAPOBJ_END),
    m_Placing(false), m_EraseMode(false), m_GrassIndex(0), m_RubbleIndex(0), m_DecoIndex(0), m_DungeonDecoIndex(0),
    m_ETCIndex(0),
    m_PreviewScaleX(1.f), m_PreviewScaleY(1.f),
    m_PreviewRotation(0.f, 0.f, 0.f), m_PreviewYOffset(0)
{
    if (m_GraphicDev)
        m_GraphicDev->AddRef();
}

CMapEditorPlacer::CMapEditorPlacer(const CMapEditorPlacer& rhs)
    : m_GraphicDev(rhs.m_GraphicDev),
    m_PreviewObj(nullptr),
    m_SelectedType(MAPOBJTYPE::MAPOBJ_END),
    m_Placing(false), m_EraseMode(false), m_GrassIndex(0), m_RubbleIndex(0), m_DecoIndex(0), m_DungeonDecoIndex(0),
    m_ETCIndex(0),
    m_PreviewScaleX(1.f), m_PreviewScaleY(1.f),
    m_PreviewRotation(0.f, 0.f, 0.f), m_PreviewYOffset(0)
{
    if (m_GraphicDev)
        m_GraphicDev->AddRef();
}

CMapEditorPlacer::~CMapEditorPlacer()
{ }

void CMapEditorPlacer::Update_MapPlacer(const _float& timeDelta)
{
    if (m_EraseMode)
    {
        Update_EraseMode();
        return;
    }

    if (!m_Placing || m_PreviewObj == nullptr)
        return;
    
    // 마우스 -> 월드좌표
    _vec3 worldPos;
    if (Get_MouseWorldPos(worldPos))
    {
        _vec3 previewPos = worldPos;
        previewPos.y += m_PreviewYOffset;
        m_PreviewObj->Set_WorldPos(previewPos);
    }

    m_PreviewObj->Update_GameObject(timeDelta);
    m_PreviewObj->LateUpdate_GameObject(timeDelta);

    // ImGui 클릭 시에는 화면 클릭 무시
    /*ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse)
        return;*/

    // 좌클릭 하면 배치하기
    auto input = CDInputMgr::GetInstance();

    // == 마우스 휠 ==
    _long wheelDelta = input->Get_DIMouseMove(DIMS_Z);
    if (wheelDelta != 0)
    {
        CTransform* transform = dynamic_cast<CTransform*>(
            m_PreviewObj->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));

        if (transform)
        {
            if (input->KeyPress(DIK_LALT) && input->KeyPress(DIK_LCONTROL))
            {
                _float rotChange = (wheelDelta > 0 ? D3DXToRadian(1.f) : -D3DXToRadian(1.f));
                m_PreviewRotation.z += rotChange;

                // Z축 회전 제한
                m_PreviewRotation.z = fmodf(m_PreviewRotation.z, D3DX_PI * 2.0f);

                transform->Set_Rotation(m_PreviewRotation);
            }
            else if (input->KeyPress(DIK_LCONTROL) || input->KeyPress(DIK_RCONTROL))
            {
                _float rotChange = (wheelDelta > 0 ? D3DXToRadian(1.f) : -D3DXToRadian(1.f));
                m_PreviewRotation.x += rotChange;

                // X축 회전 제한
                m_PreviewRotation.x = fmodf(m_PreviewRotation.x, D3DX_PI * 2.0f);

                transform->Set_Rotation(m_PreviewRotation);
            }
            else if (input->KeyPress(DIK_LALT) || input->KeyPress(DIK_RALT))
            {
                _float rotChange = (wheelDelta > 0 ? D3DXToRadian(1.f) : -D3DXToRadian(1.f));
                m_PreviewRotation.y += rotChange;

                // Y축 회전 제한
                m_PreviewRotation.y = fmodf(m_PreviewRotation.y, D3DX_PI * 2.0f);

                transform->Set_Rotation(m_PreviewRotation);

            }
            // Y값 조절
            else if (input->KeyPress(DIK_LSHIFT) || input->KeyPress(DIK_RSHIFT))
            {
                _float posChange = (wheelDelta > 0 ? 0.1f : -0.1f);

                m_PreviewYOffset += posChange;
            }
            else
            {
                // 양수면 확대, 음수면 축소
                _float scaleChange = (wheelDelta > 0 ? 0.1f : -0.1f);
                m_PreviewScaleX += scaleChange;
                m_PreviewScaleY += scaleChange;
                m_PreviewScaleX = max(0.1f, min(30.f, m_PreviewScaleX));
                m_PreviewScaleY = max(0.1f, min(30.f, m_PreviewScaleY));

                Update_PreviewScale();
            }
        }
    }

    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse)
        return;

    if (input->MouseDown(DIM_LB))
    {
        CScene* scene = CManagement::GetInstance()->Get_Scene();
        if (scene)
        {
            CLayer* layer = scene->Get_Layer(LAYERTYPE::ENVIRONMENT);
            if (layer)
            {
                CMapObjectBase* newObj = CreateMapObjectType(m_GraphicDev, m_SelectedType, Get_CurTextureIndex());
                if (newObj)
                {
                    //newObj->Set_TextureIndex(Get_CurTextureIndex());
                    newObj->Set_WorldPos(_vec3(
                        worldPos.x,
                        worldPos.y + m_PreviewYOffset,
                        worldPos.z));

                    CTransform* newTransform = dynamic_cast<CTransform*>(newObj->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));

                    if (newTransform)
                    {
                        newTransform->Set_Scale(m_PreviewScaleX, m_PreviewScaleY, m_PreviewScaleX);
                        newTransform->Set_Rotation(m_PreviewRotation);
                    }

                    layer->Add_GameObject(OBJTYPE::PLACEMENT, newObj);

                    CMapObjectMgr::GetInstance()->Add_Placement(newObj);
                }
            }
        }

    }

    // 우클릭 : 배치 종료, 지우개
    if (input->MouseDown(DIM_RB))
    {
        m_Placing = false;
        Safe_Release(m_PreviewObj);
    }
}

void CMapEditorPlacer::Update_EraseMode()
{
    auto input = CDInputMgr::GetInstance();

    // 우클릭 - 지우개
    if (input->MouseDown(DIM_RB))
    {
        CGameObject* pickedObj = PickObjectAtMouse();

        if (pickedObj)
        {
            CMapObjectMgr::GetInstance()->Remove_Placement(pickedObj);
            pickedObj->Mark_As_PendingKill();
        }
    }
}

void CMapEditorPlacer::Render_Privew()
{
    if (!m_Placing || m_PreviewObj == nullptr)
        return;

    // 알파 테스트 끄기. 꺼줘야 블렌딩과 충돌이 나지 않는다.
    m_GraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

    // 알파 블렌딩
    m_GraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_GraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_GraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    m_GraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
    m_GraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(128, 255, 255, 255));

    m_PreviewObj->Render_GameObject();

    m_GraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    m_GraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(255, 255, 255, 255));
}

void CMapEditorPlacer::Start_Place(MAPOBJTYPE type)
{
    // 기존 프리뷰 제거
    Cancel_Place();

    m_SelectedType = type;
    m_PreviewScaleX = 1.f;
    m_PreviewScaleY = 1.f;
    m_PreviewRotation = _vec3(0.f, 0.f, 0.f);
    m_PreviewYOffset = 0.f;

    // 새 프리뷰 생성
    m_PreviewObj = CreateMapObjectType(m_GraphicDev, type, Get_CurTextureIndex());
    if (m_PreviewObj)
    {
        m_PreviewObj->Set_Preview(true);
        //m_PreviewObj->Set_TextureIndex(Get_CurTextureIndex());

        CTransform* transform = dynamic_cast<CTransform*>(m_PreviewObj->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));
        if (transform)
        {
            transform->Set_Scale(m_PreviewScaleX, m_PreviewScaleY, m_PreviewScaleX);
            transform->Set_Rotation(m_PreviewRotation);
        }

        m_Placing = true;
        return;
    }

    m_Placing = false;
}

void CMapEditorPlacer::Cancel_Place()
{
    m_Placing = false;

    Safe_Release(m_PreviewObj);
    m_PreviewObj = nullptr;

    m_SelectedType = MAPOBJTYPE::MAPOBJ_END;
}

// 마우스의 현재 월드위치
bool CMapEditorPlacer::Get_MouseWorldPos(_vec3& outPos)
{
    if (m_GraphicDev == nullptr)
        return false;

    if (!PickPlaneY(m_GraphicDev, g_hWnd, 0.f, outPos))
    {
        return false;
    }

    outPos.y = 0.f;

    return true;
}

_int CMapEditorPlacer::Get_CurTextureIndex()
{
    switch (m_SelectedType)
    {
    case MAPOBJTYPE::GRASS:           return m_GrassIndex;
    case MAPOBJTYPE::RUBBLE:          return m_RubbleIndex;
    case MAPOBJTYPE::DECO:            return m_DecoIndex;
    case MAPOBJTYPE::DUNGEON_DECO:    return m_DungeonDecoIndex;
    case MAPOBJTYPE::ETC:             return m_ETCIndex;
    default:                          return 0;
    }
}

void CMapEditorPlacer::Update_PreviewScale()
{
    if (m_PreviewObj)
    {
        CTransform* transform = dynamic_cast<CTransform*>(
            m_PreviewObj->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));

        if (transform)
            transform->Set_Scale(m_PreviewScaleX, m_PreviewScaleY, m_PreviewScaleX);
    }
}

void CMapEditorPlacer::Set_PreviewScaleX(_float scaleX)
{
    m_PreviewScaleX = scaleX; Update_PreviewScale();
}

void CMapEditorPlacer::Set_PreviewScaleY(_float scaleY)
{
    m_PreviewScaleY = scaleY; Update_PreviewScale();
}

CGameObject* CMapEditorPlacer::PickObjectAtMouse()
{
    _vec3 mouseWorldPos;
    if (!Get_MouseWorldPos(mouseWorldPos))
        return nullptr;

    CScene* scene = CManagement::GetInstance()->Get_Scene();
    if (!scene) return nullptr;

    CLayer* layer = scene->Get_Layer(LAYERTYPE::ENVIRONMENT);
    if (!layer) return nullptr;

    const auto& objMap = layer->Get_Objects();
    auto iter = objMap.find(OBJTYPE::PLACEMENT);
    if (iter == objMap.end())
        return nullptr;

    const vector<CGameObject*>& placedObjects = iter->second;

    const _float MAX_PICK_DISTNACE = 2.3f;
    _float minDistance = FLT_MAX;
    CGameObject* closestObj = nullptr;

    for (auto* obj : placedObjects)
    {
        if (!obj) continue;

        CTransform* transform = dynamic_cast<CTransform*>(
            obj->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));

        if (transform)
        {
            _vec3 objPos = transform->Get_Pos();

            _vec3 res = mouseWorldPos - objPos;
            _float distance = D3DXVec3Length(&res);

            _float dx = mouseWorldPos.x - objPos.x;
            _float dz = mouseWorldPos.z - objPos.z;
            _float dist = sqrtf(dx * dx + dz * dz);

            if (dist < MAX_PICK_DISTNACE && dist < minDistance)
            {
                minDistance = dist;
                closestObj = obj;
            }
        }
    }

    return closestObj;
}

CMapEditorPlacer* CMapEditorPlacer::Create(DEVICE graphicDev)
{
    CMapEditorPlacer* placer = new CMapEditorPlacer(graphicDev);

    if (placer)
        return placer;

    MSG_BOX("Create Failed Edit_Placer");
    return nullptr;
}

void CMapEditorPlacer::Free()
{
    Cancel_Place();

    Safe_Release(m_GraphicDev);
}
