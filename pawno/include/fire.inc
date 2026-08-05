
/*
native DS_CreateFire(Float:x, Float:y, Float:z, Float:health);
native DS_DestroyFire(fireid);
native DS_DestroyAllFire();
native IsValidFire(fireid);
native IsPlayerInFire(playerid, fireid);
native IsPlayerInAnyFire(playerid);
native GetPlayerFire(playerid);
native CountFires();
*/
//Callbacks
forward OnFireDeathPlayer(fireid, playerid);
forward OnFireDamagePlayer(fireid, damagedid);
forward ApagarFogo(playerid);
forward f_OnPlayerUpdate(playerid);
forward UpdateFire();
forward f_init();





#define Holding(%0) \
	((newkeys & (%0)) == (%0))
#define MAX_FIRES 			350
#define INVALID_FIRE        0xFFFF



public f_init()
{
    DeleteAllFire();
	SetTimer("UpdateFire", 700, 1);
}	


enum f_info
{
	fireo,
 	Float:firex,
 	Float:firey,
 	Float:firez,
 	Float:fireh,
 	Text3D:firet,
 	bool:firec,
};
new fireinfo[MAX_FIRES][f_info];

stock AddFire(Float:x, Float:y, Float:z)
{
	for(new i; i < MAX_FIRES; i++)
	{
	    if(!fireinfo[i][firec])
	    {
	        fireinfo[i][firet] = CreateDynamic3DTextLabel("100/100", 0xAAAAAA80, x, y, z, 10.0);
	        
			fireinfo[i][fireo] = CreateDynamicObject(18690, x, y, z-2.0, 0.0, 0.0, 0.0);
			fireinfo[i][firex] = x;
			fireinfo[i][firey] = y;
			fireinfo[i][firez] = z;
			fireinfo[i][fireh] = 100.0;
			fireinfo[i][firec] = true;
			return i;
		}
	}
	return INVALID_FIRE;
}
stock DeleteFire(fireid)
{
	if(IsValidFire(fireid))
	{
		DestroyDynamicObject(fireinfo[fireid][fireo]);
		DestroyDynamic3DTextLabel(fireinfo[fireid][firet]);
		fireinfo[fireid][firec] = false;
	}
}
stock DeleteAllFire()
{
	for(new i; i < MAX_FIRES; i++)
	{
		DeleteFire(i);
	}
}
stock IsValidFire(fireid)
{
	if(fireinfo[fireid][firec])
	    return 1;
	return 0;
}

stock GetPlayerFire(playerid)
{
	for(new i; i < MAX_FIRES; i++)
	{
	    if(IsValidFire(i) && IsPlayerInRangeOfPoint(playerid, 1.5, fireinfo[i][firex],  fireinfo[i][firey],  fireinfo[i][firez]))
	    {
	        return i;
		}
	}
	return 0;
}


stock CountFires()
{
	new fires=0;
	for(new i; i < MAX_FIRES; i++)
	{
	    if(IsValidFire(i))
	    {
	        fires++;
		}
	}
	return fires;
}

stock IsPlayerInFire(playerid, fireid)
{
	if(GetPlayerFire(playerid) == fireid)
	    return true;
	return false;
}
stock IsPlayerInAnyFire(playerid)
{
	return GetPlayerFire(playerid);
}


public f_OnPlayerUpdate(playerid)
{
	new newkeys,l,u;
	GetPlayerKeys(playerid, newkeys, l, u);
	if(Holding(KEY_FIRE))
	{
        if(GetPlayerWeapon(playerid) == 42 && GetPlayerState(playerid) == PLAYER_STATE_ONFOOT){
        
            for(new i; i < MAX_FIRES; i++){
            
 	        	if(IsValidFire(i)){
 	        	
 	        	    if(PlayerFaces(playerid, fireinfo[i][firex],  fireinfo[i][firey],  fireinfo[i][firez], 1.0) &&
 					IsPlayerInRangeOfPoint(playerid, 5, fireinfo[i][firex],  fireinfo[i][firey],  fireinfo[i][firez]))
 	        		{
 	        		    fireinfo[i][fireh] -= 7.0;

						new stringf[45];

						format(stringf, 44, "%.0f/100", fireinfo[i][fireh]);
			    		UpdateDynamic3DTextLabelText(fireinfo[i][firet], 0xAAAAAA80, stringf);

						if(fireinfo[i][fireh] < 0.1){
							DeleteFire(i);
							CallRemoteFunction("ApagarFogo", "d", playerid);
						}
					}
				}
			}
		}
        else if(GetPlayerState(playerid) == PLAYER_STATE_DRIVER && GetVehicleModel(GetPlayerVehicleID(playerid)) == 407)
        {
            for(new i; i < MAX_FIRES; i++){
            
 	        	if(IsValidFire(i)){
 	        	
 	        	    if(PlayerFaces(playerid, fireinfo[i][firex],  fireinfo[i][firey],  fireinfo[i][firez], 3.0) &&
 					IsPlayerInRangeOfPoint(playerid, 20, fireinfo[i][firex],  fireinfo[i][firey],  fireinfo[i][firez])){
 					
 	        		    fireinfo[i][fireh] -= 10.0;

						new stringf[45];

						format(stringf, 44, "%.0f/100", fireinfo[i][fireh]);
			    		UpdateDynamic3DTextLabelText(fireinfo[i][firet], 0xAAAAAA80, stringf);


						if(fireinfo[i][fireh] < 0.1){
							DeleteFire(i);
							CallRemoteFunction("ApagarFogo", "d", playerid);
						}
					}
				}
			}
		}
	}
	return 1;
}


Float:DistanceCameraTargetToLocation(Float:CamX, Float:CamY, Float:CamZ,   Float:ObjX, Float:ObjY, Float:ObjZ,   Float:FrX, Float:FrY, Float:FrZ) {

	new Float:TGTDistance;
	TGTDistance = floatsqroot((CamX - ObjX) * (CamX - ObjX) + (CamY - ObjY) * (CamY - ObjY) + (CamZ - ObjZ) * (CamZ - ObjZ));
	new Float:tmpX, Float:tmpY, Float:tmpZ;
	tmpX = FrX * TGTDistance + CamX;
	tmpY = FrY * TGTDistance + CamY;
	tmpZ = FrZ * TGTDistance + CamZ;
	return floatsqroot((tmpX - ObjX) * (tmpX - ObjX) + (tmpY - ObjY) * (tmpY - ObjY) + (tmpZ - ObjZ) * (tmpZ - ObjZ));
}

stock PlayerFaces(playerid, Float:x, Float:y, Float:z, Float:radius)
{
        new Float:cx,Float:cy,Float:cz,Float:fx,Float:fy,Float:fz;
        GetPlayerCameraPos(playerid, cx, cy, cz);
        GetPlayerCameraFrontVector(playerid, fx, fy, fz);
        return (radius >= DistanceCameraTargetToLocation(cx, cy, cz, x, y, z, fx, fy, fz));
}

public UpdateFire()
{
    foreach(Player, i)f_OnPlayerUpdate(i);
	for(new i = 0; i<MAX_FIRES; i++)
	{
		if(IsValidFire(i))
		{
			foreach(Player, p)
			{
				if(IsPlayerInRangeOfPoint(p, 1.8, fireinfo[i][firex], fireinfo[i][firey], fireinfo[i][firez]))
				{
	  				new Float:HP;
		    		GetPlayerHealth(p, HP);
	  				SetPlayerHealth(p, HP-20);
	  				CallRemoteFunction("OnFireDamagePlayer", "dd", i, p);
	  				
	  				GetPlayerHealth(p, HP);
	  				if(HP < 0.1)
	  				{
	  				    CallRemoteFunction("OnFireDeathPlayer", "dd", i, p);
	  				}
	  				
					SetPlayerDrunkLevel(p, 8000);
					SetTimerEx("NormalDrunk", 400, false, "i", p);
					SetPVarInt(p, "DrunkL", 1);
	  				
				}	
			}
			
  			fireinfo[i][fireh] -= 0.1;
		    if(fireinfo[i][fireh] < 0.2)
		    {
		        DeleteFire(i);
		        CallRemoteFunction("ApagarFogo", "d", -1);
			}
 	        new stringf[45];
 	   	    format(stringf, 44, "%.0f/100", fireinfo[i][fireh]);
			UpdateDynamic3DTextLabelText(fireinfo[i][firet], 0xAAAAAA80, stringf);
		}
	}
	return 0;
}
