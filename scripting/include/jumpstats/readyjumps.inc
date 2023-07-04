enum TypePre {
	TP_NOT,
	TP_SW,
	TP_BW
}

enum _:InfoNames {
	info_hud,
	info_chat,
	info_console
};

new const g_eNameJumps[JumpType][InfoNames][] =  {
	{ "lj",		"LJ",	"Long jump" },
	{ "hj",		"HJ",	"High jump" },
	{ "cj",		"CJ", 	"Count jump" },
	{ "mcj",	"MCJ",	"Multi count jump" },
	{ "scj",	"SCJ",	"stand-up count jump" },
	{ "mscj",	"MSCJ",	"Multi stand-up count jump" },
	{ "bj",		"BJ",	"Bhop jump" },
	{ "sbj",	"SBJ",	"Stand up bhop jump" },
	{ "duckbhop", "Duck BJ", "Duck Bhop jump" },
	{ "wj",		"WJ",	"Weird jump" },
	{ "ldj",	"LDJ",	"Ladder jump" },
	{ "ldbj",	"LDBJ",	"Ladder bhop jump" },
	{ "rldbj",	"RLDBJ", "Real ladder bhop" },
	{ "bid",	"BID",	"Bhop in duck" },
	{ "bhop",	"Bhop",	"Bhop" },
	{ 0, 0, 0 },
};

new const g_TypePre[TypePre][InfoNames][] =  {
	{ "", "", ""},
	{ "[sw] ", " (^3Sideways^1)", "Sideways"},
	{ "[bw] ", " (^3Backwards^1)", "Backwards"},
};

new TypePre:g_eTypePre[MAX_PLAYERS + 1];


new const g_VerAir[PLAYER_VER][InfoNames][] =  {
	{ "", "", ""},
	{ "up ", "Up ", "Up "},
	{ "drop ", "Drop ", "Drop "}
};

new g_iStrAllEff[MAX_PLAYERS + 1];
new Float:g_flStrFullTime[MAX_PLAYERS + 1];
new Float:g_flStrGain[MAX_PLAYERS + 1];

public ready_jumps(id, Float:flOrigin[3]) {
	if (g_eWhichJump[id] == jt_LongJump && g_iDetectHJ[id] == 1) {
		g_eWhichJump[id] = jt_HighJump;

	}

	new bool:bFail;
	if (g_eFailJump[id] == fl_fail) {
		bFail = true;
		flOrigin = g_flFailedOrigin[id];
	} else {
		bFail = false;
	}

	flOrigin[2] = g_bInDuck[id] ? (flOrigin[2] += 18.0) : flOrigin[2];

	static Float:flDistance1;
	flDistance1 = get_distance_f(g_flJumpOffOrigin[id], flOrigin) + 32.0;

	flOrigin[2] = g_bInDuck[id] ? (flOrigin[2] -= 18.0) : flOrigin[2];

	if (g_flFrameVelocity[id][1][0] < 0.0) g_flFrameVelocity[id][1][0] *= -1.0;
	if (g_flFrameVelocity[id][1][1] < 0.0) g_flFrameVelocity[id][1][1] *= -1.0;

	static Float:flLandOrigin[3];

	flLandOrigin[2] = g_flFrameVelocity[id][0][2] * g_flFrameVelocity[id][0][2] + (2 * g_pCvar[c_iGravity] * (g_flFrameOrigin[id][0][2] - flOrigin[2]));

	static Float:rDistance[3];

	rDistance[0] = (floatsqroot(flLandOrigin[2]) * -1) - g_flFrameVelocity[id][1][2];
	rDistance[1] = g_pCvar[c_iGravity] * -1;

	static Float:frame2time;

	frame2time = floatdiv(rDistance[0], rDistance[1]);
	if (g_flFrameVelocity[id][1][0] < 0)
		g_flFrameVelocity[id][1][0] = g_flFrameVelocity[id][1][0] * -1;
	rDistance[0] = frame2time * g_flFrameVelocity[id][1][0];

	if (g_flFrameVelocity[id][1][1] < 0)
		g_flFrameVelocity[id][1][1] = g_flFrameVelocity[id][1][1] * -1;
	rDistance[1] = frame2time * g_flFrameVelocity[id][1][1];

	if (g_flFrameVelocity[id][1][2] < 0)
		g_flFrameVelocity[id][1][2] = g_flFrameVelocity[id][1][2] * -1;
	rDistance[2] = frame2time * g_flFrameVelocity[id][1][2];

	if (g_flFrameOrigin[id][1][0] < flOrigin[0])
		flLandOrigin[0] = g_flFrameOrigin[id][1][0] + rDistance[0];
	else
		flLandOrigin[0] = g_flFrameOrigin[id][1][0] - rDistance[0];
	if (g_flFrameOrigin[id][1][1] < flOrigin[1])
		flLandOrigin[1] = g_flFrameOrigin[id][1][1] + rDistance[1];
	else
		flLandOrigin[1] = g_flFrameOrigin[id][1][1] - rDistance[1];

	flOrigin[2] = g_bInDuck[id] ? (flOrigin[2] += 18.0) : flOrigin[2];

	flLandOrigin[2] = flOrigin[2];

	frame2time += (g_flLandTime[id] - g_flJumpOffTime[id]);

	static Float:flDistance2;
	flDistance2 = get_distance_f(g_flJumpOffOrigin[id], flLandOrigin) + 32.0;

	if (bFail == true) {
		g_eJumpstats[id][js_flDistance] = GetFailedDistance(32.0, g_bFailedInDuck[id], g_pCvar[c_iGravity], g_flJumpOffOrigin[id], g_flVelocity[id], g_flFailedOrigin[id], g_flFailedVelocity[id]);
	} else g_eJumpstats[id][js_flDistance] = flDistance1 > flDistance2 ? flDistance2 : flDistance1;

	if (g_eWhichJump[id] != jt_LadderJump && g_eJumpstats[id][js_flDistance] > 64.0) {
		g_eJumpstats[id][js_flLanding] = g_eJumpstats[id][js_flDistance] - (g_eJumpstats[id][js_iJumpBlock] + g_eJumpstats[id][js_flJof]);
	}

	if (!g_pCvar[c_iNoslow] && get_entvar(id, var_fuser2) == 0.0 && g_eWhichJump[id] != jt_LadderJump) {
		g_eFailJump[id] = fj_notshow;
	}

	new iDistance;
	if (g_flMaxSpeed[id] != 250.0 && g_eWhichJump[id] != jt_LadderJump) {
		iDistance = floatround((250.0 - g_flMaxSpeed[id]) * 0.73, floatround_floor);
	}

	switch(g_eWhichJump[id]) {
		case jt_LongJump, jt_HighJump, jt_CountJump, jt_Multi_CountJump, jt_StandUPCountJump, jt_Multi_StandUPCountJump, jt_WeirdJump: {
			if (g_eJumpstats[id][js_flDistance] < 200.0)
				g_eFailJump[id] = fj_notshow;
		}
		case jt_BhopJump, jt_StandUpBhopJump, jt_DuckBhopJump, jt_LadderJump, jt_BhopInDuck: {
			if (g_eJumpstats[id][js_flDistance] < 120.0)
				g_eFailJump[id] = fj_notshow;
		}
	}

	if (g_eJumpstats[id][js_flDistance] > 290.0)
		g_eFailJump[id] = fj_notshow;

	if (g_isTouched[id]) {
		g_eFailJump[id] = fl_fail;
	}
	if (g_isTrigger[id]) {
		g_eFailJump[id] = fl_fail;
	}

	calc_strafe(id);
	
	if (!g_eTypePre[id] && g_isBackWards[id]) {
		g_eTypePre[id] = TP_BW;
	}

	if (g_eFailJump[id] != fj_notshow) {
		show_strafe_hud(id);

		show_stats_hud(id);

		show_console(id);

		if (g_eFailJump[id] == fj_good)
			show_chat(id, iDistance);
	}

	g_isTrigger[id] = false;
	g_eTypePre[id] = TP_NOT;
	g_isFalling[id] = false;
	g_iVerInfo[id] = IS_MIDDLE;
	g_eJumpType[id] = IS_JUMP;
	g_eDuckType[id] = IS_DUCK_NOT;
	g_eWhichJump[id] = jt_Not;
	g_eFailJump[id] = fj_good;
	g_flDuckFirstZ[id] = 0.0;

	g_isTouched[id] = false;

	g_isBackWards[id] = false;
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

public calc_strafe(id) {
	new summad, summws;

	new Float:flStrFailSpeed[NSTRAFES];
	new iStrSyncBad;
	new iStrSyncGood;

	g_flStrFullTime[id] = g_flLandTime[id] - g_flJumpOffTime[id];
	g_flStrGain[id] = floatsub(g_eJumpstats[id][js_flEndSpeed], g_eJumpData[id][g_iJumps[id]][JUMP_PRE]);

	if (g_iStrafes[id] < NSTRAFES) {
		g_eStrafeStats[id][0][st_flTime] = g_flJumpOffTime[id];
		g_eStrafeStats[id][g_iStrafes[id]][st_flTime] = g_flLandTime[id];
		for (new str = 1; str <= g_iStrafes[id]; str++) {
			iStrSyncGood += g_eStrafeStats[id][str][st_flSyncGood];
			iStrSyncBad += g_eStrafeStats[id][str][st_flSyncBad];
			flStrFailSpeed[str] = g_eStrafeStats[id][str][st_flSpeedFail];

			if (flStrFailSpeed[str] > 200 && g_bCheckFrames[id]) {
				g_bCheckFrames[id] = false;
				g_eFailJump[id] = fl_fail;
			}

			if (g_iStrButtonsInfo[id][str] == 2 || g_iStrButtonsInfo[id][str] == 4)
				summad++;
			if (g_iStrButtonsInfo[id][str] == 1 || g_iStrButtonsInfo[id][str] == 3)
				summws++;
			}

		if (iStrSyncGood)
			g_iStrAllEff[id] = (iStrSyncGood * 100 / (iStrSyncGood + iStrSyncBad));
	}

	if (summws > summad) {
		g_eTypePre[id] = TP_SW;
	}
}

public show_strafe_hud(id) {
	new szStrMess[512];
	new iLen;

	if (g_iStrafes[id] < NSTRAFES) {
		g_eStrafeStats[id][0][st_flTime] = g_flJumpOffTime[id];
		g_eStrafeStats[id][g_iStrafes[id]][st_flTime] = g_flLandTime[id];
		for (new str = 1; str <= g_iStrafes[id]; str++) {
			new Float:flStrTime = ((g_eStrafeStats[id][str][st_flTime] - g_eStrafeStats[id][str - 1][st_flTime]) * 100) / g_flStrFullTime[id];
			
			new iStrEff;
			if ((g_eStrafeStats[id][str][st_flSyncGood] + g_eStrafeStats[id][str][st_flSyncBad]) > 0) {
				iStrEff = (g_eStrafeStats[id][str][st_flSyncGood] * 100) / (g_eStrafeStats[id][str][st_flSyncGood] + g_eStrafeStats[id][str][st_flSyncBad]);
			} else {
				iStrEff = 0;
			}

			iLen += format(szStrMess[iLen], sizeof szStrMess - iLen, "^t%s  %4.1f  %4.1f  %3.0f%  %d%%%%^n", g_szStrButtonsName[g_iStrButtonsInfo[id][str]], g_eStrafeStats[id][str][st_flSpeed], g_eStrafeStats[id][str][st_flSpeedFail], flStrTime, iStrEff);
		}

		if (g_iStrafes[id] != 0 && g_eWhichJump[id] != jt_Bhop) {
			iLen += format(szStrMess[iLen], sizeof szStrMess - iLen, "^t    (%4.1f)^t^t (%d%%%%)^n", g_flStrGain[id], g_iStrAllEff[id]);
			if (g_eWhichJump[id] == jt_LadderJump && g_eStrafeStats[id][0][st_flSpeed] != 0) {
				iLen += format(szStrMess[iLen], sizeof szStrMess - iLen, "^t^tturn speed: %4.3f", g_eStrafeStats[id][0][st_flSpeed]);
			}
			if (!g_eFailJump[id] && g_eWhichJump[id] != jt_LadderJump && g_eJumpstats[id][js_iJumpBlock] < 290.0 && g_eJumpstats[id][js_iJumpBlock] > 200.0 && g_eJumpstats[id][js_flJof] < 100.0 && g_eJumpstats[id][js_flJof] != 0.0 && (g_eJumpstats[id][js_iJumpBlock] + g_eJumpstats[id][js_flJof]) < g_eJumpstats[id][js_flDistance]) {
				if ((g_eJumpstats[id][js_iJumpBlock] + g_eJumpstats[id][js_flLanding] + g_eJumpstats[id][js_flJof]) > (g_eJumpstats[id][js_flDistance] + 10.0) || g_eJumpstats[id][js_flLanding] <= 0.0) {
					iLen += format(szStrMess[iLen], sizeof szStrMess - iLen, "^n^tblock: %d^n^tJof: %.f", g_eJumpstats[id][js_iJumpBlock], g_eJumpstats[id][js_flJof]);
				} else {
					iLen += format(szStrMess[iLen], sizeof szStrMess - iLen, "^n^tblock: %d^n^tJof: %.f	land: %.f", g_eJumpstats[id][js_iJumpBlock], g_eJumpstats[id][js_flJof], g_eJumpstats[id][js_flLanding]);
				}
			} else if (g_eFailJump[id] && g_eWhichJump[id] != jt_LadderJump && g_eJumpstats[id][js_iJumpBlock] < 290.0 && g_eJumpstats[id][js_iJumpBlock] > 200.0 && g_eJumpstats[id][js_flJof] < 100.0 && g_eJumpstats[id][js_flJof] != 0.0) {
				iLen += format(szStrMess[iLen], sizeof szStrMess - iLen, "^n^tblock: %d^n^tJof: %.f", g_eJumpstats[id][js_iJumpBlock], g_eJumpstats[id][js_flJof]);
			} else if (g_eWhichJump[id] != jt_LadderJump && g_eJumpstats[id][js_flJof] < 100.0 && g_eJumpstats[id][js_flJof] != 0.0) {
				iLen += format(szStrMess[iLen], sizeof szStrMess - iLen, "^n^tJof: %.f", g_eJumpstats[id][js_flJof]);
			} 

			if (g_isTouched[id]) {
				iLen += format(szStrMess[iLen], sizeof szStrMess - iLen, "^n^ttouch");
			}
			if (g_isTrigger[id]) {
				iLen += format(szStrMess[iLen], sizeof szStrMess - iLen, "^n^tsorry but first jump after triggers blocked");
			}
		}
	}

	if (g_eWhichJump[id] != jt_Not) {
		if (!g_eFailJump[id]) {
			set_hudmessage(g_pCvar[c_iStatsR], g_pCvar[c_iStatsG], g_pCvar[c_iStatsB], g_pCvar[c_flStrafeX], g_pCvar[c_flStrafeY], 0, 6.0, 2.5, 0.1, 0.3, g_pCvar[c_iStrafe]);
		} else {
			set_hudmessage(g_pCvar[c_iFailR], g_pCvar[c_iFailG], g_pCvar[c_iFailB], g_pCvar[c_flStrafeX], g_pCvar[c_flStrafeY], 0, 6.0, 2.5, 0.1, 0.3, g_pCvar[c_iStrafe]);
		}

		for (new i = 1; i < MaxClients; i++) {
			if ((i == id || g_isUserSpec[i]) && g_eOnOff[id][of_bStrafe] == true) {
				ShowSyncHudMsg(i, g_hudStrafe, szStrMess);
			}
		}
	}
}

public show_stats_hud(id) {
	for (new i = 1; i < MaxClients; i++) {
		if ((i == id || g_isUserSpec[i]) && g_eOnOff[i][of_bStats] == true) {

			if (!g_eFailJump[id]) {
				set_hudmessage(g_pCvar[c_iStatsR], g_pCvar[c_iStatsG], g_pCvar[c_iStatsB], g_pCvar[c_flStatsX], g_pCvar[c_flStatsY], 0, 6.0, 2.5, 0.1, 0.3, g_pCvar[c_iStats]);
			} else {
				set_hudmessage(g_pCvar[c_iFailR], g_pCvar[c_iFailG], g_pCvar[c_iFailB], g_pCvar[c_flStatsX], g_pCvar[c_flStatsY], 0, 6.0, 2.5, 0.1, 0.3, g_pCvar[c_iStats]);
			}

			switch (g_eWhichJump[id]) {
				case jt_LongJump, jt_HighJump, jt_LadderJump: {
					ShowSyncHudMsg(i, g_hudStats, "%s%s ^ndist: %.f^n%s pre: %.f^nendspeed: %.0f^nstrafes: %d", g_TypePre[g_eTypePre[id]][info_hud], g_eNameJumps[g_eWhichJump[id]][info_hud], g_eJumpstats[id][js_flDistance], g_eNameJumps[g_eWhichJump[id]][info_hud], g_eJumpData[id][g_iJumps[id]][JUMP_PRE], g_eJumpstats[id][js_flEndSpeed], g_iStrafes[id]);
				}
				case jt_BhopJump, jt_StandUpBhopJump, jt_DuckBhopJump, jt_BhopInDuck, jt_WeirdJump, jt_LadderBhopJump: {
					ShowSyncHudMsg(i, g_hudStats, "%s%s%s ^ndist: %.f^njump (%d): %.f^npre: %.f^nendspeed: %.0f^nstrafes: %d", g_TypePre[g_eTypePre[id]][info_hud], g_VerAir[g_iVerInfo[id]][info_hud], g_eNameJumps[g_eWhichJump[id]][info_hud], g_eJumpstats[id][js_flDistance], g_eJumpData[id][g_iJumps[id]][JUMP_FOG], g_eJumpData[id][g_iJumps[id]][JUMP_PRE], g_eJumpData[id][1][JUMP_PRE], g_eJumpstats[id][js_flEndSpeed], g_iStrafes[id]);
				}
				case jt_CountJump, jt_StandUPCountJump: {
					ShowSyncHudMsg(i, g_hudStats, "%s%s%s ^ndist: %.f^njump (%d): %.f^nduck: %.f^nendspeed: %.0f^nstrafes: %d", g_TypePre[g_eTypePre[id]][info_hud], g_VerAir[g_iVerInfo[id]][info_hud], g_eNameJumps[g_eWhichJump[id]][info_hud], g_eJumpstats[id][js_flDistance], g_eJumpData[id][g_iJumps[id]][JUMP_FOG], g_eJumpData[id][g_iJumps[id]][JUMP_PRE], g_eDuckData[id][1][DUCK_PRE], g_eJumpstats[id][js_flEndSpeed], g_iStrafes[id]);
				}
				case jt_Multi_CountJump, jt_Multi_StandUPCountJump: {
					if (g_iDucks[id] == 2) {
						ShowSyncHudMsg(i, g_hudStats, "%s%s%s (%d% ducks)^ndist: %.f^njump (%d): %.f^ndd (%d): %.f^nduck: %.f^nendspeed: %.0f^nstrafes: %d", g_TypePre[g_eTypePre[id]][info_hud], g_VerAir[g_iVerInfo[id]][info_hud], g_eNameJumps[g_eWhichJump[id]][info_hud], g_iDucks[id], g_eJumpstats[id][js_flDistance], g_eJumpData[id][g_iJumps[id]][JUMP_FOG], g_eJumpData[id][g_iJumps[id]][JUMP_PRE], g_eDuckData[id][2][DUCK_FOG], g_eDuckData[id][2][DUCK_PRE], g_eDuckData[id][1][DUCK_PRE], g_eJumpstats[id][js_flEndSpeed], g_iStrafes[id]);
					} else if (g_iDucks[id] == 3) {
						ShowSyncHudMsg(i, g_hudStats, "%s%s%s (%d% ducks)^ndist: %.f^njump (%d): %.f^ndd (%d): %.f^ndd (%d): %.f^nduck: %.f^nendspeed: %.0f^nstrafes: %d", g_TypePre[g_eTypePre[id]][info_hud], g_VerAir[g_iVerInfo[id]][info_hud], g_eNameJumps[g_eWhichJump[id]][info_hud], g_iDucks[id], g_eJumpstats[id][js_flDistance], g_eJumpData[id][g_iJumps[id]][JUMP_FOG], g_eJumpData[id][g_iJumps[id]][JUMP_PRE], g_eDuckData[id][3][DUCK_FOG], g_eDuckData[id][3][DUCK_PRE], g_eDuckData[id][2][DUCK_FOG], g_eDuckData[id][2][DUCK_PRE], g_eDuckData[id][1][DUCK_PRE], g_eJumpstats[id][js_flEndSpeed], g_iStrafes[id]);
					} else {
						ShowSyncHudMsg(i, g_hudStats, "%s%s%s (%d% ducks)^ndist: %.f^njump (%d): %.f^ndd (%d): %.f^n(+%d% dd)^nduck: %.f^nendspeed: %.0f^nstrafes: %d", g_TypePre[g_eTypePre[id]][info_hud], g_VerAir[g_iVerInfo[id]][info_hud], g_eNameJumps[g_eWhichJump[id]][info_hud], g_iDucks[id], g_eJumpstats[id][js_flDistance], g_eJumpData[id][g_iJumps[id]][JUMP_FOG], g_eJumpData[id][g_iJumps[id]][JUMP_PRE],  g_eDuckData[id][g_iDucks[id]][DUCK_FOG], g_eDuckData[id][g_iDucks[id]][DUCK_PRE], g_iDucks[id] - 2, g_eDuckData[id][1][DUCK_PRE], g_eJumpstats[id][js_flEndSpeed], g_iStrafes[id]);
					}
				}
			}
		}
	}
}

public show_console(id) {
	for (new i = 1; i < MaxClients; i++) {
		if ((i == id || g_isUserSpec[i]) && g_eOnOff[i][of_bStats] == true) {
			client_print(i, print_console, "");
			
			switch (g_eWhichJump[id]) {
				case jt_LongJump, jt_HighJump, jt_LadderJump: {
					client_print(i, print_console, "%s (%s)^ndistance: %.0f unit/s^nendspeed: %.0f unit/s^nprestrafe: %.0f^nframes: %d^nstrafes: %d", g_eNameJumps[g_eWhichJump[id]][info_hud], g_eNameJumps[g_eWhichJump[id]][info_console], g_eJumpstats[id][js_flDistance], g_eJumpstats[id][js_flEndSpeed], g_eJumpData[id][g_iJumps[id]][JUMP_PRE], g_eJumpstats[id][js_iFrames], g_iStrafes[id]);
				}
				case jt_BhopJump, jt_StandUpBhopJump, jt_DuckBhopJump, jt_BhopInDuck, jt_WeirdJump, jt_LadderBhopJump: {
					client_print(i, print_console, "%s%s (%s)^ndistance: %.0f unit/s^nendspeed: %.0f unit/s^njump %d FOG: pre %.0f^nPre: %.0f^nframes: %d^nstrafes: %d", g_VerAir[g_iVerInfo[id]][info_hud], g_eNameJumps[g_eWhichJump[id]][info_hud], g_eNameJumps[g_eWhichJump[id]][info_console], g_eJumpstats[id][js_flDistance], g_eJumpstats[id][js_flEndSpeed], g_eJumpData[id][g_iJumps[id]][JUMP_FOG], g_eJumpData[id][g_iJumps[id]][JUMP_PRE], g_eJumpData[id][1][JUMP_PRE], g_eJumpstats[id][js_iFrames], g_iStrafes[id]);
				}
				case jt_CountJump, jt_StandUPCountJump: {
					client_print(i, print_console, "%s%s (%s)^ndistance: %.0f unit/s^nendspeed: %.0f unit/s^njump %d FOG: pre %.0f^nduck pre: %.0f^nframes: %d^nstrafes: %d", g_VerAir[g_iVerInfo[id]][info_hud], g_eNameJumps[g_eWhichJump[id]][info_hud], g_eNameJumps[g_eWhichJump[id]][info_console], g_eJumpstats[id][js_flDistance], g_eJumpstats[id][js_flEndSpeed], g_eJumpData[id][g_iJumps[id]][JUMP_FOG], g_eJumpData[id][g_iJumps[id]][JUMP_PRE], g_eDuckData[id][1][DUCK_PRE], g_eJumpstats[id][js_iFrames], g_iStrafes[id]);
				}
				case jt_Multi_CountJump, jt_Multi_StandUPCountJump: {
					if (g_iDucks[id] == 2) {
						client_print(i, print_console, "%s%s (%s)^ndistance: %.0f unit/s^nendspeed: %.0f unit/s^njump %d FOG: pre %.0f^nduck pre: %.0f^nDucks: %d%^nframes: %d^nstrafes: %d", g_VerAir[g_iVerInfo[id]][info_hud], g_eNameJumps[g_eWhichJump[id]][info_hud], g_eNameJumps[g_eWhichJump[id]][info_console], g_eJumpstats[id][js_flDistance], g_eJumpstats[id][js_flEndSpeed], g_eJumpData[id][g_iJumps[id]][JUMP_FOG], g_eJumpData[id][g_iJumps[id]][JUMP_PRE], g_eDuckData[id][1][DUCK_PRE], g_iDucks[id], g_eJumpstats[id][js_iFrames], g_iStrafes[id]);	
					} else {
						client_print(i, print_console, "%s%s (%s)^ndistance: %.0f unit/s^nendspeed: %.0f unit/s^nprestrafe: (%.0f) %.0f^nDucks: %d%^nframes: %d^nstrafes: %d", g_VerAir[g_iVerInfo[id]][info_hud], g_eNameJumps[g_eWhichJump[id]][info_hud], g_eNameJumps[g_eWhichJump[id]][info_console], g_eJumpstats[id][js_flDistance], g_eJumpstats[id][js_flEndSpeed], g_eDuckData[id][1][DUCK_PRE], g_eJumpData[id][g_iJumps[id]][JUMP_PRE], g_iDucks[id], g_eJumpstats[id][js_iFrames], g_iStrafes[id]);
					}
				}
			}

			if (g_eWhichJump[id] != jt_Bhop) {
				client_print(i, print_console, "");
				client_print(i, print_console, "#N button gain-loss loss duration efficiency");
			}

			for (new str = 1; (str <= g_iStrafes[id]) && (str < NSTRAFES); str++) {

				if (g_eWhichJump[id] != jt_Not) {
					new Float:flStrTime = ((g_eStrafeStats[id][str][st_flTime] - g_eStrafeStats[id][str - 1][st_flTime]) * 100) / g_flStrFullTime[id];
					
					new iStrEff;
					if ((g_eStrafeStats[id][str][st_flSyncGood] + g_eStrafeStats[id][str][st_flSyncBad]) > 0) {
						iStrEff = (g_eStrafeStats[id][str][st_flSyncGood] * 100) / (g_eStrafeStats[id][str][st_flSyncGood] + g_eStrafeStats[id][str][st_flSyncBad]);
					} else {
						iStrEff = 0;
					}

					if (g_eWhichJump[id] != jt_Bhop)
						client_print(i, print_console, "%2d.  %s  %4.1f  %4.1f  %3.0f%  %3d%", str, g_szStrButtonsName[g_iStrButtonsInfo[id][str]], g_eStrafeStats[id][str][st_flSpeed], g_eStrafeStats[id][str][st_flSpeedFail], flStrTime, iStrEff);
				}
			}

			if (g_eWhichJump[id] != jt_Bhop) {
				client_print(i, print_console, "");
				client_print(i, print_console, "Total gain-loss: %4.1f / Efficiency: %d%%", g_flStrGain[id], g_iStrAllEff[id]);
			}

			if (g_eTypePre[id]) {
				client_print(i, print_console, "");
				client_print(i, print_console, "%s", g_TypePre[g_eTypePre[id]][info_console]);
			}

			if (g_eWhichJump[id] == jt_LadderJump && g_eStrafeStats[id][0][st_flSpeed] != 0) {
				client_print(i, print_console, "");
				client_print(i, print_console, "Turn speed: %.f", g_eStrafeStats[id][0][st_flSpeed]);
			} else if (g_eWhichJump[id] == jt_Multi_CountJump || g_eWhichJump[id] == jt_Multi_StandUPCountJump) {
				client_print(i, print_console, "");
				client_print(i, print_console, "Pre duck: %.f", g_eDuckData[id][1][DUCK_PRE]);
				for (new ss = 2; ss <= g_iDucks[id]; ss++)
					client_print(i, print_console, "Pre dd#%d: %.f, FOG(%d)", ss - 1, g_eDuckData[id][ss][DUCK_PRE], g_eDuckData[id][ss][DUCK_FOG]);
				client_print(i, print_console, "Pre jump: %.f, FOG(%d)", g_eJumpData[id][g_iJumps[id]][JUMP_PRE], g_eJumpData[id][g_iJumps[id]][JUMP_FOG]);
			}
			if (g_eWhichJump[id] != jt_Bhop) {
				if (g_isTouched[id]) {
					client_print(i, print_console, "");
					client_print(i, print_console, "Probably touched something");
				}
				if (!g_eFailJump[id] && g_eWhichJump[id] != jt_LadderJump && g_eJumpstats[id][js_iJumpBlock] < 290.0 && g_eJumpstats[id][js_iJumpBlock] > 200.0 && g_eJumpstats[id][js_flJof] < 100.0 && g_eJumpstats[id][js_flJof] != 0.0 && (g_eJumpstats[id][js_iJumpBlock] + g_eJumpstats[id][js_flJof]) < g_eJumpstats[id][js_flDistance]) {
					client_print(i, print_console, "");
					if ((g_eJumpstats[id][js_iJumpBlock] + g_eJumpstats[id][js_flLanding] + g_eJumpstats[id][js_flJof]) > (g_eJumpstats[id][js_flDistance] + 10.0) || g_eJumpstats[id][js_flLanding] <= 0.0)
						client_print(i, print_console, "Block: %d / Jumpoff: %.f", g_eJumpstats[id][js_iJumpBlock], g_eJumpstats[id][js_flJof]);
					else
						client_print(i, print_console, "Block: %d / Jumpoff: %.f / Landing: %.f", g_eJumpstats[id][js_iJumpBlock], g_eJumpstats[id][js_flJof], g_eJumpstats[id][js_flLanding]);
				} else if (g_eFailJump[id] && g_eWhichJump[id] != jt_LadderJump && g_eJumpstats[id][js_iJumpBlock] < 290.0 && g_eJumpstats[id][js_iJumpBlock] > 200.0 && g_eJumpstats[id][js_flJof] < 100.0 && g_eJumpstats[id][js_flJof] != 0.0 && (g_eJumpstats[id][js_iJumpBlock] + g_eJumpstats[id][js_flJof]) < g_eJumpstats[id][js_flDistance]) {
					client_print(i, print_console, "");
					client_print(i, print_console, "Fail Block: %d / Jumpoff: %.f", g_eJumpstats[id][js_iJumpBlock], g_eJumpstats[id][js_flJof]);
				} else if (g_eFailJump[id] && g_eWhichJump[id] != jt_LadderJump && g_eJumpstats[id][js_flJof] < 100.0 && g_eJumpstats[id][js_flJof] != 0.0 && g_eJumpstats[id][js_iJumpBlock] < 290.0 && g_eJumpstats[id][js_iJumpBlock] > 200.0) {
					client_print(i, print_console, "");
					client_print(i, print_console, "Fail Block: %d / Jumpoff: %.f", g_eJumpstats[id][js_iJumpBlock], g_eJumpstats[id][js_flJof]);
				} else if (g_eFailJump[id] && g_eWhichJump[id] != jt_LadderJump && g_eJumpstats[id][js_flJof] < 100.0 && g_eJumpstats[id][js_flJof] != 0.0) {
					client_print(i, print_console, "");
					client_print(i, print_console, "Fail Jumpoff: %.f", g_eJumpstats[id][js_flJof]);
				} else if (!g_eFailJump[id] && g_eWhichJump[id] != jt_LadderJump && g_eJumpstats[id][js_flJof] < 100.0 && g_eJumpstats[id][js_flJof] != 0.0) {
					client_print(i, print_console, "");
					client_print(i, print_console, "Jumpoff: %.f", g_eJumpstats[id][js_flJof]);
				}
			}
			if (g_isTrigger[id]) {
				client_print(i, print_console, "");
				client_print(i, print_console, "Sorry but first jump after Triggers blocked.");
			}
		}
	}
}

public show_chat(id, iDistance) {
	new aDistance[ChatDist];
	aDistance = get_colorchat_by_distance(g_eWhichJump[id], g_flMaxSpeed[id]);

	new szJumpBlock[20];

	if (g_eWhichJump[id] != jt_LadderJump && g_eJumpstats[id][js_iJumpBlock] < 290.0 && g_eJumpstats[id][js_iJumpBlock] > 200.0 && g_eJumpstats[id][js_flJof] < 100.0 && g_eJumpstats[id][js_flJof] != 0.0 && (g_eJumpstats[id][js_iJumpBlock] + g_eJumpstats[id][js_flJof]) < g_eJumpstats[id][js_flDistance]) {
		formatex(szJumpBlock, charsmax(szJumpBlock), " (block: ^3%d^1)", g_eJumpstats[id][js_iJumpBlock]);
	} else {
		formatex(szJumpBlock, charsmax(szJumpBlock), "");
	}

	new iPlayers[MAX_PLAYERS], iNum;
	get_players(iPlayers, iNum, "ch");

	for (new i = 0; i < iNum; i++) {
		new iPlayer = iPlayers[i];
		if (g_eOnOff[iPlayer][of_bChatInfo] == true) {
			switch (g_eWhichJump[id]) {
				case jt_Multi_CountJump, jt_Multi_StandUPCountJump: {
					if (g_eJumpstats[id][js_flDistance] >= aDistance[god]) {
						client_print_color(iPlayer, print_team_red, ">> ^4%n ^3jumped ^4%.0f ^3with ^4%s ^3(pre:^4%.0f ^3str:^4%i ^3sync:^4%i%%^3) (^4%d ^3ducks)", id, g_eJumpstats[id][js_flDistance], g_eNameJumps[g_eWhichJump[id]][info_chat], g_eJumpData[id][g_iJumps[id]][JUMP_PRE], g_iStrafes[id], g_iStrAllEff[id], g_iDucks[id]);
					} else if (g_eJumpstats[id][js_flDistance] >= aDistance[leet]) {
						client_print_color(iPlayer, print_team_red, ">> ^3%n ^1jumped ^3%.0f ^1with ^3%s ^1(pre:^3%.0f ^1str:^3%i ^1sync:^3%i%%^1) (^3%d ^1ducks)", id, g_eJumpstats[id][js_flDistance], g_eNameJumps[g_eWhichJump[id]][info_chat], g_eJumpData[id][g_iJumps[id]][JUMP_PRE], g_iStrafes[id], g_iStrAllEff[id], g_iDucks[id]);
					} else if (g_eJumpstats[id][js_flDistance] >= aDistance[holy]) {
						client_print_color(iPlayer, print_team_blue, ">> ^3%n ^1jumped ^3%.0f ^1with ^3%s ^1(pre:^3%.0f ^1str:^3%i ^1sync:^3%i%%^1) (^3%d ^1ducks)", id, g_eJumpstats[id][js_flDistance], g_eNameJumps[g_eWhichJump[id]][info_chat], g_eJumpData[id][g_iJumps[id]][JUMP_PRE], g_iStrafes[id], g_iStrAllEff[id], g_iDucks[id]);
					} else if (g_eJumpstats[id][js_flDistance] >= aDistance[pro]) {
						client_print_color(iPlayer, print_team_blue, ">> ^3%n ^1jumped ^3%.0f ^1with ^3%s ^1(pre:^3%.0f ^1str:^3%i ^1sync:^3%i%%^1) (^3%d ^1ducks)", id, g_eJumpstats[id][js_flDistance], g_eNameJumps[g_eWhichJump[id]][info_chat], g_eJumpData[id][g_iJumps[id]][JUMP_PRE], g_iStrafes[id], g_iStrAllEff[id], g_iDucks[id]);
					} else if (g_eJumpstats[id][js_flDistance] >= aDistance[good]) {
						client_print_color(iPlayer, print_team_grey, ">> ^3%n ^1jumped ^3%.0f ^1with ^3%s ^1(pre:^3%.0f ^1str:^3%i ^1sync:^3%i%%^1) (^3%d ^1ducks)", id, g_eJumpstats[id][js_flDistance], g_eNameJumps[g_eWhichJump[id]][info_chat], g_eJumpData[id][g_iJumps[id]][JUMP_PRE], g_iStrafes[id], g_iStrAllEff[id], g_iDucks[id]);
					}
				}
				default: {
					if (g_eJumpstats[id][js_flDistance] >= aDistance[god]) {
						client_print_color(iPlayer, print_team_red, ">> ^4%n ^3jumped ^4%.0f ^3with ^4%s ^3(pre:^4%.0f ^3str:^4%i ^3sync:^4%i%%^3)^1%s%s", id, g_eJumpstats[id][js_flDistance], g_eNameJumps[g_eWhichJump[id]][info_chat], g_eJumpData[id][g_iJumps[id]][JUMP_PRE], g_iStrafes[id], g_iStrAllEff[id], szJumpBlock, g_TypePre[g_eTypePre[id]][info_chat]);
					} else if (g_eJumpstats[id][js_flDistance] >= aDistance[leet]) {
						client_print_color(iPlayer, print_team_red, ">> ^3%n ^1jumped ^3%.0f ^1with ^3%s ^1(pre:^3%.0f ^1str:^3%i ^1sync:^3%i%%^1)%s%s", id, g_eJumpstats[id][js_flDistance], g_eNameJumps[g_eWhichJump[id]][info_chat], g_eJumpData[id][g_iJumps[id]][JUMP_PRE], g_iStrafes[id], g_iStrAllEff[id], szJumpBlock, g_TypePre[g_eTypePre[id]][info_chat]);
					} else if (g_eJumpstats[id][js_flDistance] >= aDistance[holy]) {
						client_print_color(iPlayer, print_team_blue, ">> ^3%n ^1jumped ^3%.0f ^1with ^3%s ^1(pre:^3%.0f ^1str:^3%i ^1sync: ^3%i%%^1)%s%s", id, g_eJumpstats[id][js_flDistance], g_eNameJumps[g_eWhichJump[id]][info_chat], g_eJumpData[id][g_iJumps[id]][JUMP_PRE], g_iStrafes[id], g_iStrAllEff[id], szJumpBlock, g_TypePre[g_eTypePre[id]][info_chat]);
					} else if (g_eJumpstats[id][js_flDistance] >= aDistance[pro]) {
						client_print_color(iPlayer, print_team_blue, ">> ^3%n ^1jumped ^3%.0f ^1with ^3%s ^1(pre:^3%.0f ^1str:^3%i ^1sync: ^3%i%%^1)%s%s", id, g_eJumpstats[id][js_flDistance], g_eNameJumps[g_eWhichJump[id]][info_chat], g_eJumpData[id][g_iJumps[id]][JUMP_PRE], g_iStrafes[id], g_iStrAllEff[id], szJumpBlock, g_TypePre[g_eTypePre[id]][info_chat]);
					} else if (g_eJumpstats[id][js_flDistance] >= aDistance[good]) {
						client_print_color(iPlayer, print_team_grey, ">> ^3%n ^1jumped ^3%.0f ^1with ^3%s ^1(pre:^3%.0f ^1str:^3%i ^1sync: ^3%i%%^1)%s%s", id, g_eJumpstats[id][js_flDistance], g_eNameJumps[g_eWhichJump[id]][info_chat], g_eJumpData[id][g_iJumps[id]][JUMP_PRE], g_iStrafes[id], g_iStrAllEff[id], szJumpBlock, g_TypePre[g_eTypePre[id]][info_chat]);
					}
				}
			}
		}
	}
}