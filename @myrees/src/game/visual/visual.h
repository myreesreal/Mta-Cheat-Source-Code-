#pragma once
#include <includes.h>
#include <sdk.h>

class c_visual {
public:
	void release();
	void draw_player_esp();
	void draw_vehicle_esp();
};

inline c_visual* visual = new c_visual();
