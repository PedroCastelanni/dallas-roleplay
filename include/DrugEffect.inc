#include <a_samp>

/*
native DrugColorChange(playerid);
native DrugOnGameModeInit();
native DrugOnGameModeExit(playerid);
*/


new Text: DrugText;
new AllColors[10][1] = {
{0xFFFF00AA},
{0xFF0000AA},
{0x0000FFAA},
{0xFF8C00AA},
{0x228B22AA},
{0xFF4500AA},
{0x8B4513AA},
{0x708090AA},
{0xFF6347AA},
{0xEE82EEAA}
};
new DrugEffectTimer[MAX_PLAYERS];
forward DrugColorChange(playerid);
forward DrugOnGameModeInit();
forward DrugOnGameModeExit(playerid);
forward DrugOnPlayerDisconnect(playerid,reason);
forward DrugOnPlayerCommandText(playerid, cmdtext[]);

public DrugOnGameModeInit()
{
	DrugText = TextDrawCreate(642.000000, 2.000000, "New Textdraw");
	TextDrawBackgroundColor(DrugText, 255);
	TextDrawFont(DrugText, 1);
	TextDrawLetterSize(DrugText, 1.000000, 26.800003);
	TextDrawSetOutline(DrugText, 0);
	TextDrawSetProportional(DrugText, 1);
	TextDrawSetShadow(DrugText, 1);
	TextDrawUseBox(DrugText, 1);
	TextDrawBoxColor(DrugText, 503316610);
	TextDrawTextSize(DrugText, -2.000000, 0.000000);
	return 1;
}
public DrugOnGameModeExit(playerid)
{
	KillTimer(DrugEffectTimer[playerid]);
	TextDrawHideForAll(DrugText);
	TextDrawDestroy(DrugText);
	return 1;
}
public DrugColorChange(playerid)
{
    new randomize=random(sizeof(AllColors));
    TextDrawBoxColor(DrugText, AllColors[randomize][0]);
    TextDrawShowForPlayer(playerid,DrugText);
    return 1;
}
public DrugOnPlayerDisconnect(playerid,reason)
{
	KillTimer(DrugEffectTimer[playerid]);
	return 1;
}
public DrugOnPlayerCommandText(playerid, cmdtext[])
{
	if (strcmp("/startdrugeffect", cmdtext, true) == 0)
	{
		SendClientMessage(playerid,0xFF8C00AA," Drug Effect Activated!");
		SendClientMessage(playerid,0xFF8C00AA," Do /stopdrugeffect to stop it!");
		DrugEffectTimer[playerid] = SetTimerEx("DrugColorChange",100,true,"d",playerid);
		return 1;
	}
	if (strcmp("/stopdrugeffect", cmdtext, true) == 0)
	{
		SendClientMessage(playerid,0xFF8C00AA," Drug Effect Stopped!");
		SendClientMessage(playerid,0xFF8C00AA," Do /startdrugeffect to activate it!");
		KillTimer(DrugEffectTimer[playerid]);
		TextDrawHideForPlayer(playerid,DrugText);
		return 1;
	}			
	return 0;
}









