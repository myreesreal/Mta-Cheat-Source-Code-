#include "visual.h"
#include <globals.h>
#include <data/elements.h>
#include <game/manager/manager.h>

bool world_to_screen(CVector& vecWorld, CVector& vecScreen, float fEdgeTolerance, bool bRelative)
{
	if (!element->content.loaded_client && !global->m_pCore)
	{
		return false;
	}

	global->m_pCore->GetGraphics()->CalcScreenCoors(&vecWorld, &vecScreen);

	float fResWidth = static_cast<float>(global->m_pCore->GetGraphics()->GetViewportWidth());
	float fResHeight = static_cast<float>(global->m_pCore->GetGraphics()->GetViewportHeight());

	float fToleranceX = bRelative ? fEdgeTolerance * fResWidth : fEdgeTolerance;
	float fToleranceY = bRelative ? fEdgeTolerance * fResHeight : fEdgeTolerance;

	fToleranceX = Clamp(0.f, fToleranceX, fResWidth * 10);
	fToleranceY = Clamp(0.f, fToleranceY, fResHeight * 10);

	if (vecScreen.fX >= -fToleranceX && vecScreen.fX <= fResWidth + fToleranceX && vecScreen.fY >= -fToleranceY && vecScreen.fY <= fResHeight + fToleranceY && vecScreen.fZ > 0.1f)
	{
		return true;
	}

	return false;
}

void draw_player_name(CClientPlayer* player)
{
	CClientEntity* entity = static_cast<CClientEntity*>(player);
	if (!entity)
		return;

	char szName[64];
	snprintf(szName, sizeof(szName), "%s", player->GetNametagText());

	CVector vecTargetPos, vecScreenPos;
	entity->GetPosition(vecTargetPos);
	vecTargetPos.fZ -= 1.1f;

	if (!world_to_screen(vecTargetPos, vecScreenPos, 0.0f, false))
		return;

	ImGui::GetBackgroundDrawList()->AddText(ImVec2(vecScreenPos.fX, vecScreenPos.fY), IM_COL32(255, 255, 255, 255), szName);
}

void draw_player_weapon(CClientPlayer* player)
{
	CClientEntity* entity = static_cast<CClientEntity*>(player);
	if (!entity)
		return;

	eWeaponType weaponId = manager->get_weapon_type(player);
	const char* szWeaponName = manager->get_weapon_name(weaponId);
	if (!szWeaponName || strlen(szWeaponName) == 0)
		return;

	CVector vecTargetPos, vecScreenPos;
	entity->GetPosition(vecTargetPos);
	vecTargetPos.fZ += 1.1f;

	if (!world_to_screen(vecTargetPos, vecScreenPos, 0.0f, false))
		return;

	ImGui::GetBackgroundDrawList()->AddText(ImVec2(vecScreenPos.fX, vecScreenPos.fY), IM_COL32(255, 255, 255, 255), szWeaponName);
}

void draw_player_box(CClientPlayer* player)
{
	CClientEntity* entity = static_cast<CClientEntity*>(player);
	if (!entity)
		return;

	CVector vecMin;
	entity->GetPosition(vecMin);
	vecMin.fZ += 1.0f;

	CVector vecMax;
	entity->GetPosition(vecMax);
	vecMax.fZ -= 1.0f;

	CVector screenTopLeft, screenBottomRight;
	if (!world_to_screen(vecMin, screenTopLeft, 0.0f, false) || !world_to_screen(vecMax, screenBottomRight, 0.0f, false))
		return;

	float height = screenBottomRight.fY - screenTopLeft.fY;
	float width = height / 2.0f;
	float left = screenTopLeft.fX - width / 2;
	float right = screenTopLeft.fX + width / 2;

	ImGui::GetBackgroundDrawList()->AddRect(ImVec2(left, screenTopLeft.fY), ImVec2(right, screenBottomRight.fY), IM_COL32(255, 255, 255, 255), 1.0f);
}

void draw_player_bone(CClientPlayer* player)
{
	
}

void draw_player_line(CClientPlayer* player)
{
	CClientEntity* entity = static_cast<CClientEntity*>(player);
	if (!entity)
		return;

	CClientEntity* local = static_cast<CClientEntity*>(global->m_pPlayerManager->GetLocalPlayer());
	if (!local)
		return;

	CVector vecLocalPos, vecTargetPos;
	local->GetPosition(vecLocalPos);
	vecLocalPos.fZ -= 1.0f;

	entity->GetPosition(vecTargetPos);
	vecTargetPos.fZ -= 1.0f;

	CVector vecLocalScreen, vecTargetScreen;
	if (!world_to_screen(vecLocalPos, vecLocalScreen, 0.0f, false) || !world_to_screen(vecTargetPos, vecTargetScreen, 0.0f, false))
		return;

	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(vecLocalScreen.fX, vecLocalScreen.fY), ImVec2(vecTargetScreen.fX, vecTargetScreen.fY), IM_COL32(255, 255, 255, 255), 1.0f);
}

void draw_vehicle_name(CClientVehicle* vehicle)
{
	CClientEntity* entity = static_cast<CClientEntity*>(vehicle);
	if (!entity)
		return;

	char szName[64];
	snprintf(szName, sizeof(szName), "%s [%d]", vehicle->GetNamePointer(), vehicle->GetModel());

	CVector vecTargetPos, vecScreenPos;
	entity->GetPosition(vecTargetPos);
	vecTargetPos.fZ += 1.5f;

	if (!world_to_screen(vecTargetPos, vecScreenPos, 0.0f, false))
		return;

	ImGui::GetBackgroundDrawList()->AddText(ImVec2(vecScreenPos.fX, vecScreenPos.fY), IM_COL32(255, 255, 255, 255), szName);
}


void draw_vehicle_box(CClientVehicle* vehicle)
{
	
}

void draw_vehicle_line(CClientVehicle* vehicle)
{
	CClientPlayer* local = manager->get_local_player();
	if (!local)
		return;

	CClientEntity* vehicle_entity = static_cast<CClientEntity*>(vehicle);
	if (!vehicle_entity)
		return;

	CClientEntity* local_entity = static_cast<CClientEntity*>(local);
	if (!local_entity)
		return;

	CVector vecLocalPos, vecTargetPos;
	local_entity->GetPosition(vecLocalPos);
	vecLocalPos.fZ -= 1.0f;

	vehicle_entity->GetPosition(vecTargetPos);
	vecTargetPos.fZ -= 1.0f;

	CVector vecLocalScreen, vecTargetScreen;
	if (!world_to_screen(vecLocalPos, vecLocalScreen, 0.0f, false) || !world_to_screen(vecTargetPos, vecTargetScreen, 0.0f, false))
		return;

	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(vecLocalScreen.fX, vecLocalScreen.fY), ImVec2(vecTargetScreen.fX, vecTargetScreen.fY), IM_COL32(0, 255, 0, 255), 1.0f);
}


void c_visual::draw_player_esp()
{
	if (!manager->is_loaded())
		return;

	if (!element->esp.state)
		return;

	std::vector<CClientPlayer*> players = manager->get_players();
	for (auto player : players)
	{
		if (!player || player->IsLocalPlayer())
			continue;

		if (element->esp.name)
			draw_player_name(player);

		if (element->esp.weapon)
			draw_player_weapon(player);

		if (element->esp.box)
			draw_player_box(player);

		if (element->esp.bone)
			draw_player_bone(player);

		if (element->esp.line)
			draw_player_line(player);
	}
}

void c_visual::draw_vehicle_esp()
{
	if (!manager->is_loaded())
		return;

	if (!element->esp.veh_state)
		return;

	std::vector<CClientVehicle*> vehicles = manager->get_vehicles();
	for (auto vehicle : vehicles)
	{
		if (!vehicle)
			continue;

		if (element->esp.veh_name)
			draw_vehicle_name(vehicle);

		if (element->esp.veh_box)
			draw_vehicle_box(vehicle);

		if (element->esp.veh_line)
			draw_vehicle_line(vehicle);
	}
}