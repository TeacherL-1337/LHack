#pragma once

#include <Windows.h>
#include <vector>

class Memory
{
public:
	template <class T> 
	T ReadMemory(const DWORD64 addrees)
	{
		T temp{};

		DWORD oldProtect;
		
		VirtualProtect((LPVOID)addrees, sizeof(T), PAGE_EXECUTE_READWRITE, &oldProtect);
		temp = *(T*)addrees;
		VirtualProtect((LPVOID)addrees, sizeof(T), oldProtect, &oldProtect);

		return temp;
	}

	template <class T> 
	void WriteMemory(const DWORD64 addrees, const T& value)
	{
		DWORD oldProtect;
		VirtualProtect((LPVOID)addrees, sizeof(T), PAGE_EXECUTE_READWRITE, &oldProtect);
		*(T*)addrees = value;
		VirtualProtect((LPVOID)addrees, sizeof(T), oldProtect, &oldProtect);
	}
};

Memory inline* mem = new Memory();