#include <jumpstats/index>

public plugin_init() {
	register_plugin("HNS JumpStats", "beta 0.3.9", "WessTorn");

	init_cvars();
	init_cmds();

	RegisterHookChain(RG_CBasePlayer_Spawn, "rgPlayerSpawn", true);
	RegisterHookChain(RG_PM_Move, "rgPM_Move");
	RegisterHookChain(RG_PM_AirMove, "rgPM_AirMove", true);

	g_hudStrafe = CreateHudSyncObj();
	g_hudStats = CreateHudSyncObj();
	g_hudPreSpeed = CreateHudSyncObj();
}

public rgPlayerSpawn(id) {
	reset_stats(id);
	g_eFailJump[id] = fj_notshow;
}

public rgPM_Move(id) {
	if (!is_user_connected(id) || is_user_bot(id) || is_user_hltv(id)) {
		return HC_CONTINUE;
	}

	static iFog;

	g_iPrevButtons[id] = get_pmove(pm_oldbuttons);
	g_flMaxSpeed[id] = get_pmove(pm_maxspeed);

	get_pmove(pm_origin, g_flOrigin[id]);
	get_pmove(pm_velocity, g_flVelocity[id]);

	g_flHorSpeed[id] = vector_hor_length(g_flVelocity[id]);
	g_flPrevHorSpeed[id] = vector_hor_length(g_flPrevVelocity[id]);

	g_bInDuck[id] = bool:(get_pmove(pm_flags) & FL_DUCKING);

	new bool:isLadder = bool:(get_pmove(pm_movetype) == MOVETYPE_FLY);

	new bool:isGound = !bool:(get_pmove(pm_onground) == -1);

	isGound = isGound || isLadder;

	g_isOldGround[id] = g_isOldGround[id] || g_bPrevLadder[id];

	for (new i = 1; i < MaxClients; i++) {
		g_isUserSpec[i] = is_user_spectating_player(i, id);
	}

	if (g_eOnOff[id][of_bSpeed] || g_eOnOff[id][of_bJof] || g_eOnOff[id][of_bPre])
		show_prespeed(id);

	if (isGound) {
		iFog++;

		if (isLadder) {
			new iEnt[1];
			find_sphere_class(id, "func_ladder", 18.0, iEnt, 1);

			if (iEnt[0] != 0) {
				get_entvar(iEnt[0], var_maxs, g_flLadderXYZ[id]);
				get_entvar(iEnt[0], var_size, g_flLadderSize[id]);
			}

		}

		if (!g_isOldGround[id]) {
			g_flPreHorSpeed[id] = g_flHorSpeed[id];
			if (g_eWhichJump[id] != jt_Not) {
				ready_jumps(id, g_eWhichJump[id], g_flOrigin[id]);
			}
		}

		if (iFog == 1) {
			if (g_bInDuck[id]) {
				g_eDuckType[id] = g_eDuckType[id] != IS_DUCK_NOT ? IS_SGS : g_eDuckType[id];
				g_eJumpType[id] = g_eJumpType[id] != IS_DUCKBHOP ? IS_SBJ : g_eJumpType[id];
			}
		}

		if (!g_eJumpType[id]) {
			if (g_ePreStats[id][ptBackWards])
				g_ePreStats[id][ptBackWards] = !bool:(g_iPrevButtons[id] & IN_FORWARD);
			else
				g_ePreStats[id][ptBackWards] = bool:(g_iPrevButtons[id] & IN_BACK);
		}

		static bool:bOneReset;

		if (iFog <= 10) {
			bOneReset = true;
		} else if (bOneReset) {
			reset_stats(id);
			bOneReset = false;
		}
	} else {
		if (g_eWhichJump[id] != jt_Not && !g_eFailJump[id]) {
			if ((g_bInDuck[id] ? (g_flOrigin[id][2] + 18.0) : g_flOrigin[id][2]) - g_flFirstJump[id][2] < 0) {
				g_eFailJump[id] = g_eWhichJump[id] == jt_LadderJump ? fj_notshow : fj_fail;
				ready_jumps(id, g_eWhichJump[id], g_flPrevOrigin[id]);
			}
		}

		if (g_isOldGround[id]) {
			new bool:isDuck = !g_bInDuck[id] && !(g_iPrevButtons[id] & IN_JUMP) && g_iOldButtons[id] & IN_DUCK;
			new bool:isJump = !isDuck && g_iPrevButtons[id] & IN_JUMP && !(g_iOldButtons[id] & IN_JUMP);

			reset_strafes(id);

			if (g_bPrevLadder[id]) {
				in_ladder(id, isJump);
			} else {
				if (isDuck) {
					in_ducks(id, iFog);
				}
				if (isJump) {
					in_bhop_js(id, iFog);

					calc_jof_block(id, g_flFirstJump[id]);
				}
			}
			if (!isDuck && !isJump && g_flVelocity[id][2] <= -4.0) {
				if (iFog > 10) {
					g_isFalling[id] = true;
					g_eJumpType[id] = IS_JUMP;
					g_iJumps[id]++;
					g_eJumpData[id][g_iJumps[id]][JUMP_PRE] = g_flHorSpeed[id];
					show_pre(id, pre_fall, g_flHorSpeed[id]);
					// ФАЛЛ
				}
			}
		} else {
			if (g_eWhichJump[id] != jt_Not)
				g_isTouched[id] = get_pmove(pm_numtouch) ? true : g_isTouched[id];
		}

		iFog = 0;
	}
	
	g_iOldButtons[id] = g_iPrevButtons[id];

	g_flPrevVelocity[id] = g_flVelocity[id];
	g_flPrevOrigin[id] = g_flOrigin[id];

	g_isOldGround[id] = isGound;
	g_bPrevLadder[id] = isLadder;
	g_bPrevInDuck[id] = g_bInDuck[id];

	return HC_CONTINUE;
}

public rgPM_AirMove(id) {
	if (!is_user_alive(id) || is_user_bot(id)) {
		return HC_CONTINUE;
	}
	
	if (g_eWhichJump[id] == jt_Not) {
		return HC_CONTINUE;
	}

	if (isUserSurfing(id)) {
		reset_stats(id);
		return HC_CONTINUE;
	}

	if (g_eWhichJump[id] == jt_LongJump) {
		detect_hj(id, g_flOrigin[id], g_flFirstJump[id][2]);
	}

	if (g_iStrafes[id] >= NSTRAFES) {
		return HC_CONTINUE;
	}

	new iButtons = get_ucmd(get_pmove(pm_cmd), ucmd_buttons);
	static iOldButtons;

	new Float:flVelocity[3]; get_pmove(pm_velocity, flVelocity);

	new Float:flStrSpeed = vector_hor_length(flVelocity);

	new Float:flAngles[3]; get_entvar(id, var_angles, flAngles);
	static Float:flOldAngle;

	g_eJumpstats[id][js_iFrames]++;

	new bool:isTiring = bool:(flOldAngle != flAngles[1]);

	if (iButtons & IN_MOVELEFT && !(iOldButtons & IN_MOVELEFT) && !(iButtons & (IN_MOVERIGHT|IN_BACK|IN_FORWARD)) && (isTiring)) {
		g_iStrafes[id]++;
		g_iStrButtonsInfo[id][g_iStrafes[id]] = bi_A;
	} else if (iButtons & IN_MOVERIGHT && !(iOldButtons & IN_MOVERIGHT) && !(iButtons & (IN_MOVELEFT|IN_BACK|IN_FORWARD)) && (isTiring)) {
		g_iStrafes[id]++;
		g_iStrButtonsInfo[id][g_iStrafes[id]] = bi_D;
	} else if (iButtons & IN_BACK && !(iOldButtons & IN_BACK) && !(iButtons & (IN_MOVELEFT|IN_MOVERIGHT|IN_FORWARD)) && (isTiring)) {
		g_iStrafes[id]++;
		g_iStrButtonsInfo[id][g_iStrafes[id]] = bi_S;
	} else if (iButtons & IN_FORWARD && !(iOldButtons & IN_FORWARD) && !(iButtons & (IN_MOVELEFT|IN_MOVERIGHT|IN_BACK)) && (isTiring)) {
		g_iStrafes[id]++;
		g_iStrButtonsInfo[id][g_iStrafes[id]] = bi_W;
	}

	if (iButtons & IN_MOVERIGHT || iButtons & IN_MOVELEFT || iButtons & IN_FORWARD || iButtons & IN_BACK) {
		if (flStrSpeed > g_flHorSpeed[id]) {
			g_eStrafeStats[id][g_iStrafes[id]][st_iFrameGood] += 1;
		} else {
			g_eStrafeStats[id][g_iStrafes[id]][st_iFrameBad] += 1;
		}
		g_eStrafeStats[id][g_iStrafes[id]][st_iFrame] += 1;

	}

	if (flStrSpeed > g_flStartSpeed[id]) {
		g_eStrafeStats[id][g_iStrafes[id]][st_flSpeed] += flStrSpeed - g_flStartSpeed[id];
		g_flStartSpeed[id] = flStrSpeed;
		g_eJumpstats[id][js_flEndSpeed] = flStrSpeed;
	}

	if (flStrSpeed < g_flTempSpeed[id]) {
		g_eStrafeStats[id][g_iStrafes[id]][st_flSpeedFail] += g_flTempSpeed[id] - flStrSpeed;
	}

	g_flTempSpeed[id] = flStrSpeed;

	flOldAngle = flAngles[1];

	if ((iButtons & IN_MOVERIGHT && iButtons & (IN_MOVELEFT|IN_FORWARD|IN_BACK))
	|| (iButtons & IN_MOVELEFT && iButtons & (IN_FORWARD|IN_BACK|IN_MOVERIGHT))
	|| (iButtons & IN_FORWARD && iButtons & (IN_BACK|IN_MOVERIGHT|IN_MOVELEFT))
	|| (iButtons & IN_BACK && iButtons & (IN_MOVERIGHT|IN_MOVELEFT|IN_FORWARD)))
		iOldButtons = 0;
	else if (isTiring)
		iOldButtons = iButtons;

	return HC_CONTINUE;
}

public client_connect(id) {
	arrayset(g_eOnOff[id], true, JS_ONOFF); // Ну потом

	reset_stats(id);
}