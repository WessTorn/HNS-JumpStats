#define NSTRAFES 14

enum JumpType {
	jt_LongJump = 0,
	jt_HighJump = 1,
	jt_CountJump = 2,
	jt_Multi_CountJump = 3,
	jt_StandUPCountJump = 4,
	jt_Multi_StandUPCountJump = 5,
	jt_BhopJump = 6,
	jt_StandUpBhopJump = 7,
	jt_DuckBhopJump = 8,
	jt_WeirdJump = 9,
	jt_LadderJump = 10,
	jt_LadderBhopJump = 11,
	jt_RealLadderBhopJump = 12,
	jt_BhopInDuck = 13,
	jt_Bhop = 14,
	jt_Not = 15
};


new JumpType:g_eWhichJump[MAX_PLAYERS + 1];

enum _:JUMPSTATS {
	Float:js_flDistance,
	js_iJumpBlock,
	Float:js_flEndSpeed,
	js_iFrames,
	Float:js_flJof,
	Float:js_flLanding
};

new g_eJumpstats[MAX_PLAYERS + 1][JUMPSTATS];

enum _:buttons_info {
	bi_not = 0, bi_W, bi_A, bi_S, bi_D
};

new g_szStrButtonsName[buttons_info][] = { "err", "W", "A", "S", "D" };
new g_iStrButtonsInfo[MAX_PLAYERS + 1][100];

enum _:StrafeStats {
	Float:st_flSpeed,
	Float:st_flSpeedFail,
	st_flSyncGood,
	st_flSyncBad,
	Float:st_flTime
};

new g_eStrafeStats[MAX_PLAYERS + 1][14][StrafeStats];
new g_iStrafes[MAX_PLAYERS + 1];
new g_iStrOldButtons[MAX_PLAYERS + 1];

enum PLAYER_VER {
	IS_MIDDLE,
	IS_UPPED,
	IS_DROPPED
};

new PLAYER_VER:g_iVerInfo[MAX_PLAYERS + 1];
new Float:g_flDuckFirstZ[MAX_PLAYERS + 1];
new Float:g_flJumpFirstZ[MAX_PLAYERS + 1];

enum DUCK_DATA {
	IS_DUCK_NOT = 0,
	IS_DUCK,
	IS_DD,
	IS_SGS
};

new DUCK_DATA:g_eDuckType[MAX_PLAYERS + 1];

enum STATS_DUCKS {
	Float:DUCK_PRE,
	DUCK_FOG
};

new g_eDuckData[MAX_PLAYERS + 1][100][STATS_DUCKS];
new g_iDucks[MAX_PLAYERS + 1];

enum JUMP_DATA {
	IS_JUMP_NOT = 0,
	IS_JUMP,
	IS_JUMP_INFALL,
	IS_JUMP_INDUCK,
	IS_JUMP_INLADDER,
	IS_BHJ,
	IS_DUCKBHOP,
	IS_SBJ
};

new JUMP_DATA:g_eJumpType[MAX_PLAYERS + 1];

enum STATS_JUMPS {
	Float:JUMP_PRE,
	JUMP_FOG
};

new g_eJumpData[MAX_PLAYERS + 1][100][STATS_JUMPS];
new g_iJumps[MAX_PLAYERS + 1];

enum FAIL_JUMP {
	fj_good = 0,
	fl_fail,
	fj_notshow
}

new FAIL_JUMP:g_eFailJump[MAX_PLAYERS + 1];

new g_iFlags[MAX_PLAYERS + 1];
new g_iOldFlags[MAX_PLAYERS + 1];

new g_iButtons[MAX_PLAYERS + 1];
new g_iPrevButtons[MAX_PLAYERS + 1];
new g_iOldButtons[MAX_PLAYERS + 1];

new g_iMoveType[MAX_PLAYERS + 1];

new Float:g_flOrigin[MAX_PLAYERS + 1][3];
new Float:g_flPrevOrigin[MAX_PLAYERS + 1][3];
new Float:g_flFailedOrigin[MAX_PLAYERS + 1][3];
new Float:g_flJumpOffOrigin[MAX_PLAYERS + 1][3];

new Float:g_flVelocity[MAX_PLAYERS + 1][3];
new Float:g_flPrevVelocity[MAX_PLAYERS + 1][3];
new Float:g_flFailedVelocity[MAX_PLAYERS + 1][3];

new Float:g_flHorSpeed[MAX_PLAYERS + 1];
new Float:g_flPrevHorSpeed[MAX_PLAYERS + 1];
new Float:g_flPreHorSpeed[MAX_PLAYERS + 1];

new Float:g_flMaxSpeed[MAX_PLAYERS + 1]

new bool:g_bInDuck[MAX_PLAYERS + 1];
new bool:g_bPrevInDuck[MAX_PLAYERS + 1];
new bool:g_bFailedInDuck[MAX_PLAYERS + 1];

new bool:g_bLadder[MAX_PLAYERS + 1];
new bool:g_bPrevLadder[MAX_PLAYERS + 1];

new bool:g_isFalling[MAX_PLAYERS + 1];
new bool:g_isBackWards[MAX_PLAYERS + 1];
new bool:g_isTouched[MAX_PLAYERS + 1];
new bool:g_isTrigger[MAX_PLAYERS + 1];

new bool:g_bCheckFrames[MAX_PLAYERS + 1];

new Float:g_flTempSpeed[MAX_PLAYERS + 1];
new Float:g_flJumpOffTime[MAX_PLAYERS + 1];
new Float:g_flLandTime[MAX_PLAYERS + 1];

new g_iDetectHJ[MAX_PLAYERS + 1];

new g_isUserSpec[MAX_PLAYERS + 1];

new g_hudStrafe;
new g_hudStats;
new g_hudPreSpeed;