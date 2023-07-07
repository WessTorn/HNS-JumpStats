#include <jumpstats/index>

// Фиксить
// Ладдер (срейфы)

public plugin_init() {
	register_plugin("HNS JumpStats", "beta 0.2.5", "WessTorn");

	init_cvars();
	init_cmds();

	register_forward(FM_Touch, "fwdTouch", 1);

	RegisterHookChain(RG_PM_Move, "rgPM_Move");
	RegisterHookChain(RG_PM_AirMove, "rgPM_AirMove", true);

	g_hudStrafe = CreateHudSyncObj();
	g_hudStats = CreateHudSyncObj();
	g_hudPreSpeed = CreateHudSyncObj();
}

public rgPM_Move(id) {
	if (!is_user_connected(id) || is_user_bot(id) || is_user_hltv(id)) {
		return HC_CONTINUE;
	}

	static iFog;

	g_iButtons[id] = get_pmove(pm_oldbuttons);
	g_flMaxSpeed[id] = get_pmove(pm_maxspeed);

	get_pmove(pm_origin, g_flOrigin[id]);
	get_pmove(pm_velocity, g_flVelocity[id]);

	g_flHorSpeed[id] = vector_hor_length(g_flVelocity[id]);
	g_flPrevHorSpeed[id] = vector_hor_length(g_flPrevVelocity[id]);

	g_bInDuck[id] = get_pmove(pm_flags) & FL_DUCKING ? true : false;

	new bool:isLadder = get_pmove(pm_movetype) == MOVETYPE_FLY ? true : false;

	new bool:isGound = get_pmove(pm_onground) == -1 ? false : true;
	isGound = isGound || isLadder;

	g_isOldGround[id] = g_isOldGround[id] || g_bPrevLadder[id];

	for (new i = 1; i < MaxClients; i++) {
		g_isUserSpec[i] = is_user_spectating_player(i, id);
	}

	if (g_eOnOff[id][of_bSpeed])
		show_prespeed(id);

	if (isGound) {
		iFog++;

		if (!g_isOldGround[id]) {
			g_flPreHorSpeed[id] = g_flHorSpeed[id];
			if (g_eWhichJump[id] != jt_Not) {
				ready_jumps(id, g_flOrigin[id]);
			}
		}

		if (isLadder) {
			new iEnt[1];
			find_sphere_class(id, "func_ladder", 18.0, iEnt, 1);

			if (iEnt[0] != 0) {
				get_entvar(iEnt[0], var_maxs, g_flLadderXYZ[id]);
				get_entvar(iEnt[0], var_size, g_flLadderSize[id]);
			}
		}

		if (iFog == 1) {
			if (g_bInDuck[id]) {
				g_eDuckType[id] = g_eDuckType[id] != IS_DUCK_NOT ? IS_SGS : g_eDuckType[id];
				g_eJumpType[id] = g_eJumpType[id] != IS_JUMP_NOT && g_eJumpType[id] != IS_DUCKBHOP ? IS_SBJ : g_eJumpType[id];
			}
		}

		if (!g_eJumpType[id] && g_iButtons[id] & IN_BACK && !g_ePreStats[id][ptBackWards]) {
			g_ePreStats[id][ptBackWards] = true;
		} else if (!g_eJumpType[id] && g_iButtons[id] & IN_FORWARD && g_ePreStats[id][ptBackWards]) {
			g_ePreStats[id][ptBackWards] = false;
		} else if (g_eWhichJump[id] == jt_BhopJump || g_eWhichJump[id] == jt_StandUpBhopJump) {
			g_ePreStats[id][ptBackWards] = false;
		}

		if (iFog > 10) {
			g_isFalling[id] = false;
			g_iVerInfo[id] = IS_MIDDLE;
			g_eJumpType[id] = IS_JUMP_NOT;
			g_eDuckType[id] = IS_DUCK_NOT;
			g_eWhichJump[id] = jt_Not;
			g_iDucks[id] = 0;
			g_iJumps[id] = 0;
			g_flDuckFirstZ[id] = 0.0;
			g_flJumpFirstZ[id] = 0.0;

			g_isTouched[id] = false;

			g_ePreStats[id][ptBackWards] = false;
			g_eFailJump[id] = fj_good;
			g_iDetectHJ[id] = 0;
			g_eJumpstats[id][js_iJumpBlock] = 1000;
			g_eJumpstats[id][js_iFrames] = 0;
			g_iStrafes[id] = 0;
			g_eJumpstats[id][js_flJof] = 0.0;
			g_isLadderBhop[id] = false;

			g_bCheckFrames[id] = false;
			for (new i = 0; i < NSTRAFES; i++) {
				g_iStrButtonsInfo[id][i] = bi_not;
			}
		}
	} else {
		if (g_eWhichJump[id] != jt_Not && !g_eFailJump[id]) {
			if ((g_bInDuck[id] ? (g_flOrigin[id][2] + 18.0) : g_flOrigin[id][2]) - g_flFirstJump[id][2] < 0) {
				g_eFailJump[id] = fl_fail;
				ready_jumps(id, g_flPrevOrigin[id]);
			}
		}

		if (g_isOldGround[id]) {
			new bool:isDuck = !g_bInDuck[id] && !(g_iButtons[id] & IN_JUMP) && g_iPrevButtons[id] & IN_DUCK;
			new bool:isJump = !isDuck && g_iButtons[id] & IN_JUMP && !(g_iPrevButtons[id] & IN_JUMP);

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
		}

		g_flLandTime[id] = get_gametime();
		iFog = 0;
	}
	
	g_iPrevButtons[id] = g_iButtons[id];

	vector_copy(g_flVelocity[id], g_flPrevVelocity[id]);
	vector_copy(g_flOrigin[id], g_flPrevOrigin[id]);

	g_isOldGround[id] = isGound || isLadder;
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

	g_eJumpstats[id][js_iFrames]++;

	new iButtons = get_entvar(id, var_button);
	new Float:flVelocity[3]; get_pmove(pm_velocity, flVelocity);

	new Float:flStrSpeed = vector_hor_length(flVelocity);

	new Float:flAngles[3]; get_entvar(id, var_angles, flAngles);
	static Float:flOldAngle;


	g_isTouched[id] = get_pmove(pm_numtouch) ? true : g_isTouched[id];

	if (g_eWhichJump[id] == jt_LongJump) {
		detect_hj(id, g_flOrigin[id], g_flFirstJump[id][2]);
	}

	new bool:isTurningLeft;
	new bool:isTurningRight;

	if (flOldAngle > flAngles[1]) {
		isTurningLeft = false;
		isTurningRight = true;
	} else if (flOldAngle < flAngles[1]) {
		isTurningLeft = true;
		isTurningRight = false;
	} else {
		isTurningLeft = false;
		isTurningRight = false;
	}

	if (iButtons & IN_MOVELEFT && !(g_iStrOldButtons[id] & IN_MOVELEFT) && !(iButtons & IN_MOVERIGHT) && !(iButtons & IN_BACK) && !(iButtons & IN_FORWARD) && (isTurningLeft || isTurningRight)) {
		if (g_iStrafes[id] < NSTRAFES)
			g_eStrafeStats[id][g_iStrafes[id]][st_flTime] = get_gametime();
		g_iStrafes[id] += 1;

		if (g_iStrafes[id] > 0 && g_iStrafes[id] < 100) {
			g_iStrButtonsInfo[id][g_iStrafes[id]] = bi_A;
		}
	} else if (iButtons & IN_MOVERIGHT && !(g_iStrOldButtons[id] & IN_MOVERIGHT) && !(iButtons & IN_MOVELEFT) && !(iButtons & IN_BACK) && !(iButtons & IN_FORWARD) && (isTurningLeft || isTurningRight)) {
		if (g_iStrafes[id] < NSTRAFES)
			g_eStrafeStats[id][g_iStrafes[id]][st_flTime] = get_gametime();
		g_iStrafes[id] += 1;

		if (g_iStrafes[id] > 0 && g_iStrafes[id] < 100) {
			g_iStrButtonsInfo[id][g_iStrafes[id]] = bi_D;
		}
	} else if (iButtons & IN_BACK && !(g_iStrOldButtons[id] & IN_BACK) && !(iButtons & IN_MOVELEFT) && !(iButtons & IN_MOVERIGHT) && !(iButtons & IN_FORWARD) && (isTurningLeft || isTurningRight)) {
		if (g_iStrafes[id] < NSTRAFES)
			g_eStrafeStats[id][g_iStrafes[id]][st_flTime] = get_gametime();
		g_iStrafes[id] += 1;

		if (g_iStrafes[id] > 0 && g_iStrafes[id] < 100) {
			g_iStrButtonsInfo[id][g_iStrafes[id]] = bi_S;
		}
	} else if (iButtons & IN_FORWARD && !(g_iStrOldButtons[id] & IN_FORWARD) && !(iButtons & IN_MOVELEFT) && !(iButtons & IN_MOVERIGHT) && !(iButtons & IN_BACK) && (isTurningLeft || isTurningRight)) {
		if (g_iStrafes[id] < NSTRAFES)
			g_eStrafeStats[id][g_iStrafes[id]][st_flTime] = get_gametime();
		g_iStrafes[id] += 1;

		if (g_iStrafes[id] > 0 && g_iStrafes[id] < 100) {
			g_iStrButtonsInfo[id][g_iStrafes[id]] = bi_W;
		}
	}

	if (iButtons & IN_MOVERIGHT || iButtons & IN_MOVELEFT || iButtons & IN_FORWARD || iButtons & IN_BACK) {
		if (g_iStrafes[id] < NSTRAFES) {
			if (flStrSpeed > g_flHorSpeed[id]) {
				g_eStrafeStats[id][g_iStrafes[id]][st_flSyncGood] += 1;
			} else {
				g_eStrafeStats[id][g_iStrafes[id]][st_flSyncBad] += 1;
			}
		}

	}
	if ((iButtons & IN_MOVERIGHT && (iButtons & IN_MOVELEFT || iButtons & IN_FORWARD || iButtons & IN_BACK))
	|| (iButtons & IN_MOVELEFT && (iButtons & IN_FORWARD || iButtons & IN_BACK || iButtons & IN_MOVERIGHT))
	|| (iButtons & IN_FORWARD && (iButtons & IN_BACK || iButtons & IN_MOVERIGHT || iButtons & IN_MOVELEFT))
	|| (iButtons & IN_BACK && (iButtons & IN_MOVERIGHT || iButtons & IN_MOVELEFT || iButtons & IN_FORWARD)))
		g_iStrOldButtons[id] = 0;
	else if (isTurningLeft || isTurningRight)
		g_iStrOldButtons[id] = iButtons;

	if (g_flHorSpeed[id] > g_eJumpstats[id][js_flEndSpeed]) {
		if (g_iStrafes[id] < NSTRAFES) {
			g_eStrafeStats[id][g_iStrafes[id]][st_flSpeed] += g_flHorSpeed[id] - g_eJumpstats[id][js_flEndSpeed];
		}
		g_eJumpstats[id][js_flEndSpeed] = g_flHorSpeed[id];
	}

	if ((g_flHorSpeed[id] < g_flTempSpeed[id]) && (g_iStrafes[id] < NSTRAFES)) {
		g_eStrafeStats[id][g_iStrafes[id]][st_flSpeedFail] += g_flTempSpeed[id] - g_flHorSpeed[id];
		if (g_eStrafeStats[id][g_iStrafes[id]][st_flSpeedFail] > 5) {
			g_bCheckFrames[id] = true;
		}
	}
	g_flTempSpeed[id] = g_flHorSpeed[id];

	flOldAngle = flAngles[1];

	return HC_CONTINUE;
}

public client_connect(id) {
	client_cmd(id, "gl_vsync 0");

	g_eOnOff[id][of_bChatInfo] = true;
	g_eOnOff[id][of_bStrafe] = true;
	g_eOnOff[id][of_bJof] = true;
	g_eOnOff[id][of_bSpeed] = true;
	g_eOnOff[id][of_bPre] = true;

	g_bCheckFrames[id] = false;
	g_ePreStats[id][ptBackWards] = false;
	g_isTouched[id] = false;
	g_eOnOff[id][of_bStats] = true;
}