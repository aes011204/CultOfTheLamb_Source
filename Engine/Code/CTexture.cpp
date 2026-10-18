#include "CTexture.h"

CRITICAL_SECTION CTexture::s_D3DLock;
bool CTexture::s_LockInitialized = false;

CTexture::CTexture(LPDIRECT3DDEVICE9 graphicDev)
    : CComponent(graphicDev)
{ }

CTexture::CTexture(const CTexture& rhs)
    : CComponent(rhs)
{
    size_t size = rhs.m_Textures.size();

    m_Textures.reserve(size);

    m_Textures = rhs.m_Textures;

    for (_uint i = 0; i < size; ++i)
    {
        m_Textures[i]->AddRef();
    }

    // 깊은 복사
    for (const auto& keyValue : rhs.m_AnimTextures)
    {
        const wstring&                        key      = keyValue.first;
        const vector<IDirect3DBaseTexture9*>& textures = keyValue.second;

        vector<IDirect3DBaseTexture9*>& dstFrames = m_AnimTextures[key];
        dstFrames.reserve(textures.size());

        for (auto* tex : textures)
        {
            dstFrames.push_back(tex);
            if (tex)
                tex->AddRef();
        }
    }
}

CTexture::~CTexture()
{ }

HRESULT CTexture::Ready_Texture(TEXTUREID texType, const wstring& filePath, const _uint& count)
{
    m_Textures.reserve(count);

    for (_uint i = 0; i < count; ++i)
    {
        wchar_t fileName[MAX_PATH] = L"";
        swprintf_s(fileName, MAX_PATH, filePath.c_str(), i + 1);

        IDirect3DBaseTexture9* texture = nullptr;
        HRESULT hr = S_OK;

        EnterCriticalSection(&s_D3DLock);

        switch (texType)
        {
        case TEX_NORMAL:
            hr = D3DXCreateTextureFromFile(m_GraphicDev, fileName, (LPDIRECT3DTEXTURE9*)&texture);
            break;

        case TEX_CUBE:
            hr = D3DXCreateCubeTextureFromFile(m_GraphicDev, fileName, (LPDIRECT3DCUBETEXTURE9*)&texture);
            break;

        default:
            hr = E_FAIL;
            break;
        }

        LeaveCriticalSection(&s_D3DLock);

        if (FAILED(hr) || texture == nullptr)
            return E_FAIL;

        m_Textures.push_back(texture);
    }

    return S_OK;
}

// 안은수 : 새로운 Ready_Texture_List 백터를 받아오는
HRESULT CTexture::Ready_Texture_List(const vector<wstring>& fileList, TEXTUREID texType)
{
    m_Textures.reserve(fileList.size());

    for (const auto& file : fileList)
    {
        IDirect3DBaseTexture9* texture = nullptr;

		if (!s_LockInitialized)
		{
			OutputDebugString(L"[CTexture] ERROR: s_D3DLock NOT initialized!\n");
			return E_FAIL;
		}

		EnterCriticalSection(&s_D3DLock);

        if (texType == TEX_NORMAL)
        {
            if (FAILED(D3DXCreateTextureFromFile(
                m_GraphicDev, file.c_str(), (LPDIRECT3DTEXTURE9*)&texture)))
            {
                LeaveCriticalSection(&s_D3DLock);
                OutputDebugStringW((L"[Texture Load Failed] " + file + L"\n").c_str());
                return E_FAIL;
            }
        }
        else if (texType == TEX_CUBE)
        {
            if (FAILED(D3DXCreateCubeTextureFromFile(
                m_GraphicDev, file.c_str(), (LPDIRECT3DCUBETEXTURE9*)&texture)))
            {
                LeaveCriticalSection(&s_D3DLock);
                OutputDebugStringW((L"[Texture Load Failed] " + file + L"\n").c_str());
                return E_FAIL;
            }
        }
        else
        {
            LeaveCriticalSection(&s_D3DLock);
            return E_FAIL;
        }

        LeaveCriticalSection(&s_D3DLock);

        m_Textures.push_back(texture);
    }
    return S_OK;
}

void CTexture::Set_Texture(const _uint& index)
{
    if (m_Textures.empty() || m_Textures.size() <= index)
        return;

    m_GraphicDev->SetTexture(0, m_Textures[index]);

}

HRESULT CTexture::Add_Texture(const wstring& animKey, TEXTUREID texType, const wstring& filePathPattern, _uint count)
{
    if (m_AnimTextures.find(animKey) != m_AnimTextures.end())
        return S_OK;

	EnterCriticalSection(&s_D3DLock);

    vector<IDirect3DBaseTexture9*>& frames = m_AnimTextures[animKey];
    frames.reserve(count);

	LeaveCriticalSection(&s_D3DLock);

    for (_uint i = 0; i < count; ++i)
    {
        wchar_t fileName[MAX_PATH] = L"";

        swprintf_s(fileName, MAX_PATH, filePathPattern.c_str(), i + 1);

        IDirect3DBaseTexture9* texture = nullptr;

		EnterCriticalSection(&s_D3DLock);

        switch (texType)
        {
        case TEX_NORMAL:
            if (FAILED(D3DXCreateTextureFromFile(
                m_GraphicDev, fileName, (LPDIRECT3DTEXTURE9*)&texture)))
            {
				LeaveCriticalSection(&s_D3DLock);
                return E_FAIL;
            }
            break;

        case TEX_CUBE:
        {
            if (FAILED(D3DXCreateCubeTextureFromFile(
                m_GraphicDev, fileName, (LPDIRECT3DCUBETEXTURE9*)&texture)))
            {
				LeaveCriticalSection(&s_D3DLock);
                return E_FAIL;
            }
        }
        break;
        }

		LeaveCriticalSection(&s_D3DLock);

		EnterCriticalSection(&s_D3DLock);
		frames.push_back(texture);
		LeaveCriticalSection(&s_D3DLock);
    }

    return S_OK;
}

void CTexture::Set_Texture(const wstring& animKey, _uint frameIndex)
{
    auto iter = m_AnimTextures.find(animKey);

    if (iter == m_AnimTextures.end())
        return;

    const vector<IDirect3DBaseTexture9*>& frames = iter->second;

    if (frames.empty() || frameIndex >= frames.size())
        return;

    m_GraphicDev->SetTexture(0, frames[frameIndex]);
}

IDirect3DBaseTexture9* CTexture::Get_BaseTexture(_uint index) const
{
    if (index >= m_Textures.size())
        return nullptr;

    return m_Textures[index];
}

_uint CTexture::Get_TextureIndex() const
{
    return static_cast<_uint>(m_Textures.size());
}

bool CTexture::Get_TextureSize(int index, float& outWidth, float& outHeight, wstring* key)
{
    IDirect3DBaseTexture9* tex = nullptr;
    // 애니메이션인지 아닌지 구분 애니메이션이면 처음의 처음 반환
    if ((int)m_Textures.size() == 0)
    {
        if (m_AnimTextures.empty())
            return false;
        else
        {

            auto it  = find_if(m_AnimTextures.begin(), m_AnimTextures.end(), CTag_Finder(*key));

            if (it != m_AnimTextures.end())
            {
                tex = it->second[0];
            }
            else
                return false;

        }
    }
    else
    {
        if (index < 0 || index >= (int)m_Textures.size())
            return false;
        
        tex = m_Textures[index];
    }
    if (tex == nullptr)
        return false;
    // 2d 인지 박스 인지 구분
    if (tex->GetType() != D3DRESOURCETYPE::D3DRTYPE_TEXTURE)
        return false;
    IDirect3DTexture9* tex2D = static_cast<IDirect3DTexture9*>(tex);

    // 가로 세로 픽셀 가져오는 함수
    D3DSURFACE_DESC desc;
    if (FAILED(tex2D->GetLevelDesc(0, &desc)))
        return false;

    outWidth = desc.Width;
    outHeight = desc.Height;

    return true;
}

CTexture* CTexture::Create(DEVICE graphicDev, TEXTUREID texType, const wstring& filePath, const _uint& count)
{
    auto texture = new CTexture(graphicDev);

    if (FAILED(texture->Ready_Texture(texType, filePath, count)))
    {
        Safe_Release(texture);
        MSG_BOX("Texture Create Failed");
        return nullptr;
    }

    return texture;
}

// 안은수 : 새로운 오버로드 create
CTexture* CTexture::Create(LPDIRECT3DDEVICE9      graphicDev,
                           TEXTUREID              texType,
                           const vector<wstring>& fileList)
{
    auto texture = new CTexture(graphicDev);

    if (FAILED(texture->Ready_Texture_List(fileList, texType)))
    {
        Safe_Release(texture);
        MSG_BOX("Texture Create Failed (List)");
        return nullptr;
    }

    return texture;
}

CComponent* CTexture::Clone()
{
    return new CTexture(*this);
}

void CTexture::Free()
{
    if (s_LockInitialized)
    {
        EnterCriticalSection(&s_D3DLock);
    }
    else
    {
        OutputDebugString(L"[WARNING] CTexture::Free called but s_D3DLock not initialized!\n");
    }

    for_each(m_Textures.begin(), m_Textures.end(), CDeleteObj());
    m_Textures.clear();

    for_each(m_AnimTextures.begin(),
        m_AnimTextures.end(),
        [](auto& keyValue)
        {
            const auto& key = keyValue.first;
            auto& vec = keyValue.second;

            for (auto*& tex : vec)
            {
                if (tex == nullptr)
                    continue;

                Safe_Release(tex);
            }
        });

    m_AnimTextures.clear();
    if (s_LockInitialized)
    {
        LeaveCriticalSection(&s_D3DLock);
    }
}

void CTexture::InitTextureSystem()
{
	if (!s_LockInitialized)
	{
		InitializeCriticalSection(&s_D3DLock);
		s_LockInitialized = true;
	}
}

void CTexture::CleanupTextureSystem()
{
    if (s_LockInitialized)
    {
        DeleteCriticalSection(&s_D3DLock);
        s_LockInitialized = false;
    }
}
