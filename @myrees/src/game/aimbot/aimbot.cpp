#include "aimbot.h"
#include <globals.h>
#include <game/manager/manager.h>
#include <data/elements.h>

void c_aimbot::draw_aimbot_fov()
{
	if (!element->aimbot.state)
		return;

	if (!element->aimbot.draw_fov)
		return;

	CCamera* camera = global->m_pGame->GetCamera();
	if (!camera)
		return;

	eCamMode cmera_viewmode = static_cast<eCamMode>(camera->GetCam(camera->GetActiveCam())->GetMode());
	if (cmera_viewmode == 53 || cmera_viewmode == 7)
	{
		s_aimbot_config.is_player_aiming = true;
	}
	else
	{
		s_aimbot_config.is_player_aiming = false;
	}

	eWeaponType weapon = manager->get_weapon_type(manager->get_local_player());
	if (weapon != WEAPONTYPE_SNIPERRIFLE)
	{
		s_aimbot_config.vec_crosshair.fX = ImGui::GetIO().DisplaySize.x * 0.5299999714f;
		s_aimbot_config.vec_crosshair.fY = ImGui::GetIO().DisplaySize.y * 0.4f;
	}
	else
	{
		s_aimbot_config.vec_crosshair.fX = ImGui::GetIO().DisplaySize.x / 2.f;
		s_aimbot_config.vec_crosshair.fY = ImGui::GetIO().DisplaySize.y / 2.f;
	}


	ImGui::GetBackgroundDrawList()->AddCircle(ImVec2(s_aimbot_config.vec_crosshair.fX, s_aimbot_config.vec_crosshair.fY), element->aimbot.fov, IM_COL32(255, 255, 255, 255));
}

void c_aimbot::draw_target_player()
{
	s_aimbot_config.i_target_player = nullptr;
	float fNearestDistance = 9999.f;

	if (!element->aimbot.state)
		return;

	if (!s_aimbot_config.is_player_aiming)
		return;

	CClientPlayer* local = manager->get_local_player();
	if (!local || !local->GetGamePlayer())
		return;

	CVector vec_camera, vec_origin, vec_target;
	if (!local->GetGamePlayer()->GetBonePosition(BONE_RIGHTWRIST, &vec_origin))
		return;

	if (!global->m_pGame || !global->m_pGame->GetCamera())
		return;

	if (!global->m_pGame->GetWorld())
		return;

	global->m_pGame->GetCamera()->Find3rdPersonCamTargetVector(100.f, &vec_origin, &vec_camera, &vec_target);

	std::vector<CClientPlayer*> players = manager->get_players();
	for (CClientPlayer* target : players)
	{
		if (!target || !target->GetGamePlayer() || target->IsLocalPlayer() || target->IsDeadOnNetwork())
			continue;

		eBone targetBone = (element->aimbot.bone == 1) ? BONE_PELVIS : BONE_HEAD;

		CVector vec_bone, vec_bone_screen;
		if (!target->GetGamePlayer()->GetBonePosition(targetBone, &vec_bone))
			continue;

		if (!global->m_pCore || !global->m_pCore->GetGraphics())
			continue;

		global->m_pCore->GetGraphics()->CalcScreenCoors(&vec_bone, &vec_bone_screen);

		if (vec_bone_screen.fZ < 1.0f)
			continue;

		float fCentreDistance = sqrt(pow(vec_bone_screen.fX - s_aimbot_config.vec_crosshair.fX, 2) + pow(vec_bone_screen.fY - s_aimbot_config.vec_crosshair.fY, 2));
		if (fCentreDistance >= element->aimbot.fov)
			continue;

		if (fCentreDistance < fNearestDistance)
		{
			fNearestDistance = fCentreDistance;
			s_aimbot_config.i_target_player = target;
			s_aimbot_config.i_target_bone = targetBone;
			s_aimbot_config.vec_target_bone = vec_bone_screen;
		}
	}
}

void c_aimbot::draw_aimbot()
{
	if (!element->aimbot.state)
		return;

	if (!s_aimbot_config.is_player_aiming)
		return;

	if (!s_aimbot_config.i_target_player)
		return;

	if (!s_aimbot_config.i_target_player->GetGamePlayer())
		return;

	if (s_aimbot_config.i_target_player->IsBeingDeleted())
		return;

	CClientPlayer* local = manager->get_local_player();
	if (!local || !local->GetGamePlayer())
		return;

	CCamera* m_pCamera = global->m_pGame->GetCamera();
	if (!m_pCamera)
		return;

	CCam* m_pCam = m_pCamera->GetCam(m_pCamera->GetActiveCam());
	if (!m_pCam)
		return;

	CVector* source = m_pCam->GetSource();
	if (!source)
		return;

	CVector vec_origin = *source;

	CVector vec_target;
	if (!s_aimbot_config.i_target_player->GetGamePlayer()->GetBonePosition((eBone)s_aimbot_config.i_target_bone, &vec_target))
		return;

	CVector vec_vector = vec_origin - vec_target;

	float fVecX = 0.f;
	float fFix = 0.f;
	float fZ = 0.f;

	eCamMode cmera_viewmode = static_cast<eCamMode>(m_pCamera->GetCam(m_pCamera->GetActiveCam())->GetMode());
	if (cmera_viewmode == 53 || cmera_viewmode == 55)
	{
		float fMult = tan(m_pCam->GetFOV() / 2.0f * 0.017453292f);
		float& fAspectRatio = *(float*)0xC3EFA4;
		float& f3rdPersonCHairMultY = *reinterpret_cast<float*>(0xB6EC10);

		fZ = M_PI - atan2(1.0f, fMult * ((1.0f - f3rdPersonCHairMultY * 2.0f) * (1.0f / fAspectRatio)));
	}
	else
	{
		fZ = M_PI / 2;
	}

	eWeaponType byteWeapon = manager->get_weapon_type(local);
	if (byteWeapon >= 22 && byteWeapon <= 29 || byteWeapon == 32)
	{
		fFix = 0.04253f;
	}
	else if (byteWeapon == 30 || byteWeapon == 31)
	{
		fFix = 0.028f;
	}
	else if (byteWeapon == 33)
	{
		fFix = 0.01897f;
	}

	float fDistX = sqrt(vec_vector.fX * vec_vector.fX + vec_vector.fY * vec_vector.fY);
	if (fDistX < 0.0001f)
		return;

	float fDistZ = sqrt(vec_vector.fX * vec_vector.fX + vec_vector.fY * vec_vector.fY);
	if (fDistX < 0.0001f)
		return;

	float horizontal, vertical;
	m_pCam->GetDirection(horizontal, vertical);

	if (fDistX > 0.0f)
	{
		if (vec_vector.fX <= 0.0f && vec_vector.fY >= 0.0f || vec_vector.fX >= 0.0f && vec_vector.fY >= 0.0f)
		{
			fVecX = (acosf(vec_vector.fX / fDistX) + fFix) - horizontal;
		}
		else if (vec_vector.fX >= 0.0f && vec_vector.fY <= 0.0f || vec_vector.fX <= 0.0f && vec_vector.fY <= 0.0f)
		{
			fVecX = (-acosf(vec_vector.fX / fDistX) + fFix) - horizontal;
		}

		if (element->aimbot.smooth > 0.0f)
		{
			float fSmoothX = fVecX / element->aimbot.smooth;
			float fSmoothZ = (atan2f(fDistZ, vec_vector.fZ) - fZ - vertical) / element->aimbot.smooth;

			if (fSmoothX > -1.0f && fSmoothX < 0.5f && fVecX > -2.0f && fVecX < 2.0f)
			{
				m_pCam->SetDirection(horizontal + fSmoothX, vertical + fSmoothZ);
			}
		}
	}
}

void c_aimbot::draw_player_aimbot()
{
	if (!manager->is_loaded())
		return;

	draw_aimbot_fov();
	draw_aimbot();
	draw_target_player();
}