#include "hookManger.h"

DWORD64 localPlayer{};
std::vector<DWORD64> entityList;

static void __fastcall hkPlayerMove(__int64 a1, const char* a2, __int64 a3, __int64 a4, __int64 a5, char a6, __int64 a7, __int64 a8) //a4: LocalPlayer
{
	localPlayer = a4;

	oPlayerMove(a1, a2, a3, a4, a5, a6, a7, a8);
}

static void __fastcall hkEntityReader(__int64 a1, int a2) //a1: EntityList
{
	if (std::find(entityList.begin(), entityList.end(), a1) == entityList.end())
	{
		entityList.push_back(a1);
	}

	oEntityReader(a1, a2);
}

DWORD64 HookManger::GetModuleBase(VOID)
{
	return (DWORD64)GetModuleHandle("Minecraft.Windows.exe");
}

DWORD64 HookManger::GetEntityType_s(DWORD64 pEntity)
{
	__try {
		return *(DWORD64*)(*(DWORD64*)(pEntity + 0x330) + 0xA);
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
		return 0;
	}
}

HookManger::HookManger()
{
	if (MH_Initialize() != MH_OK)
	{
		throw std::runtime_error("Failed to initialize MinHook.");
	}

	if (MH_CreateHook((void*)(this->GetModuleBase() + this->m_pPlayerMove), (void*)(&hkPlayerMove), (void**)(&oPlayerMove)) != MH_OK)
	{
		throw std::runtime_error("Failed to create Hook.");
	}

	if (MH_CreateHook((void*)(this->GetModuleBase() + this->m_pEntityReader), (void*)(&hkEntityReader), (void**)(&oEntityReader)) != MH_OK)
	{
		throw std::runtime_error("Failed to create hook.");
	}

	if (MH_EnableHook((void*)(this->GetModuleBase() + this->m_pPlayerMove)) != MH_OK)
	{
		throw std::runtime_error("Failed to enable hook.");
	}

	if (MH_EnableHook((void*)(this->GetModuleBase() + this->m_pEntityReader)) != MH_OK)
	{
		throw std::runtime_error("Failed to enable hook.");
	}
}

DWORD64 HookManger::GetLocalPlayer(VOID)
{
	return localPlayer;
}

std::vector<DWORD64> HookManger::GetPlayerList(VOID)
{
	std::vector<DWORD64> result;

	for (auto& pEntity : entityList)
	{
		if ((this->GetEntityType_s(pEntity)) != 0x302E726579616C70) //0x302E726579616C70 = "player.0"
		{
			continue;
		}

		if (pEntity == this->GetLocalPlayer())
		{
			continue;
		}

		result.push_back(pEntity);
	}

	return result;
}

VOID HookManger::DisableAll(VOID)
{
	MH_DisableHook(MH_ALL_HOOKS);
	MH_Uninitialize();
}