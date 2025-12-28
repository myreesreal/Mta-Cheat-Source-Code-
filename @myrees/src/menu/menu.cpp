#include "menu/menu.h"
#include <globals.h>
#include <data/elements.h>
#include <data/variables.h>
#include <console/console.h>
#include <utilities/utilities.h>
#include <game/visual/visual.h>
#include <game/aimbot/aimbot.h>
#include <game/manager/manager.h>
#include <game/packet/packet.h>
#include <game/player/player.h>
#include <game/silent/silent.h>
#include <imgui/imgui_elements.h>
#include <imgui/data/fonts.h>
#include <imgui/data/images.h>
#include <client/client.h>
#include <game/weapon/weapon.h>

LRESULT WINAPI h_wndproc(HWND handle, UINT message, WPARAM word_param, LPARAM long_param)
{
    ImGui_ImplWin32_WndProcHandler(handle, message, word_param, long_param);
    const auto& io = ImGui::GetIO();
    if (var->gui.is_open && (io.WantCaptureMouse || io.WantCaptureKeyboard))
    {
        return true;
    }
    return CallWindowProcA(var->winapi.wnd_proc, handle, message, word_param, long_param);
}

HRESULT __stdcall h_present(IDirect3DDevice9* self, const RECT* sourceRect, const RECT* destRect, HWND destWindowOverride, const RGNDATA* dirtyRegion)
{
    var->winapi.device_dx9 = self;
    if (var->winapi.device_dx9)
    {
        menu->initialize();
        menu->draw();
    }
    return menu->o_present(self, sourceRect, destRect, destWindowOverride, dirtyRegion);
}

HRESULT __stdcall h_reset(IDirect3DDevice9* self, D3DPRESENT_PARAMETERS* presentationParameters)
{
    menu->shutdown(true);
    HRESULT result = menu->o_reset(self, presentationParameters);
    menu->shutdown(false);
    return result;
}

BOOL WINAPI h_cursor(int pos_x, int pos_y)
{
    if (var->gui.is_open)
        return FALSE;
    return menu->o_cursor(pos_x, pos_y);
}

void c_menu::initialize()
{
    if (var->gui.initialized)
        return;
    if (!var->winapi.device_dx9)
        return;
    var->winapi.device_dx9->GetCreationParameters(&var->winapi.device_par);
    if (!var->winapi.device_par.hFocusWindow)
        return;
    var->winapi.wnd_proc = (WNDPROC)SetWindowLongW(var->winapi.device_par.hFocusWindow, GWL_WNDPROC, (LONG)h_wndproc);
    if (!ImGui::GetCurrentContext())
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
    }
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.IniFilename = NULL;
    if (!ImGui::GetIO().BackendPlatformUserData)
        ImGui_ImplWin32_Init(var->winapi.device_par.hFocusWindow);
    if (!ImGui::GetIO().BackendRendererUserData)
        ImGui_ImplDX9_Init(var->winapi.device_dx9);
    style();
    if (!var->gui.dpi_changed)
    {
        ImFontConfig cfg;
        cfg.FontDataOwnedByAtlas = false;
        io.Fonts->Clear();
        var->assets.font[0] = io.Fonts->AddFontFromMemoryTTF(g_Font, sizeof g_Font, 24.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
        var->assets.font[1] = io.Fonts->AddFontFromMemoryTTF(g_Font, sizeof g_Font, 18.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
        var->assets.font[2] = io.Fonts->AddFontFromMemoryTTF(g_Font2, sizeof g_Font2, 18.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
        var->assets.font[3] = io.Fonts->AddFontFromMemoryTTF(g_IconFont, sizeof g_IconFont, 28.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
        var->assets.font[4] = io.Fonts->AddFontFromMemoryTTF(g_IconFont2, sizeof g_IconFont2, 18.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
        var->assets.font[5] = io.Fonts->AddFontFromMemoryTTF(g_IconFont3, sizeof g_IconFont3, 26.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
        io.Fonts->Build();
        HRESULT hr;
        if (!var->assets.profile_icon)
            hr = D3DXCreateTextureFromFileInMemoryEx(var->winapi.device_dx9, &g_ProfileIcon, sizeof(g_ProfileIcon), 16, 16, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &var->assets.profile_icon);
        if (!var->assets.profiles_icon)
            hr = D3DXCreateTextureFromFileInMemoryEx(var->winapi.device_dx9, &g_ProfilesIcon, sizeof(g_ProfilesIcon), 16, 16, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &var->assets.profiles_icon);
        if (!var->assets.vehicle_icon)
            hr = D3DXCreateTextureFromFileInMemoryEx(var->winapi.device_dx9, &g_VehicleIcon, sizeof(g_VehicleIcon), 16, 16, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &var->assets.vehicle_icon);
        if (!var->assets.aim_icon)
            hr = D3DXCreateTextureFromFileInMemoryEx(var->winapi.device_dx9, &g_AimIcon, sizeof(g_AimIcon), 16, 16, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &var->assets.aim_icon);
        if (!var->assets.weapon_icon)
            hr = D3DXCreateTextureFromFileInMemoryEx(var->winapi.device_dx9, &g_WeaponIcon, sizeof(g_WeaponIcon), 16, 16, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &var->assets.weapon_icon);
        if (!var->assets.settings_icon)
            hr = D3DXCreateTextureFromFileInMemoryEx(var->winapi.device_dx9, &g_SettingsIcon, sizeof(g_SettingsIcon), 16, 16, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &var->assets.settings_icon);
        if (!var->assets.visual_icon)
            hr = D3DXCreateTextureFromFileInMemoryEx(var->winapi.device_dx9, &g_VisualIcon, sizeof(g_VisualIcon), 16, 16, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &var->assets.visual_icon);
        var->gui.dpi_changed = true;
    }
    if (!m_editor)
    {
        m_editor = std::make_unique<TextEditor>();
        m_editor->SetLanguageDefinition(TextEditor::LanguageDefinition::Lua());
        m_editor->SetShowWhitespaces(false);
        m_editor->SetPalette(TextEditor::GetDarkPalette());
    }
    var->gui.initialized = true;
}

// Resize handle işlevi
bool DrawResizeHandle(ImVec2& winSize, float handleSize = 20.0f)
{
    ImVec2 windowPos = ImGui::GetWindowPos();
    ImVec2 windowSize = ImGui::GetWindowSize();

    // Sağ alt köşedeki boyutlandırma handle'ının konumu
    ImVec2 handlePos(windowPos.x + windowSize.x - handleSize, windowPos.y + windowSize.y - handleSize);

    // Handle'ı çiz - küçük bir üçgen şeklinde
    ImDrawList* drawList = ImGui::GetForegroundDrawList();
    drawList->AddTriangleFilled(
        ImVec2(handlePos.x + handleSize, handlePos.y + handleSize),
        ImVec2(handlePos.x + handleSize, handlePos.y),
        ImVec2(handlePos.x, handlePos.y + handleSize),
        ImColor(0.94f, 0.47f, 0.22f, 0.8f)
    );

    // Fare handle bölgesinde mi kontrol et
    ImVec2 mousePos = ImGui::GetMousePos();
    bool isHovered = (mousePos.x >= handlePos.x && mousePos.x <= handlePos.x + handleSize &&
        mousePos.y >= handlePos.y && mousePos.y <= handlePos.y + handleSize);

    // İlk tıklama için bastırıp sürüklemeyi başlat
    static bool isDragging = false;
    static ImVec2 dragStart;
    static ImVec2 initialSize;

    if (isHovered && ImGui::IsMouseClicked(0))
    {
        isDragging = true;
        dragStart = mousePos;
        initialSize = winSize;
    }

    // Sürükleme ve boyutlandırma
    if (isDragging)
    {
        if (ImGui::IsMouseDown(0))
        {
            ImVec2 dragDelta = ImVec2(mousePos.x - dragStart.x, mousePos.y - dragStart.y);
            winSize = ImVec2(initialSize.x + dragDelta.x, initialSize.y + dragDelta.y);

            // Minimum boyut kısıtlaması
            winSize.x = ImMax(winSize.x, 600.0f);
            winSize.y = ImMax(winSize.y, 400.0f);

            return true; // Boyut değişti
        }
        else
        {
            isDragging = false; // Fare bırakıldı, sürüklemeyi bitir
        }
    }

    return false; // Boyut değişmedi
}

void c_menu::draw()
{
    if (element->binds.menu_bind != 0 && GetAsyncKeyState(element->binds.menu_bind) & 1)
    {
        var->gui.is_open = !var->gui.is_open;
    }

    // F3 tuşu kontrolü - Lua Executor için
    if (GetAsyncKeyState(VK_F3) & 1)
    {
        m_show_executor_window = !m_show_executor_window;
    }

    // F4 tuşu kontrolü - Packet menüsü için
    if (GetAsyncKeyState(VK_F4) & 1)
    {
        m_show_packets_window = !m_show_packets_window;
    }

    RECT screen_rect;
    GetWindowRect(GetDesktopWindow(), &screen_rect);
    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    if (var->gui.is_open)
    {
        ImGui::SetNextWindowPos(ImVec2(screen_rect.right - var->window.size.x / 2, screen_rect.bottom - var->window.size.y / 2), ImGuiCond_Once);
        ImGui::SetNextWindowSize(var->window.size);
        ImGui::Begin("Toolkit", 0, var->window.flags);
        {
            ImDrawList* draw = ImGui::GetWindowDrawList();
            const auto& pos = ImGui::GetWindowPos();
            // Modern header with gradient
            draw->AddRectFilledMultiColor(
                ImVec2(pos.x, pos.y),
                ImVec2(pos.x + var->window.size.x, pos.y + 60),
                ImColor(15, 15, 15),
                ImColor(20, 20, 20),
                ImColor(20, 20, 20),
                ImColor(15, 15, 15)
            );
            // Logo and title with glow
            float logoSize = 32.0f;
            draw->AddText(var->assets.font[1], 28, ImVec2(pos.x + 14, pos.y + 12), ImColor(255, 255, 255), "EMPERIA");
            draw->AddText(var->assets.font[1], 28, ImVec2(pos.x + 120, pos.y + 12), ImColor(0.94f, 0.47f, 0.22f, 1.00f), "CHEATS");
            // Accent line with glow
            draw->AddRectFilledMultiColor(
                ImVec2(pos.x, pos.y + 60),
                ImVec2(pos.x + var->window.size.x, pos.y + 62),
                ImColor(0.94f, 0.47f, 0.22f, 1.00f),
                ImColor(0.94f, 0.47f, 0.22f, 1.00f),
                ImColor(0.94f, 0.47f, 0.22f, 0.50f),
                ImColor(0.94f, 0.47f, 0.22f, 0.50f)
            );
            // Modern sidebar with gradient
            draw->AddRectFilledMultiColor(
                ImVec2(pos.x, pos.y + 62),
                ImVec2(pos.x + 160, pos.y + var->window.size.y),
                ImColor(12, 12, 12),
                ImColor(15, 15, 15),
                ImColor(15, 15, 15),
                ImColor(12, 12, 12)
            );
            // Sidebar content with better spacing and hover effects
            ImGui::SetCursorPos(ImVec2(10, 70));
            ImGui::BeginGroup();
            {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));
                ImGui::TextLine("GENERAL");
                ImGui::PopStyleColor();
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 8));
                if (ImGui::Tab("Self", var->assets.profile_icon, 0 == var->gui.m_selectedTab)) { var->gui.m_selectedTab = 0; }
                if (ImGui::Tab("Vehicle", var->assets.vehicle_icon, 1 == var->gui.m_selectedTab)) { var->gui.m_selectedTab = 1; }
                if (ImGui::Tab("Weapon", var->assets.weapon_icon, 2 == var->gui.m_selectedTab)) { var->gui.m_selectedTab = 2; }
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));
                ImGui::TextLine("VISUALS");
                ImGui::PopStyleColor();
                if (ImGui::Tab("Players", var->assets.visual_icon, 3 == var->gui.m_selectedTab)) { var->gui.m_selectedTab = 3; }
                if (ImGui::Tab("Vehicles", var->assets.visual_icon, 4 == var->gui.m_selectedTab)) { var->gui.m_selectedTab = 4; }
                ImGui::TextLine("COMBAT");
                if (ImGui::Tab("Aimbot", var->assets.aim_icon, 5 == var->gui.m_selectedTab)) { var->gui.m_selectedTab = 5; }
                if (ImGui::Tab("Silent", var->assets.aim_icon, 6 == var->gui.m_selectedTab)) { var->gui.m_selectedTab = 6; }
                ImGui::TextLine("LUA");
                if (ImGui::Tab("Executor", var->assets.settings_icon, 7 == var->gui.m_selectedTab)) { var->gui.m_selectedTab = 7; }
                if (ImGui::Tab("Resources", var->assets.settings_icon, 8 == var->gui.m_selectedTab)) { var->gui.m_selectedTab = 8; }
                if (ImGui::Tab("Events", var->assets.settings_icon, 9 == var->gui.m_selectedTab)) { var->gui.m_selectedTab = 9; }
                if (ImGui::Tab("Packets", var->assets.settings_icon, 10 == var->gui.m_selectedTab)) { var->gui.m_selectedTab = 10; }
                ImGui::TextLine("Other");
                if (ImGui::Tab("Settings", var->assets.settings_icon, 11 == var->gui.m_selectedTab)) { var->gui.m_selectedTab = 11; }
                ImGui::PopStyleVar();
            }
            ImGui::EndGroup();
            // Content area with subtle gradient
            draw->AddRectFilledMultiColor(
                ImVec2(pos.x + 170, pos.y + 70),
                ImVec2(pos.x + var->window.size.x - 10, pos.y + var->window.size.y - 10),
                ImColor(10, 10, 10),
                ImColor(12, 12, 12),
                ImColor(12, 12, 12),
                ImColor(10, 10, 10)
            );
            switch (var->gui.m_selectedTab)
            {
            case 0:
                ImGui::SetCursorPos(ImVec2(170, 60));
                component->begin_child("Player", ImVec2(250, 430));
                {
                    // Modern group styling
                    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.08f, 0.08f, 0.08f, 1.0f));
                    ImGui::BeginChild("PlayerFeatures", ImVec2(230, 150), true);
                    {
                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.94f, 0.47f, 0.22f, 1.00f));
                        ImGui::Text("Player Features");
                        ImGui::PopStyleColor();
                        ImGui::Separator();
                        ImGui::Spacing();
                        // Modern checkbox style
                        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.12f, 0.12f, 0.12f, 1.0f));
                        ImGui::Checkbox("Godmode", &element->player.godmode);
                        ImGui::Checkbox("Airbreak", &element->player.airbreak);
                        ImGui::Checkbox("Sprint", &element->player.sprint);
                        ImGui::PopStyleColor();
                    }
                    ImGui::EndChild();
                    ImGui::PopStyleColor();
                    ImGui::Spacing();
                    ImGui::Spacing();
                    // Speed controls group
                    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.08f, 0.08f, 0.08f, 1.0f));
                    ImGui::BeginChild("SpeedControls", ImVec2(230, 100), true);
                    {
                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.94f, 0.47f, 0.22f, 1.00f));
                        ImGui::Text("Speed Settings");
                        ImGui::PopStyleColor();
                        ImGui::Separator();
                        ImGui::Spacing();
                        // Modern slider style
                        ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(0.94f, 0.47f, 0.22f, 1.00f));
                        ImGui::SliderFloat("Airbreak Speed", &element->player.airbreak_speed, 0.1f, 10.0f);
                        ImGui::SliderFloat("Sprint Speed", &element->player.sprint_speed, 0.1f, 10.0f);
                        ImGui::PopStyleColor();
                    }
                    ImGui::EndChild();
                    ImGui::PopStyleColor();
                    ImGui::Spacing();
                    ImGui::Spacing();
                    // Health & Armor group
                    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.08f, 0.08f, 0.08f, 1.0f));
                    ImGui::BeginChild("HealthArmor", ImVec2(230, 140), true);
                    {
                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.94f, 0.47f, 0.22f, 1.00f));
                        ImGui::Text("Health & Armor");
                        ImGui::PopStyleColor();
                        ImGui::Separator();
                        ImGui::Spacing();
                        ImGui::SliderFloat("Health", &element->player.health, 0.0f, 100.0f);
                        if (ImGui::Button("Set Health", ImVec2(208, 25)))
                        {
                            CClientPlayer* local = manager->get_local_player();
                            if (local && local->GetGamePlayer())
                                local->GetGamePlayer()->SetHealth(element->player.health);
                        }
                        ImGui::Spacing();
                        ImGui::SliderFloat("Armor", &element->player.armor, 0.0f, 100.0f);
                        if (ImGui::Button("Set Armor", ImVec2(208, 25)))
                        {
                            CClientPlayer* local = manager->get_local_player();
                            if (local && local->GetGamePlayer())
                                local->GetGamePlayer()->SetArmor(element->player.armor);
                        }
                    }
                    ImGui::EndChild();
                    ImGui::PopStyleColor();
                }
                component->end_child();
                break;
            case 1:
                ImGui::SetCursorPos(ImVec2(170, 60));
                component->begin_child("Vehicle", ImVec2(250, 430));
                {
                    ImGui::Checkbox("Run Engine", &element->vehicle.run_engine);
                    ImGui::Checkbox("Unlock Vehicle", &element->vehicle.unlock_vehicle);
                    if (ImGui::Button("Fix Vehicle", ImVec2(208, 25)))
                        element->vehicle.fix_vehicle = true;
                }
                component->end_child();
                break;
            case 2:
                ImGui::SetCursorPos(ImVec2(170, 60));
                component->begin_child("Weapon", ImVec2(250, 430));
                {
                    ImGui::Combo("Weapon", &element->weapon.selected_weapon, element->weapon.weapon_list, 47);
                    if (ImGui::Button("Give Weapon", ImVec2(208, 25)))
                    {
                        CClientPlayer* local = manager->get_local_player();
                        if (local)
                        {
                            weapon->give_weapon(local, (eWeaponType)element->weapon.selected_weapon, 200, true);
                        }
                    }
                    ImGui::Checkbox("Rapid Fire", &element->weapon.rapid_fire);
                    ImGui::SliderFloat("Rapid Fire Speed", &element->weapon.rapid_fire_speed, 0.1f, 10.0f);
                    ImGui::Checkbox("Infinite Ammo", &element->weapon.infinite_ammo);
                }
                component->end_child();
                break;
            case 3:
                ImGui::SetCursorPos(ImVec2(170, 60));
                component->begin_child("Players ESP", ImVec2(250, 430));
                {
                    ImGui::Checkbox("Enable ESP", &element->esp.state);
                    ImGui::Checkbox("Name", &element->esp.name);
                    ImGui::Checkbox("Weapon", &element->esp.weapon);
                    ImGui::Checkbox("Box", &element->esp.box);
                    ImGui::Checkbox("Bone", &element->esp.bone);
                    ImGui::Checkbox("Line", &element->esp.line);
                    ImGui::Checkbox("Health", &element->esp.health);
                    ImGui::SliderInt("Max Distance", &element->esp.distance, 0, 6000);
                    ImGui::ColorEdit4("##Player esp color", (float*)&element->esp.color, ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoTooltip);
                }
                component->end_child();
                break;
            case 4:
                ImGui::SetCursorPos(ImVec2(170, 60));
                component->begin_child("Vehicle ESP", ImVec2(250, 430));
                {
                    ImGui::Checkbox("Enable ESP", &element->esp.veh_state);
                    ImGui::Checkbox("Name", &element->esp.veh_name);
                    ImGui::Checkbox("Health", &element->esp.veh_health);
                    ImGui::Checkbox("Status", &element->esp.veh_status);
                    ImGui::Checkbox("Box", &element->esp.veh_box);
                    ImGui::Checkbox("Line", &element->esp.veh_line);
                    ImGui::SliderInt("Max Distance", &element->esp.veh_distance, 0, 6000);
                    ImGui::ColorEdit4("##Vehicle esp color", (float*)&element->esp.veh_color, ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoTooltip);
                }
                component->end_child();
                break;
            case 5:
                ImGui::SetCursorPos(ImVec2(170, 60));
                component->begin_child("Aimbot", ImVec2(250, 430));
                {
                    ImGui::Checkbox("Enable Aimbot", &element->aimbot.state);
                    ImGui::Checkbox("Draw FOV", &element->aimbot.draw_fov);
                    ImGui::SliderFloat("FOV", &element->aimbot.fov, 10.0f, 500.0f);
                    ImGui::SliderFloat("Smooth", &element->aimbot.smooth, 1.0f, 100.0f);
                    ImGui::Combo("Target Bone", &element->aimbot.bone, "Head\0Body\0");
                    ImGui::ColorEdit4("##Aimbot fov color", (float*)&element->aimbot.color, ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoTooltip);
                }
                component->end_child();
                break;
            case 6:
                ImGui::SetCursorPos(ImVec2(170, 60));
                component->begin_child("Silent Aim", ImVec2(250, 430));
                {
                    ImGui::Checkbox("Enable Silent", &element->silent.enabled);
                    ImGui::Checkbox("Draw FOV", &element->silent.draw_fov);
                    ImGui::SliderFloat("FOV", &element->silent.fov, 10.0f, 500.0f);
                    ImGui::SliderFloat("Hit Chance", &element->silent.chance, 0.0f, 100.0f, "%.1f%%");
                    ImGui::Combo("Target Bone", &element->silent.bone, "Head\0Body\0");
                    ImGui::ColorEdit4("##Silent fov color", (float*)&element->silent.color, ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoTooltip);
                }
                component->end_child();
                break;
            case 7:
                if (!m_show_executor_window)  // Eğer pencere kapalıysa
                {
                    m_show_executor_window = true;  // Pencereyi aç
                }
                break;
            case 8:
                ImGui::SetCursorPos(ImVec2(170, 60));
                component->begin_child("Resources", ImVec2(var->window.size.x - 414, 435));
                {
                    // Get resources list
                    std::vector<CResource*> resources = manager->get_resources();
                    // Add search box for resources
                    static char resourceSearchBuffer[256] = "";
                    ImGui::InputText("Search Resource", resourceSearchBuffer, sizeof(resourceSearchBuffer));
                    ImGui::Separator();
                    if (ImGui::BeginTable("ResourcesTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg))
                    {
                        ImGui::TableSetupColumn("Resource Name", ImGuiTableColumnFlags_WidthStretch);
                        ImGui::TableSetupColumn("Status", ImGuiTableColumnFlags_WidthFixed, 100.0f);
                        ImGui::TableHeadersRow();
                        for (auto* resource : resources)
                        {
                            if (!resource || !resource->GetName())
                                continue;
                            // Filter by search if needed
                            if (resourceSearchBuffer[0] != '\0')
                            {
                                if (!strstr(resource->GetName(), resourceSearchBuffer))
                                    continue;
                            }
                            ImGui::TableNextRow();
                            // Resource Name
                            ImGui::TableNextColumn();
                            ImGui::Text("%s", resource->GetName());
                            // Status
                            ImGui::TableNextColumn();
                            bool isRunning = resource->IsActive();
                            ImGui::TextColored(
                                isRunning ? ImVec4(0.3f, 1.0f, 0.3f, 1.0f) : ImVec4(1.0f, 0.3f, 0.3f, 1.0f),
                                "%s", isRunning ? "Running" : "Stopped"
                            );
                        }
                        ImGui::EndTable();
                    }
                }
                component->end_child();
                break;
            case 9:
                ImGui::SetCursorPos(ImVec2(170, 60));
                component->begin_child("Events", ImVec2(var->window.size.x - 180, var->window.size.y - 70));
                {
                    // Add search box for events
                    static char eventSearchBuffer[256] = "";
                    ImGui::InputText("Search Event", eventSearchBuffer, sizeof(eventSearchBuffer));
                    ImGui::Separator();
                    if (ImGui::BeginTable("EventsTable", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg))
                    {
                        ImGui::TableSetupColumn("Event Name", ImGuiTableColumnFlags_WidthStretch);
                        ImGui::TableSetupColumn("Status", ImGuiTableColumnFlags_WidthFixed, 100.0f);
                        ImGui::TableSetupColumn("Actions", ImGuiTableColumnFlags_WidthFixed, 100.0f);
                        ImGui::TableHeadersRow();
                        for (auto& event : element->event_handler.events_list)
                        {
                            // Filter by search if needed
                            if (eventSearchBuffer[0] != '\0')
                            {
                                if (!strstr(event.event_name.c_str(), eventSearchBuffer))
                                    continue;
                            }
                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            ImGui::Text("%s", event.event_name.c_str());
                            ImGui::TableNextColumn();
                            ImGui::TextColored(
                                event.is_blocked ? ImVec4(1.0f, 0.3f, 0.3f, 1.0f) : ImVec4(0.3f, 1.0f, 0.3f, 1.0f),
                                "%s", event.is_blocked ? "BLOCKED" : "ACTIVE"
                            );
                            ImGui::TableNextColumn();
                            ImGui::PushID(event.event_name.c_str());
                            // Modern toggle button - Ortalanmış buton
                            float buttonWidth = 90.0f;
                            float columnWidth = ImGui::GetColumnWidth();
                            float buttonX = (columnWidth - buttonWidth) * 0.5f;
                            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + buttonX);
                            ImGui::PushStyleColor(ImGuiCol_Button, event.is_blocked ?
                                ImVec4(0.8f, 0.3f, 0.3f, 0.7f) : ImVec4(0.3f, 0.8f, 0.3f, 0.7f));
                            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, event.is_blocked ?
                                ImVec4(0.9f, 0.3f, 0.3f, 0.9f) : ImVec4(0.3f, 0.9f, 0.3f, 0.9f));
                            ImGui::PushStyleColor(ImGuiCol_ButtonActive, event.is_blocked ?
                                ImVec4(0.7f, 0.2f, 0.2f, 1.0f) : ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
                            // Buton metni ve stil
                            ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.5f));
                            if (ImGui::Button(event.is_blocked ? "UNBLOCK" : "BLOCK", ImVec2(buttonWidth, 25)))
                            {
                                event.is_blocked = !event.is_blocked;
                            }
                            ImGui::PopStyleVar();
                            ImGui::PopStyleColor(3);
                            ImGui::PopID();
                        }
                        ImGui::EndTable();
                    }
                }
                component->end_child();
                break;
            case 10:
                if (!m_show_packets_window)  // Eğer pencere kapalıysa
                {
                    m_show_packets_window = true;  // Pencereyi aç
                }
                break;
            case 11:
                ImGui::SetCursorPos(ImVec2(170, 60));
                component->begin_child("Settings", ImVec2(250, 430));
                {
                    static bool isBinding = false;
                    ImGui::Text("Last Packet Resend Bind:");
                    if (ImGui::Button(isBinding ? "Waiting..." : "[ Bind Key ]", ImVec2(208, 25)))
                    {
                        isBinding = true;
                    }
                    if (isBinding)
                    {
                        for (int i = 0; i < 256; i++)
                        {
                            if (GetAsyncKeyState(i) & 0x8000)
                            {
                                packet->SetResendKey(i);
                                isBinding = false;
                                break;
                            }
                        }
                    }
                    ImGui::Spacing();
                    ImGui::Separator();
                    ImGui::Spacing();
                    // Add packet drop toggle
                    static bool packetDropEnabled = false;
                    if (ImGui::Checkbox("Enable Packet Drop", &packetDropEnabled))
                    {
                        packet->SetPacketDropEnabled(packetDropEnabled);
                    }
                    ImGui::Spacing();
                    ImGui::Separator();
                    ImGui::Spacing();
                    // Add 100x resend option
                    static bool multiResendEnabled = false;
                    if (ImGui::Checkbox("100x Resend on Hotkey", &multiResendEnabled))
                    {
                        packet->SetMultiResendEnabled(multiResendEnabled);
                    }

                    // Kısayol tuşları için bilgi
                    ImGui::Spacing();
                    ImGui::Separator();
                    ImGui::Spacing();
                    ImGui::TextColored(ImVec4(0.94f, 0.47f, 0.22f, 1.00f), "Hotkeys:");
                    ImGui::Text("F3 - Toggle Lua Executor");
                    ImGui::Text("F4 - Toggle Packet Window");
                    // ... rest of settings ...
                }
                component->end_child();
                break;
            }

            // Resize handle - Ana menü penceresine boyutlandırma kolu ekle
            if (DrawResizeHandle(var->window.size))
            {
                // Boyut değiştiğinde GUI'yi yeniden boyutlandırıyoruz
                ImGui::SetWindowSize(var->window.size);
            }
        }
        ImGui::End();
    }

    // Packet Window - Allow it to be toggled with F4
    static ImVec2 packetWindowSize(800, 600); // Packet penceresi için dinamik boyut
    if (m_show_packets_window)
    {
        ImGui::SetNextWindowSize(packetWindowSize, ImGuiCond_FirstUseEver);
        // Pencere başlığını ve kapatma butonunu göster
        bool window_open = true;
        if (ImGui::Begin("Packet Resend", &window_open, ImGuiWindowFlags_NoCollapse))
        {
            // Search and Control Section
            {
                ImGui::BeginChild("PacketControls", ImVec2(ImGui::GetWindowWidth() - 16, 80), true);
                ImGui::PushItemWidth(200);
                ImGui::InputText("Search Packet ID", packet->GetSearchBuffer(), 256);
                ImGui::PopItemWidth();
                ImGui::Spacing();
                // Actions Menu
                if (ImGui::Button("Actions Menu", ImVec2(150, 25)))
                    ImGui::OpenPopup("packet_actions");
                if (ImGui::BeginPopup("packet_actions"))
                {
                    if (ImGui::MenuItem("Clear All"))
                    {
                        packet->clear();
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::EndPopup();
                }
                ImGui::EndChild();
            }
            // Packet History Section
            ImGui::BeginChild("PacketHistory", ImVec2(0, 0), true);
            if (ImGui::BeginTable("PacketTable", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg))
            {
                ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, 70.0f);
                ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed, 70.0f);
                ImGui::TableSetupColumn("Timestamp", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Actions", ImGuiTableColumnFlags_WidthFixed, 200.0f);
                ImGui::TableHeadersRow();
                const auto& packets = packet->GetPackets();
                for (size_t i = 0; i < packets.size(); i++)
                {
                    // Filter by search if needed
                    if (packet->GetSearchBufferConst()[0] != '\0')
                    {
                        char packetIdStr[8];
                        sprintf(packetIdStr, "%d", packets[i].packetId);
                        if (!strstr(packetIdStr, packet->GetSearchBufferConst()))
                            continue;
                    }
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("%d", packets[i].packetId);
                    ImGui::TableNextColumn();
                    ImGui::Text("%zu", packets[i].dataSize);
                    ImGui::TableNextColumn();
                    ImGui::Text("%s", packets[i].timestamp.c_str());
                    ImGui::TableNextColumn();
                    ImGui::PushID(static_cast<int>(i));
                    if (ImGui::Button("Resend", ImVec2(60, 20)))
                    {
                        packet->resend(i);
                    }
                    ImGui::SameLine();
                    if (ImGui::Button(packets[i].blocked ? "Unblock" : "Block", ImVec2(60, 20)))
                    {
                        packet->block(packets[i].packetId, !packets[i].blocked);
                    }
                    ImGui::PopID();
                }
                ImGui::EndTable();
            }
            ImGui::EndChild();

            // Packet penceresi için boyutlandırma kolu ekle
            if (DrawResizeHandle(packetWindowSize))
            {
                // Boyut değiştiğinde pencereyi yeniden boyutlandır
                ImGui::SetWindowSize(packetWindowSize);
            }

            ImGui::End();
        }
        // Pencere kapatıldıysa flag'i güncelle
        if (!window_open)
        {
            m_show_packets_window = false;
        }
    }

    // Executor Window - Allow it to be toggled with F3
    static ImVec2 executorSize(800, 600); // Executor penceresi için dinamik boyut
    if (m_show_executor_window)
    {
        ImGui::SetNextWindowSize(executorSize, ImGuiCond_FirstUseEver);
        bool window_open = true;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15, 15));
        if (ImGui::Begin("Lua Executor", &window_open, ImGuiWindowFlags_NoCollapse))
        {
            // Status banner
            ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.94f, 0.47f, 0.22f, 0.2f));
            ImGui::BeginChild("StatusBanner", ImVec2(ImGui::GetWindowWidth() - 30, 40), true);
            {
                ImGui::PushFont(var->assets.font[1]);
                ImGui::SetCursorPos(ImVec2(10, 10));
                ImGui::TextColored(ImVec4(0.94f, 0.47f, 0.22f, 1.0f), "Debug Hook Spoofer Working");
                ImGui::PopFont();
            }
            ImGui::EndChild();
            ImGui::PopStyleColor();
            ImGui::Spacing();
            // Resource Selection Bar
            {
                float comboWidth = ImGui::GetWindowWidth() * 0.7f - 15;
                ImGui::SetNextItemWidth(comboWidth);
                const char* current_item = element->executor.resources.empty() ?
                    "Select Resource" :
                    element->executor.resources[element->executor.selected_resource].c_str();
                if (ImGui::BeginCombo("##Resources", current_item, ImGuiComboFlags_HeightLarge))
                {
                    // Get and update resources list if empty
                    if (element->executor.resources.empty())
                    {
                        std::vector<CResource*> resources = manager->get_resources();
                        for (auto* resource : resources)
                        {
                            if (resource && resource->GetName())
                            {
                                element->executor.resources.push_back(resource->GetName());
                            }
                        }
                    }
                    for (int i = 0; i < element->executor.resources.size(); i++)
                    {
                        bool is_selected = (element->executor.selected_resource == i);
                        if (ImGui::Selectable(element->executor.resources[i].c_str(), is_selected))
                        {
                            element->executor.selected_resource = i;
                        }
                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }
                ImGui::SameLine();
                if (ImGui::Button("Refresh", ImVec2(100, 0)))
                {
                    element->executor.resources.clear();
                    std::vector<CResource*> resources = manager->get_resources();
                    for (auto* resource : resources)
                    {
                        if (resource && resource->GetName())
                        {
                            element->executor.resources.push_back(resource->GetName());
                        }
                    }
                    if (!element->executor.resources.empty())
                    {
                        element->executor.selected_resource = 0;
                    }
                }
            }
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            // Editor Toolbar
            {
                // Execute button with orange theme - En üstte
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.94f, 0.47f, 0.22f, 0.80f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.94f, 0.47f, 0.22f, 1.00f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.94f, 0.47f, 0.22f, 0.60f));
                if (ImGui::Button("Execute", ImVec2(120, 30)) && !element->executor.resources.empty())
                {
                    if (element->executor.selected_resource < element->executor.resources.size())
                    {
                        const std::string& script = m_editor->GetText();
                        if (!script.empty())
                        {
                            bool previous_bypass_state = element->settings.break_debughook;
                            element->settings.break_debughook = true;
                            std::vector<CResource*> resources = manager->get_resources();
                            for (auto* resource : resources)
                            {
                                if (resource && resource->GetName() == element->executor.resources[element->executor.selected_resource])
                                {
                                    CLuaMain* luaVM = resource->GetVM();
                                    if (luaVM)
                                    {
                                        Send_Script_Packet = true;
                                        client->o_load_script(luaVM, script.c_str(), script.length(), element->executor.resources[element->executor.selected_resource].c_str());
                                    }
                                    break;
                                }
                            }
                            element->settings.break_debughook = previous_bypass_state;
                        }
                    }
                }
                ImGui::PopStyleColor(3);
                ImGui::SameLine();
                float spacing = ImGui::GetWindowWidth() - 520; // Sağdaki butonlar için boşluk
                ImGui::Dummy(ImVec2(spacing, 0));
                ImGui::SameLine();
                // Diğer butonlar sağda
                if (ImGui::Button("Clear", ImVec2(80, 30))) {
                    m_editor->SetText("");
                }
                ImGui::SameLine();
                if (ImGui::Button("Copy", ImVec2(80, 30))) {
                    ImGui::SetClipboardText(m_editor->GetText().c_str());
                }
                ImGui::SameLine();
                if (ImGui::Button("Paste", ImVec2(80, 30))) {
                    m_editor->SetText(ImGui::GetClipboardText());
                }
                ImGui::SameLine();
                if (ImGui::Button("Save", ImVec2(80, 30))) {
                    // Save functionality
                }
            }
            ImGui::Spacing();
            // Script Editor
            ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.06f, 0.06f, 0.06f, 1.0f));
            ImGui::BeginChild("Editor", ImVec2(0, 0), true);
            m_editor->Render("##editor");
            ImGui::EndChild();
            ImGui::PopStyleColor();

            // Executor penceresi için boyutlandırma kolu ekle
            if (DrawResizeHandle(executorSize))
            {
                // Boyut değiştiğinde pencereyi yeniden boyutlandır
                ImGui::SetWindowSize(executorSize);
            }

            ImGui::End();
        }
        ImGui::PopStyleVar();
        if (!window_open)
            m_show_executor_window = false;
    }

    player->release();
    weapon->release();
    visual->draw_player_esp();
    visual->draw_vehicle_esp();
    aimbot->draw_player_aimbot();
    silent->get_target();
    silent->draw_fov();
    ImGui::GetIO().MouseDrawCursor = var->gui.is_open;
    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
    // F6 hotkey handling can stay here
    if (!var->gui.is_open)
    {
        if (GetAsyncKeyState(packet->GetResendKey()) & 1)
        {
            const auto& packets = packet->GetPackets();
            if (!packets.empty()) {
                packet->resend(packets.size() - 1);
            }
        }
    }
}

void c_menu::shutdown(bool before)
{
    if (before)
    {
        ImGui_ImplDX9_InvalidateDeviceObjects();
        return;
    }
    // Recreate device objects
    if (var->winapi.device_dx9)
    {
        ImGui_ImplDX9_CreateDeviceObjects();
    }
}

void c_menu::style()
{
    ImGuiStyle* Style = &ImGui::GetStyle();
    // Modern rounded corners
    Style->WindowRounding = 8.0f;
    Style->ChildRounding = 6.0f;
    Style->FrameRounding = 5.0f;
    Style->PopupRounding = 4.0f;
    Style->ScrollbarRounding = 4.0f;
    Style->GrabRounding = 4.0f;
    Style->TabRounding = 4.0f;
    // Better spacing and shadows
    Style->WindowPadding = ImVec2(10, 10);
    Style->FramePadding = ImVec2(8, 4);
    Style->ItemSpacing = ImVec2(10, 8);
    Style->ItemInnerSpacing = ImVec2(6, 6);
    Style->IndentSpacing = 22.0f;
    Style->ScrollbarSize = 12.0f;
    Style->GrabMinSize = 8.0f;
    Style->WindowBorderSize = 1.0f;
    Style->ChildBorderSize = 1.0f;
    Style->PopupBorderSize = 1.0f;
    Style->FrameBorderSize = 1.0f;
    // Modern color scheme with gradients
    ImVec4* colors = Style->Colors;
    colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
    colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.10f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.94f, 0.47f, 0.22f, 0.40f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.94f, 0.47f, 0.22f, 0.60f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.94f, 0.47f, 0.22f, 0.60f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.04f, 0.04f, 0.04f, 0.60f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.94f, 0.47f, 0.22f, 0.30f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.94f, 0.47f, 0.22f, 0.50f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.94f, 0.47f, 0.22f, 0.70f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.94f, 0.47f, 0.22f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.94f, 0.47f, 0.22f, 0.40f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.94f, 0.47f, 0.22f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.94f, 0.47f, 0.22f, 0.60f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.94f, 0.47f, 0.22f, 1.00f);
}

bool c_menu::release()
{
    var->winapi.mh_status = MH_CreateHook(&::SetCursorPos, &h_cursor, reinterpret_cast<LPVOID*>(&o_cursor));
    if (var->winapi.mh_status != MH_OK)
    {
        console->error("Failed to create Hook at Cursor: %d\n", var->winapi.mh_status);
        return false;
    }
    var->winapi.mh_status = MH_EnableHook(&::SetCursorPos);
    if (var->winapi.mh_status != MH_OK)
    {
        console->error("Failed to enable Hook at Cursor: %d\n", var->winapi.mh_status);
        return false;
    }
    /* ============ */
    void* dw_present = reinterpret_cast<void*>(utilities::c_device::get_address(17));
    var->winapi.mh_status = MH_CreateHook(dw_present, &h_present, reinterpret_cast<LPVOID*>(&o_present));
    if (var->winapi.mh_status != MH_OK)
    {
        console->error("Failed to make Hook at Present: %d\n", var->winapi.mh_status);
        return false;
    }
    var->winapi.mh_status = MH_EnableHook(dw_present);
    if (var->winapi.mh_status != MH_OK)
    {
        console->error("Failed to enable Hook at Present: %d\n", var->winapi.mh_status);
        return false;
    }
    /* ============ */
    void* dw_reset = reinterpret_cast<void*>(utilities::c_device::get_address(16));
    var->winapi.mh_status = MH_CreateHook(dw_reset, &h_reset, reinterpret_cast<LPVOID*>(&o_reset));
    if (var->winapi.mh_status != MH_OK)
    {
        console->error("Failed to make Hook at Reset: %d\n", var->winapi.mh_status);
        return false;
    }
    var->winapi.mh_status = MH_EnableHook(dw_reset);
    if (var->winapi.mh_status != MH_OK)
    {
        console->error("Failed to enable Hook at Reset: %d\n", var->winapi.mh_status);
        return false;
    }
    return true;
}