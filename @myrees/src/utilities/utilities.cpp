#include "utilities.h"
#include <console/console.h>
#include <random>

MODULEINFO utilities::c_pattern::get_module_info(const char* hModule)
{
	MODULEINFO dwModuleInfo = { 0 };

	HMODULE dwModule = GetModuleHandleA(hModule);
	if (!dwModule)
	{
		console->error("Failed to get module handle ('%s')", hModule);
		return dwModuleInfo;
	}

	K32GetModuleInformation(GetCurrentProcess(), dwModule, &dwModuleInfo, sizeof(MODULEINFO));

	return dwModuleInfo;
}

DWORD utilities::c_pattern::find_pattern(const char* hModule, const char* pattern, const char* mask)
{
	MODULEINFO dwModuleInfo = get_module_info(hModule);
	DWORD BaseOfDll = (DWORD)dwModuleInfo.lpBaseOfDll;
	DWORD SizeOfImage = (DWORD)dwModuleInfo.SizeOfImage;
	DWORD PatternLength = (DWORD)strlen(mask);

	for (DWORD i = 0; i < SizeOfImage - PatternLength; i++)
	{
		bool found = true;
		for (DWORD j = 0; j < PatternLength; j++)
		{
			found &= mask[j] == '?' || pattern[j] == *(char*)(BaseOfDll + i + j);
		}

		if (found)
		{
			return BaseOfDll + i;
		}
	}

	return NULL;
}

DWORD utilities::c_device::find_vtable(DWORD type)
{
	DWORD dwBaseObj = 0;

	char info[MAX_PATH];
	GetSystemDirectoryA(info, MAX_PATH);
	strcat_s(info, MAX_PATH, "\\d3d9.dll");

	dwBaseObj = (DWORD)LoadLibraryA(info);

	while (dwBaseObj++ < dwBaseObj + type)
	{
		if ((*(WORD*)(dwBaseObj + 0x00)) == 0x06C7 && (*(WORD*)(dwBaseObj + 0x06)) == 0x8689 && (*(WORD*)(dwBaseObj + 0x0C)) == 0x8689)
		{
			dwBaseObj += 2;
			break;
		}
	}

	return(dwBaseObj);
}

DWORD utilities::c_device::get_address(int vtable)
{
	PDWORD table;
	*(DWORD*)&table = *(DWORD*)find_vtable(0x128000);
	return table[vtable];
}