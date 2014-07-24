/*
 * ArcEmu MMORPG Server
 * Copyright (C) 2005-2007 Ascent Team <http://www.ascentemu.com/>
 * Copyright (C) 2008-2012 <http://www.ArcEmu.org/>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

///////////////////////////////////////////////
//  Admin Movement Commands
//

#include "StdAfx.h"

/*
#define isalpha(c)  {isupper(c) || islower(c))
#define isupper(c)  (c >=  'A' && c <= 'Z')
#define islower(c)  (c >=  'a' && c <= 'z')
*/

bool ChatHandler::HandleDeleteCommand(const char* args, WorldSession* m_session)
{

	uint64 guid = m_session->GetPlayer()->GetSelection();
	if(guid == 0)
	{
		SystemMessage(m_session, "No selection.");
		return true;
	}

	Creature* unit = m_session->GetPlayer()->GetMapMgr()->GetCreature(GET_LOWGUID_PART(guid));
	if(!unit)
	{
		SystemMessage(m_session, "You should select a creature.");
		return true;
	}
	if(unit->IsPet())
	{
		SystemMessage(m_session, "You can't delete a pet.");
		return true;
	}
	sGMLog.writefromsession(m_session, "used npc delete, sqlid %u, creature %s, pos %f %f %f", unit->GetSQL_id(), unit->GetCreatureInfo()->Name, unit->GetPositionX(), unit->GetPositionY(), unit->GetPositionZ());

	unit->GetAIInterface()->hideWayPoints(m_session->GetPlayer());

	unit->DeleteFromDB();

	if(unit->IsSummon())
	{
		unit->Delete();
	}
	else
	{

		if(unit->m_spawn)
		{
			uint32 cellx = uint32(((_maxX - unit->m_spawn->x) / _cellSize));
			uint32 celly = uint32(((_maxY - unit->m_spawn->y) / _cellSize));

			if(cellx <= _sizeX && celly <= _sizeY)
			{
				CellSpawns* sp = unit->GetMapMgr()->GetBaseMap()->GetSpawnsList(cellx, celly);
				if(sp != NULL)
				{
					for(CreatureSpawnList::iterator itr = sp->CreatureSpawns.begin(); itr != sp->CreatureSpawns.end(); ++itr)
						if((*itr) == unit->m_spawn)
						{
							sp->CreatureSpawns.erase(itr);
							break;
						}
				}
				delete unit->m_spawn;
				unit->m_spawn = NULL;
			}
		}

		unit->RemoveFromWorld(false, true);
	}

	BlueSystemMessage(m_session, "Creature deleted");

	return true;
}

bool ChatHandler::HandleItemCommand(const char* args, WorldSession* m_session)
{
	char* pitem = strtok((char*)args, " ");
	if(!pitem)
		return false;

	uint64 guid = m_session->GetPlayer()->GetSelection();
	if(guid == 0)
	{
		SystemMessage(m_session, "No selection.");
		return true;
	}

	Creature* pCreature = m_session->GetPlayer()->GetMapMgr()->GetCreature(GET_LOWGUID_PART(guid));
	if(!pCreature)
	{
		SystemMessage(m_session, "You should select a creature.");
		return true;
	}

	uint32 item = atoi(pitem);
	int amount = -1;

	char* pamount = strtok(NULL, " ");
	if(pamount)
		amount = atoi(pamount);

	if(amount == -1)
	{
		SystemMessage(m_session, "You need to specify an amount.");
		return true;
	}

	uint32 costid = 0;
	char* pcostid = strtok(NULL, " ");
	if(pcostid)
		costid = atoi(pcostid);

	ItemExtendedCostEntry* ec = (costid > 0) ? dbcItemExtendedCost.LookupEntryForced(costid) : NULL;
	if(costid > 0 && dbcItemExtendedCost.LookupEntryForced(costid) == NULL)
	{
		SystemMessage(m_session, "You've entered invalid extended cost id.");
		return true;
	}

	ItemPrototype* tmpItem = ItemPrototypeStorage.LookupEntry(item);

	std::stringstream sstext;
	if(tmpItem)
	{
		std::stringstream ss;
		ss << "INSERT INTO vendors VALUES ('" << pCreature->GetEntry() << "', '" << item << "', '" << amount << "', 0, 0, " << costid << " )" << '\0';
		WorldDatabase.Execute(ss.str().c_str());

		pCreature->AddVendorItem(item, amount, ec);

		sstext << "Item '" << item << "' '" << tmpItem->Name1 << "' Added to list";
		if(costid > 0)
			sstext << "with extended cost " << costid;
		sstext << '\0';
	}
	else
	{
		sstext << "Item '" << item << "' Not Found in Database." << '\0';
	}

	sGMLog.writefromsession(m_session, "added item %u to vendor %u", item, pCreature->GetEntry());
	SystemMessage(m_session,  sstext.str().c_str());

	return true;
}

bool ChatHandler::HandleItemRemoveCommand(const char* args, WorldSession* m_session)
{
	char* iguid = strtok((char*)args, " ");
	if(!iguid)
		return false;

	uint64 guid = m_session->GetPlayer()->GetSelection();
	if(guid == 0)
	{
		SystemMessage(m_session, "No selection.");
		return true;
	}

	Creature* pCreature = m_session->GetPlayer()->GetMapMgr()->GetCreature(GET_LOWGUID_PART(guid));
	if(!pCreature)
	{
		SystemMessage(m_session, "You should select a creature.");
		return true;
	}

	uint32 itemguid = atoi(iguid);
	int slot = pCreature->GetSlotByItemId(itemguid);

	std::stringstream sstext;
	if(slot != -1)
	{
		uint32 creatureId = pCreature->GetEntry();

		std::stringstream ss;
		ss << "DELETE FROM vendors WHERE entry = " << creatureId << " AND item = " << itemguid << '\0';
		WorldDatabase.Execute(ss.str().c_str());

		pCreature->RemoveVendorItem(itemguid);
		ItemPrototype* tmpItem = ItemPrototypeStorage.LookupEntry(itemguid);
		if(tmpItem)
		{
			sstext << "Item '" << itemguid << "' '" << tmpItem->Name1 << "' Deleted from list" << '\0';
		}
		else
		{
			sstext << "Item '" << itemguid << "' Deleted from list" << '\0';
		}
		sGMLog.writefromsession(m_session, "removed item %u from vendor %u", itemguid, creatureId);
	}
	else
	{
		sstext << "Item '" << itemguid << "' Not Found in List." << '\0';
	}

	SystemMessage(m_session, sstext.str().c_str());

	return true;
}

bool ChatHandler::HandleNPCFlagCommand(const char* args, WorldSession* m_session)
{
	if(!*args)
		return false;

	uint32 npcFlags = (uint32) atoi((char*)args);

	uint64 guid = m_session->GetPlayer()->GetSelection();
	if(guid == 0)
	{
		SystemMessage(m_session, "No selection.");
		return true;
	}

	Creature* pCreature = m_session->GetPlayer()->GetMapMgr()->GetCreature(GET_LOWGUID_PART(guid));
	if(!pCreature)
	{
		SystemMessage(m_session, "You should select a creature.");
		return true;
	}

	pCreature->SetUInt32Value(UNIT_NPC_FLAGS , npcFlags);
	WorldDatabase.Execute("UPDATE creature_proto SET npcflags = '%lu' WHERE entry = %lu", npcFlags, pCreature->GetProto()->Id);
	SystemMessage(m_session, "Value saved, you may need to rejoin or clean your client cache.");

	sGMLog.writefromsession(m_session, "changed npc flags of creature %u [%s] to %u", pCreature->GetEntry(), pCreature->GetCreatureInfo()->Name, npcFlags);

	return true;
}

bool ChatHandler::HandleSaveAllCommand(const char* args, WorldSession* m_session)
{
	PlayerStorageMap::const_iterator itr;
	uint32 stime = now();
	uint32 count = 0;
	objmgr._playerslock.AcquireReadLock();
	for(itr = objmgr._players.begin(); itr != objmgr._players.end(); ++itr)
	{
		if(itr->second->GetSession())
		{
			itr->second->SaveToDB(false);
			count++;
		}
	}
	objmgr._playerslock.ReleaseReadLock();
	char msg[100];
	snprintf(msg, 100, "Saved all %u online players in %u msec.", count, now() - stime);
	sWorld.SendWorldText(msg);
	sWorld.SendWorldWideScreenText(msg);
	sGMLog.writefromsession(m_session, "saved all players");
	//sWorld.SendIRCMessage(msg);
	return true;
}

bool ChatHandler::HandleMonsterSayCommand(const char* args, WorldSession* m_session)
{
	Unit* crt = getSelectedCreature(m_session, false);
	if(!crt)
		crt = getSelectedChar(m_session, false);

	if(!crt)
	{
		RedSystemMessage(m_session, "Please select a creature or player before using this command.");
		return true;
	}
	if(crt->IsPlayer())
	{
		WorldPacket* data = this->FillMessageData(CHAT_MSG_SAY, LANG_UNIVERSAL, args, crt->GetGUID(), 0);
		crt->SendMessageToSet(data, true);
		delete data;
	}
	else
	{
		crt->SendChatMessage(CHAT_MSG_MONSTER_SAY, LANG_UNIVERSAL, args);
	}

	return true;
}

bool ChatHandler::HandleMonsterYellCommand(const char* args, WorldSession* m_session)
{
	Unit* crt = getSelectedCreature(m_session, false);
	if(!crt)
		crt = getSelectedChar(m_session, false);

	if(!crt)
	{
		RedSystemMessage(m_session, "Please select a creature or player before using this command.");
		return true;
	}
	if(crt->IsPlayer())
	{
		WorldPacket* data = this->FillMessageData(CHAT_MSG_YELL, LANG_UNIVERSAL, args, crt->GetGUID(), 0);
		crt->SendMessageToSet(data, true);
		delete data;
	}
	else
	{
		crt->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, args);
	}

	return true;
}

bool ChatHandler::HandleReviveStringCommand(const char* args, WorldSession* m_session)
{
	Player* plr = objmgr.GetPlayer(args, false);
	if(!plr)
	{
		RedSystemMessage(m_session, "Could not find player %s.", args);
		return true;
	}

	if(plr->IsDead())
	{
		if(plr->GetInstanceID() == m_session->GetPlayer()->GetInstanceID())
			plr->RemoteRevive();
		else
			sEventMgr.AddEvent(plr, &Player::RemoteRevive, EVENT_PLAYER_REST, 1, 1, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);

		GreenSystemMessage(m_session, "Revived player %s.", args);
		sGMLog.writefromsession(m_session, "revived player %s", args);
	}
	else
	{
		GreenSystemMessage(m_session, "Player %s is not dead.", args);
	}
	return true;
}

bool ChatHandler::HandleAddAIAgentCommand(const char* args, WorldSession* m_session)
{
	char* agent = strtok((char*)args, " ");
	if(!agent)
		return false;
	char* procEvent = strtok(NULL, " ");
	if(!procEvent)
		return false;
	char* procChance = strtok(NULL, " ");
	if(!procChance)
		return false;
	char* procCount = strtok(NULL, " ");
	if(!procCount)
		return false;
	char* spellId = strtok(NULL, " ");
	if(!spellId)
		return false;
	char* spellType = strtok(NULL, " ");
	if(!spellType)
		return false;
	char* spelltargetType = strtok(NULL, " ");
	if(!spelltargetType)
		return false;
	char* spellCooldown = strtok(NULL, " ");
	if(!spellCooldown)
		return false;
	char* floatMisc1 = strtok(NULL, " ");
	if(!floatMisc1)
		return false;
	char* Misc2 = strtok(NULL, " ");
	if(!Misc2)
		return false;

	Creature* target = m_session->GetPlayer()->GetMapMgr()->GetCreature(GET_LOWGUID_PART(m_session->GetPlayer()->GetSelection()));
	if(!target)
	{
		RedSystemMessage(m_session, "You have to select a Creature!");
		return false;
	}

	std::stringstream qry;
	qry << "INSERT INTO ai_agents SET entry = '" << target->GetEntry() << "', type = '" << atoi(agent) << "', event = '" << atoi(procEvent) << "', chance = '" << atoi(procChance) << "', maxcount = '" << atoi(procCount) << "', spell = '" << atoi(spellId) << "', spelltype = '" << atoi(spellType) << "', targettype_overwrite = '" << atoi(spelltargetType) << "', cooldown_overwrite = '" << atoi(spellCooldown) << "', floatMisc1 = '" << atof(floatMisc1) << "', Misc2  ='" << atoi(Misc2) << "'";
	WorldDatabase.Execute(qry.str().c_str());

	AI_Spell* sp = new AI_Spell;
	sp->agent = static_cast<uint16>(atoi(agent));
	sp->procChance = atoi(procChance);
	/*	sp->procCount = atoi(procCount);*/
	sp->spell = dbcSpell.LookupEntry(atoi(spellId));
	sp->spellType = static_cast<uint8>(atoi(spellType));
//	sp->spelltargetType = atoi(spelltargetType);
	sp->floatMisc1 = (float)atof(floatMisc1);
	sp->Misc2 = (uint32)atof(Misc2);
	sp->cooldown = (uint32)atoi(spellCooldown);
	sp->procCount = 0;
	sp->procCounter = 0;
	sp->cooldowntime = 0;
	sp->minrange = GetMinRange(dbcSpellRange.LookupEntry(dbcSpell.LookupEntry(atoi(spellId))->rangeIndex));
	sp->maxrange = GetMaxRange(dbcSpellRange.LookupEntry(dbcSpell.LookupEntry(atoi(spellId))->rangeIndex));

	target->GetProto()->spells.push_back(sp);

	if(sp->agent == AGENT_CALLFORHELP)
		target->GetAIInterface()->m_canCallForHelp = true;
	else if(sp->agent == AGENT_FLEE)
		target->GetAIInterface()->m_canFlee = true;
	else if(sp->agent == AGENT_RANGED)
		target->GetAIInterface()->m_canRangedAttack = true;
	else
		target->GetAIInterface()->addSpellToList(sp);

	return true;
}

bool ChatHandler::HandleListAIAgentCommand(const char* args, WorldSession* m_session)
{
	Unit* target = m_session->GetPlayer()->GetMapMgr()->GetCreature(GET_LOWGUID_PART(m_session->GetPlayer()->GetSelection()));
	if(!target)
	{
		RedSystemMessage(m_session, "You have to select a Creature!");
		return false;
	}

	std::stringstream sstext;
	sstext << "agentlist of creature: " << target->GetGUID() << '\n';

	std::stringstream ss;
	ss << "SELECT * FROM ai_agents where entry=" << target->GetEntry();
	QueryResult* result = WorldDatabase.Query(ss.str().c_str());

	if(!result)
		return false;

	do
	{
		Field* fields = result->Fetch();
		sstext << "agent: "   << fields[1].GetUInt16()
		       << " | spellId: " << fields[5].GetUInt32()
		       << " | Event: "   << fields[2].GetUInt32()
		       << " | chance: "  << fields[3].GetUInt32()
		       << " | count: "   << fields[4].GetUInt32() << '\n';
	}
	while(result->NextRow());

	delete result;

	SendMultilineMessage(m_session, sstext.str().c_str());

	return true;
}

bool ChatHandler::HandleNpcComeCommand(const char* args, WorldSession* m_session)
{
	// moves npc to players location
	Player* plr = m_session->GetPlayer();
	Creature* crt = getSelectedCreature(m_session, true);
	if(!crt) return true;

	crt->GetAIInterface()->MoveTo(plr->GetPositionX(), plr->GetPositionY(), plr->GetPositionZ(), plr->GetOrientation());
	sGMLog.writefromsession(m_session, "used npc come command on %s, sqlid %u", crt->GetCreatureInfo()->Name, crt->GetSQL_id());
	return true;
}

bool ChatHandler::HandleNPCEquipOneCommand(const char* args, WorldSession* m_session)
{
	if(!args)
		return false;

	uint32 ItemID = atol(args);
	Creature* SelectedCreature = getSelectedCreature(m_session, false);
	if(!SelectedCreature)
	{
		m_session->SystemMessage("Please select a creature to modify.");
		return true;
	}

	m_session->SystemMessage("Creature: %s (%u) SpawnID: %u - Item1: %u.", SelectedCreature->GetCreatureInfo()->Name, SelectedCreature->GetProto()->Id, SelectedCreature->spawnid, SelectedCreature->GetEquippedItem(MELEE));

	if(ItemID == 0)
	{
		SelectedCreature->SetEquippedItem(MELEE, 0);
		SelectedCreature->SaveToDB();
		m_session->SystemMessage("Reset item 1 of %s (%u).", SelectedCreature->GetCreatureInfo()->Name, SelectedCreature->GetProto()->Id);
		return true;
	}

	ItemPrototype* ItemProvided = ItemPrototypeStorage.LookupEntry(ItemID);
	if(!ItemProvided)
	{
		m_session->SystemMessage("Item ID: %u does not exist.", ItemID);
		return true;
	}
	SelectedCreature->SetEquippedItem(MELEE, ItemID);
	SelectedCreature->SaveToDB();
	return true;
}

bool ChatHandler::HandleNPCEquipTwoCommand(const char* args, WorldSession* m_session)
{
	if(!args)
		return false;

	uint32 ItemID = atol(args);
	Creature* SelectedCreature = getSelectedCreature(m_session, false);
	if(!SelectedCreature)
	{
		m_session->SystemMessage("Please select a creature to modify.");
		return true;
	}

	m_session->SystemMessage("Creature: %s (%u) SpawnID: %u - Item2: %u.", SelectedCreature->GetCreatureInfo()->Name, SelectedCreature->GetProto()->Id, SelectedCreature->spawnid, SelectedCreature->GetEquippedItem(OFFHAND));

	if(ItemID == 0)
	{
		SelectedCreature->SetEquippedItem(OFFHAND, 0);
		SelectedCreature->SaveToDB();
		m_session->SystemMessage("Reset item 2 of %s (%u).", SelectedCreature->GetCreatureInfo()->Name, SelectedCreature->GetProto()->Id);
		return true;
	}

	ItemPrototype* ItemProvided = ItemPrototypeStorage.LookupEntry(ItemID);
	if(!ItemProvided)
	{
		m_session->SystemMessage("Item ID: %u does not exist.", ItemID);
		return true;
	}
	SelectedCreature->SetEquippedItem(OFFHAND, ItemID);
	SelectedCreature->SaveToDB();
	return true;
}

bool ChatHandler::HandleNPCEquipThreeCommand(const char* args, WorldSession* m_session)
{
	if(!args)
		return false;

	uint32 ItemID = atol(args);
	Creature* SelectedCreature = getSelectedCreature(m_session, false);
	if(!SelectedCreature)
	{
		m_session->SystemMessage("Please select a creature to modify.");
		return true;
	}

	m_session->SystemMessage("Creature: %s (%u) SpawnID: %u - Item3: %u.", SelectedCreature->GetCreatureInfo()->Name, SelectedCreature->GetProto()->Id, SelectedCreature->spawnid, SelectedCreature->GetEquippedItem(RANGED));

	if(ItemID == 0)
	{
		SelectedCreature->SetEquippedItem(RANGED, 0);
		SelectedCreature->SaveToDB();
		m_session->SystemMessage("Reset item 3 of %s (%u).", SelectedCreature->GetCreatureInfo()->Name, SelectedCreature->GetProto()->Id);
		return true;
	}

	ItemPrototype* ItemProvided = ItemPrototypeStorage.LookupEntry(ItemID);
	if(!ItemProvided)
	{
		m_session->SystemMessage("Item ID: %u does not exist.", ItemID);
		return true;
	}
	SelectedCreature->SetEquippedItem(RANGED, ItemID);
	SelectedCreature->SaveToDB();
	return true;
}

ARCEMU_INLINE void RepairItem2(Player* pPlayer, Item* pItem)
{
	pItem->SetDurabilityToMax();
	pItem->m_isDirty = true;
}
