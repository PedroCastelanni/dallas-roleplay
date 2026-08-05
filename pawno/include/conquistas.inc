
	/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * [url]http://www.mozilla.org/MPL/[/url]
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is the Lawedan Computers, Inc Achievements System.
 *
 * The Initial Developer of the Original Code is
 * Magnus Burton (Flyfishes), Zack Moshonas (Hal) and Matthew Striker (Deskoft).
 * Portions created by the Initial Developer are Copyright (C) 2011
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Adam Booth
 * Jamie Morrison
 * Robin Strumberg
 * Loyd Rafolds
 * All the Lawedan Team!
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#if defined _achievements_included
	#endinput
#endif
#define _achievements_included

#include <a_samp>


/*
native ShowPlayerAchievement(playerid, short[], title[], text[], fadeout);
native SetAchievementColor(box, color);
*/

new Text:A_Textdraws[4];

public OnGameModeInit()
{
	A_Textdraws[0] = TextDrawCreate(190.000000, 155.000000, "                                                                            ");
	TextDrawBackgroundColor(A_Textdraws[0], 255);
	TextDrawFont(A_Textdraws[0], 1);
	TextDrawLetterSize(A_Textdraws[0], 0.500000, 2.399999);
	TextDrawColor(A_Textdraws[0], -1);
	TextDrawSetOutline(A_Textdraws[0], 0);
	TextDrawSetProportional(A_Textdraws[0], 1);
	TextDrawSetShadow(A_Textdraws[0], 1);
	TextDrawUseBox(A_Textdraws[0], 1);
	TextDrawBoxColor(A_Textdraws[0], 7039829);
	TextDrawTextSize(A_Textdraws[0], 448.000000, 113.000000);

	A_Textdraws[1] = TextDrawCreate(172.000000, 147.000000, "NEW ACHIEVEMENT");
	TextDrawBackgroundColor(A_Textdraws[1], 255);
	TextDrawFont(A_Textdraws[1], 1);
	TextDrawLetterSize(A_Textdraws[1], 0.390000, 1.300003);
	TextDrawColor(A_Textdraws[1], -1);
	TextDrawSetOutline(A_Textdraws[1], 1);
	TextDrawSetProportional(A_Textdraws[1], 1);
	TextDrawUseBox(A_Textdraws[1], 1);
	TextDrawBoxColor(A_Textdraws[1], 7027575);
	TextDrawTextSize(A_Textdraws[1], 431.000000, 0.000000);

	A_Textdraws[2] = TextDrawCreate(191.000000, 162.000000, "TITLE");
	TextDrawBackgroundColor(A_Textdraws[2], 255);
	TextDrawFont(A_Textdraws[2], 1);
	TextDrawLetterSize(A_Textdraws[2], 0.370000, 0.799999);
	TextDrawColor(A_Textdraws[2], -1);
	TextDrawSetOutline(A_Textdraws[2], 1);
	TextDrawSetProportional(A_Textdraws[2], 1);

	A_Textdraws[3] = TextDrawCreate(192.000000, 169.000000, "description of the achievement");
	TextDrawBackgroundColor(A_Textdraws[3], 255);
	TextDrawFont(A_Textdraws[3], 1);
	TextDrawLetterSize(A_Textdraws[3], 0.300000, 0.799999);
	TextDrawColor(A_Textdraws[3], -1);
	TextDrawSetOutline(A_Textdraws[3], 1);
	TextDrawSetProportional(A_Textdraws[3], 1);
	TextDrawUseBox(A_Textdraws[3], 1);
	TextDrawBoxColor(A_Textdraws[3], 623191296);
	TextDrawTextSize(A_Textdraws[3], 446.000000, 2.000000);
	return CallLocalFunction("achievements_OnGameModeInit", "");
}
#if defined _ALS_OnGameModeInit
    #undef OnGameModeInit
#else
    #define _ALS_OnGameModeInit
#endif
#define OnGameModeInit achievements_OnGameModeInit
forward achievements_OnGameModeInit();

stock ShowPlayerAchievement(playerid, short[], title[], text[], fadeout)
{
	TextDrawSetString(A_Textdraws[2], title);
	TextDrawSetString(A_Textdraws[3], text);
	
	printf("ACHIEVEMENTS: Gave playerid %d the achievement %s", playerid, short);

	for(new i = 0; i < 4; i++) TextDrawShowForPlayer(playerid, A_Textdraws[i]);
    
	SetTimerEx("A_FadeOut", fadeout, false, "i", playerid);
	return 1;
}

forward A_FadeOut(playerid);
public A_FadeOut(playerid)
{
	for(new i = 0; i < 4; i++) TextDrawHideForPlayer(playerid, A_Textdraws[i]);
	return 1;
}

/*
box types;
	0 - Description box
	1 - NEW ACHIEVEMENT box
*/

stock SetAchievementColor(box, color)
{
	TextDrawBoxColor(A_TextDraws[box], color);
	return 1;
}
