
stock Float:vector_hor_length(Float:flVel[3]) {
	new Float:flNorma = floatpower(flVel[0], 2.0) + floatpower(flVel[1], 2.0);
	if (flNorma > 0.0)
		return floatsqroot(flNorma);
		
	return 0.0;
}

stock vector_copy(Float:flVec1[3], Float:flVec2[3]) {
	flVec2[0] = flVec1[0];
	flVec2[1] = flVec1[1];
	flVec2[2] = flVec1[2];
	return 0;
}


stock detect_hj(id, Float:flOrigin[3], Float:VerOrigin) {
	new Float:flStarthj[3];
	flStarthj[0] = flOrigin[0];
	flStarthj[1] = flOrigin[1];
	flStarthj[2] = VerOrigin + 28.0;

	new Float:flStophj[3]
	flStophj[0] = flOrigin[0];
	flStophj[1] = flOrigin[1];
	flStophj[2] = flStarthj[2] - 133.0;

	engfunc(EngFunc_TraceLine, flStarthj, flStophj, IGNORE_MONSTERS, id, 0);

	new Float:flEndHJ[3];

	get_tr2(0, TR_vecEndPos, flEndHJ);

	if (flStarthj[2] - flEndHJ[2] < 133.0 && (flStarthj[2] - flEndHJ[2] - 64) != 0 && (flStarthj[2] - flEndHJ[2] - 64) > 0 && g_iDetectHJ[id] != 1) {
		g_iDetectHJ[id] = 2;
	}

	if (flStarthj[2] - flEndHJ[2] >= 133.0 && g_iDetectHJ[id] != 2) {
		g_iDetectHJ[id] = 1;
	}
}

enum _:ChatDist {
	good = 0, pro, holy, leet, god
}

public get_colorchat_by_distance(JumpType:iJumpType, Float:mSpeed) {
	new aDistance[ChatDist];

	new iTdist = floatround((250.0 - mSpeed) * 0.73, floatround_floor);

	new aircj;
	if (get_cvar_pointer("sv_airaccelerate") > 10) {
		aircj = 10;
	} else {
		aircj = 0;
	}

	switch (iJumpType) {
		case jt_LongJump, jt_HighJump: {
			aDistance[god] = 255;
			aDistance[leet] = 253;
			aDistance[holy] = 250;
			aDistance[pro] = 245;
			aDistance[good] = 240;
		}
		case jt_CountJump, jt_StandUPCountJump: {
			aDistance[god] = 270;
			aDistance[leet] = 265;
			aDistance[holy] = 260;
			aDistance[pro] = 255;
			aDistance[good] = 250;
		}
		case jt_Multi_CountJump, jt_Multi_StandUPCountJump: {
			aDistance[god] = 262 + aircj;
			aDistance[leet] = 257 + aircj;
			aDistance[holy] = 255 + aircj;
			aDistance[pro] = 250 + aircj;
			aDistance[good] = 245 + aircj;
		}
		case jt_WeirdJump, jt_LadderBhopJump: {
			aDistance[god] = 272;
			aDistance[leet] = 270;
			aDistance[holy] = 265;
			aDistance[pro] = 260;
			aDistance[good] = 255;
		}
		case jt_LadderJump: {
			aDistance[god] = 190;
			aDistance[leet] = 180;
			aDistance[holy] = 170;
			aDistance[pro] = 160;
			aDistance[good] = 150;
		}
		case jt_BhopJump, jt_StandUpBhopJump: {
			aDistance[god] = 247;
			aDistance[leet] = 245;
			aDistance[holy] = 240;
			aDistance[pro] = 235;
			aDistance[good] = 230;
		}
		case jt_BhopInDuck: {
			aDistance[god] = 220;
			aDistance[leet] = 217;
			aDistance[holy] = 215;
			aDistance[pro] = 210;
			aDistance[good] = 205;
		}
		case jt_DuckBhopJump: {
			aDistance[god] = 220;
			aDistance[leet] = 217;
			aDistance[holy] = 215;
			aDistance[pro] = 210;
			aDistance[good] = 205;
		}
	}

	if (mSpeed != 250.0 && iJumpType != jt_LadderJump) {
		aDistance[god] = aDistance[god] - iTdist;
		aDistance[leet] = aDistance[leet] - iTdist;
		aDistance[holy] = aDistance[holy] - iTdist;
		aDistance[pro] = aDistance[pro] - iTdist;
		aDistance[good] = aDistance[good] - iTdist;
	}
	return aDistance;
}


stock Float:GetFailedDistance(Float:ExDist, bool:ducking, Float:gravity, Float:g_flJumpOffOrigin[3], Float:current_velocity[3], Float:g_flFailedOrigin[3], Float:g_flFailedVelocity[3]) {
	static Float:airtime, Float:land_origin[3], Float:distance;
	
	if(ducking)
		g_flJumpOffOrigin[2] -= 18.0;
	
	airtime = ((floatsqroot((g_flFailedVelocity[2] * g_flFailedVelocity[2]) - (2.0 * -gravity * (g_flFailedOrigin[2] - g_flJumpOffOrigin[2])))*-1) - g_flFailedVelocity[2]) / -gravity;
	
	land_origin[0] = floatabs(g_flFailedOrigin[0] - g_flJumpOffOrigin[0]) + floatabs(current_velocity[0] * airtime);
	land_origin[1] = floatabs(g_flFailedOrigin[1] - g_flJumpOffOrigin[1]) + floatabs(current_velocity[1] * airtime);
	
	distance = vector_length(land_origin) + ExDist;
	
	if(ducking)
		g_flJumpOffOrigin[2] += 18.0;
	
	return distance;
}

enum TypeInfo {
	all = 0, one, multi
}

stock is_user_spectating_player(spectator, player) {
        if(!is_user_connected(spectator) || !is_user_connected(player))
            return 0;

        if(is_user_alive(spectator) || !is_user_alive(player))
            return 0;

        static specmode;
        specmode = pev(spectator, pev_iuser1);

        if(specmode == 3)
            return 0;
       
        if(pev(spectator, pev_iuser2) == player)
            return 1;
       
        return 0;
}

public calc_jof_block(id, Float:flOrigin[3]) {
	new Float:tmpblock[3], tmpjblock[3], Float:tmpOrigin[3], Float:tmpOrigin2[3];

	flOrigin[2] = flOrigin[2] - 36.1;


	new block_checking[3];

	for (new i = 0, j = -18; i < 3; i++, j = j + 18) {
		tmpOrigin = flOrigin;
		tmpOrigin2 = flOrigin;
		tmpblock = flOrigin;
		if (g_flVelocity[id][1] > 0 && floatabs(g_flVelocity[id][1]) > floatabs(g_flVelocity[id][0])) {
			tmpOrigin[1] = flOrigin[1] + 100;
			tmpOrigin2[1] = flOrigin[1] - 16;

			tmpOrigin[0] = tmpOrigin[0] + j;
			tmpOrigin2[0] = tmpOrigin2[0] + j;

			tmpblock[1] = flOrigin[1] + 290.0 + 1;
			tmpblock[0] = tmpblock[0] + j;
		} else if (g_flVelocity[id][1] < 0 && floatabs(g_flVelocity[id][1]) > floatabs(g_flVelocity[id][0])) {
			tmpOrigin[1] = flOrigin[1] - 100;
			tmpOrigin2[1] = flOrigin[1] + 16;

			tmpOrigin[0] = tmpOrigin[0] + j;
			tmpOrigin2[0] = tmpOrigin2[0] + j;

			tmpblock[1] = flOrigin[1] - 290.0 + 1;
			tmpblock[0] = tmpblock[0] + j;
		} else if (g_flVelocity[id][0] > 0 && floatabs(g_flVelocity[id][0]) > floatabs(g_flVelocity[id][1])) {
			tmpOrigin[0] = flOrigin[0] + 100;
			tmpOrigin2[0] = flOrigin[0] - 16;

			tmpOrigin[1] = tmpOrigin[1] + j;
			tmpOrigin2[1] = tmpOrigin2[1] + j;

			tmpblock[0] = flOrigin[0] + 290.0 + 1;
			tmpblock[1] = tmpblock[1] + j;
		} else if (g_flVelocity[id][0] < 0 && floatabs(g_flVelocity[id][0]) > floatabs(g_flVelocity[id][1])) {
			tmpOrigin[0] = flOrigin[0] - 100;
			tmpOrigin2[0] = flOrigin[0] + 16;

			tmpOrigin[1] = tmpOrigin[1] + j;
			tmpOrigin2[1] = tmpOrigin2[1] + j;

			tmpblock[0] = flOrigin[0] - 290.0 + 1;
			tmpblock[1] = tmpblock[1] + j;
		}

		new Float:tmpEdgeOrigin[3];

		engfunc(EngFunc_TraceLine, tmpOrigin, tmpOrigin2, IGNORE_GLASS, id, 0);
		get_tr2(0, TR_vecEndPos, tmpEdgeOrigin);

		if (get_distance_f(tmpEdgeOrigin, tmpOrigin2) != 0.0) {
			g_eJumpstats[id][js_flJof] = get_distance_f(tmpEdgeOrigin, tmpOrigin2) - 0.031250;
		}

		new Float:tmpblockOrigin[3];

		engfunc(EngFunc_TraceLine, tmpEdgeOrigin, tmpblock, IGNORE_GLASS, id, 0);
		get_tr2(0, TR_vecEndPos, tmpblockOrigin);

		if (get_distance_f(tmpblockOrigin, tmpEdgeOrigin) != 0.0) {
			tmpjblock[i] = floatround(get_distance_f(tmpblockOrigin, tmpEdgeOrigin), floatround_floor) + 1;
		}

		new Float:checkblock1[3], Float:checkblock2[3];
		tmpblockOrigin[2] = tmpblockOrigin[2] - 1.0;

		checkblock1 = tmpblockOrigin;

		if (g_flVelocity[id][1] > 0 && floatabs(g_flVelocity[id][1]) > floatabs(g_flVelocity[id][0])) {
			checkblock1[1] = checkblock1[1] + 2.0;
		} else if (g_flVelocity[id][1] < 0 && floatabs(g_flVelocity[id][1]) > floatabs(g_flVelocity[id][0])) {
			checkblock1[1] = checkblock1[1] - 2.0;
		} else if (g_flVelocity[id][0] > 0 && floatabs(g_flVelocity[id][0]) > floatabs(g_flVelocity[id][1])) {
			checkblock1[0] = checkblock1[0] + 2.0;
		} else if (g_flVelocity[id][0] < 0 && floatabs(g_flVelocity[id][0]) > floatabs(g_flVelocity[id][1])) {
			checkblock1[0] = checkblock1[0] - 2.0;
		}

		checkblock2 = checkblock1;
		checkblock2[2] = checkblock2[2] + 18.0;

		new Float:tmpcheckblock[3];
		engfunc(EngFunc_TraceLine, checkblock2, checkblock1, IGNORE_GLASS, id, 0);
		get_tr2(0, TR_vecEndPos, tmpcheckblock);

		if (floatabs(tmpblockOrigin[2] - tmpcheckblock[2]) == 0.0) {
			block_checking[i] = 1;
		}
	}

	if (tmpjblock[0] != 0 && tmpjblock[0] <= tmpjblock[1] && tmpjblock[0] <= tmpjblock[2]) {
		if (!block_checking[0])
			g_eJumpstats[id][js_iJumpBlock] = tmpjblock[0];
	} else if (tmpjblock[1] != 0 && tmpjblock[1] <= tmpjblock[2] && tmpjblock[0] <= tmpjblock[0]) {
		if (!block_checking[1])
			g_eJumpstats[id][js_iJumpBlock] = tmpjblock[1];
	} else if (tmpjblock[2] != 0 && tmpjblock[2] <= tmpjblock[1] && tmpjblock[0] <= tmpjblock[0]) {
		if (!block_checking[2])
			g_eJumpstats[id][js_iJumpBlock] = tmpjblock[2];
	} else g_eJumpstats[id][js_iJumpBlock] = 0;

	if (g_eJumpstats[id][js_flJof] != 0.0 && (g_eOnOff[id][of_bJof])) {
		show_jof(id, g_eJumpstats[id][js_flJof]);
	}

}