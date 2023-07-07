public in_ladder(id, bool:isJump) {
	g_flFirstJump[id] = g_flPrevOrigin[id];

	g_eJumpType[id] = IS_JUMP;
	g_iJumps[id]++;
	g_eJumpData[id][g_iJumps[id]][JUMP_PRE] = g_flPrevHorSpeed[id];
	g_eJumpstats[id][js_flEndSpeed] = g_flPrevHorSpeed[id];

	g_isLadderBhop[id] = true;

	if (isJump) {
		show_pre(id, pre_ladder_jump, g_flPrevHorSpeed[id]);
	} else {
		g_eWhichJump[id] = jt_LadderJump;

		g_flFirstJump[id][2] = g_flLadderXYZ[id][2] + 35.031250;

		g_flLadderDist[id] = (g_flLadderSize[id][0] <= g_flLadderSize[id][1] ?  g_flLadderSize[id][0] : g_flLadderSize[id][1]) + 0.03125;

		g_flLadderDist[id] = g_flLadderDist[id] > 10 ? 4.0 : g_flLadderDist[id];
		
		show_pre(id, pre_ladder, g_flPrevHorSpeed[id]);
	}
}