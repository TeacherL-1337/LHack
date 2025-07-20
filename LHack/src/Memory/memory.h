#pragma once

#include <Windows.h>
#include <Intsafe.h>
#include <vector>

class Memory
{
public:
	template <class T> 
	T ReadMemory(CONST DWORD64 _address)
	{
		T temp{};

		DWORD oldProtect;
		
		VirtualProtect((LPVOID)_address, sizeof(T), PAGE_EXECUTE_READWRITE, &oldProtect);
		memcpy(&temp, (void*)_address, sizeof(T));
		VirtualProtect((LPVOID)_address, sizeof(T), oldProtect, &oldProtect);

		return temp;
	}

	template <class T> 
	VOID WriteMemory(CONST DWORD64 _address, CONST T& _value)
	{
		DWORD oldProtect;
		VirtualProtect((LPVOID)_address, sizeof(T), PAGE_EXECUTE_READWRITE, &oldProtect);
		memcpy((void*)_address, _value, sizeof(T));
		VirtualProtect((LPVOID)_address, sizeof(T), oldProtect, &oldProtect);
	}

	VOID memset_s(VOID* _Dst, INT _Val, SIZE_T _Size);
	VOID memcpy_S(VOID* _Dst, VOID* _Src, SIZE_T _Size);

	DWORD64 WINAPI QuickFindPattern(CONST PCHAR pbMask, CONST PCHAR pszMask, CONST PCHAR pszModule = NULL);
};

Memory inline* mem = new Memory();