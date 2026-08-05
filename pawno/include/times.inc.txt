//Times by UltraScripter / / //////////////////////////////////////////
//Times by UltraScripter / / //////////////////////////////////////////

#if defined _Times_included
	#endinput
#define _Times_included
#endif

new RaiseTimeTimer;
new PRaiseTimeTimer[MAX_PLAYERS];
new SRTFA;
new PSRTFA[MAX_PLAYERS];
new bool:RTTU;
new bool:PRTTU[MAX_PLAYERS];

#define TIMES_GAME_TIME 0
#define TIMES_REAL_TIME 1
#define TIMES_WEEKS 0
#define TIMES_DAYS 1

/*
native SetDays(days);
native SetHours(hours);
native SetMinutes(minutes);
native SetSeconds(seconds);
native GetDays();
native GetHours();
native GetMinutes();
native GetSeconds();
native StartTime(type, week_type);
native EndTime();
native ResetTime();
native SetPlayerDays(playerid, pdays)
native SetPlayerHours(playerid, phours)
native SetPlayerMinutes(playerid, pminutes)
native SetPlayerSeconds(playerid, pseconds)
native GetPlayerDays(playerid);
native GetPlayerHours(playerid);
native GetPlayerMinutes(playerid);
native GetPlayerSeconds(playerid);
native StartPlayerTime(playerid, ptype, pweek_type);
native EndPlayerTime(playerid)
native ResetPlayerTime(playerid);
native SetRealTime();
native SetPlayerRealTime(playerid);
*/

enum Times_HMS
{
	Days,
	Hours,
	Minutes,
	Seconds
}
new Times_SMH[Times_HMS];

enum Times_PHMS
{
	PDays,
	PHours,
	PMinutes,
	PSeconds
}
new Times_PSMH[MAX_PLAYERS][Times_PHMS];

stock SetDays(days)
{
	return Times_SMH[Days] = days;
}

stock SetHours(hours)
{
	return Times_SMH[Hours] = hours;
}

stock SetMinutes(minutes)
{
	return Times_SMH[Minutes] = minutes;
}

stock SetSeconds(seconds)
{
	return Times_SMH[Seconds] = seconds;
}

stock GetDays()
{
	return Times_SMH[Days];
}

stock GetHours()
{
	return Times_SMH[Hours];
}

stock GetMinutes()
{
	return Times_SMH[Minutes];
}

stock GetSeconds()
{
	return Times_SMH[Seconds];
}

stock StartTime(type, week)
{
	if(RTTU == true) return 0;
	if(RTTU == false)
	{
	   if(week == TIMES_WEEKS)
	   {
	      if(type == TIMES_GAME_TIME)
	      {
			 RTTU = true;
             RaiseTimeTimer = SetTimer("RaiseTimeS", 100, true);
 	      }
	      if(type == TIMES_REAL_TIME)
 	      {
             RTTU = true;
	         RaiseTimeTimer = SetTimer("RaiseTimeS", 1000, true);
	      }
	   }
	   if(week == TIMES_DAYS)
	   {
          if(type == TIMES_GAME_TIME)
	      {
             RTTU = true;
             RaiseTimeTimer = SetTimer("RaiseTime", 100, true);
 	      }
	      if(type == TIMES_REAL_TIME)
 	      {
             RTTU = true;
	         RaiseTimeTimer = SetTimer("RaiseTime", 1000, true);
	      }
	   }
	}
	return 1;
}

stock EndTime()
{
	RTTU = false;
	KillTimer(SRTFA);
	KillTimer(RaiseTimeTimer);
	return 1;
}

forward RaiseTime();

public RaiseTime()
{
	Times_SMH[Seconds] ++;
	if(Times_SMH[Seconds] == 60)
	{
       Times_SMH[Seconds] = 0;
	   Times_SMH[Minutes] ++;
	}
	if(Times_SMH[Minutes] == 60)
	{
       Times_SMH[Minutes] = 0;
	   Times_SMH[Hours] ++;
	}
	if(Times_SMH[Hours] == 24)
	{
	   Times_SMH[Hours] = 0;
	   Times_SMH[Days] ++;
	}
	return 1;
}

stock SetRealTime()
{
	SRTFA = SetTimer("SetRealTimeForAll", 1000, true);
	return 1;
}

stock SetPlayerRealTime(playerid)
{
	PSRTFA[playerid] = SetTimerEx("PSetRealTimeForAll", 1000, true, "i", playerid);
	return 1;
}

forward SetRealTimeForAll();

public SetRealTimeForAll()
{
	for(new i = 0; i < GetMaxPlayers(); i ++)
	{
	   SetPlayerTime(i, Times_SMH[Hours], Times_SMH[Minutes]);
	}
	return 1;
}

forward PSetRealTimeForAll(playerid);

public PSetRealTimeForAll(playerid)
{
    SetPlayerTime(playerid, Times_PSMH[playerid][PHours], Times_PSMH[playerid][PMinutes]);
	return 1;
}

forward RaiseTimeS();

public RaiseTimeS()
{
    Times_SMH[Seconds] ++;
	if(Times_SMH[Seconds] == 60)
	{
       Times_SMH[Seconds] = 0;
	   Times_SMH[Minutes] ++;
	}
	if(Times_SMH[Minutes] == 60)
	{
       Times_SMH[Minutes] = 0;
	   Times_SMH[Hours] ++;
	}
	if(Times_SMH[Hours] == 24)
	{
	   Times_SMH[Hours] = 0;
	   Times_SMH[Days] ++;
	}
	if(Times_SMH[Days] == 8)
	{
	   Times_SMH[Days] = 1;
	}
	return 1;
}

stock ResetTime()
{
    Times_SMH[Days] = 0;
    Times_SMH[Hours] = 0;
    Times_SMH[Minutes] = 0;
    Times_SMH[Seconds] = 0;
	return 1;
}

stock SetPlayerDays(playerid, pdays)
{
	return Times_PSMH[playerid][PDays] = pdays;
}

stock SetPlayerHours(playerid, phours)
{
	return Times_PSMH[playerid][PHours] = phours;
}

stock SetPlayerMinutes(playerid, pminutes)
{
	return Times_PSMH[playerid][PMinutes] = pminutes;
}

stock SetPlayerSeconds(playerid, pseconds)
{
	return Times_PSMH[playerid][PSeconds] = pseconds;
}

stock GetPlayerDays(playerid)
{
	return Times_PSMH[playerid][PDays];
}

stock GetPlayerHours(playerid)
{
	return Times_PSMH[playerid][PHours];
}

stock GetPlayerMinutes(playerid)
{
	return Times_PSMH[playerid][PMinutes];
}

stock GetPlayerSeconds(playerid)
{
	return Times_PSMH[playerid][PSeconds];
}

stock StartPlayerTime(playerid, ptype, pweek)
{
	if(PRTTU[playerid] == true) return 0;
	if(PRTTU[playerid] == false)
	{
	   if(pweek == TIMES_WEEKS)
	   {
	      if(ptype == TIMES_GAME_TIME)
	      {
             PRTTU[playerid] = true;
             PRaiseTimeTimer[playerid] = SetTimerEx("PRaiseTimeS", 100, true, "i", playerid);
 	      }
	      if(ptype == TIMES_REAL_TIME)
 	      {
             PRTTU[playerid] = true;
	         PRaiseTimeTimer[playerid] = SetTimerEx("PRaiseTimeS", 1000, true, "i", playerid);
	      }
	   }
	   if(pweek == TIMES_DAYS)
 	   {
          if(ptype == TIMES_GAME_TIME)
	      {
             PRTTU[playerid] = true;
             PRaiseTimeTimer[playerid] = SetTimerEx("PRaiseTime", 100, true, "i", playerid);
 	      }
	      if(ptype == TIMES_REAL_TIME)
 	      {
             PRTTU[playerid] = true;
	         PRaiseTimeTimer[playerid] = SetTimerEx("PRaiseTime", 1000, true, "i", playerid);
	      }
	   }
	}
	return 1;
}

stock EndPlayerTime(playerid)
{
	PRTTU[playerid] = false;
	KillTimer(PSRTFA[playerid]);
	KillTimer(PRaiseTimeTimer[playerid]);
	return 1;
}

forward PRaiseTime(playerid);

public PRaiseTime(playerid)
{
	Times_PSMH[playerid][PSeconds] ++;
	if(Times_PSMH[playerid][PSeconds] == 60)
	{
       Times_PSMH[playerid][PSeconds] = 0;
	   Times_PSMH[playerid][PMinutes] ++;
	}
	if(Times_PSMH[playerid][PMinutes] == 60)
	{
       Times_PSMH[playerid][PMinutes] = 0;
	   Times_PSMH[playerid][PHours] ++;
	}
	if(Times_PSMH[playerid][PHours] == 24)
	{
	   Times_PSMH[playerid][PHours] = 0;
	   Times_PSMH[playerid][PDays] ++;
	}
	return 1;
}

forward PRaiseTimeS(playerid);

public PRaiseTimeS(playerid)
{
    Times_PSMH[playerid][PSeconds] ++;
	if(Times_PSMH[playerid][PSeconds] == 60)
	{
       Times_PSMH[playerid][PSeconds] = 0;
	   Times_PSMH[playerid][PMinutes] ++;
	}
	if(Times_PSMH[playerid][PMinutes] == 60)
	{
       Times_PSMH[playerid][PMinutes] = 0;
	   Times_PSMH[playerid][PHours] ++;
	}
	if(Times_PSMH[playerid][PHours] == 24)
	{
	   Times_PSMH[playerid][PHours] = 0;
	   Times_PSMH[playerid][PDays] ++;
	}
	if(Times_PSMH[playerid][PDays] == 8)
	{
	   Times_PSMH[playerid][PDays] = 1;
	}
	return 1;
}

stock ResetPlayerTime(playerid)
{
    Times_PSMH[playerid][PDays] = 0;
    Times_PSMH[playerid][PHours] = 0;
    Times_PSMH[playerid][PMinutes] = 0;
    Times_PSMH[playerid][PSeconds] = 0;
	return 1;
}

//Times by UltraScripter / / //////////////////////////////////////////
//Times by UltraScripter / / //////////////////////////////////////////