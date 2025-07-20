#include "memory.h"

VOID Memory::memset_s(VOID* _Dst, INT _Val, SIZE_T _Size)
{
	DWORD oldProtect;

	VirtualProtect((LPVOID)_Dst, _Size, PAGE_EXECUTE_READWRITE, &oldProtect);
	memset(_Dst, _Val, _Size);
	VirtualProtect((LPVOID)_Dst, _Size, oldProtect, &oldProtect);
}

VOID Memory::memcpy_S(VOID* _Dst, VOID* _Src, SIZE_T _Size)
{
	DWORD oldProtect;

	VirtualProtect((LPVOID)_Dst, _Size, PAGE_EXECUTE_READWRITE, &oldProtect);
	memcpy(_Dst, _Src, _Size);
	VirtualProtect((LPVOID)_Dst, _Size, oldProtect, &oldProtect);
}

DWORD64 WINAPI Memory::QuickFindPattern(CONST PCHAR pbMask, CONST PCHAR pszMask, CONST PCHAR pszModule) 
{
	// Local variables
	DWORD64 dwModuleBase = 0;
	DWORD64 dwModuleSize = 0;
	DWORD64 dwFinal = 0;
	DWORD64 dwAddress = 0;
	DWORD64 dwMaskLength = 0;
	DWORD64 dwBytesFound = 0;

	// Get module base address or process base address if no module is passed
	if (pszModule)
		dwModuleBase = (DWORD64)GetModuleHandle(pszModule);
	else
		dwModuleBase = (DWORD64)GetModuleHandle(NULL);

	// Couldn't find module
	if (dwModuleBase == NULL)
		return NULL;

	// Grab DOS header
	PIMAGE_DOS_HEADER pDOSHeader = (PIMAGE_DOS_HEADER)dwModuleBase;

	// Verify DOS header
	if (pDOSHeader->e_magic != IMAGE_DOS_SIGNATURE)
		return NULL;

	// Grab NT headers
	PIMAGE_NT_HEADERS pNTHeaders = (PIMAGE_NT_HEADERS)(dwModuleBase + pDOSHeader->e_lfanew);

	// Calculate where the code section starts, as that's where we'll be scanning for signatures
	dwModuleBase += pNTHeaders->OptionalHeader.BaseOfCode;
	dwModuleSize = pNTHeaders->OptionalHeader.SizeOfCode;

	// Calculate signature length and total size of block to search in
	dwMaskLength = strlen(pszMask);
	dwFinal = (dwModuleBase + dwModuleSize) - dwMaskLength;

	// Start searching for a signature
	do
	{
		// Get current opcodes
		BYTE bCurrentOpcode = *(BYTE*)(dwModuleBase);
		BYTE bSigOpcode = pbMask[dwBytesFound];
		BOOL bCompare = pszMask[dwBytesFound] == 'x';

		// Make sure that this part of the signature isn't masked
		switch (bCompare)
		{
		case TRUE:
			// Verify our signature is still valid
			if (bCurrentOpcode == bSigOpcode)
				dwBytesFound++;
			else
			{
				// Reset base back to the byte directly after we started searching for our current signature
				dwModuleBase -= (dwBytesFound - 1);
				dwBytesFound = 0;
			}
			break;
		case FALSE:
			// Masked byte, just keep searching
			dwBytesFound++;
			break;
		};

		// Signature found
		if (dwBytesFound >= dwMaskLength)
			return ((dwModuleBase - dwBytesFound) + 1);

	} while (dwModuleBase++ < dwFinal);

	// Could not find signature 
	return NULL;
}