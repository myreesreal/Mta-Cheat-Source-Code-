#pragma once
#include <includes.h>
#include <imgui/imgui_texteditor.h>

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class c_menu {
private:
	using present_t = HRESULT(__stdcall*)(IDirect3DDevice9*, const RECT*, const RECT*, HWND, const RGNDATA*);
	using reset_t = HRESULT(__stdcall*)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
	using cursor_t = BOOL(WINAPI*)(int, int);

	std::unique_ptr<TextEditor> m_editor;
	bool m_show_packets_window = false;
	bool m_show_executor_window = false;

	int m_selectedPacketId = 0;
	int m_selectedActionPacket = -1;

public:
	present_t o_present;
	reset_t o_reset;
	cursor_t o_cursor;

	bool release();
	void initialize();
	void draw();
	void draw_packets_window();
	void shutdown(bool before);
	void style();
};

inline c_menu* menu = new c_menu();