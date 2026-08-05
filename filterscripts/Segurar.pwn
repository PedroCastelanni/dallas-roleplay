#include <a_samp>
#include <sscanf2>
#include <zcmd>
#include <streamer>

new Levendo[MAX_PLAYERS];
new Levendo2[MAX_PLAYERS];

public OnPlayerConnect(playerid)
{
    Levendo[playerid] = -1;
    Levendo2[playerid] = -1;
    return 1;
}

public OnPlayerDisconnect(playerid, reason)
{
    if(Levendo2[playerid] >= 0)
    {
        TogglePlayerControllable(Levendo2[playerid], 1);
        TogglePlayerControllable(Levendo[playerid], 1);
        Levendo[Levendo2[playerid]] = -1;
        Levendo2[Levendo2[playerid]] = -1;
        Levendo[Levendo[playerid]] = -1;
        Levendo2[Levendo[playerid]] = -1;
    }
    if(Levendo[playerid] >= 0)
    {
        TogglePlayerControllable(Levendo2[playerid], 1);
        TogglePlayerControllable(Levendo[playerid], 1);
        Levendo[Levendo2[playerid]] = -1;
        Levendo2[Levendo2[playerid]] = -1;
        Levendo[Levendo[playerid]] = -1;
        Levendo2[Levendo[playerid]] = -1;
    }
    Levendo[playerid] = -1;
    Levendo2[playerid] = -1;
    return 1;
}

CMD:soltar(playerid, params[])
{
    if(Levendo2[playerid] < 0)
    {
        
        return 1;
    }
    TogglePlayerControllable(playerid, 1);
    TogglePlayerControllable(Levendo2[playerid], 1);
    TogglePlayerControllable(Levendo[playerid], 1);
    Levendo[Levendo2[playerid]] = -1;
    Levendo[playerid] = -1;
    Levendo2[playerid] = -1;
    return 1;
}

CMD:segurar(playerid, params[])
{
    new id;
    if(sscanf(params, "d", id))return SendClientMessage(playerid, -1, "[BPS] Use: /segurar [ID]");
    if(!IsPlayerConnected(id)) return SendClientMessage(playerid, -1, "Jogador invalido");
    if(id == playerid) { SendClientMessage(playerid, -1, "Nao pode segurar si mesmo!"); return 1; }
    new Float:PosP[3]; GetPlayerPos(playerid,PosP[0],PosP[1],PosP[2]);
	if(!IsPlayerInRangeOfPoint(id,2.0, PosP[0],PosP[1],PosP[2])) return SendClientMessage(playerid, 0xFF0000FF, "Este jogador nao esta proximo de voce!");
    if(Levendo2[playerid] >= 0)
    {
        SendClientMessage(playerid, -1, "[BPS] {00FFFF}Ja esta segurando algem");
        return 1;
    }
    if(Levendo[playerid] >= 0)
    {
        SendClientMessage(playerid, -1, "[BPS] {00FFFF}Voce ja esta sendo segurado");
        return 1;
    }
    if(Levendo[id] >= 0)
    {
        SendClientMessage(playerid, -1, "[BPS] {00FFFF}Ja esta srguro");
        return 1;
    }
    Levendo[id] = playerid;
    Levendo2[playerid] = id;
    TogglePlayerControllable(id, 0);
    SetTimerEx("EstaSeguro", 300, false, "i", id);
    return 1;
}

forward EstaSeguro(playerid);
public EstaSeguro(playerid)
{
    new Float:plocx,Float:plocy,Float:plocz;
    if(IsPlayerConnected(Levendo[playerid]))
    {
        if(IsPlayerInAnyVehicle(Levendo[playerid]))
        {
            TogglePlayerControllable(playerid, 1);
            Levendo2[Levendo[playerid]] = -1;
            Levendo[playerid] = -1;
            return 1;
        }
        TogglePlayerControllable(playerid, 0);
        GetPlayerPos(Levendo[playerid], plocx, plocy, plocz);
        SetPlayerPos(playerid,plocx,plocy+0.5, plocz);
        SetPlayerInterior(playerid, GetPlayerInterior(Levendo[playerid]));
        SetPlayerVirtualWorld(playerid, GetPlayerVirtualWorld(Levendo[playerid]));
        SetTimerEx("EstaSeguro", 300, false, "i", playerid);
        return 1;
    }
    TogglePlayerControllable(playerid, 1);
    Levendo2[Levendo[playerid]] = -1;
    Levendo[playerid] = -1;
    return 1;
}



