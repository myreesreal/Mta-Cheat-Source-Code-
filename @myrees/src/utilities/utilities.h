#pragma once
#include <includes.h>

namespace utilities {
	class c_pattern {
	public:
		static MODULEINFO get_module_info(const char* hModule);
		static DWORD find_pattern(const char* hModule, const char* pattern, const char* mask);
	};

	class c_device {
	public:
		static DWORD find_vtable(DWORD type);
		static DWORD get_address(int vtable);
	};
}