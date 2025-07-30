#pragma once

#include "../../ext/ImGui/imgui.h"
#include "../../ext/ImGui/imgui_impl_dx11.h"
#include "../../ext/ImGui/imgui_impl_win32.h"
#include "../../ext/MinHook/include/MinHook.h"

#include "../Cheats/cheatsManager.h"
#include "../Hook/hookManager.h"
#include "../Log/log.h"

#include <chrono>
#include <d3d11.h>
#include <iostream>
#pragma comment(lib, "d3d11.lib")

inline bool LHackExitFlag = false; //Use this var to control thread berak.

class Gui
{
public:
	Gui();

	BOOL Show(VOID);
	VOID DisableAll(VOID);
};