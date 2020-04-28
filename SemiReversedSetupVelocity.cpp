

void CCSGOPlayerAnimState::SetupVelocity(CCSGOPlayerAnimState* state, C_BasePlayer* player)
{

	auto entindex = player->entindex();

	if (!(player->m_iEFlags() & 0x1000))
		CBaseEntity::CalcAbsoluteVelocity((float *)player);

	auto v7 = state->anim_update_delta * 2000.0;
	auto velocity = state->velocity;
	state->speed_2d = velocity.z;
	velocity.z = 0.0f;

	float leanspd = m_vecLastSetupLeanVelocity.LengthSqr();

	m_bIsAccelerating = velocity.Length2D() > leanspd;

	auto weapon = player->get_weapon();

	float flMaxMovementSpeed = 260.0f;
	if (weapon)
		flMaxMovementSpeed = std::fmax(weapon->GetMaxWeaponSpeed(), 0.001f);

	state->speed_2d = std::fmin(state->velocity.Length(), 260.0f);
	state->speed_norm = Math::clamp(state->speed_2d / flMaxMovementSpeed, 0.0f, 1.0f);
	state->RandomFeetStuff = state->speed_2d / (flMaxMovementSpeed * 0.520f);
	state->feet_speed = state->speed_2d / (flMaxMovementSpeed * 0.340f);

	auto m_flVelocityUnknown = *(float*)(uintptr_t(state) + 0x2A4);

	if (state->RandomFeetStuff < 1.0f)
	{
		if (state->RandomFeetStuff < 0.5f)
		{
			float vel = m_flVelocityUnknown;
			float delta = state->anim_update_delta * 60.0f;
			float newvel;
			if ((80.0f - vel) <= delta)
			{
				if (-delta <= (80.0f - vel))
					newvel = 80.0f;
				else
					newvel = vel - delta;
			}
			else
			{
				newvel = vel + delta;
			}
			m_flVelocityUnknown = newvel;
		}
	}
	else
		m_flVelocityUnknown = state->speed_2d;

	bool bWasMovingLastUpdate = false;
	bool bJustStartedMovingLastUpdate = false;
	if (state->speed_2d <= 0.0f)
	{
		state->t_since_started_moving = 0.0f;
		bWasMovingLastUpdate = state->t_since_stopped_moving <= 0.0f;
		state->t_since_stopped_moving += state->anim_update_delta;
	}
	else
	{
		state->t_since_stopped_moving = 0.0f;
		bJustStartedMovingLastUpdate = state->t_since_started_moving <= 0.0f;
		state->t_since_started_moving = state->anim_update_delta + state->t_since_started_moving;
	}

	if (!*(_BYTE *)(state + 0x130)
		&& v27
		&& *(_BYTE *)(state + 0xF8)
		&& !*(_BYTE *)(state + 0x120)
		&& !*(_BYTE *)(state + 0x101)
		&& *(float *)(state + 0x2A8) < 50.0)
	{
		v30 = CCSGOPlayerAnimState::SelectSequenceFromActMods((_DWORD *)state, 980);
		CCSGOPlayerAnimState::SetLayerSequence(state, a3, 3, v30);
		*(_BYTE *)(state + 304) = 1;
	}

	if (CCSGOPlayerAnimState::GetLayerActivity((_DWORD *)state, 3) == 980
		|| CCSGOPlayerAnimState::GetLayerActivity((_DWORD *)state, 3) == 979)
	{
		if (*(_BYTE *)(state + 304) && *(float *)(state + 236) <= 0.25)
		{
			v31 = 3;
			v32 = *(_DWORD *)(*(_DWORD *)(state + 80) + 1248);
			if (v32 - 1 < 3)
				v31 = v32 - 1;
			if (v32 && (v33 = *(_DWORD *)(*(_DWORD *)(v4 + 80) + 1236) + 92 * v31) != 0)
				v34 = *(float *)(v33 + 24);
			else
				v34 = 0.0;
			v35 = v34;
			CCSGOPlayerAnimState::GetLayerWeight(v4, 3, 0);
			v36 = CCSGOPlayerAnimState::IsLayerSequenceCompleted((_DWORD *)v4, 3);
			CCSGOPlayerAnimState::SetLayerWeight(v4, *(float *)&v36, 3);
			CCSGOPlayerAnimState::SetLayerWeightRate(v4, v35, 3);
			v37 = 3;
			v38 = *(_DWORD *)(*(_DWORD *)(v4 + 80) + 1248);
			if (v38 - 1 < 3)
				v37 = v38 - 1;
			if (v38 && (v39 = *(_DWORD *)(*(_DWORD *)(v4 + 80) + 1236) + 92 * v37) != 0)
				*(_BYTE *)(v4 + 304) = (float)((float)(*(float *)(v39 + 16) * *(float *)(v4 + 100)) + *(float *)(v39 + 12)) < 1.0;
			else
				*(_BYTE *)(v4 + 304) = 1;
		}
		else
		{
			v40 = *(_DWORD **)(state + 80);
			v41 = 3;
			*(_BYTE *)(state + 304) = 0;
			v42 = v40[312];
			if (v42 - 1 < 3)
				v41 = v42 - 1;
			if (v42 && (v43 = *(_DWORD *)(*(_DWORD *)(v4 + 80) + 1236) + 92 * v41) != 0)
				v44 = *(float *)(v43 + 24);
			else
				v44 = 0.0;
			v45 = *(float *)(v4 + 100) * 5.0;
			if (COERCE_FLOAT(LODWORD(v44) ^ xmmword_108CF530) <= v45)
			{
				if (COERCE_FLOAT(LODWORD(v45) ^ xmmword_108CF530) <= COERCE_FLOAT(LODWORD(v44) ^ xmmword_108CF530))
					v46 = 0.0;
				else
					v46 = v44 - v45;
			}
			else
			{
				v46 = v45 + v44;
			}
			CCSGOPlayerAnimState::SetLayerWeight(state, v46, 3);
			CCSGOPlayerAnimState::SetLayerWeightRate(state, v44, 3);
		}
	}

	state->old_abs_yaw = state->abs_yaw;

	auto v47 = Math::clamp(state->abs_yaw, -360.0f, 360.0f);
	auto eyeYawDelta = Math::normalize_angle(Math::AngleDiff(state->eye_yaw, v47));

	if (state->feet_speed >= 0.0)
		state->feet_speed = fminf(state->feet_speed, 1.0);
	else
		state->feet_speed = 0.0;

	auto v54 = state->duck_amt;
	auto flYawModifier = ((((*(float*)((uintptr_t)state + 0x11C)) * -0.30000001) - 0.19999999) * state->feet_speed) + 1.0f;
	if (v54 > 0.0)
	{
		if (state->RandomFeetStuff >= 0.0)
			state->RandomFeetStuff = fminf(state->RandomFeetStuff, 1.0);
		else
			state->RandomFeetStuff = 0.0;

		flYawModifier += ((state->RandomFeetStuff * v54) * (0.5f - flYawModifier));
	}

	auto flMaxYawModifier = *(float*)((uintptr_t)state + 0x334) * flYawModifier;
	auto flMinYawModifier = *(float*)((uintptr_t)state + 0x330) * flYawModifier;

	if (eyeYawDelta <= flMaxYawModifier)
	{
		if (flMinYawModifier > eyeYawDelta)
			state->abs_yaw = fabs(flMinYawModifier) + state->eye_yaw;
	}
	else
		state->abs_yaw = state->eye_yaw - fabs(flMaxYawModifier);

	state->abs_yaw = Math::normalize_angle(state->abs_yaw);

	if (state->speed_2d > 0.1 || fabs(state->speed_up) > 100.0)
	{
		state->abs_yaw = ApproachAngle(Eyeyaw,GoalFeetYaw((m_flGroundFraction * 20.0f) + 30.0f)*m_flLastClientSideAnimationUpdateTimeDelta);

		NextLBYUpdateTime = Source::m_pGlobalVars->curtime + 0.22000001;
		if (player->m_flLowerBodyYawTarget() != state->m_flEyeYaw)
		{
			v67 = (int)(v134 - 2535);
			if (v67->idk)
			{
				*(_BYTE *)(v67 + 92) |= 1u;
				*v134 = state->m_flEyeYaw;
			}
			else
			{
				if (v67->send_updates)
				{
					NetworkStateChanged(v67->lby_prop, 10140);
				}
				v66 = state->m_flEyeYaw;
			}
		}
		player->m_flLowerBodyYawTarget() = state->eye_yaw;
	}
	else
	{
		state->abs_yaw = ApproachAngle(player->m_flLowerBodyYawTarget(),state->abs_yaw,state->anim_update_delta * 100.0f);

		if (Source::m_pGlobalVars->curtime > NextLBYUpdateTime)
		{
			auto ABSEyeYawDelta = Math::AngleDiff(player->state->abs_yaw(), state->eye_yaw);

			if (abs(ABSEyeYawDelta) > 35.0)
			{
				NextLBYUpdateTime = Source::m_pGlobalVars->curtime + 1.1;
				if (player->m_flLowerBodyYawTarget() != state->m_flEyeYaw)
				{
					v67 = (int)(v134 - 2535);
					if (v67->something)
					{
						*(_BYTE *)(v67 + 92) |= 1u;
						*v134 = state->m_flEyeYaw;
					}
					else
					{
						if (v67->send_updates)
						{
							NetworkStateChanged(v67->lby_prop, 10140);
						}
						v66 = state->m_flEyeYaw;
					}
				}
				player->m_flLowerBodyYawTarget() = state->eye_yaw;
			}
		}
	}

	if (state->speed_2d <= 1.0
		&& state->on_ground
		&& state->anim_update_delta > 0.0
		&& (Math::AngleDiff(state->old_abs_yaw, state->abs_yaw) / state->anim_update_delta) > 120.0)
	{
		v76 = CCSGOPlayerAnimState::SelectSequenceFromActMods(v4, 979);
		CCSGOPlayerAnimState::SetLayerSequence((int)v4, ABSEyeYawDelta, 3, v76);
	};

	if (state->speed_2d > 0.0)
	{
		float velAngle = (atan2(-state->velocity.y, -state->velocity.x) * 180.0f) * (1.0f / M_PI);

		if (velAngle < 0.0f)
			velAngle += 360.0f;

		state->vel_lean = Math::normalize_angle(Math::AngleDiff(velAngle, state->abs_yaw));
	}

	state->lean = Math::normalize_angle(Math::AngleDiff(state->vel_lean, state->torso_yaw));

	if (bJustStartedMovingLastUpdate && state->feet_rate <= 0.0)
	{
		state->torso_yaw = state->vel_lean;
		// unreversed shit below
		/*
		v79 = *(_DWORD *)(AnimState + 0x50);
		v80 = 6;
		*(_DWORD *)(AnimState + 120) = *(_DWORD *)(AnimState + 124);
		v81 = *(_DWORD *)(v79 + 1248);
		if ( v81 - 1 < 6 )
		  v80 = v81 - 1;
		v82 = v81 == 0;
		v83 = *(_DWORD *)(AnimState + 80);
		if ( v82 || (v84 = *(_DWORD *)(v83 + 1236) + 92 * v80) == 0 )
		{
		  v85 = 0;
		  v136 = 0;
		}
		*/
	}
	else
	{
		if (state->speed_2d > 0.1f)
		{
			state->torso_yaw = state->vel_lean;
		}
		else
		{
			if (state->RandomFeetStuff >= 0.0)
				state->RandomFeetStuff = fminf(state->RandomFeetStuff, 1.0);
			else
				state->RandomFeetStuff = 0.0;

			if (state->feet_speed >= 0.0)
				state->feet_speed = fminf(state->feet_speed, 1.0);
			else
				state->feet_speed = 0.0;

			//dword_10ADA7F8 = 1075729671;

			auto v105 = ((state->RandomFeetStuff - state->feet_speed) * state->duck_amt) + state->feet_speed;
			auto v156 = Math::normalize_angle((((v105 + 0.1f) * state->vel_lean) + state->torso_yaw));

			state->torso_yaw = v156;
		}
	}

	float eye_pitch_normalized = Math::normalize_angle(state->m_flPitch);
	float new_body_pitch_pose;

	if (eye_pitch_normalized <= 0.0f)
		new_body_pitch_pose = (eye_pitch_normalized / m_flMaximumPitch) * -90.0f;
	else
		new_body_pitch_pose = (eye_pitch_normalized / m_flMinimumPitch) * 90.0f;

	m_arrPoseParameters[7].SetValue(pBaseEntity, new_body_pitch_pose);
	/*
	v118 = *(_DWORD **)(AnimState + 80);
	v137 = *(_DWORD **)(AnimState + 80);
	v157 = AnimState + 508;
	if ( !*(_BYTE *)(AnimState + 508) )
	{
	sub_10476FA0((_DWORD *)(AnimState + 508), v118, *(_DWORD *)(AnimState + 516));
	if ( !*(_BYTE *)(AnimState + 508) )
	goto LABEL_221;
	v118 = v137;
	}
	if ( v118 )
	{
	v119 = dword_10B36028;
	(*(void (__thiscall **)(int))(*(_DWORD *)dword_10B36028 + 132))(dword_10B36028);
	sub_10134D70(*(_DWORD *)(v157 + 4));
	(*(void (__thiscall **)(int))(*(_DWORD *)v119 + 136))(v119);
	AnimState = (int)v138;
	}
	LABEL_221:
	v120 = *(_DWORD **)(AnimState + 80);
	v121 = *(_DWORD *)(AnimState + 232);
	v149 = *(_DWORD **)(AnimState + 80);
	v158 = AnimState + 436;
	if ( !*(_BYTE *)(AnimState + 436) )
	{
	sub_10476FA0((_DWORD *)(AnimState + 436), v120, *(_DWORD *)(AnimState + 444));
	if ( !*(_BYTE *)(AnimState + 436) )
	goto LABEL_226;
	v120 = v149;
	}
	if ( v120 )
	{
	v122 = dword_10B36028;
	(*(void (__thiscall **)(int))(*(_DWORD *)dword_10B36028 + 132))(dword_10B36028);
	sub_10134D70(*(_DWORD *)(v158 + 4));
	(*(void (__thiscall **)(int))(*(_DWORD *)v122 + 136))(v122);
	AnimState = (int)v138;
	}
	LABEL_226:
	v123 = *(float *)(AnimState + 284) * *(float *)(AnimState + 148);
	v124 = *(_DWORD **)(AnimState + 80);
	v125 = (_DWORD *)(AnimState + 0x214);
	v150 = v124;
	v159 = v125;
	if ( !*(_BYTE *)v125 )
	{
	sub_10476FA0(v125, v124, v125[2]);
	if ( !*(_BYTE *)v125 )
	return (*(int (__thiscall **)(int))(*(_DWORD *)v2 + 136))(v2);
	v124 = v150;
	}
	if ( v124 )
	{
	v126 = dword_10B36028;
	(*(void (__thiscall **)(int))(*(_DWORD *)dword_10B36028 + 132))(dword_10B36028);
	sub_10134D70(v159[1]);
	(*(void (__thiscall **)(int))(*(_DWORD *)v126 + 136))(v126);
	}
	return (*(int (__thiscall **)(int))(*(_DWORD *)v2 + 136))(v2);
	*/
}