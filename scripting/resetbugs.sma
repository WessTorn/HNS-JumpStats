#include <amxmodx>

new g_hResetBugForward;

new g_szBadCmds[][] = {
	"tele", "tp", 
	"gocheck", "gc", 
	"stuck", "unstuck", 
	"start", "reset", 
	"restart", "spawn", "respawn"
};

public plugin_init() {
	register_plugin("Reset jump bugs", "1.0", "WessTorn");

	g_hResetBugForward = CreateMultiForward("fwResetBug", ET_IGNORE, FP_CELL);
}

public client_command(id) {
	if(is_user_alive(id)) {
		new szCommand[32];
		read_argv(0, szCommand, charsmax(szCommand));

		if(equali(szCommand, "say")) {
			read_args(szCommand, charsmax(szCommand));
			remove_quotes(szCommand);
		}
		
		if(equali(szCommand, "+hook") || equali(szCommand, "-hook")) {
			new iReturn;
			ExecuteForward(g_hResetBugForward, iReturn, id);
		} else if(szCommand[0] == '/' || szCommand[0] == '.') {
			copy(szCommand, charsmax(szCommand), szCommand[1]);
			
			for(new i ; i < sizeof(g_szBadCmds) ; i++) {
				if(equali(szCommand, g_szBadCmds[i])) {
					new iReturn;
					ExecuteForward(g_hResetBugForward, iReturn, id);

					break;
				}
			}
		}
	}
}