public in_ducks(id, iFog) {	
	if (iFog > 10) {
		g_iDucks[id] = 0;
		g_iDucks[id]++;
		g_eDuckData[id][g_iDucks[id]][DUCK_PRE] = g_flPrevHorSpeed[id];
		g_eDuckData[id][g_iDucks[id]][DUCK_FOG] = iFog;

		g_iVerInfo[id] = IS_MIDDLE;

		g_flDuckFirstZ[id] = g_bPrevInDuck[id] ? g_flPrevOrigin[id][2] + 18.0 : g_flPrevOrigin[id][2];

		g_eDuckType[id] = g_eDuckType[id] != IS_SGS && g_eDuckType[id] != IS_DD ? IS_DUCK : g_eDuckType[id];

		show_pre(id, pre_duck, g_flPrevHorSpeed[id]);
	} else {
		g_iDucks[id]++;
		g_eDuckData[id][g_iDucks[id]][DUCK_PRE] = g_flPrevHorSpeed[id];
		g_eDuckData[id][g_iDucks[id]][DUCK_FOG] = iFog;

		if (g_iVerInfo[id] == IS_MIDDLE && !g_isLadderBhop[id] && !g_isFalling[id]) {
			new Float:flDuckZ = g_bPrevInDuck[id] ? g_flPrevOrigin[id][2] + 18.0 : g_flPrevOrigin[id][2];
			
			if (flDuckZ - g_flDuckFirstZ[id] < -4.0) {
				g_iVerInfo[id] = IS_DROPPED;
			} else if (flDuckZ - g_flDuckFirstZ[id] > 4.0) {
				g_iVerInfo[id] = IS_UPPED;
			}
		}

		g_eDuckType[id] = g_eDuckType[id] == IS_SGS ? IS_SGS : IS_DD;

		show_pre(id, pre_dd, g_flPreHorSpeed[id], iFog, g_flPrevHorSpeed[id]);
	}
}