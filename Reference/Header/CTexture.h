#pragma once
#include "CComponent.h"
#include "Engine_Define.h"

BEGIN(Engine)
class ENGINE_DLL CTexture : public CComponent
{
private:
    explicit CTexture(DEVICE graphicDev);
    explicit CTexture(const CTexture& rhs);
    ~CTexture() override;

public:
    HRESULT Ready_Texture(TEXTUREID texType, const wstring& filePath, const _uint& count = 1);
    HRESULT Ready_Texture_List(const vector<wstring>& fileList, TEXTUREID texType);

    HRESULT Add_Texture(const wstring& animKey,
                        TEXTUREID      texType,
                        const wstring& filePathPattern,
                        _uint          count);

    void Set_Texture(const _uint& index = 0);
    void Set_Texture(const wstring& animKey, _uint frameIndex);

    // index 번째 텍스처 포인터 반환
    IDirect3DBaseTexture9* Get_BaseTexture(_uint index = 0) const;
    // 텍스처 개수
    _uint Get_TextureIndex() const;
    bool Get_TextureSize(int index, float& outWidth, float& outHeight, wstring* key = nullptr);

public:
    static CTexture* Create(DEVICE graphicDev, TEXTUREID texType, const wstring& filePath, const _uint& count);
    static CTexture* Create(LPDIRECT3DDEVICE9 graphicDev, TEXTUREID texType, const vector<wstring>& fileList);

    CComponent* Clone() override;

private:
    void Free() override;

private:
    vector<IDirect3DBaseTexture9*> m_Textures;
    map<wstring, vector<IDirect3DBaseTexture9*>> m_AnimTextures;

private:
    static CRITICAL_SECTION s_D3DLock;
    static bool s_LockInitialized;

public:
    static void InitTextureSystem();
    static void CleanupTextureSystem();

};

END
