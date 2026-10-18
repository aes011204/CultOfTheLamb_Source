#ifndef PCH_H
#define PCH_H

#include "framework.h"

// DirectX 9
#include <d3d9.h>
#include <d3dx9.h>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dxguid.lib")

#include <iostream>
#include "string"

#include "Editor_Typedef.h"

// ImGui
#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include "ImGuiFileDialog.h"

#endif //PCH_H
