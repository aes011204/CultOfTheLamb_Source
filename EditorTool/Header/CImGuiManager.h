#pragma once


class CImGuiManager
{
private:
	CImGuiManager();
	~CImGuiManager();

public:
	static LRESULT MsgProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);

private:
	unsigned int fileID;

public:
	static CImGuiManager* GetInstance()
	{
		if (!m_pInstance)
			m_pInstance = new CImGuiManager;

		return m_pInstance;
	}

	static void DestroyInstance()
	{
		delete m_pInstance;

		m_pInstance = nullptr;
	}

	void InitImGui(HWND hWnd, LPDIRECT3DDEVICE9 pGraphicDev);
	void ResizeScreen();

	void Update();
	void Render();
	void Free();


private:
	static CImGuiManager* m_pInstance;

    D3DXVECTOR4 clear_color = { 0.f, 0.f, 0.f, 0.f };
	bool show_demo_window = true;
	bool show_another_window = true;

};
