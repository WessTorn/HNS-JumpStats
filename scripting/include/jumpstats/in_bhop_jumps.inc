public in_bhop_js(id, iFog) {
	g_iStrafes[id] = 0;
	g_flFirstJump[id] = g_flPrevOrigin[id];
	g_flJumpOffTime[id] = get_gametime();
	g_flTempSpeed[id] = 0.0;

	for (new i = 0; i < NSTRAFES; i++) {
		g_eStrafeStats[id][i][st_flSpeed] = 0.0;
		g_eStrafeStats[id][i][st_flSpeedFail] = 0.0;
		g_eStrafeStats[id][i][st_flSyncGood] = 0;
		g_eStrafeStats[id][i][st_flSyncBad] = 0;
		g_eStrafeStats[id][i][st_flTime] = 0.0;
	}

	g_iStrOldButtons[id] = 0;
	if (iFog > 10)
	{
		g_iJumps[id] = 0;
		g_iJumps[id]++;
		g_eJumpData[id][g_iJumps[id]][JUMP_PRE] = g_flHorSpeed[id];
		g_eJumpData[id][g_iJumps[id]][JUMP_FOG] = iFog;
		g_iVerInfo[id] = IS_MIDDLE;
		
		if (g_bPrevInDuck[id]) {
			g_eJumpType[id] = IS_DUCKBHOP;
			g_flFirstJump[id][2] = g_flFirstJump[id][2] + 18.0;
			g_flJumpFirstZ[id] = g_flPrevOrigin[id][2] + 18.0;
		} else {
			g_eJumpType[id] = IS_JUMP;
			g_eWhichJump[id] = jt_LongJump;
			g_eJumpstats[id][js_flEndSpeed] = g_flHorSpeed[id];
			g_flJumpFirstZ[id] = g_flPrevOrigin[id][2];
		}

		show_pre(id, pre_jump, g_flPrevHorSpeed[id]);
		// ЖАМП
	}
	else
	{
		g_iJumps[id]++;
		g_eJumpData[id][g_iJumps[id]][JUMP_PRE] = g_flHorSpeed[id];
		g_eJumpData[id][g_iJumps[id]][JUMP_FOG] = iFog;

		g_flFirstJump[id][2] = g_bPrevInDuck[id] ? g_flFirstJump[id][2] + 18.0 : g_flFirstJump[id][2];

		if (g_eDuckType[id] != IS_DUCK_NOT && g_iDucks[id]) {
			if (g_iVerInfo[id] == IS_MIDDLE) {
				new Float:flDuckZ = g_bPrevInDuck[id] ? g_flPrevOrigin[id][2] + 18.0 : g_flPrevOrigin[id][2]; 

				if (flDuckZ - g_flDuckFirstZ[id] < -4.0) {
					g_iVerInfo[id] = IS_DROPPED;
				} else if (flDuckZ - g_flDuckFirstZ[id] > 4.0) {
					g_iVerInfo[id] = IS_UPPED;
				}
			}

			if (g_eDuckType[id] == IS_SGS) {
				if (g_iDucks[id] == 1) {
					g_eWhichJump[id] = jt_StandUPCountJump;
				} else {
					g_eWhichJump[id] = jt_Multi_StandUPCountJump;
				}

				g_eJumpType[id] = IS_JUMP;
				show_pre(id, pre_sbj, g_flPreHorSpeed[id], iFog, g_flHorSpeed[id]);
			} else if (g_eDuckType[id] == IS_DD || g_eDuckType[id] == IS_DUCK) {
				if (g_iDucks[id] == 1) {
					g_eWhichJump[id] = jt_CountJump;
				} else {
					g_eWhichJump[id] =  jt_Multi_CountJump;
				}

				g_eJumpType[id] = IS_JUMP;
				show_pre(id, pre_sbj, g_flPreHorSpeed[id], iFog, g_flHorSpeed[id]);
			}
		} else {
			if (g_eJumpType[id] != IS_JUMP_NOT) {
				if (g_iJumps[id] <= 2) {
					if (g_eJumpType[id] == IS_DUCKBHOP) {
						g_eWhichJump[id] = jt_DuckBhopJump;
						show_pre(id, pre_duckbhop, g_flPreHorSpeed[id], iFog, g_flHorSpeed[id]);
					} else if (g_bInDuck[id]) {
						g_eWhichJump[id] = jt_BhopInDuck
						show_pre(id, pre_bid, g_flPreHorSpeed[id], iFog, g_flHorSpeed[id]);
					} else if (g_eJumpType[id] == IS_SBJ) {
						g_eWhichJump[id] = jt_StandUpBhopJump
						show_pre(id, pre_sbj, g_flPreHorSpeed[id], iFog, g_flHorSpeed[id]);
					} else if (g_eJumpType[id] == IS_JUMP) {
						g_eWhichJump[id] = jt_BhopJump;
						show_pre(id, pre_bhop, g_flPreHorSpeed[id], iFog, g_flHorSpeed[id]);
					}

					if (g_iVerInfo[id] == IS_MIDDLE && !g_isLadderBhop[id] && !g_isFalling[id]) {
						new Float:flJumpZ = g_bPrevInDuck[id] ? g_flPrevOrigin[id][2] + 18.0 : g_flPrevOrigin[id][2];

						if (flJumpZ - g_flJumpFirstZ[id] < -4.0) {
							g_iVerInfo[id] = IS_DROPPED;
						} else if (flJumpZ - g_flJumpFirstZ[id] > 4.0) {
							g_iVerInfo[id] = IS_UPPED;
						}
					} else if (g_isLadderBhop[id]) {
						g_ePreStats[id][ptLadderBhop] = true;
						g_isLadderBhop[id] = false;
						g_iVerInfo[id] = IS_MIDDLE;
					}
				} else {
					g_eWhichJump[id] = jt_Not;

					if (g_eJumpType[id] == IS_DUCKBHOP) {
						show_pre(id, pre_duckbhop, g_flPreHorSpeed[id], iFog, g_flHorSpeed[id]);
					} else if (g_bInDuck[id]) {
						show_pre(id, pre_bid, g_flPreHorSpeed[id], iFog, g_flHorSpeed[id]);
					} else if (g_eJumpType[id] == IS_SBJ) {
						show_pre(id, pre_sbj, g_flPreHorSpeed[id], iFog, g_flHorSpeed[id]);
					} else if (g_eJumpType[id] == IS_JUMP) {
						show_pre(id, pre_bhop, g_flPreHorSpeed[id], iFog, g_flHorSpeed[id]);
					}
				}
				g_eJumpType[id] = IS_JUMP;
			}
		}
		g_eJumpstats[id][js_flEndSpeed] = g_flHorSpeed[id];
		// БХОП
	}
}