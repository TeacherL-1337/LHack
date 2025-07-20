#pragma once

#include <bit>
#include <chrono>
#include <string>
#include "../../ext/MinHook/include/MinHook.h"
#include "../Memory/memory.h"
#include "../Cheats/cheatsManager.h"

typedef void(__fastcall* PlayerMove_t)(__int64 a1, __int64 a2, __int64 a3, __int64 a4, __int64 a5, char a6, __int64 a7, __int64 a8);
inline PlayerMove_t oPlayerMove = nullptr;

typedef void(__fastcall* EntityReader_t)(__int64 a1, int a2);
inline EntityReader_t oEntityReader = nullptr;

typedef void(__fastcall* CreateVector_t)(__int64 pEntity, FLOAT* pVector);
inline CreateVector_t oCreateVector = nullptr;

typedef void(__fastcall* PermissionManager_t)(__int64 a1, __int64 a2, unsigned __int8 a3);
inline PermissionManager_t oPermissionManager = nullptr;

enum FuntionIndex
{
	PlayerMove,
	EntityReader,
	CreateVector,
	PermissionManager
};

class HookManager
{
private:
	DWORD64					pPlayerMove = NULL;
	DWORD64					pEntityReader = NULL;
	DWORD64					pCreateVector = NULL;
	DWORD64					pPermissionManager = NULL;

private:
	BOOL					InitFunctionsAddr	(VOID);

public:
	HookManager();

	DWORD64					GetModuleBase		(VOID);
	DWORD64					GetLocalPlayer		(VOID);
	std::vector<DWORD64>	GetPlayerList		(VOID);
	DWORD64					GetFunctionAddress	(FuntionIndex index);
	VOID					DisableAll			(VOID);

};

HookManager inline* hookManager = new HookManager();