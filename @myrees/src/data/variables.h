#pragma once
#include <includes.h>

class c_variables {
public:
	struct
	{
		HMODULE hModule;
		MH_STATUS mh_status;
		WNDPROC wnd_proc;
		D3DDEVICE_CREATION_PARAMETERS device_par;
		IDirect3DDevice9* device_dx9{ nullptr };
	} winapi;

	struct
	{
		ImGuiWindowFlags flags{ ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBringToFrontOnFocus };
		ImVec2 size{ 700, 500 };
	} window;

	struct
	{
		bool dpi_changed{ false };
		bool initialized{ false };
		bool is_open{ false };
		int m_selectedTab{ 0 };
	} gui;

	struct
	{
		ImFont* font[6];
		IDirect3DTexture9* profile_icon;
		IDirect3DTexture9* profiles_icon;
		IDirect3DTexture9* vehicle_icon;
		IDirect3DTexture9* visual_icon;
		IDirect3DTexture9* aim_icon;
		IDirect3DTexture9* weapon_icon;
		IDirect3DTexture9* settings_icon;
	} assets;
};

inline c_variables* var = new c_variables();