#pragma once
#ifndef INCLUDES
#define INCLUDES
#define WIN32_LEAN_AND_MEAN 
#define _DX9_SDK_INSTALLED
#define _CRT_SECURE_NO_WARNINGS
#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#define IMGUI_DEFINE_MATH_OPERATORS

#include <Windows.h>
#include <iostream>
#include <vector>
#include <string>
#include <d3d9.h>
#include <d3dx9.h>
#include <Psapi.h>
#include <TlHelp32.h>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/imgui_impl_dx9.h>
#include <imgui/imgui_impl_win32.h>

#include <minhook/include/MinHook.h>
#endif

extern bool Send_Script_Packet;