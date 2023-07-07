public in_ladder(id, bool:isJump) {
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
	g_iJumps[id]++;
	g_iVerInfo[id] = IS_MIDDLE;
	g_eJumpType[id] = IS_JUMP;
	g_isLadderBhop[id] = true;
	g_eJumpData[id][g_iJumps[id]][JUMP_PRE] = g_flHorSpeed[id];

	if (isJump) {
		show_pre(id, pre_ladder_jump, g_flHorSpeed[id]);
	} else {
		g_isLadderBhop[id] = true;

		g_eWhichJump[id] = jt_LadderJump;

		g_flFirstJump[id][2] = g_flLadderXYZ[id][2] + 35.031250;

		if (g_flLadderSize[id][0] <= g_flLadderSize[id][1]) {
			g_flLadderDist[id] = g_flLadderSize[id][0] + 0.03125;
		} else if (g_flLadderSize[id][0] > g_flLadderSize[id][1]) {
			g_flLadderDist[id] = g_flLadderSize[id][1] + 0.03125;
		}

		if (g_flLadderDist[id] > 10) {
			g_flLadderDist[id] = 4.0;
		}
		
		show_pre(id, pre_ladder, g_flHorSpeed[id]);
	}
}