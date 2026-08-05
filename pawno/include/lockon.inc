//Mobile include by Jekmant

/*
 	ATENÇÃO! Inclui a versão do plugin "Pawn.RakNet" necessária "1.4.1" ou superior!
    Funcão:
    bool:IsPlayerHaveAutoaim(playerid) - verificar jogador autoaiming. return "true" se o autoaming habilitado
*/

#if defined _mobile_included
	#endinput
#endif
#define _mobile_included

#tryinclude <a_samp>
#include <Pawn.RakNet>

#if !defined gpci
    native gpci(playerid, buffer[], size = sizeof(buffer));
#endif

#define MIN_PACKET_SIZE     3

const ID_CUSTOM_SYNC = 221;
const RPC_INIT_MOBILE = 0x10;

enum pMobileInfo
{
    bool:isHaveAutoaim
}

new PlayerMobileInfo[MAX_PLAYERS][pMobileInfo];
public OnPlayerConnect(playerid)
{
    new gpciStr[64];
	gpci(playerid, gpciStr, sizeof gpciStr);

    #if defined mob_OnPlayerConnect
		return mob_OnPlayerConnect(playerid);
	#else
		return 1;
	#endif
}
#if defined _ALS_OnPlayerConnect
	#undef OnPlayerConnect
#else
	#define _ALS_OnPlayerConnect
#endif
#define OnPlayerConnect mob_OnPlayerConnect
#if defined mob_OnPlayerConnect
	forward mob_OnPlayerConnect(playerid);
#endif

public OnPlayerDisconnect(playerid, reason)
{
    PlayerMobileInfo[playerid][isHaveAutoaim] = false;

    #if defined mob_OnPlayerDisconnect
		return mob_OnPlayerDisconnect(playerid, reason);
	#else
		return 1;
	#endif
}
#if defined _ALS_OnPlayerDisconnect
	#undef OnPlayerDisconnect
#else
	#define _ALS_OnPlayerDisconnect
#endif
#define OnPlayerDisconnect mob_OnPlayerDisconnect
#if defined mob_OnPlayerDisconnect
	forward mob_OnPlayerDisconnect(playerid, reason);
#endif

IRawPacket:ID_CUSTOM_SYNC(playerid, BitStream:bs)
{
	new bytes, rpcid;
	BS_GetNumberOfBytesUsed(bs, bytes);
	if(bytes < MIN_PACKET_SIZE) return 0;

	BS_ReadValue(bs,
		PR_IGNORE_BITS, 8,
		PR_UINT8, rpcid);

	if(rpcid == RPC_INIT_MOBILE)
	{
		new autoaim;
		BS_ReadValue(bs, PR_UINT8, autoaim);

		PlayerMobileInfo[playerid][isHaveAutoaim] = bool:autoaim;
	}
	return 1;
}

stock bool:IsPlayerHaveAutoaim(playerid)
{
    return PlayerMobileInfo[playerid][isHaveAutoaim];
}
