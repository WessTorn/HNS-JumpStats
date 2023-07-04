public in_ladder(id, bool:isJump) {
	if (isJump) {
		g_eJumpType[id] = IS_JUMP_INLADDER;
	} else {
		g_eWhichJump[id] = jt_LadderJump;
		g_flFirstJump[id] = g_flPrevOrigin[id];
		g_flFirstJump[id][2] = g_flPrevOrigin[id][2];
		g_flJumpOffTime[id] = get_gametime();
		g_iStrOldButtons[id] = 0;
		g_flTempSpeed[id] = 0.0;
		for (new i = 0; i < NSTRAFES; i++) {
			g_eStrafeStats[id][i][st_flSpeed] = 0.0;
			g_eStrafeStats[id][i][st_flSpeedFail] = 0.0;
			g_eStrafeStats[id][i][st_flSyncGood] = 0;
			g_eStrafeStats[id][i][st_flSyncBad] = 0;
			g_eStrafeStats[id][i][st_flTime] = 0.0;
		}
	}
	show_pre(id, pre_ladder, g_flPrevHorSpeed[id]);
}