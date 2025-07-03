#pragma once

#include <chrono>
#include <string>
#include "../../ext/MinHook/include/MinHook.h"
#include "../Memory/memory.h"

typedef void(__fastcall* PlayerMove)(__int64 a1, const char* a2, __int64 a3, __int64 a4, __int64 a5, char a6, __int64 a7, __int64 a8);

inline PlayerMove oPlayerMove = nullptr;

typedef void(__fastcall* EntityReader)(__int64 a1, int a2);

inline EntityReader oEntityReader = nullptr;

class HookManger
{
private:
	DWORD64					m_pPlayerMove =		0x1F22E00;
	DWORD64					m_pEntityReader =	0X2ABD6B0;

private:
	DWORD64					GetModuleBase		(VOID);
	DWORD64					GetEntityType_s		(DWORD64 pEntity);

public:
	HookManger();

	DWORD64					GetLocalPlayer		(VOID);
	std::vector<DWORD64>	GetPlayerList		(VOID);
	VOID					DisableAll			(VOID);

};

HookManger inline* hookManger = new HookManger();