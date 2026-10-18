#ifndef Engine_Macro_h__
#define Engine_Macro_h__

namespace Engine
{
// ========================================
// 윈도우 / 지형 관련 상수 매크로
// ========================================

#define	WINCX		            1280    // 기본 윈도우 가로 해상도
#define	WINCY		            720     // 기본 윈도우 세로 해상도

#define	TILE_CNT_X		        37      // 지형 격자의 X 축 정점 수
#define TILE_CNT_Z		        37      // 지형 격자의 Z 축 정점 수
#define VTX_IV		            1       // 지형 격자 정점 간격

// ========================================
// 편의성 메시지 박스 매크로
// ========================================

#ifndef MSG_BOX
#define MSG_BOX(_message)       MessageBox(NULL, TEXT(_message), L"System Message", MB_OK)
#endif

// ========================================
// 네임스페이스 관련 매크로
// ========================================

#define BEGIN(NAMESPACE)        namespace NAMESPACE {
#define END						}

#define USING(NAMESPACE)        using namespace NAMESPACE;

// ========================================
// DLL import/export 제어 매크로
// ========================================

#ifdef ENGINE_EXPORTS
#define ENGINE_DLL              _declspec(dllexport)
#else
#define ENGINE_DLL              _declspec(dllimport)
#endif

// ========================================
// 널 포인터 검사용 매크로
// ========================================

// _ptr이 nullptr이면 함수를 즉시 종료
#define NULL_CHECK(_ptr)                                        \
{                                                               \
    if ((_ptr) == 0) {                                          \
        return;                                                 \
    }                                                           \
}

// _ptr이 nullptr이면 반환 값을 지정하고 종료
#define NULL_CHECK_RETURN(_ptr, _return)                        \
{                                                               \
	if ((_ptr) == 0) {                                          \
		return _return;                                         \
	}                                                           \
}

// _ptr이 nullptr이면 메시지 박스를 띄움
#define NULL_CHECK_MSG(_ptr, _message)                          \
{                                                               \
	if ((_ptr) == 0) {                                          \
		MessageBox(NULL, _message, L"System Message", MB_OK);   \
	}                                                           \
}

// _ptr이 nullptr이면 메시지 박스를 띄우고 지정된 값을 반환
#define NULL_CHECK_RETURN_MSG(_ptr, _return, _message)          \
{                                                               \
	if ((_ptr) == 0) {                                          \
		MessageBox(NULL, _message, L"System Message", MB_OK);   \
		return _return;                                         \
	}                                                           \
}

// ========================================
// HRESULT 에러 검사용 매크로
// ========================================

// HRESULT가 실패면 메시지를 띄우고 E_FAIL을 반환
#define FAILED_CHECK(_hr)                                       \
{                                                               \
    if (((HRESULT)(_hr)) < 0) {                                 \
        MessageBoxW(NULL, L"Failed", L"System Error", MB_OK);   \
        return E_FAIL;                                          \
    }                                                           \
}

// HRESULT가 실패면 메시지를 띄우고 지정한 값을 반환
#define FAILED_CHECK_RETURN(_hr, _return)                       \
{                                                               \
    if (((HRESULT)(_hr)) < 0) {                                 \
        MessageBoxW(NULL, L"Failed", L"System Error", MB_OK);   \
        return _return;                                         \
    }                                                           \
}

// HRESULT가 실패면 지정한 메시지를 띄우고 E_FAIL을 반환
#define FAILED_CHECK_MSG(_hr, _message)                         \
{                                                               \
    if (((HRESULT)(_hr)) < 0) {                                 \
        MessageBoxW(NULL, _message, L"System Message", MB_OK);  \
        return E_FAIL;                                          \
    }                                                           \
}

// HRESULT가 실패면 지정한 메시지를 띄우고 지정한 값을 반환
#define FAILED_CHECK_RETURN_MSG(_hr, _return, _message)         \
{                                                               \
    if (((HRESULT)(_hr)) < 0) {                                 \
        MessageBoxW(NULL, _message, L"System Message", MB_OK);  \
        return _return;                                         \
    }                                                           \
}

// ========================================
// 클래스 설계 유틸리티 매크로
// ========================================

// 복사, 대입, 이동을 금지하여 싱글톤 등에서 안전한 사용을 보장
#define NO_COPY(CLASSNAME)                                      \
			public:                                             \
			CLASSNAME(const CLASSNAME&) = delete;               \
			CLASSNAME& operator = (const CLASSNAME&) = delete;  \
			CLASSNAME(CLASSNAME&&) = delete;                    \
			CLASSNAME& operator=(CLASSNAME&&) = delete;

// 클래스를 싱글톤으로 선언할 때 사용한다. 헤더에서 호출
#define DECLARE_SINGLETON(CLASSNAME)                            \
			NO_COPY(CLASSNAME)                                  \
			private:                                            \
			static CLASSNAME*   m_Instance;                     \
			public:                                             \
			static CLASSNAME*   GetInstance( void );            \
			static void DestroyInstance( void );

// DECLARE_SINGLETON과 짝을 이루며 cpp에서 정의 부분을 제공
#define IMPLEMENT_SINGLETON(CLASSNAME)                          \
            CLASSNAME*  CLASSNAME::m_Instance = NULL;           \
            CLASSNAME*  CLASSNAME::GetInstance( void ) {        \
                if(NULL == m_Instance) {                        \
                    m_Instance = new CLASSNAME;                 \
                }                                               \
                return m_Instance;                              \
            }                                                   \
            void CLASSNAME::DestroyInstance( void ) {           \
                if(NULL != m_Instance)	{                       \
                    delete m_Instance;                          \
                    m_Instance = NULL;                          \
                }                                               \
            }
}

#endif // Engine_Macro_h__
