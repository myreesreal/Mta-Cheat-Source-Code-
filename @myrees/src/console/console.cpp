#include "console.h"

void c_console::initialize()
{
	AllocConsole();

	freopen_s(reinterpret_cast<_iobuf**>(__acrt_iob_func(0)), "conin$", "r", static_cast<_iobuf*>(__acrt_iob_func(0)));
	freopen_s(reinterpret_cast<_iobuf**>(__acrt_iob_func(1)), "conout$", "w", static_cast<_iobuf*>(__acrt_iob_func(1)));
	freopen_s(reinterpret_cast<_iobuf**>(__acrt_iob_func(2)), "conout$", "w", static_cast<_iobuf*>(__acrt_iob_func(2)));
}

void c_console::destroy()
{
	fclose(static_cast<_iobuf*>(__acrt_iob_func(0)));
	fclose(static_cast<_iobuf*>(__acrt_iob_func(1)));
	fclose(static_cast<_iobuf*>(__acrt_iob_func(2)));

	FreeConsole();
}

void c_console::color(WORD color)
{
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	if (!hStdout)
		return;

	SetConsoleTextAttribute(hStdout, color);
}

void c_console::print(const char* message, ...)
{
	char buffer[1024];
	va_list args;
	va_start(args, message);
	vsnprintf(buffer, sizeof(buffer), message, args);
	va_end(args);

	printf("[INFO]: %s", buffer);
	color(7);
}

void c_console::error(const char* message, ...)
{
	color(4);

	char buffer[1024];
	va_list args;
	va_start(args, message);
	vsnprintf(buffer, sizeof(buffer), message, args);
	va_end(args);

	printf("[ERROR]: %s", buffer);
	color(7);
}

void c_console::warning(const char* message, ...)
{
	color(6);

	char buffer[1024];
	va_list args;
	va_start(args, message);
	vsnprintf(buffer, sizeof(buffer), message, args);
	va_end(args);

	printf("[DEBUG]: %s", buffer);
	color(7);
}

void c_console::success(const char* message, ...)
{
	color(10);

	char buffer[1024];
	va_list args;
	va_start(args, message);
	vsnprintf(buffer, sizeof(buffer), message, args);
	va_end(args);

	printf("[INPUT]: %s", buffer);
	color(7);
}
