#pragma once
#include <includes.h>

class c_console {
public:
	void initialize();
	void destroy();
	void color(WORD color);
	void print(const char* message, ...);
	void error(const char* message, ...);
	void warning(const char* message, ...);
	void success(const char* message, ...);
};

inline c_console* console = new c_console();