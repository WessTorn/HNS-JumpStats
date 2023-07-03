enum JS_ONOFF {
	bool:of_bStrafe,
	bool:of_bStats,
	bool:of_bJof,
	bool:of_bSpeed,
	bool:of_bPre,
	bool:of_bChatInfo
};

new g_eOnOff[MAX_PLAYERS + 1][JS_ONOFF];

public init_cmds() {
	RegisterSayCmd("ljsmenu", "ljs",		"ljsmenu",			ADMIN_ALL, "Jump stats menu");
	RegisterSayCmd("strafe", "strafestats",	"cmdShowStrafe",	ADMIN_ALL, "Show strafe stats");
	RegisterSayCmd("stats", "ljstats",		"cmdShowStats",		ADMIN_ALL, "Show main stats");
	RegisterSayCmd("chatinfo", "ci",		"cmdShowChatInfo",	ADMIN_ALL, "Show jump chat onfo");
	RegisterSayCmd("showspeed", "speed",	"cmdShowSpeed",		ADMIN_ALL, "Show speed");
	RegisterSayCmd("showpre", "pre",		"cmdShowPre",		ADMIN_ALL, "Show pre");
	RegisterSayCmd("jumpoff", "jof",		"cmdShowJof",		ADMIN_ALL, "Show jof");
}

public cmdShowChatInfo(id) {
	if (!g_eOnOff[id][of_bChatInfo]) {
		g_eOnOff[id][of_bChatInfo] = true;
		client_print_color(id, print_team_blue, ">> Chatinfo: ^3enabled");
	} else {
		g_eOnOff[id][of_bChatInfo] = false;
		client_print_color(id, print_team_red, ">> Chatinfo: ^3disabled");
	}
	return PLUGIN_HANDLED;
}

public cmdShowStats(id) {
	if (g_eOnOff[id][of_bStats] == true) {
		client_print_color(id, print_team_red, ">> Stats: ^3disabled");
		g_eOnOff[id][of_bStats] = false;
		g_eOnOff[id][of_bChatInfo] = false;
	} else {
		if (g_eOnOff[id][of_bStrafe] == false) {
			g_eOnOff[id][of_bStrafe] = true;
		}
		g_eOnOff[id][of_bStats] = true;
		g_eOnOff[id][of_bChatInfo] = true;
		client_print_color(id, print_team_blue, ">> Stats: ^3enabled");
	}
	return PLUGIN_HANDLED;
}

public cmdShowStrafe(id) {
	if (g_eOnOff[id][of_bStrafe] == true) {
		client_print_color(id, print_team_red, ">> Strafe stats: ^3disabled");
		g_eOnOff[id][of_bStrafe] = false;
	} else {
		g_eOnOff[id][of_bStrafe] = true;
		client_print_color(id, print_team_blue, ">> Strafe stats: ^3enabled");
	}
	return PLUGIN_HANDLED;
}

public cmdShowJof(id) {
	if (g_eOnOff[id][of_bJof] == false) {
		client_print_color(id, print_team_blue, ">> JumpOff: ^3enabled");
		g_eOnOff[id][of_bJof] = true;
	} else {
		g_eOnOff[id][of_bJof] = false;
		client_print_color(id, print_team_red, ">> JumpOff: ^3disabled");
	}
	return PLUGIN_HANDLED;
}

public cmdShowSpeed(id) {
	if (g_eOnOff[id][of_bSpeed] == false) {
		client_print_color(id, print_team_blue, ">> Speed: ^3enabled");
		g_eOnOff[id][of_bSpeed] = true;
	} else {
		g_eOnOff[id][of_bSpeed] = false;
		client_print_color(id, print_team_red, ">> Speed: ^3disabled");
	}
	return PLUGIN_HANDLED;
}

public cmdShowPre(id) {
	if (g_eOnOff[id][of_bPre] == false) {
		client_print_color(id, print_team_blue, ">> Pre: ^3enabled");
		g_eOnOff[id][of_bPre] = true;
	} else {
		g_eOnOff[id][of_bPre] = false;
		client_print_color(id, print_team_red, ">> Pre: ^3disabled");
	}
	return PLUGIN_HANDLED;
}

stock RegisterSayCmd(const szCmd[], const szShort[], const szFunc[], flags = -1, szInfo[] = "") {
	new szTemp[65], szInfoLang[65];
	format(szInfoLang, 64, "%L", LANG_SERVER, szInfo);

	format(szTemp, 64, "say /%s", szCmd);
	register_clcmd(szTemp, szFunc, flags, szInfoLang);

	format(szTemp, 64, "say .%s", szCmd);
	register_clcmd(szTemp, szFunc, flags, szInfoLang);

	format(szTemp, 64, "/%s", szCmd);
	register_clcmd(szTemp, szFunc, flags, szInfoLang);

	format(szTemp, 64, "%s", szCmd);
	register_clcmd(szTemp, szFunc, flags, szInfoLang);

	format(szTemp, 64, "say /%s", szShort);
	register_clcmd(szTemp, szFunc, flags, szInfoLang);

	format(szTemp, 64, "say .%s", szShort);
	register_clcmd(szTemp, szFunc, flags, szInfoLang);

	format(szTemp, 64, "/%s", szShort);
	register_clcmd(szTemp, szFunc, flags, szInfoLang);

	format(szTemp, 64, "%s", szShort);
	register_clcmd(szTemp, szFunc, flags, szInfoLang);

	return 1;
}