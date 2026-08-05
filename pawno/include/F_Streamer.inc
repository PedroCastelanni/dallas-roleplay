
                                                                                  ///////////////////////
                                                                                  ////Object Streamer////
                                                                                  //////Created by://////
                                                                                  ////////Fallout////////
                                                                                  /////////////////v1.2//


#include <a_samp>

//Natives/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*

Functions:
----------
native F_CreateObject(modelid, Float:x, Float:y, Float:z, Float:rx, Float:ry, Float:rz, Float:vdist=0.0, bool:InstantUpdate=false);                 //creates an object, returns the objectid or -1 if not successful. (viewdistance and InstantUpdate parameter  are optional)
native F_CreatePlayerObject(playerid, modelid, Float:x, Float:y, Float:z, Float:rx, Float:ry, Float:rz, Float:vdist=0.0, bool:InstantUpdate=false); //creates a player-specific object, returns the objectid or -1 if not successful. (viewdistance and InstantUpdate parameter  are optional)
native F_DestroyObject(objectid);                                                                                                                   //destroys an object.
native F_IsObjectCreated(objectid);                                                                                                                 //returns 1 if the object is created, returns 0 if it's not.
native F_IsObjectCreatedForPlayer(playerid, objectid);                                                                                              //returns 1 if the object is created for a certain player, returns 0 if it's not.
native F_PlayerObjectUpdate(playerid, Float:x, Float:y, Float:z);                                                                                   //update objects for a player in a certain position.
native F_MoveObject(objectid, Float:x, Float:y, Float:z, Float:speed, Float:rx=0.0, Float:ry=0.0, Float:rz=0.0, bool:UpdatePos=true);               //moves an object. (rotation parameters rx ry and rz and UpdatePos are optional) (UpdatePos will update the object's coordinates for streaming at the right position)
native F_StopObject(objectid);                                                                                                                      //stops an object from moving, returns 1 on success and 0 if the object is invalid.
native F_IsObjectMoving(objectid);                                                                                                                  //returns 1 if the object is moving, returns 0 if the object is not moving, returns -1 if the object is invalid.
native F_AttachObjectToPlayer(objectid, playerid, Float:x, Float:y, Float;z, Float:rx, Float:ry, Float:rz, bool:InstantUpdate=false);               //attaches an object to a player.
native F_AttachObjectToVehicle(objectid, vehicleid, Float:x, Float:y, Float:z, Float:rx, Float:ry, Float:rz, bool:InstantUpdate=false);             //attaches an object to a vehicle.
native F_SetObjectPos(objectid, Float:x, Float:y, Float:z, bool:InstantUpdate=false);                                                               //sets the position of an object. (InstantUpdate parameter  is optional)
native F_GetObjectPos(objectid, &Float:x, &Float:y, &Float:z);                                                                                      //gets the position of an object in x, y and z.
native F_SetObjectRot(objectid, Float:rx, Float:ry, Float:rz);                                                                                      //sets the rotation of an object.
native F_GetObjectRot(objectid, &Float:rx, &Float:ry, &Float:rz);                                                                                   //gets the rotation of an object in rx, ry and rz.
native F_CountObjects();                                                                                                                            //returns the amount of created objects.
native F_RefreshObjects(playerid);                                                                                                                  //recreates (refreshes) a certain player's objects. (useful to recreate broken objects like glass, boxes, barrels, ...)
native F_ObjectUpdateForAll();                                                                                                                      //instantly updates the objects for all players.
native SetPlayerPos(playerid, Float:x, Float:y, Float:z, bool:InstantUpdate=true);                                                                  //added extra parameter (optional) to disable instant object update.

Callbacks:
----------
native F_OnObjectMoved(objectid);                                                                                                                   //callback which is called when an object has finished moving.

*/
//Configuration///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define F_MAX_OBJECTS           2000    //montante máximo de objetos do streamer criarão; Por favor, altere esta com a quantidade de objetos que você está usando.
#define UpdateTime              1000    // tempo de atualização em ms (milissegundos). Um valor mais baixo significa a atualização mais rápida.
#define ObjectsToStream         500     //maximum number of objects that will be streamed for one player. (maximum = 1000 objects in 0.3e)
#define StreamRange             500.0   //the player's object view range, doesn't need to be changed because objects already fade at 300 distance.
#define DistanceBeforeUpdate    5.0     //the distance a player must travel before objects get updated. (also matters a lot for CPU usage)
#define DebugMode               0       //change this to 1 if you want to see debug messages in-game. (time taken for update, objects streamed, objects created, objects destroyed, ...)
#pragma dynamic                 30000   //increase this value if you have problems with stach/heap size.

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum OInfo
{
	ModelID,
	ObjectID[MAX_PLAYERS],
	Float:ox, Float:oy, Float:oz,
	Float:orx, Float:ory, Float:orz,
	Float:ovdist,
	PlayerID,
	ObjectAttachedID,
	ObjectAttachedVID,
	Float:aox, Float:aoy, Float:aoz,
	Float:aorx, Float:aory, Float:aorz,
	bool:ObjectMoving,
	bool:ObjectCreated[MAX_PLAYERS],
}

new ObjectInfo[F_MAX_OBJECTS][OInfo];
new bool:ObjectUpdatetRunning;
new bool:CantCreateMore;
new bool:RefreshObjects[MAX_PLAYERS];
new bool:DontUpdate[MAX_PLAYERS];
new bool:ObjectDestroyed;
new Float:OldX[MAX_PLAYERS], Float:OldY[MAX_PLAYERS], Float:OldZ[MAX_PLAYERS];
new ObjectCount;
new ContinueUpdatet[MAX_PLAYERS];
new bool:ContinueUpdatetRunning[MAX_PLAYERS];

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

stock F_CreateObject(modelid, Float:x, Float:y, Float:z, Float:rx, Float:ry, Float:rz, Float:vdist=0.0, bool:InstantUpdate=false, playerid=-1)
{
	if(modelid < 1)
	{
		printf("Invalid modelid for object: F_CreateObject(%i, %f, %f, %f, %f, %f, %f);\a\a\a\n", modelid, x, y, z, rx, ry, rz);
		return -1;
	}

	new objectid;

	if(CantCreateMore == false)
	{
		if(ObjectDestroyed == false) //if no object has been destroyed yet, the objectid's can be assigned in a simple way (a lot faster than searching for a free slot)
		{
		    if(ObjectCount == F_MAX_OBJECTS-1)
		    {
				printf("F_Streamer.inc\n--------------\nERROR: Only the first %i objects could be created - object limit exceeded.\a\a\a\n", F_MAX_OBJECTS);
				CantCreateMore = true;
		    }
		    if(ObjectInfo[ObjectCount][ModelID] == 0)
			{
			    objectid = ObjectCount;
			    ObjectCount ++;
			}
		}
		else //if an object was destroyed already once, this loop will search for a free space in the objectid list (slow)
		{
			for(new i; i<F_MAX_OBJECTS; i++)
			{
			    if(i == F_MAX_OBJECTS-1)
				{
					printf("F_Streamer.inc\n--------------\nERROR: Only the first %i objects could be created - object limit exceeded.\a\a\a\n", F_MAX_OBJECTS);
					CantCreateMore = true;
				}

			    if(ObjectInfo[i][ModelID] == 0)
				{
					objectid = i;
					break;
				}
			}
		}
	}
	else { return -1; }

	ObjectInfo[objectid][ModelID] = modelid;
	ObjectInfo[objectid][ox] = x; ObjectInfo[objectid][oy] = y; ObjectInfo[objectid][oz] = z;
	ObjectInfo[objectid][orx] = rx; ObjectInfo[objectid][ory] = ry; ObjectInfo[objectid][orz] = rz;
	ObjectInfo[objectid][ovdist] = vdist;
	ObjectInfo[objectid][PlayerID] = playerid;
	ObjectInfo[objectid][ObjectAttachedID] = -1;
	ObjectInfo[objectid][ObjectAttachedVID] = -1;

	if(InstantUpdate)
	{
		if(playerid == -1) { F_ObjectUpdateForAll(); }
		else { F_PlayerObjectUpdate(playerid, OldX[playerid], OldY[playerid], OldZ[playerid]); }
	}
	return objectid;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

stock F_CreatePlayerObject(playerid, modelid, Float:x, Float:y, Float:z, Float:rx, Float:ry, Float:rz, Float:vdist=0.0, bool:InstantUpdate=false)
{
	return F_CreateObject(modelid, x, y, z, rx, ry, rz, vdist, InstantUpdate, playerid);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

stock F_IsObjectCreated(objectid)
{
	if(ObjectInfo[objectid][ModelID] == 0) return 0;
	return 1;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

stock F_IsObjectCreatedForPlayer(playerid, objectid)
{
	if(IsPlayerConnected(playerid) && !IsPlayerNPC(playerid) && ObjectInfo[objectid][ObjectCreated][playerid] == true) return 1;
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

stock F_CountObjects()
{
	new count;
	for(new objectid; objectid<F_MAX_OBJECTS; objectid++)
	{
	    if(F_IsObjectCreated(objectid)) count++;
	}
	return count;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

stock F_AttachObjectToPlayer(objectid, playerid, Float:x, Float:y, Float:z, Float:rx, Float:ry, Float:rz, bool:InstantUpdate=false)
{
	if(F_IsObjectCreated(objectid) && IsPlayerConnected(playerid))
	{
		for(new playerid2; playerid2<MAX_PLAYERS; playerid2++)
		{
			if(F_IsObjectCreatedForPlayer(playerid2, objectid))
			{
				AttachPlayerObjectToPlayer(playerid2, ObjectInfo[objectid][ObjectID][playerid2], playerid, x, y, z, rx, ry, rz);
			}
		}

		ObjectInfo[objectid][aox] = x; ObjectInfo[objectid][aoy] = y; ObjectInfo[objectid][aoz] = z;
		ObjectInfo[objectid][aorx] = rx; ObjectInfo[objectid][aory] = ry; ObjectInfo[objectid][aorz] = rz;

		ObjectInfo[objectid][ObjectAttachedVID] = -1;
		ObjectInfo[objectid][ObjectAttachedID] = playerid;

		if(InstantUpdate) { F_ObjectUpdateForAll(); }
		return 1;
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

stock F_AttachObjectToVehicle(objectid, vehicleid, Float:x, Float:y, Float:z, Float:rx, Float:ry, Float:rz, bool:InstantUpdate=false)
{
	if(F_IsObjectCreated(objectid) && GetVehicleModel(vehicleid))
	{
		for(new playerid2; playerid2<MAX_PLAYERS; playerid2++)
		{
			if(F_IsObjectCreatedForPlayer(playerid2, objectid))
			{
				AttachPlayerObjectToVehicle(playerid2, ObjectInfo[objectid][ObjectID][playerid2], vehicleid, x, y, z, rx, ry, rz);
			}
		}

		ObjectInfo[objectid][aox] = x; ObjectInfo[objectid][aoy] = y; ObjectInfo[objectid][aoz] = z;
		ObjectInfo[objectid][aorx] = rx; ObjectInfo[objectid][aory] = ry; ObjectInfo[objectid][aorz] = rz;

		ObjectInfo[objectid][ObjectAttachedID] = -1;
		ObjectInfo[objectid][ObjectAttachedVID] = vehicleid;

		if(InstantUpdate) { F_ObjectUpdateForAll(); }
		return 1;
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

stock F_DestroyObject(objectid)
{
	if(F_IsObjectCreated(objectid))
	{
		for(new playerid; playerid<MAX_PLAYERS; playerid++)
		{
			if(F_IsObjectCreatedForPlayer(playerid, objectid))
			{
				DestroyPlayerObject(playerid, ObjectInfo[objectid][ObjectID][playerid]);
				ObjectInfo[objectid][ObjectCreated][playerid] = false;
			}
		}
		ObjectInfo[objectid][ModelID] = 0;

		if(ObjectDestroyed == false) ObjectDestroyed = true;
		return 1;
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

stock F_MoveObject(objectid, Float:x, Float:y, Float:z, Float:speed, Float:rx=999999999.99, Float:ry=999999999.99, Float:rz=999999999.99, bool:UpdatePos=true)
{
	if(F_IsObjectCreated(objectid))
	{
	    F_StopObject(objectid); //fixes problems with moving objects which were already moving

		if(rx == 999999999.99) rx = ObjectInfo[objectid][orx];
		if(ry == 999999999.99) ry = ObjectInfo[objectid][ory];
		if(rz == 999999999.99) rz = ObjectInfo[objectid][orz];

		new time;
		for(new playerid; playerid<MAX_PLAYERS; playerid++)
		{
			if(F_IsObjectCreatedForPlayer(playerid, objectid))
			{
				time = MovePlayerObject(playerid, ObjectInfo[objectid][ObjectID][playerid], x, y, z, speed, rx, ry, rz);
			}
		}

		if(UpdatePos)
		{
		    ObjectInfo[objectid][ox] = x; ObjectInfo[objectid][oy] = y; ObjectInfo[objectid][oz] = z;
		}

		ObjectInfo[objectid][ObjectMoving] = true;
		return time;
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

forward F_OnObjectMoved(objectid); public OnPlayerObjectMoved(playerid, objectid)
{
	for(new objectid2; objectid2<F_MAX_OBJECTS; objectid2++)
	{
		if(F_IsObjectCreated(objectid2))
		{
			if(ObjectInfo[objectid2][ObjectID][playerid] == objectid && ObjectInfo[objectid2][ObjectMoving] == true)
			{
				CallLocalFunction("F_OnObjectMoved", "i", objectid2);
				ObjectInfo[objectid][ObjectMoving] = false;
			}
		}
	}
	return 1;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

stock F_StopObject(objectid)
{
	if(F_IsObjectCreated(objectid))
	{
		for(new playerid; playerid<MAX_PLAYERS; playerid++)
		{
			if(F_IsObjectCreatedForPlayer(playerid, objectid))
			{
				StopPlayerObject(playerid, ObjectInfo[objectid][ObjectID][playerid]);
			}
		}
		ObjectInfo[objectid][ObjectMoving] = false;
		return 1;
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

stock F_IsObjectMoving(objectid)
{
	if(F_IsObjectCreated(objectid))
	{
	    if(ObjectInfo[objectid][ObjectMoving]) return 1;
		return 0;
	}
	return -1;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

stock F_SetObjectPos(objectid, Float:x, Float:y, Float:z, bool:InstantUpdate=false)
{
	if(F_IsObjectCreated(objectid))
	{
		ObjectInfo[objectid][ox] = x; ObjectInfo[objectid][oy] = y; ObjectInfo[objectid][oz] = z;
		for(new playerid; playerid<MAX_PLAYERS; playerid++)
		{
			if(F_IsObjectCreatedForPlayer(playerid, objectid))
			{
				SetPlayerObjectPos(playerid, ObjectInfo[objectid][ObjectID][playerid], x, y, z);
			}
		}
		if(InstantUpdate) F_ObjectUpdateForAll();
		return 1;
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

stock F_GetObjectPos(objectid, &Float:x, &Float:y, &Float:z)
{
	if(F_IsObjectCreated(objectid))
	{
		x = ObjectInfo[objectid][ox]; y = ObjectInfo[objectid][oy]; z = ObjectInfo[objectid][oz];
		return 1;
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

stock F_SetObjectRot(objectid, Float:rx, Float:ry, Float:rz)
{
	if(F_IsObjectCreated(objectid))
	{
		ObjectInfo[objectid][orx] = rx; ObjectInfo[objectid][ory] = ry; ObjectInfo[objectid][orz] = rz;

		for(new playerid; playerid<MAX_PLAYERS; playerid++)
		{
			if(F_IsObjectCreatedForPlayer(playerid, objectid))
			{
				SetPlayerObjectRot(playerid, ObjectInfo[objectid][ObjectID][playerid], rx, ry, rz);
			}
		}
		return 1;
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

stock F_GetObjectRot(objectid, &Float:rx, &Float:ry, &Float:rz)
{
	if(F_IsObjectCreated(objectid))
	{
		rx = ObjectInfo[objectid][orx]; ry = ObjectInfo[objectid][ory]; rz = ObjectInfo[objectid][orz];
		return 1;
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

stock F_RefreshObjects(playerid)
{
	if(IsPlayerConnected(playerid) && !IsPlayerNPC(playerid))
	{
		RefreshObjects[playerid] = true;

		new Float:x, Float:y, Float:z;
		GetPlayerPos(playerid, x, y, z);
		F_PlayerObjectUpdate(playerid, x, y, z);
		return 1;
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public OnPlayerConnect(playerid)
{
	if(ObjectUpdatetRunning == false)
	{
		SetTimer("F_ObjectUpdate", UpdateTime, 1);
		ObjectUpdatetRunning = true;
	}

	OldX[playerid] = 999999999.99; OldY[playerid] = 999999999.99; OldZ[playerid] = 999999999.99;
	RefreshObjects[playerid] = false;

	if(funcidx("F_OnPlayerConnect") != -1) { CallLocalFunction("F_OnPlayerConnect", "i", playerid); }
	return 1;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public OnPlayerDisconnect(playerid, reason)
{
	for(new objectid; objectid<F_MAX_OBJECTS; objectid++)
	{
		if(ObjectInfo[objectid][ObjectCreated][playerid] == true)
		{
			DestroyPlayerObject(playerid, ObjectInfo[objectid][ObjectID][playerid]);
			ObjectInfo[objectid][ObjectCreated][playerid] = false;
		}

		if(ObjectInfo[objectid][ObjectAttachedID] == playerid) ObjectInfo[objectid][ObjectAttachedID] = -1;

	}

	if(funcidx("F_OnPlayerDisconnect") != -1) { CallLocalFunction("F_OnPlayerDisconnect", "ii", playerid, reason); }
	return 1;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

forward F_ObjectUpdate(bool:DontCheckDistance); public F_ObjectUpdate(bool:DontCheckDistance)
{
	new Closest[ObjectsToStream], bool:Firstloop, nr, oidx, AmountToStream, TimesToLoop;
	new Float:ObjDistance[F_MAX_OBJECTS], ObjectArr[F_MAX_OBJECTS], bool:DontDestroy[F_MAX_OBJECTS], bool:DontCreate[F_MAX_OBJECTS], bool:PosUpdated[F_MAX_OBJECTS];

	for(new playerid; playerid<MAX_PLAYERS; playerid++)
	{
		if(IsPlayerConnected(playerid) && DontUpdate[playerid] == false && !IsPlayerNPC(playerid))
		{
			if(DontCheckDistance || !IsPlayerInRangeOfPoint(playerid, DistanceBeforeUpdate, OldX[playerid], OldY[playerid], OldZ[playerid]))
			{
       			GetPlayerPos(playerid, OldX[playerid], OldY[playerid], OldZ[playerid]);
			#if DebugMode
			    new String[128];
			    new ObjectsCreated;
			    new ObjectsDestroyed;
			    new count = GetTickCount();
				SendClientMessage(playerid, 0xFFFFFFAA, " ");
			#endif
				nr = 0;
				for(new objectid; objectid<F_MAX_OBJECTS; objectid++) //check the distances from each object to the player
				{
					if(ObjectInfo[objectid][ModelID] != 0 && (ObjectInfo[objectid][PlayerID] == -1 || ObjectInfo[objectid][PlayerID] == playerid))
					{
						if(PosUpdated[objectid] == false) //update the object coordinates if it's attached to a player or vehicle.
						{
						    if(ObjectInfo[objectid][ObjectAttachedID] != -1) { GetPlayerPos(ObjectInfo[objectid][ObjectAttachedID], ObjectInfo[objectid][ox], ObjectInfo[objectid][oy], ObjectInfo[objectid][oz]); }
							if(ObjectInfo[objectid][ObjectAttachedVID] != -1) { GetVehiclePos(ObjectInfo[objectid][ObjectAttachedVID], ObjectInfo[objectid][ox], ObjectInfo[objectid][oy], ObjectInfo[objectid][oz]); }
							PosUpdated[objectid] = true;
						}

						ObjDistance[objectid] = GetPlayerDistanceFromPoint(playerid, ObjectInfo[objectid][ox], ObjectInfo[objectid][oy], ObjectInfo[objectid][oz]);

						if(floatcmp(ObjDistance[objectid], StreamRange) == -1)
						{
							ObjectArr[nr] = objectid;
							nr++;
						}
					}
				}

				if(nr > ObjectsToStream)
				{
				    AmountToStream = ObjectsToStream;

				    if(nr > ObjectsToStream*2) //sorting the closest '#ObjectsToStream' objects on distance
				    {
						for(new loop; loop<ObjectsToStream; loop++)
						{
							Firstloop = true;
							for(new objectid; objectid<nr; objectid++)
							{
								if(ObjDistance[ObjectArr[objectid]] != 999999999.99 && (Firstloop || floatcmp(ObjDistance[ObjectArr[objectid]], ObjDistance[Closest[loop]]) == -1))
								{
									Firstloop = false;
									Closest[loop] = ObjectArr[objectid];
								}
							}
							ObjDistance[Closest[loop]] = 999999999.99;
						}
					}
					else //sorting the furthest '#ObjectsToStream' objects on distance and creating the object array without those
					{
						for(new objectid; objectid<F_MAX_OBJECTS; objectid++) DontCreate[objectid] = false;

					    TimesToLoop = nr-ObjectsToStream;
						for(new loop; loop<TimesToLoop; loop++)
						{
							Firstloop = true;
							for(new objectid; objectid<nr; objectid++)
							{
								if(ObjDistance[ObjectArr[objectid]] != 999999999.99 && (Firstloop || floatcmp(ObjDistance[ObjectArr[objectid]], ObjDistance[Closest[loop]]) == 1))
								{
									Firstloop = false;
									Closest[loop] = ObjectArr[objectid]; //'Closest' actually means furthest here but I didn't want to create a new unnecessary array
								}
							}
							ObjDistance[Closest[loop]] = 999999999.99;
							DontCreate[Closest[loop]] = true;
						}

					    oidx = 0;
						for(new objectid; objectid<nr; objectid++)
						{
						    if(DontCreate[ObjectArr[objectid]] == false)
						    {
								Closest[oidx] = ObjectArr[objectid];
								oidx ++;
							}
						}
					}
				}
				else
				{
				    AmountToStream = nr;
					for(new objectid; objectid<nr; objectid++) Closest[objectid] = ObjectArr[objectid];
				}

				for(new objectid; objectid<F_MAX_OBJECTS; objectid++) DontDestroy[objectid] = false;
				for(new objectid; objectid<AmountToStream; objectid++) DontDestroy[Closest[objectid]] = true;

				for(new objectid; objectid<F_MAX_OBJECTS; objectid++)
				{
					if(ObjectInfo[objectid][ObjectCreated][playerid] == true && DontDestroy[objectid] == false)
					{
						DestroyPlayerObject(playerid, ObjectInfo[objectid][ObjectID][playerid]);
						ObjectInfo[objectid][ObjectCreated][playerid] = false;
					#if DebugMode
						ObjectsDestroyed ++;
					#endif
					}
				}

				for(new loop; loop<AmountToStream; loop++)
				{
					if(ObjectInfo[Closest[loop]][ObjectCreated][playerid] == false)
					{
						ObjectInfo[Closest[loop]][ObjectID][playerid] = CreatePlayerObject(playerid, ObjectInfo[Closest[loop]][ModelID], ObjectInfo[Closest[loop]][ox], ObjectInfo[Closest[loop]][oy],
						ObjectInfo[Closest[loop]][oz], ObjectInfo[Closest[loop]][orx], ObjectInfo[Closest[loop]][ory], ObjectInfo[Closest[loop]][orz], ObjectInfo[Closest[loop]][ovdist]);
						ObjectInfo[Closest[loop]][ObjectCreated][playerid] = true;

						if(ObjectInfo[Closest[loop]][ObjectAttachedID] != -1)
						{
							AttachPlayerObjectToPlayer(playerid, ObjectInfo[Closest[loop]][ObjectID][playerid], ObjectInfo[Closest[loop]][ObjectAttachedID], ObjectInfo[Closest[loop]][aox],
							ObjectInfo[Closest[loop]][aoy], ObjectInfo[Closest[loop]][aoz], ObjectInfo[Closest[loop]][aorx], ObjectInfo[Closest[loop]][aory], ObjectInfo[Closest[loop]][aorz]);
						}
						if(ObjectInfo[Closest[loop]][ObjectAttachedVID] != -1)
						{
							AttachPlayerObjectToVehicle(playerid, ObjectInfo[Closest[loop]][ObjectID][playerid], ObjectInfo[Closest[loop]][ObjectAttachedVID], ObjectInfo[Closest[loop]][aox],
							ObjectInfo[Closest[loop]][aoy], ObjectInfo[Closest[loop]][aoz], ObjectInfo[Closest[loop]][aorx], ObjectInfo[Closest[loop]][aory], ObjectInfo[Closest[loop]][aorz]);
						}

					#if DebugMode
						ObjectsCreated ++;
					#endif
					}
				}
			#if DebugMode
				//format(String, 128, "Time taken for update: %ims", GetTickCount()-count);
				//SendClientMessage(playerid, 0xFFFFFFAA, String);

				//format(String, 128, "Total amount of objects to be streamed: %i", F_CountObjects());
				//SendClientMessage(playerid, 0xFFFFFFAA, String);

				//format(String, 128, "Objects within StreamRange: %i", nr);
				//SendClientMessage(playerid, 0xFFFFFFAA, String);

				//format(String, 128, "Objects created during this update: %i", ObjectsCreated);
				//SendClientMessage(playerid, 0xFFFFFFAA, String);

				//format(String, 128, "Objects destroyed during this update: %i", ObjectsDestroyed);
				//SendClientMessage(playerid, 0xFFFFFFAA, String);

				//format(String, 128, "Maximum amount of objects per player: %i", ObjectsToStream);
				//SendClientMessage(playerid, 0xFFFFFFAA, String);

				//format(String, 128, "Objects created for player%i: %i", playerid, AmountToStream);
				//SendClientMessage(playerid, 0xFFFFFFAA, String);
			#endif
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

stock F_ObjectUpdateForAll() { F_ObjectUpdate(true); }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

stock F_PlayerObjectUpdate(playerid, Float:x, Float:y, Float:z) //Same function but just for one player. Since this is called a lot in player teleporting, it's useful to have this separate function which is a lot faster.
{
    if(IsPlayerConnected(playerid) && !IsPlayerNPC(playerid))
    {
		new nr;
		new Float:ObjDistance[F_MAX_OBJECTS];
		new ObjectArr[F_MAX_OBJECTS];
		new Float:fX, Float:fY, Float:fZ;

	#if DebugMode
	    new String[128];
	    new ObjectsCreated;
	    new ObjectsDestroyed;
	    new count = GetTickCount();
		SendClientMessage(playerid, 0xFFFFFFAA, " ");
	#endif

		for(new objectid; objectid<F_MAX_OBJECTS; objectid++)
		{
			if(ObjectInfo[objectid][ModelID] != 0 && (ObjectInfo[objectid][PlayerID] == -1 || ObjectInfo[objectid][PlayerID] == playerid))
			{
				if(ObjectInfo[objectid][ObjectAttachedID] != -1) { GetPlayerPos(ObjectInfo[objectid][ObjectAttachedID], ObjectInfo[objectid][ox], ObjectInfo[objectid][oy], ObjectInfo[objectid][oz]); }
				if(ObjectInfo[objectid][ObjectAttachedVID] != -1) { GetVehiclePos(ObjectInfo[objectid][ObjectAttachedVID], ObjectInfo[objectid][ox], ObjectInfo[objectid][oy], ObjectInfo[objectid][oz]); }

				fX = ObjectInfo[objectid][ox] - x;
				fY = ObjectInfo[objectid][oy] - y;
				fZ = ObjectInfo[objectid][oz] - z;

				ObjDistance[objectid] = fX*fX + fY*fY + fZ*fZ; //squared distance

				if(floatcmp(ObjDistance[objectid], StreamRange*StreamRange) == -1)
				{
					ObjectArr[nr] = objectid;
					nr++;
				}
			}
		}

		new Closest[ObjectsToStream];
		new AmountToStream;

		if(nr > ObjectsToStream)
		{
			AmountToStream = ObjectsToStream;

		    if(nr > ObjectsToStream*2)
		    {
				for(new loop; loop<ObjectsToStream; loop++)
				{
					new bool:Firstloop = true;
					for(new objectid; objectid<nr; objectid++)
					{
						if(ObjDistance[ObjectArr[objectid]] != 999999999.99 && (Firstloop || floatcmp(ObjDistance[ObjectArr[objectid]], ObjDistance[Closest[loop]]) == -1))
						{
							Firstloop = false;
							Closest[loop] = ObjectArr[objectid];
						}
					}
					ObjDistance[Closest[loop]] = 999999999.99;
				}
			}
			else
			{
				new bool:DontCreate[F_MAX_OBJECTS];

			    new TimesToLoop = nr-ObjectsToStream;
				for(new loop; loop<TimesToLoop; loop++)
				{
					new bool:Firstloop = true;
					for(new objectid; objectid<nr; objectid++)
					{
						if(ObjDistance[ObjectArr[objectid]] != 999999999.99 && (Firstloop || floatcmp(ObjDistance[ObjectArr[objectid]], ObjDistance[Closest[loop]]) == 1))
						{
							Firstloop = false;
							Closest[loop] = ObjectArr[objectid];
						}
					}
					ObjDistance[Closest[loop]] = 999999999.99;
					DontCreate[Closest[loop]] = true;
				}

			    new oidx;
				for(new objectid; objectid<nr; objectid++)
				{
				    if(DontCreate[ObjectArr[objectid]] == false)
				    {
						Closest[oidx] = ObjectArr[objectid];
						oidx ++;
					}
				}
			}
		}
		else
		{
			AmountToStream = nr;
			for(new objectid; objectid<nr; objectid++) Closest[objectid] = ObjectArr[objectid];
		}

		new bool:DontDestroy[F_MAX_OBJECTS];
		for(new objectid; objectid<AmountToStream; objectid++) DontDestroy[Closest[objectid]] = true;

		for(new objectid; objectid<F_MAX_OBJECTS; objectid++)
		{
			if(ObjectInfo[objectid][ObjectCreated][playerid] == true && (DontDestroy[objectid] == false || RefreshObjects[playerid] == true))
			{
				DestroyPlayerObject(playerid, ObjectInfo[objectid][ObjectID][playerid]);
				ObjectInfo[objectid][ObjectCreated][playerid] = false;
			#if DebugMode
				ObjectsDestroyed ++;
			#endif
			}
		}
		RefreshObjects[playerid] = false;

		for(new loop; loop<AmountToStream; loop++)
		{
			if(ObjectInfo[Closest[loop]][ObjectCreated][playerid] == false)
			{
				ObjectInfo[Closest[loop]][ObjectID][playerid] = CreatePlayerObject(playerid, ObjectInfo[Closest[loop]][ModelID], ObjectInfo[Closest[loop]][ox], ObjectInfo[Closest[loop]][oy],
				ObjectInfo[Closest[loop]][oz], ObjectInfo[Closest[loop]][orx], ObjectInfo[Closest[loop]][ory], ObjectInfo[Closest[loop]][orz], ObjectInfo[Closest[loop]][ovdist]);
				ObjectInfo[Closest[loop]][ObjectCreated][playerid] = true;

				if(ObjectInfo[Closest[loop]][ObjectAttachedID] != -1)
				{
					AttachPlayerObjectToPlayer(playerid, ObjectInfo[Closest[loop]][ObjectID][playerid], ObjectInfo[Closest[loop]][ObjectAttachedID], ObjectInfo[Closest[loop]][aox],
					ObjectInfo[Closest[loop]][aoy], ObjectInfo[Closest[loop]][aoz], ObjectInfo[Closest[loop]][aorx], ObjectInfo[Closest[loop]][aory], ObjectInfo[Closest[loop]][aorz]);
				}
				if(ObjectInfo[Closest[loop]][ObjectAttachedVID] != -1)
				{
					AttachPlayerObjectToVehicle(playerid, ObjectInfo[Closest[loop]][ObjectID][playerid], ObjectInfo[Closest[loop]][ObjectAttachedVID], ObjectInfo[Closest[loop]][aox],
					ObjectInfo[Closest[loop]][aoy], ObjectInfo[Closest[loop]][aoz], ObjectInfo[Closest[loop]][aorx], ObjectInfo[Closest[loop]][aory], ObjectInfo[Closest[loop]][aorz]);
				}
			#if DebugMode
				ObjectsCreated ++;
			#endif
			}
		}
	#if DebugMode
		//format(String, 128, "Time taken for update: %ims", GetTickCount()-count);
		//SendClientMessage(playerid, 0xFFFFFFAA, String);

		//format(String, 128, "Total amount of objects to be streamed: %i", F_CountObjects());
		//SendClientMessage(playerid, 0xFFFFFFAA, String);

		//format(String, 128, "Objects within StreamRange: %i", nr);
		//SendClientMessage(playerid, 0xFFFFFFAA, String);

		//format(String, 128, "Objects created during this update: %i", ObjectsCreated);
		//SendClientMessage(playerid, 0xFFFFFFAA, String);

		//format(String, 128, "Objects destroyed during this update: %i", ObjectsDestroyed);
		//SendClientMessage(playerid, 0xFFFFFFAA, String);

		//format(String, 128, "Maximum amount of objects per player: %i", ObjectsToStream);
		//SendClientMessage(playerid, 0xFFFFFFAA, String);

		//format(String, 128, "Objects created for player%i: %i", playerid, AmountToStream);
		//SendClientMessage(playerid, 0xFFFFFFAA, String);
	#endif
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

stock F_SetPlayerPos(playerid, Float:x, Float:y, Float:z, bool:InstantUpdate=true)
{
	if(InstantUpdate)
	{
		DontUpdate[playerid] = true;
		F_PlayerObjectUpdate(playerid, x, y, z);
		SetPlayerPos(playerid, x, y, z);

		if(ContinueUpdatetRunning[playerid] == true) { KillTimer(ContinueUpdatet[playerid]); }
		ContinueUpdatet[playerid] = SetTimerEx("ContinueUpdate", UpdateTime, 0, "i", playerid);
		ContinueUpdatetRunning[playerid] = true;
		return 1;
	}
	SetPlayerPos(playerid, x, y, z);
	return 1;
}

#define SetPlayerPos F_SetPlayerPos

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

forward ContinueUpdate(playerid); public ContinueUpdate(playerid)
{
	DontUpdate[playerid] = false;
	ContinueUpdatetRunning[playerid] = false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined _ALS_OnPlayerConnect
	#undef OnPlayerConnect
#else
	#define _ALS_OnPlayerConnect
#endif

#define OnPlayerConnect F_OnPlayerConnect
forward F_OnPlayerConnect(playerid);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined _ALS_OnPlayerDisconnect
	#undef OnPlayerDisconnect
#else
	#define _ALS_OnPlayerDisconnect
#endif

#define OnPlayerDisconnect F_OnPlayerDisconnect
forward F_OnPlayerDisconnect(playerid, reason);

//EOF/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
