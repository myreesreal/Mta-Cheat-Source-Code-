#pragma once
#include <includes.h>

struct s_resource_dump
{
    std::string script_name;
    std::vector<char> buffer;
    size_t size;
};

struct s_events
{
    std::string event_name;
    bool is_blocked = false;
};

class c_elements {
public:
    struct {
        bool loaded_client{ false };
    } content;

    struct {
        int menu_bind{ 45 };
    } binds;

    struct {
        bool godmode{ false };
        bool airbreak{ false };
        bool sprint{ false };
        bool jetpack{ false };

        float airbreak_speed{ 1.0f };
        float sprint_speed{ 1.0f };
        float health{ 100.0f };
        float armor{ 100.0f };

        int skin_id{ 1 };
    } player;

    struct {
        bool run_engine{ false };
        bool unlock_vehicle{ false };
        bool fix_vehicle{ false };
        bool flip_vehicle{ false };
    } vehicle;

    struct {
        // Weapon features
        bool rapid_fire{ false };
        bool infinite_ammo{ false };
        int selected_weapon{ 0 };
        float rapid_fire_speed{ 1 };
        const char* weapon_list[47] = { "Fist", "Brass Knuckles", "Golf Club", "Nightstick", "Knife", "Baseball Bat", "Shovel", "Pool Cue", "Katana", "Chainsaw", "Purple Dildo", "Dildo", "Vibrator", "Silver Vibrator", "Flowers", "Cane", "Grenade", "Tear Gas", "Molotov Cocktail", "Rocket", "Heat Seeking RPG", "Freefall Bomb", "Colt 45", "Silenced 9mm", "Desert Eagle", "Shotgun", "Sawnoff Shotgun", "Combat Shotgun", "Micro SMG/Uzi", "MP5", "AK-47", "M4", "Tec-9", "Country Rifle", "Sniper Rifle", "RPG", "HS Rocket", "Flamethrower", "Minigun", "Satchel Charge", "Detonator", "Spraycan", "Fire Extinguisher", "Camera", "Night Vision Goggles", "Thermal Goggles", "Parachute" };
    } weapon;

    struct {
        // ESP Settings
        bool state{ false };
        bool name{ false };
        bool weapon{ false };
        bool box{ false };
        bool bone{ false };
        bool line{ false };
        bool health{ false };
        int distance{ 100 };
        ImColor color{ 255, 255, 255, 255 };

        // Vehicle ESP
        bool veh_state{ false };
        bool veh_name{ false };
        bool veh_health{ false };
        bool veh_status{ false };
        bool veh_box{ false };
        bool veh_line{ false };
        int veh_distance{ 100 };
        ImColor veh_color{ 255, 255, 255, 255 };
    } esp;

    struct {
        bool state{ false };
        bool draw_fov{ false };
        float fov{ 100.0f };
        float smooth{ 1.0f };
        int bone{ 0 };
        ImColor color{ 255, 0, 0, 255 };
    } aimbot;

    struct {
        bool enabled{ false };
        bool draw_fov{ false };
        float fov{ 100.0f };
        float chance{ 100.0f };
        int bone{ 0 };
        ImColor color{ 255, 0, 0, 255 };
    } silent;

    struct {
        std::string current_script;
        std::vector<std::string> resources;
        int selected_resource{ 0 };
    } executor;

    struct {
        std::vector<s_resource_dump> dumps_list;
        int selected_resource{ 0 };
        bool auto_save{ false };
        char save_path[256] = "C:\\MTA\\resources";
    } resources;

    struct {
        std::vector<s_events> events_list;
        char search_buffer[256] = "";
        bool log_events{ true };
    } event_handler;

    struct {
        bool break_debughook{ false };
    } settings;
};

inline c_elements* element = new c_elements();