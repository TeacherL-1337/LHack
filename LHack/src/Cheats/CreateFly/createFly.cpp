#include "../cheatsManager.h"

DWORD64 playerCanFly{};

static void __fastcall hkPermissionManager(__int64 a1, __int64 a2, unsigned __int8 a3)
{
	playerCanFly = a1 + 0x1B8;

	oPermissionManager(a1, a2, a3);
}

VOID CheatsManager::CreateFly(BOOL Enable)
{
	MH_CreateHook((void*)hookManager->GetFunctionAddress(PermissionManager), (void*)(&hkPermissionManager), (void**)(&oPermissionManager));
	MH_EnableHook((void*)hookManager->GetFunctionAddress(PermissionManager));

	if (Enable && playerCanFly != NULL)
	{
		*(BYTE*)playerCanFly = 1;
	}
	else if (playerCanFly != NULL)
	{
		*(BYTE*)playerCanFly = 0;
	}
}