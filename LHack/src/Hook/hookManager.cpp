#include "hookManager.h"

DWORD64 localPlayer{};
std::vector<DWORD64> entityList;
std::vector<DWORD64> playerList;

static void __fastcall hkPlayerMove(__int64 a1, __int64 a2, __int64 a3, __int64 a4, __int64 a5, char a6, __int64 a7, __int64 a8) //a4: LocalPlayer
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

DWORD64 static GetEntityType_s(DWORD64 pEntity)
{
	__try {
		return *(DWORD64*)(*(DWORD64*)(pEntity + 0x330) + 0xA);
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
		return 0;
	}
}

static void HookManagerThread()
{
	while (true)
	{
		for (auto& pEntity : entityList)
		{
			if (!GetEntityType_s(pEntity))
			{
				entityList.erase(std::remove(entityList.begin(), entityList.end(), pEntity), entityList.end());
				continue;
			}

			if (GetEntityType_s(pEntity) != 0x302E726579616C70) //0x302E726579616C70 = "player.0"
			{
				continue;
			}

			if (pEntity == localPlayer)
			{
				continue;
			}

			if (std::find(playerList.begin(), playerList.end(), pEntity) != playerList.end())
			{
				continue;
			}

			playerList.push_back(pEntity);
		}

		for (auto& pEntity : playerList)
		{
			if (!GetEntityType_s(pEntity) || GetEntityType_s(pEntity) != 0x302E726579616C70) //0x302E726579616C70 = "player.0"
			{
				playerList.erase(std::remove(playerList.begin(), playerList.end(), pEntity), playerList.end());
				continue;
			}
		}

		if (LHackExitFlag)
		{
			break;
		}
	}
}

DWORD64 HookManager::GetModuleBase(VOID)
{
	return (DWORD64)GetModuleHandle("Minecraft.Windows.exe");
}

BOOL HookManager::InitFunctionsAddr(VOID)
{
	this->pPlayerMove = mem->QuickFindPattern(
		(PCHAR)"\x48\x8B\xC4\x55\x53\x56\x57\x41\x55\x41\x56\x41\x57\x48\x8D\xA8\x00\x00\x00\x00\x48\x81\xEC\x00\x00\x00\x00\x0F\x29\x70\xB8\x0F\x29\x78\xA8\x44\x0F\x29\x40\x00\x44\x0F\x29\x48\x00",
		(PCHAR)"xxxxxxxxxxxxxxxx????xxx????xxxxxxxxxxxx?xxxx?",
		(PCHAR)"Minecraft.Windows.exe"
	);

	this->pEntityReader = mem->QuickFindPattern(
		(PCHAR)"\x40\x53\x48\x83\xEC\x20\x48\x8B\x01\x8B\xDA\x48\x8B\x80\xE0\x02\x00\x00\xFF\x15\x00\x00\x00\x00\x3B",
		(PCHAR)"xxxxxxxxxxxxxxxxxxxx????x",
		(PCHAR)"Minecraft.Windows.exe"
	);
	
	this->pCreateVector = mem->QuickFindPattern(
		(PCHAR)"\x48\x83\xEC\x28\x4C\x8B\x81\xF8\x02\x00\x00\x4D\x85\xC0\x74\x19\x8B\x02\x41\x89\x40\x18",
		(PCHAR)"xxxxxxxxxxxxxxxxxxxxxx",
		(PCHAR)"Minecraft.Windows.exe"
	);

	this->pPermissionManager = mem->QuickFindPattern(
		(PCHAR)"\x48\x89\x5C\x24\x08\x48\x89\x7C\x24\x10\x4C\x8B\xDA",
		(PCHAR)"xxxxxxxxxxxxx",
		(PCHAR)"Minecraft.Windows.exe"
	);

	if (!this->pPlayerMove) { return false; }
	if (!this->pEntityReader) { return false; }
	if (!this->pCreateVector) { return false; }
	if (!this->pPermissionManager) { return false; }

	return true;
}

HookManager::HookManager()
{
	if (!this->InitFunctionsAddr())
	{
		throw std::runtime_error("Failed to initialize functions address");
	}

	if (MH_Initialize() != MH_OK)
	{
		throw std::runtime_error("Failed to initialize MinHook.");
	}

	if (MH_CreateHook((void*)this->pPlayerMove, (void*)(&hkPlayerMove), (void**)(&oPlayerMove)) != MH_OK)
	{
		throw std::runtime_error("Failed to create hook.");
	}

	if (MH_CreateHook((void*)this->pEntityReader, (void*)(&hkEntityReader), (void**)(&oEntityReader)) != MH_OK)
	{
		throw std::runtime_error("Failed to create hook.");
	}

	if (MH_EnableHook((void*)this->pPlayerMove) != MH_OK)
	{
		throw std::runtime_error("Failed to enable hook.");
	}

	if (MH_EnableHook((void*)this->pEntityReader) != MH_OK)
	{
		throw std::runtime_error("Failed to enable hook.");
	}

	CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)HookManagerThread, GetModuleHandleA(NULL), 0, nullptr));
}

DWORD64 HookManager::GetLocalPlayer(VOID)
{
	return localPlayer;
}

std::vector<DWORD64> HookManager::GetPlayerList(VOID)
{
	return playerList;
}

DWORD64 HookManager::GetFunctionAddress(FuntionIndex index)
{
	switch (index)
	{
	case PlayerMove:
		return this->pPlayerMove;
	case CreateVector:
		return this->pCreateVector;
	case EntityReader:
		return this->pEntityReader;
	case PermissionManager:
		return this->pPermissionManager;
	default:
		return NULL;
	}
}

VOID HookManager::DisableAll(VOID)
{
	MH_DisableHook(MH_ALL_HOOKS);
	MH_Uninitialize();
}