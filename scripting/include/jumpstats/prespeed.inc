enum _:TYPE_PRE {
	pre_dd = 0,
	pre_bhop = 1,
	pre_sbj = 2,
	pre_bid = 3,
	pre_duckbhop = 4,
	pre_ladder = 5,
	pre_jump = 6,
	pre_duck = 7,
	pre_fall = 8
};

new g_szPreType[][] = {
	"dd",
	"bhop",
	"sbj",
	"bid",
	"duckbhop",
	"ladder",
	"jump",
	"duck",
	"fall"
};


enum STRUCT_PRE {
	sp_iType,
	sp_iFog,
	Float:sp_flPre,
	Float:sp_flPost
};

new g_ePreData[MAX_PLAYERS + 1][STRUCT_PRE];
new bool:g_isPre[MAX_PLAYERS + 1];

new bool:g_isJof[MAX_PLAYERS + 1];
new Float:g_flJof[MAX_PLAYERS + 1];

new Float:g_flHudTime[MAX_PLAYERS + 1];

public show_prespeed(id) {
	new Float:g_flGameTime = get_gametime();
	
	if((g_flHudTime[id] + 0.05) > g_flGameTime)
		return;

	static bool:isPre;
	static Float:flPreTime;

	if (!isPre) {
		isPre = g_isPre[id];
		if (isPre) {
			flPreTime = g_flGameTime + 1.0;
			g_isPre[id] = false;
		}
	} else {
		if (g_isPre[id]) {
			flPreTime = g_flGameTime + 1.0;
			g_isPre[id] = false;
		}
	}

	if(flPreTime < g_flGameTime) {
		isPre = false;
	}

	set_hudmessage(g_pCvar[c_iPreSpeedR], g_pCvar[c_iPreSpeedG], g_pCvar[c_iPreSpeedB], g_pCvar[c_iPreSpeedX], g_pCvar[c_iPreSpeedY], 0, 1.0, 0.15, 0.0, 0.0, g_pCvar[c_iPreSpeed]);

	new szJof[8];

	if (g_isJof[id])
		formatex(szJof, charsmax(szJof), "jof: %.0f", g_flJof[id])

	if (g_eOnOff[id][of_bPre] && isPre) {
		new iType = g_ePreData[id][sp_iType];
		switch (iType) {
			case 0..4: {
				ShowSyncHudMsg(id, g_hudPreSpeed, "%s^n%.0f u/s^n%s %d fog:^n%.2f^n%.2f", g_isJof[id] ? szJof : "", g_flHorSpeed[id], g_szPreType[iType], g_ePreData[id][sp_iFog], g_ePreData[id][sp_flPre], g_ePreData[id][sp_flPost]);
			}
			default: {
				ShowSyncHudMsg(id, g_hudPreSpeed, "%s^n%.0f u/s^n%s pre:^n%.2f", g_isJof[id] ? szJof : "", g_flHorSpeed[id], g_szPreType[iType], g_ePreData[id][sp_flPre]);
			}
		}
	} else { 
		ShowSyncHudMsg(id, g_hudPreSpeed, "%s^n%.0f u/s", g_isJof[id] ? szJof : "", g_flHorSpeed[id]);
	}

	g_flHudTime[id] = g_flGameTime;
}

stock show_pre(id, iType, Float:flPreSpeed, iFog = 0, Float:flPostSpeed = 0.0) {
	g_isPre[id] = true;

	g_ePreData[id][sp_iType] = iType;
	g_ePreData[id][sp_iFog] = iFog;
	g_ePreData[id][sp_flPre] = flPreSpeed;
	g_ePreData[id][sp_flPost] = flPostSpeed;
}

stock show_jof(id, Float:flJof) {
	g_isJof[id] = true;
	g_flJof[id] = flJof;
	set_task(1.0, "reset_jofhud", id)
}

public reset_jofhud(id) {
	g_isJof[id] = false;
}