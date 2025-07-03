#pragma once

#include "../../ext/ImGui/imgui.h"
#include "../../ext/ImGui/imgui_impl_win32.h"
#include "../../ext/ImGui/imgui_impl_dx11.h"
#include "../../ext/MinHook/include/MinHook.h"
#include "../Hook/hookManger.h"
#include "../Memory/memory.h"
#include "../Math/Vector.h"
#include "Draw.h"

#include <iostream>
#include <chrono>
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")



class Gui
{
public:
	Gui();

	BOOL Show(VOID);
	VOID DisableAll(VOID);
};