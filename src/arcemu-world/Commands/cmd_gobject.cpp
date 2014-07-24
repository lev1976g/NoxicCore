/*
 * NoxicCore MMORPG Server
 * Copyright (c) 2011-2014 Crimoxic Team
 * Copyright (c) 2008-2013 ArcEmu Team <http://www.arcemu.org/>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "../StdAfx.h"
#include "../Chat.h"
#include "../ObjectMgr.h"
#include "../Master.h"

bool ChatHandler::HandleGOSelect(const char* args, WorldSession* m_session)
{
	GameObject* GObj = NULL;
	GameObject* GObjs = m_session->GetPlayer()->GetSelectedGo();

	std::set<Object*>::iterator Itr = m_session->GetPlayer()->GetInRangeSetBegin();
	std::set<Object*>::iterator Itr2 = m_session->GetPlayer()->GetInRangeSetEnd();
	float cDist = 9999.0f;
	float nDist = 0.0f;
	bool bUseNext = false;

	if(args)
	{
		if(args[0] == '1')
		{
			if(!GObjs)
				bUseNext = true;

			for(;; Itr++)
			{
				if(Itr == Itr2 && !GObj&& bUseNext)
					Itr = m_session->GetPlayer()->GetInRangeSetBegin();
				else if(Itr == Itr2)
					break;

				if((*Itr)->IsGameObject())
				{
					// Find the current go, move to the next one
					if(bUseNext)
					{
						// Select the first.
						GObj = TO< GameObject* >(*Itr);
						break;
					}
					else
					{
						if(((*Itr) == GObjs))
						{
							// Found him. Move to the next one, or beginning if we're at the end
							bUseNext = true;
						}
					}
				}
			}
		}
	}
	if(!GObj)
	{
		for(; Itr != Itr2; Itr++)
		{
			if((*Itr)->IsGameObject())
			{
				if((nDist = m_session->GetPlayer()->CalcDistance(*Itr)) < cDist)
				{
					cDist = nDist;
					nDist = 0.0f;
					GObj = TO_GAMEOBJECT(*Itr);
				}
			}
		}
	}


	if(!GObj)
	{
		RedSystemMessage(m_session, "No in-range GameObject found.");
		return true;
	}

	m_session->GetPlayer()->m_GM_SelectedGO = GObj->GetGUID();

	GreenSystemMessage(m_session, "Selected GameObject [ %s ] which is %.3f meters away from you.",
	                   GameObjectNameStorage.LookupEntry(GObj->GetEntry())->Name, m_session->GetPlayer()->CalcDistance(GObj));

	return true;
}

bool ChatHandler::HandleGODelete(const char* args, WorldSession* m_session)
{
	GameObject* GObj = m_session->GetPlayer()->GetSelectedGo();
	if(GObj == NULL)
	{
		RedSystemMessage(m_session, "No selected GameObject...");
		return true;
	}

	if(GObj->IsInBg())
	{
		RedSystemMessage(m_session, "GameObjects can't be deleted in Battlegrounds");
		return true;
	}

	if(GObj->m_spawn != NULL && GObj->m_spawn->entry == GObj->GetEntry())
	{
		uint32 cellx = uint32(((_maxX - GObj->m_spawn->x) / _cellSize));
		uint32 celly = uint32(((_maxY - GObj->m_spawn->y) / _cellSize));

		if(cellx < _sizeX && celly < _sizeY)
		{
			CellSpawns* sp = GObj->GetMapMgr()->GetBaseMap()->GetSpawnsList(cellx, celly);
			if(sp != NULL)
			{
				for(GOSpawnList::iterator itr = sp->GOSpawns.begin(); itr != sp->GOSpawns.end(); ++itr)
					if((*itr) == GObj->m_spawn)
					{
						sp->GOSpawns.erase(itr);
						break;
					}
			}
			GObj->DeleteFromDB();
			delete GObj->m_spawn;
			GObj->m_spawn = NULL;
		}
	}
	sGMLog.writefromsession(m_session, "deleted game object entry %u on map %u at X:%f Y:%f Z:%f Name %s", GObj->GetEntry(), GObj->GetMapId(), GObj->GetPositionX(), GObj->GetPositionY(), GObj->GetPositionZ(), GameObjectNameStorage.LookupEntry(GObj->GetEntry())->Name);
	GObj->Despawn(0, 0); // We do not need to delete the object because GameObject::Despawn with no time => ExpireAndDelete() => _Expire() => delete GObj;

	m_session->GetPlayer()->m_GM_SelectedGO = 0;

	return true;
}

bool ChatHandler::HandleGOSpawn(const char* args, WorldSession* m_session)
{
	std::stringstream sstext;

	char* pEntryID = strtok((char*)args, " ");
	if(!pEntryID)
		return false;

	uint32 EntryID  = atoi(pEntryID);

	bool Save = false;
	char* pSave = strtok(NULL, " ");
	if(pSave)
		Save = (atoi(pSave) > 0 ? true : false);

	GameObjectInfo* goi = GameObjectNameStorage.LookupEntry(EntryID);
	if(!goi)
	{
		sstext << "GameObject Info '" << EntryID << "' Not Found" << '\0';
		SystemMessage(m_session, sstext.str().c_str());
		return true;
	}

	LOG_DEBUG("Spawning GameObject By Entry '%u'", EntryID);
	sstext << "Spawning GameObject By Entry '" << EntryID << "'" << '\0';
	SystemMessage(m_session, sstext.str().c_str());

	Player* chr = m_session->GetPlayer();

	GameObject* go = chr->GetMapMgr()->CreateGameObject(EntryID);

	uint32 mapid = chr->GetMapId();
	float x = chr->GetPositionX();
	float y = chr->GetPositionY();
	float z = chr->GetPositionZ();
	float o = chr->GetOrientation();

	go->CreateFromProto(EntryID, mapid, x, y, z, o);
	go->PushToWorld(chr->GetMapMgr());
	go->Phase(PHASE_SET, chr->GetPhase());
	// Create spawn instance
	GOSpawn* gs = new GOSpawn;
	gs->entry = go->GetEntry();
	gs->facing = go->GetOrientation();
	gs->faction = go->GetFaction();
	gs->flags = go->GetUInt32Value(GAMEOBJECT_FLAGS);
	gs->id = objmgr.GenerateGameObjectSpawnID();
	gs->o = 0.0f;
	gs->o1 = go->GetParentRotation(0);
	gs->o2 = go->GetParentRotation(2);
	gs->o3 = go->GetParentRotation(3);
	gs->scale = go->GetScale();
	gs->x = go->GetPositionX();
	gs->y = go->GetPositionY();
	gs->z = go->GetPositionZ();
	gs->state = go->GetByte(GAMEOBJECT_BYTES_1, 0);
	//gs->stateNpcLink = 0;
	gs->phase = go->GetPhase();
	gs->overrides = go->GetOverrides();

	uint32 cx = chr->GetMapMgr()->GetPosX(chr->GetPositionX());
	uint32 cy = chr->GetMapMgr()->GetPosY(chr->GetPositionY());

	chr->GetMapMgr()->GetBaseMap()->GetSpawnsListAndCreate(cx, cy)->GOSpawns.push_back(gs);
	go->m_spawn = gs;

	MapCell* mCell = chr->GetMapMgr()->GetCell(cx, cy);

	if(mCell != NULL)
		mCell->SetLoaded();

	if(Save == true)
	{
		// If we're saving, create template and add index
		go->SaveToDB();
		go->m_loadedFromDB = true;
	}
	sGMLog.writefromsession(m_session, "spawned gameobject %s, entry %u at %u %f %f %f%s", GameObjectNameStorage.LookupEntry(gs->entry)->Name, gs->entry, chr->GetMapId(), gs->x, gs->y, gs->z, Save ? ", saved in DB" : "");
	return true;
}

bool ChatHandler::HandleGOPhaseCommand(const char* args, WorldSession* m_session)
{
	char* sPhase = strtok((char*)args, " ");
	if(!sPhase)
		return false;

	uint32 newphase = atoi(sPhase);

	bool Save = false;
	char* pSave = strtok(NULL, " ");
	if(pSave)
		Save = (atoi(pSave) > 0 ? true : false);

	GameObject* go = m_session->GetPlayer()->GetSelectedGo();
	if(!go)
	{
		RedSystemMessage(m_session, "No selected GameObject...");
		return true;
	}

	go->Phase(PHASE_SET, newphase);

	GOSpawn* gs = go->m_spawn;
	if(gs == NULL)
	{
		RedSystemMessage(m_session, "The GameObject got no spawn, not saving and not logging...");
		return true;
	}
	//VLack: We have to have a spawn, or SaveToDB would write a 0 into the first column (ID), and would erroneously overwrite something in the DB.
	//The code which saves creatures is a bit more forgiving, as it creates a new spawn on-demand, but the gameobject code does not.

	gs->phase = go->GetPhase();

	uint32 cx = m_session->GetPlayer()->GetMapMgr()->GetPosX(m_session->GetPlayer()->GetPositionX());
	uint32 cy = m_session->GetPlayer()->GetMapMgr()->GetPosY(m_session->GetPlayer()->GetPositionY());

	MapCell* mCell = m_session->GetPlayer()->GetMapMgr()->GetCell(cx, cy);

	if(mCell != NULL)
		mCell->SetLoaded();

	if(Save == true)
	{
		// If we're saving, create template and add index
		go->SaveToDB();
		go->m_loadedFromDB = true;
	}
	sGMLog.writefromsession(m_session, "phased gameobject %s to %u, entry %u at %u %f %f %f%s", GameObjectNameStorage.LookupEntry(gs->entry)->Name, newphase, gs->entry, m_session->GetPlayer()->GetMapId(), gs->x, gs->y, gs->z, Save ? ", saved in DB" : "");
	return true;
}

bool ChatHandler::HandleGOInfo(const char* args, WorldSession* m_session)
{
	GameObjectInfo* GOInfo = NULL;
	GameObject* GObj = m_session->GetPlayer()->GetSelectedGo();
	if(!GObj)
	{
		RedSystemMessage(m_session, "No selected GameObject...");
		return true;
	}

	SystemMessage(m_session, "%s Information:",	MSG_COLOR_SUBWHITE);
	SystemMessage(m_session, "%s SpawnID:%s%u",	MSG_COLOR_GREEN, MSG_COLOR_LIGHTBLUE, GObj->m_spawn != NULL ? GObj->m_spawn->id : 0);
	SystemMessage(m_session, "%s Entry:%s%u",	MSG_COLOR_GREEN, MSG_COLOR_LIGHTBLUE, GObj->GetEntry());
	SystemMessage(m_session, "%s Model:%s%u",	MSG_COLOR_GREEN, MSG_COLOR_LIGHTBLUE, GObj->GetUInt32Value(GAMEOBJECT_DISPLAYID));
	SystemMessage(m_session, "%s State:%s%u",	MSG_COLOR_GREEN, MSG_COLOR_LIGHTBLUE, GObj->GetByte(GAMEOBJECT_BYTES_1, 0));
	SystemMessage(m_session, "%s flags:%s%u",	MSG_COLOR_GREEN, MSG_COLOR_LIGHTBLUE, GObj->GetUInt32Value(GAMEOBJECT_FLAGS));
	SystemMessage(m_session, "%s dynflags:%s%u", MSG_COLOR_GREEN, MSG_COLOR_LIGHTBLUE, GObj->GetUInt32Value(GAMEOBJECT_DYNAMIC));
	SystemMessage(m_session, "%s faction:%s%u",	MSG_COLOR_GREEN, MSG_COLOR_LIGHTBLUE, GObj->GetFaction());
	SystemMessage(m_session, "%s phase:%s%u",	MSG_COLOR_GREEN, MSG_COLOR_LIGHTBLUE, GObj->GetPhase());

	char gotypetxt[50];
	switch(GObj->GetType())
	{
		case GAMEOBJECT_TYPE_DOOR:
			strcpy(gotypetxt, "Door");
			break;
		case GAMEOBJECT_TYPE_BUTTON:
			strcpy(gotypetxt, "Button");
			break;
		case GAMEOBJECT_TYPE_QUESTGIVER:
			strcpy(gotypetxt, "Quest Giver");
			break;
		case GAMEOBJECT_TYPE_CHEST:
			strcpy(gotypetxt, "Chest");
			break;
		case GAMEOBJECT_TYPE_BINDER:
			strcpy(gotypetxt, "Binder");
			break;
		case GAMEOBJECT_TYPE_GENERIC:
			strcpy(gotypetxt, "Generic");
			break;
		case GAMEOBJECT_TYPE_TRAP:
			strcpy(gotypetxt, "Trap");
			break;
		case GAMEOBJECT_TYPE_CHAIR:
			strcpy(gotypetxt, "Chair");
			break;
		case GAMEOBJECT_TYPE_SPELL_FOCUS:
			strcpy(gotypetxt, "Spell Focus");
			break;
		case GAMEOBJECT_TYPE_TEXT:
			strcpy(gotypetxt, "Text");
			break;
		case GAMEOBJECT_TYPE_GOOBER:
			strcpy(gotypetxt, "Goober");
			break;
		case GAMEOBJECT_TYPE_TRANSPORT:
			strcpy(gotypetxt, "Transport");
			break;
		case GAMEOBJECT_TYPE_AREADAMAGE:
			strcpy(gotypetxt, "Area Damage");
			break;
		case GAMEOBJECT_TYPE_CAMERA:
			strcpy(gotypetxt, "Camera");
			break;
		case GAMEOBJECT_TYPE_MAP_OBJECT:
			strcpy(gotypetxt, "Map Object");
			break;
		case GAMEOBJECT_TYPE_MO_TRANSPORT:
			strcpy(gotypetxt, "Mo Transport");
			break;
		case GAMEOBJECT_TYPE_DUEL_ARBITER:
			strcpy(gotypetxt, "Duel Arbiter");
			break;
		case GAMEOBJECT_TYPE_FISHINGNODE:
			strcpy(gotypetxt, "Fishing Node");
			break;
		case GAMEOBJECT_TYPE_RITUAL:
			strcpy(gotypetxt, "Ritual");
			break;
		case GAMEOBJECT_TYPE_MAILBOX:
			strcpy(gotypetxt, "Mailbox");
			break;
		case GAMEOBJECT_TYPE_AUCTIONHOUSE:
			strcpy(gotypetxt, "Auction House");
			break;
		case GAMEOBJECT_TYPE_GUARDPOST:
			strcpy(gotypetxt, "Guard Post");
			break;
		case GAMEOBJECT_TYPE_SPELLCASTER:
			strcpy(gotypetxt, "Spell Caster");
			break;
		case GAMEOBJECT_TYPE_MEETINGSTONE:
			strcpy(gotypetxt, "Meeting Stone");
			break;
		case GAMEOBJECT_TYPE_FLAGSTAND:
			strcpy(gotypetxt, "Flag Stand");
			break;
		case GAMEOBJECT_TYPE_FISHINGHOLE:
			strcpy(gotypetxt, "Fishing Hole");
			break;
		case GAMEOBJECT_TYPE_FLAGDROP:
			strcpy(gotypetxt, "Flag Drop");
			break;
		case GAMEOBJECT_TYPE_DESTRUCTIBLE_BUILDING:
			strcpy(gotypetxt, "Destructible Building");
			break;
		default:
			strcpy(gotypetxt, "Unknown.");
			break;
	}
	SystemMessage(m_session, "%s Type:%s%u -- %s", MSG_COLOR_GREEN, MSG_COLOR_LIGHTBLUE, GObj->GetType(), gotypetxt);

	SystemMessage(m_session, "%s Distance:%s%f", MSG_COLOR_GREEN, MSG_COLOR_LIGHTBLUE, GObj->CalcDistance(m_session->GetPlayer()));

	GOInfo = GameObjectNameStorage.LookupEntry(GObj->GetEntry());
	if(!GOInfo)
	{
		RedSystemMessage(m_session, "This GameObject doesn't have template, you won't be able to get some information nor to spawn a GO with this entry.");
		return true;
	}

	if(GOInfo->Name)
		SystemMessage(m_session, "%s Name:%s%s", MSG_COLOR_GREEN, MSG_COLOR_LIGHTBLUE, GOInfo->Name);
	SystemMessage(m_session, "%s Size:%s%f", MSG_COLOR_GREEN, MSG_COLOR_LIGHTBLUE, GObj->GetScale());
	SystemMessage(m_session, "%s Parent Rotation O1:%s%f", MSG_COLOR_GREEN, MSG_COLOR_LIGHTBLUE, GObj->GetParentRotation(1));
	SystemMessage(m_session, "%s Parent Rotation O2:%s%f", MSG_COLOR_GREEN, MSG_COLOR_LIGHTBLUE, GObj->GetParentRotation(2));
	SystemMessage(m_session, "%s Parent Rotation O3:%s%f", MSG_COLOR_GREEN, MSG_COLOR_LIGHTBLUE, GObj->GetParentRotation(3));

	if( GOInfo->Type == GAMEOBJECT_TYPE_DESTRUCTIBLE_BUILDING ){
		SystemMessage(m_session, "%s HP:%s%u/%u", MSG_COLOR_GREEN, MSG_COLOR_LIGHTBLUE, GObj->GetHP(), GObj->GetMaxHP() );
	}

	return true;
}

bool ChatHandler::HandleGODamageCommand( const char *args, WorldSession *session ){
	GameObject *go = session->GetPlayer()->GetSelectedGo();
	if( go == NULL ){
		RedSystemMessage( session, "You need to select a GO first!" );
		return true;
	}

	if( go->GetInfo()->Type != GAMEOBJECT_TYPE_DESTRUCTIBLE_BUILDING ){
		RedSystemMessage( session, "The selected GO must be a destructible building!" );
		return true;
	}

	if( *args == '\0' ){
		RedSystemMessage( session, "You need to specify how much you want to damage the selected GO!" );
		return true;
	}

	uint32 damage = 0;
	std::stringstream ss( args );

	ss >> damage;
	if( ss.fail() ){
		RedSystemMessage( session, "You need to specify how much you want to damage the selected GO!" );
		return true;
	}

	uint64 guid = session->GetPlayer()->GetGUID();

	BlueSystemMessage( session, "Attempting to damage GO..." );

	go->Damage( damage, guid, guid, 0 );

	return true;
}

bool ChatHandler::HandleGORebuildCommand( const char *args, WorldSession *session ){
	GameObject *go = session->GetPlayer()->GetSelectedGo();
	if( go == NULL ){
		RedSystemMessage( session, "You need to select a GO first!" );
		return true;
	}

	if( go->GetInfo()->Type != GAMEOBJECT_TYPE_DESTRUCTIBLE_BUILDING ){
		RedSystemMessage( session, "The selected GO must be a destructible building!" );
		return true;
	}

	BlueSystemMessage( session, "Attempting to rebuild building..." );

	go->Rebuild();

	return true;
}

bool ChatHandler::HandleGOActivate(const char* args, WorldSession* m_session)
{
	GameObject* GObj = m_session->GetPlayer()->GetSelectedGo();
	if(!GObj)
	{
		RedSystemMessage(m_session, "No selected GameObject...");
		return true;
	}
	if(GObj->GetByte(GAMEOBJECT_BYTES_1, 0) == 1)
	{
		// Close/Deactivate
		GObj->SetByte(GAMEOBJECT_BYTES_1, 0, 0);
		GObj->RemoveFlag(GAMEOBJECT_FLAGS, 1);
		BlueSystemMessage(m_session, "Gameobject closed.");
	}
	else
	{
		// Open/Activate
		GObj->SetByte(GAMEOBJECT_BYTES_1, 0, 1);
		GObj->SetFlag(GAMEOBJECT_FLAGS, 1);
		BlueSystemMessage(m_session, "Gameobject opened.");
	}
	sGMLog.writefromsession(m_session, "opened/closed gameobject %s, entry %u", GameObjectNameStorage.LookupEntry(GObj->GetEntry())->Name, GObj->GetEntry());
	return true;
}

bool ChatHandler::HandleGOEnable(const char* args, WorldSession* m_session)
{
	GameObject* GObj = m_session->GetPlayer()->GetSelectedGo();
	if(!GObj)
	{
		RedSystemMessage(m_session, "No selected GameObject...");
		return true;
	}
	if(GObj->GetUInt32Value(GAMEOBJECT_DYNAMIC) == 1)
	{
		// Deactivate
		GObj->SetUInt32Value(GAMEOBJECT_DYNAMIC, 0);
		BlueSystemMessage(m_session, "Gameobject deactivated.");
	}
	else
	{
		// /Activate
		GObj->SetUInt32Value(GAMEOBJECT_DYNAMIC, 1);
		BlueSystemMessage(m_session, "Gameobject activated.");
	}
	sGMLog.writefromsession(m_session, "activated/deactivated gameobject %s, entry %u", GameObjectNameStorage.LookupEntry(GObj->GetEntry())->Name, GObj->GetEntry());
	return true;
}

bool ChatHandler::HandleGOScale(const char* args, WorldSession* m_session)
{
	GameObject* go = m_session->GetPlayer()->GetSelectedGo();
	if(!go)
	{
		RedSystemMessage(m_session, "No selected GameObject...");
		return true;
	}
	if(!args)
	{
		RedSystemMessage(m_session, "Invalid syntax. Should be .gobject scale 1.0");
		return false;
	}
	float scale = (float)atof(args);
	if(!scale) scale = 1;
	go->SetScale(scale);
	BlueSystemMessage(m_session, "Set scale to %.3f", scale);
	sGMLog.writefromsession(m_session, "set scale on gameobject %s to %.3f, entry %u", GameObjectNameStorage.LookupEntry(go->GetEntry())->Name, scale, go->GetEntry());
	uint32 NewGuid = m_session->GetPlayer()->GetMapMgr()->GenerateGameobjectGuid();
	go->RemoveFromWorld(true);
	go->SetNewGuid(NewGuid);
	go->PushToWorld(m_session->GetPlayer()->GetMapMgr());
	go->SaveToDB();
	//lets reselect the object that can be really annoying...
	m_session->GetPlayer()->m_GM_SelectedGO = NewGuid;
	return true;
}

bool ChatHandler::HandleGOAnimProgress(const char* args, WorldSession* m_session)
{
	GameObject* GObj = m_session->GetPlayer()->GetSelectedGo();

	if(!GObj)
		return false;

	uint32 ap = atol(args);
	GObj->SetByte(GAMEOBJECT_BYTES_1, 3, static_cast<uint8>(ap));
	BlueSystemMessage(m_session, "Set ANIMPROGRESS to %u", ap);
	return true;
}

bool ChatHandler::HandleGOFactionCommand( const char *args, WorldSession *session ){
	GameObject *go = session->GetPlayer()->GetSelectedGo();
	if( go == NULL ){
		RedSystemMessage( session, "You need to select a GO first!" );
		return true;
	}

	if( *args == '\0' ){
		RedSystemMessage( session, "You need to specify a faction!" );
		return true;
	}

	std::stringstream ss( args );
	uint32 faction = 0;

	ss >> faction;
	if( ss.fail() ){
		RedSystemMessage( session, "You need to specify a faction!" );
		return true;
	}

	go->SetFaction( faction );

	BlueSystemMessage( session, "GameObject faction has been changed to %u", faction );

	return true;
}

bool ChatHandler::HandleGOExport(const char* args, WorldSession* m_session)
{
	/*if(!m_session->GetPlayer()->m_GM_SelectedGO)
		return false;

	std::stringstream name;
	if (*args)
	{
		name << "GO_" << args << ".sql";
	}
	else
	{
		name << "GO_" << m_session->GetPlayer()->m_GM_SelectedGO->GetEntry() << ".sql";
	}

	m_session->GetPlayer()->m_GM_SelectedGO->SaveToFile(name);

	BlueSystemMessage(m_session, "Go saved to: %s", name.str().c_str());*/

	Creature* pCreature = getSelectedCreature(m_session, true);
	if(!pCreature) return true;
	WorldDatabase.WaitExecute("INSERT INTO creature_names_export SELECT * FROM creature_names WHERE entry = %u", pCreature->GetEntry());
	WorldDatabase.WaitExecute("INSERT INTO creature_proto_export SELECT * FROM creature_proto WHERE entry = %u", pCreature->GetEntry());
	WorldDatabase.WaitExecute("INSERT INTO vendors_export SELECT * FROM vendors WHERE entry = %u", pCreature->GetEntry());
	QueryResult* qr = WorldDatabase.Query("SELECT * FROM vendors WHERE entry = %u", pCreature->GetEntry());
	if(qr != NULL)
	{
		do
		{
			WorldDatabase.WaitExecute("INSERT INTO items_export SELECT * FROM items WHERE entry = %u", qr->Fetch()[1].GetUInt32());
		}
		while(qr->NextRow());
		delete qr;
	}
	return true;
}

bool ChatHandler::HandleGOMove(const char* args, WorldSession* m_session)
{
	// move the go to player's coordinates
	GameObject* go = m_session->GetPlayer()->GetSelectedGo();
	if(!go)
	{
		RedSystemMessage(m_session, "No selected GameObject...");
		return true;
	}

	// new GO position (player location)
	// orientation doesn't change
	float x = m_session->GetPlayer()->GetPositionX();
	float y = m_session->GetPlayer()->GetPositionY();
	float z = m_session->GetPlayer()->GetPositionZ();
	float o = go->GetOrientation();

	go->RemoveFromWorld(true);
	go->SetPosition(x, y, z, o);
//	go->SetFloatValue(GAMEOBJECT_POS_X, x);
//	go->SetFloatValue(GAMEOBJECT_POS_Y, y);
//	go->SetFloatValue(GAMEOBJECT_POS_Z, z);
	uint32 NewGuid = m_session->GetPlayer()->GetMapMgr()->GenerateGameobjectGuid();
	go->SetNewGuid(NewGuid);
	go->PushToWorld(m_session->GetPlayer()->GetMapMgr());
	go->SaveToDB();
	m_session->GetPlayer()->m_GM_SelectedGO = NewGuid;
	return true;
}

bool ChatHandler::HandleGORotate(const char* args, WorldSession* m_session)
{
	char Axis;
	float deg;
	if(sscanf(args, "%c %f", &Axis, &deg) < 1) return false;
	GameObject* go = m_session->GetPlayer()->GetSelectedGo();
	if(!go)
	{
		RedSystemMessage(m_session, "No selected GameObject...");
		return true;
	}

	// float rad = deg * (float(M_PI) / 180.0f);

	switch(tolower(Axis))
	{
		case 'x':
//		go->ModFloatValue(GAMEOBJECT_ROTATION, rad);
			break;
		case 'y':
//		go->ModFloatValue(GAMEOBJECT_ROTATION_01, rad);
			break;
		case 'o':
			if(m_session->GetPlayer())
			{
				float ori = m_session->GetPlayer()->GetOrientation();
				go->SetParentRotation(2, sinf(ori / 2));
				go->SetParentRotation(3, cosf(ori / 2));
				go->SetOrientation(ori);
				go->UpdateRotation();
			}
			break;
		default:
			RedSystemMessage(m_session, "Invalid Axis, Please use x, y, or o.");
			return true;
	}

	uint32 NewGuid = m_session->GetPlayer()->GetMapMgr()->GenerateGameobjectGuid();
	go->RemoveFromWorld(true);
	go->SetNewGuid(NewGuid);
	go->PushToWorld(m_session->GetPlayer()->GetMapMgr());
	go->SaveToDB();
	//lets reselect the object that can be really annoying...
	m_session->GetPlayer()->m_GM_SelectedGO = NewGuid;
	return true;
}

bool ChatHandler::HandlePortToGameObjectSpawnCommand(const char* args, WorldSession* m_session)
{
	if(!*args)
	{
		return false;
	}

	uint32 spawn_id, spawn_map;
	float spawn_x, spawn_y, spawn_z;

	if(sscanf(args, "%u", (unsigned int*)&spawn_id) != 1)
	{
		return false;
	}
	QueryResult* QR = WorldDatabase.Query("SELECT * FROM gameobject_spawns WHERE id=%u", spawn_id);
	if(!QR)
	{
		RedSystemMessage(m_session, "No spawn found with id %u.", spawn_id);
		return true;
	}
	spawn_map = QR->Fetch()[2].GetUInt32();
	spawn_x = QR->Fetch()[3].GetFloat();
	spawn_y = QR->Fetch()[4].GetFloat();
	spawn_z = QR->Fetch()[5].GetFloat();
	LocationVector vec(spawn_x, spawn_y, spawn_z);
	m_session->GetPlayer()->SafeTeleport(spawn_map, 0, vec);
	delete QR;
	return true;
}