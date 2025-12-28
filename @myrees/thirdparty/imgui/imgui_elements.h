#pragma once
#define IMGUI_DEFINE_MATH_OPERATORS

#include <cstdint>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <map>

#include "imgui.h"
#include "imgui_internal.h"

using namespace ImGui;

class components {
public:
    bool begin_child_ex(const char* name, ImGuiID id, const ImVec2& size_arg, bool border, ImGuiWindowFlags flags);
    bool begin_child(const char* str_id, const ImVec2& size = ImVec2(0, 0), bool border = false, ImGuiWindowFlags flags = 0);
    bool begin_child(ImGuiID id, const ImVec2& size = ImVec2(0, 0), bool border = false, ImGuiWindowFlags flags = 0);
    void end_child();
};

inline components* component = new components();