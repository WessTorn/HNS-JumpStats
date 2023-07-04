#include <jumpstats/index>

public plugin_init() {
	register_plugin("HNS JumpStats", "beta 0.1.2", "WessTorn");

	init_cvars();
	init_cmds();

	register_forward(FM_Touch, "fwdTouch", 1);

	RegisterHam(Ham_Touch, "player", "HamTouch");
	
	RegisterHookChain(RG_CBasePlayer_PostThink,	"rgPlayerPostThink", false);
	RegisterHookChain(RG_CBasePlayer_PreThink,	"rgPlayerPreThink", false);

	g_hudStrafe = CreateHudSyncObj();
	g_hudStats = CreateHudSyncObj();
	g_hudPreSpeed = CreateHudSyncObj();
}

public rgPlayerPreThink(id) {
	if (!is_user_connected(id) || is_user_bot(id) || is_user_hltv(id)) {
		return HC_CONTINUE;
	}

	static iFog;

	g_iFlags[id] = get_entvar(id, var_flags);
	g_iButtons[id] = get_entvar(id, var_button);
	g_iMoveType[id] = get_entvar(id, var_movetype);
	g_flMaxSpeed[id] = get_entvar(id, var_maxspeed);

	g_bInDuck[id] = g_iFlags[id] & FL_DUCKING ? true : false;
	g_bLadder[id] = g_iMoveType[id] == MOVETYPE_FLY;

	get_entvar(id, var_origin, g_flOrigin[id]);
	get_entvar(id, var_velocity, g_flVelocity[id]);

	g_flHorSpeed[id] = vector_hor_length(g_flVelocity[id]);
	g_flPrevHorSpeed[id] = vector_hor_length(g_flPrevVelocity[id]);

	new bool:isGound = g_iFlags[id] & FL_ONGROUND ? true : false;
	isGound = isGound || g_bLadder[id];
	new bool:isOldGound = g_iOldFlags[id] & FL_ONGROUND ? true : false;
	isOldGound = isOldGound || g_bPrevLadder[id];

	for (new i = 1; i < MaxClients; i++) {
		g_isUserSpec[i] = is_user_spectating_player(i, id);
	}

	if (isGound) {
		iFog++;

		if (!isOldGound) {
			g_flPreHorSpeed[id] = g_flHorSpeed[id];
			if (g_eWhichJump[id] != jt_Not) {
				ready_jumps(id, g_flPrevOrigin[id]);
			}
		}

		if (iFog == 1) {
			if (g_bInDuck[id]) {
				if (g_eDuckType[id] != IS_DUCK_NOT) {
					g_eDuckType[id] = IS_SGS;
				} else if (g_eJumpType[id] != IS_JUMP_NOT && g_eJumpType[id] != IS_DUCKBHOP) {
					g_eJumpType[id] = IS_SBJ;
				}
			}
		}

		if (!g_eJumpType[id] && g_iButtons[id] & IN_BACK && g_isBackWards[id] == false) {
			g_isBackWards[id] = true;
		} else if (!g_eJumpType[id] && g_iButtons[id] & IN_FORWARD && g_isBackWards[id]) {
			g_isBackWards[id] = false;
		} else if (g_eWhichJump[id] == jt_BhopJump || g_eWhichJump[id] == jt_StandUpBhopJump) {
			g_isBackWards[id] = false;
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

			g_isBackWards[id] = false;
			g_eFailJump[id] = fj_good;
			g_iDetectHJ[id] = 0;
			g_eJumpstats[id][js_iJumpBlock] = 1000;
			g_eJumpstats[id][js_iFrames] = 0;
			g_iStrafes[id] = 0;
			g_eJumpstats[id][js_flJof] = 0.0;

			g_bCheckFrames[id] = false;
			for (new i = 0; i < NSTRAFES; i++) {
				g_iStrButtonsInfo[id][i] = bi_not;
			}
		}

		if (!get_entvar(id, var_solid)) {
			static ClassName[32];
			get_entvar(get_entvar(id, var_groundentity), var_classname, ClassName, 32);

			if (equali(ClassName, "func_train") ||
				equali(ClassName, "func_conveyor") ||
				equali(ClassName, "trigger_push") || equali(ClassName, "trigger_gravity")) {
				g_eFailJump[id] = fj_notshow;
			} else if (equali(ClassName, "func_door") || equali(ClassName, "func_door_rotating")) {
				g_eFailJump[id] = fj_notshow;
			}
		}
	} else {
		if (g_eJumpType[id]) {
			g_eJumpstats[id][js_iFrames]++;

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

			detect_hj(id, g_flOrigin[id], g_flJumpOffOrigin[id][2]);
		}

		if (g_eWhichJump[id] != jt_Not && !g_eFailJump[id]) {
			if ((g_flOrigin[id][2] - g_flJumpOffOrigin[id][2] < 0)) {
				g_eFailJump[id] = fl_fail;
				ready_jumps(id, g_flOrigin[3]);
			} else if ((g_bInDuck[id] ? (g_flPrevOrigin[id][2] + 18) : g_flPrevOrigin[id][2]) >= g_flJumpOffOrigin[id][2]) {
				g_flFailedOrigin[id] = g_flPrevOrigin[id];
				g_bFailedInDuck[id] = g_bInDuck[id];
				g_flFailedVelocity[id] = g_flVelocity[id];
			}
		}

		if (isOldGound) {
			new bool:isDuck = !g_bPrevInDuck[id] && !(g_iPrevButtons[id] & IN_DUCK) && g_iOldButtons[id] & IN_DUCK;
			new bool:isJump = !isDuck && g_iPrevButtons[id] & IN_JUMP && !(g_iOldButtons[id] & IN_JUMP);

			if (g_bPrevLadder[id]) {
				in_ladder(id, isJump);
			} else {
				if (isDuck) {
					in_ducks(id, iFog);
				}
				if (isJump) {
					in_bhop_js(id, iFog);

					calc_jof_block(id, g_flJumpOffOrigin[id]);
				}
			}
			if (!isDuck && !isJump && g_flVelocity[id][2] <= -4.0) {
				if (iFog > 10) {
					g_isFalling[id] = true;
					// ФАЛЛ
				}
			}
		}

		g_flLandTime[id] = get_gametime();
		iFog = 0;
	}
	
	g_iOldFlags[id] = g_iFlags[id];
	g_iOldButtons[id] = g_iPrevButtons[id];
	g_iPrevButtons[id] = g_iButtons[id];

	vector_copy(g_flVelocity[id], g_flPrevVelocity[id]);
	vector_copy(g_flOrigin[id], g_flPrevOrigin[id]);

	g_bPrevLadder[id] = g_bLadder[id];
	g_bPrevInDuck[id] = g_bInDuck[id];

	return HC_CONTINUE;
}

public rgPlayerPostThink(id) {
	if (!is_user_alive(id) || is_user_bot(id)) {
		return HC_CONTINUE;
	}

	if (g_eOnOff[id][of_bSpeed])
		show_prespeed(id);
	
	if (!g_eJumpType[id]) {
		return HC_CONTINUE;
	}

	new iButtons = get_entvar(id, var_button);
	new Float:flVelocity[3]; get_entvar(id, var_velocity, flVelocity);

	new Float:flStrSpeed = vector_hor_length(flVelocity);

	new Float:flAngles[3]; get_entvar(id, var_angles, flAngles);
	static Float:flOldAngle;

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

	flOldAngle = flAngles[1];

	return HC_CONTINUE;
}

public HamTouch(id, entity) {
	if (is_user_alive(id)) {
		static Float:flVelocity[3];
		get_entvar(id, var_velocity, flVelocity);
		if (g_eJumpType[id] && !(get_entvar(id, var_flags) & FL_ONGROUND) && floatround(flVelocity[2], floatround_floor) < 0) {
			g_isTouched[id] = true;
		}
	}
}

public fwdTouch(ent, id) {
	static ClassName[32];
	if (is_entity(ent)) {
		get_entvar(ent, var_classname, ClassName, 31);
	}

	static ClassName2[32];
	get_entvar(id, var_classname, ClassName2, 31);

	if (equali(ClassName2, "player")) {
		if (equali(ClassName, "func_train") ||
			equali(ClassName, "func_conveyor") ||
			equali(ClassName, "trigger_push") || equali(ClassName, "trigger_gravity")) {
			g_isTrigger[id] = true;
		}
	}
}

public client_connect(id) {
	client_cmd(id, "gl_vsync 0");

	g_eOnOff[id][of_bChatInfo] = true;
	g_eOnOff[id][of_bStrafe] = true;
	g_eOnOff[id][of_bJof] = true;
	g_eOnOff[id][of_bSpeed] = true;
	g_eOnOff[id][of_bPre] = true;

	g_bCheckFrames[id] = false;
	g_isBackWards[id] = false;
	g_isTouched[id] = false;
	g_isTrigger[id] = false;
	g_eOnOff[id][of_bStats] = true;
}