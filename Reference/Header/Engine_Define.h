#ifndef Engine_Define_h__
#define Engine_Define_h__

// DirectX 9
#pragma warning(push)
#pragma warning(disable: 26495)
#include <d3d9.h>
#include <d3dx9.h>
#pragma warning(pop)

#include <algorithm>
#include <ctime>
#include <functional>
#include <iostream>
#include <list>
#include <map>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <fstream>
#include <filesystem>

#include "Engine_Enum.h"
#include "Engine_Macro.h"
#include "Engine_Typedef.h"
#include "Engine_Global.h"
#include "Engine_Struct.h"
#include "Engine_Function.h"
#include "Engine_Math.h"

// DInput
#define DIRECTINPUT_VERSION	0x0800
#include <dinput.h>
#pragma warning(disable : 4251)

//Fmod
#include "../../Libs/Fmod/fmod.hpp"
#pragma comment(lib, "../../Libs/Fmod/fmodL_vc.lib")

#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW

#endif
#endif

using namespace std;
using namespace Engine;

#endif // Engine_Define_h__
