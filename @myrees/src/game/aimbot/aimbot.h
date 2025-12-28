#pragma once
#include <includes.h>
#include <sdk.h>

#define M_PI	3.14159265358979323844f

class c_aimbot {
private:
    struct {
        CClientPlayer* i_target_player;
        CVector vec_crosshair;
        CVector vec_target_bone;
        int i_target_bone;
        bool is_target_player;
        bool is_player_aiming;
    } s_aimbot_config;
public:
	void draw_player_aimbot();
	void draw_aimbot_fov();
	void draw_target_player();
    void draw_aimbot();
};

inline c_aimbot* aimbot = new c_aimbot();
