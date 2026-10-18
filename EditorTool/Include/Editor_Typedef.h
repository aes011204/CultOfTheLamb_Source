#pragma once

template <typename T>
class Singleton
{
public:
	static T* GetInstance()
	{
		if (!m_pInstance)
			m_pInstance = new T;

		return m_pInstance;
	}

public:
	virtual ~Singleton() {}

	void DestroyInstance()
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}

private:
	static T* m_pInstance;
};
