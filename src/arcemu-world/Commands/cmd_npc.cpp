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
#include <git_version.h>
#include "../Chat.h"
#include "../ObjectMgr.h"
#include "../Master.h"

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

bool ChatHandler::HandleEmoteCommand(const char* args, WorldSession* m_session)
{
	uint32 emote = atoi((char*)args);
	Unit* target = this->getSelectedCreature(m_session);
	if(!target) return false;
	if(target) target->SetEmoteState(emote);

	return true;
}

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

struct UnitFlagNames
{
	uint32 Flag;
	const char* Name;
};

struct UnitDynFlagNames
{
	uint32 Flag;
	const char* Name;
};

static const char* POWERTYPE[] =
{
	"Mana",
	"Rage",
	"Focus",
	"Energy",
	"Happiness",
	"Runes",
	"Runic Power"
};

static const UnitFlagNames UnitFlagToName[] =
{
	{ UNIT_FLAG_UNKNOWN_1, "UNIT_FLAG_UNKNOWN_1" },
	{ UNIT_FLAG_NOT_ATTACKABLE_2, "UNIT_FLAG_NOT_ATTACKABLE_2" },
	{ UNIT_FLAG_LOCK_PLAYER, "UNIT_FLAG_LOCK_PLAYER" },
	{ UNIT_FLAG_PVP_ATTACKABLE, "UNIT_FLAG_PVP_ATTACKABLE" },
	{ UNIT_FLAG_UNKNOWN_5, "UNIT_FLAG_UNKNOWN_5" },
	{ UNIT_FLAG_NO_REAGANT_COST, "UNIT_FLAG_NO_REAGANT_COST" },
	{ UNIT_FLAG_PLUS_MOB, "UNIT_FLAG_PLUS_MOB" },
	{ UNIT_FLAG_UNKNOWN_8, "UNIT_FLAG_UNKNOWN_8" },
	{ UNIT_FLAG_NOT_ATTACKABLE_9, "UNIT_FLAG_NOT_ATTACKABLE_9" },
	{ UNIT_FLAG_UNKNOWN_10, "UNIT_FLAG_UNKNOWN_10" },
	{ UNIT_FLAG_LOOTING, "UNIT_FLAG_LOOTING" },
	{ UNIT_FLAG_SELF_RES, "UNIT_FLAG_SELF_RES" },
	{ UNIT_FLAG_PVP, "UNIT_FLAG_PVP" },
	{ UNIT_FLAG_SILENCED, "UNIT_FLAG_SILENCED" },
	{ UNIT_FLAG_DEAD, "UNIT_FLAG_DEAD" },
	{ UNIT_FLAG_UNKNOWN_16, "UNIT_FLAG_UNKNOWN_16" },
	{ UNIT_FLAG_ALIVE, "UNIT_FLAG_ALIVE" },
	{ UNIT_FLAG_PACIFIED, "UNIT_FLAG_PACIFIED" },
	{ UNIT_FLAG_STUNNED, "UNIT_FLAG_STUNNED" },
	{ UNIT_FLAG_COMBAT, "UNIT_FLAG_COMBAT" },
	{ UNIT_FLAG_MOUNTED_TAXI, "UNIT_FLAG_MOUNTED_TAXI" },
	{ UNIT_FLAG_DISARMED, "UNIT_FLAG_DISARMED" },
	{ UNIT_FLAG_CONFUSED, "UNIT_FLAG_CONFUSED" },
	{ UNIT_FLAG_FLEEING, "UNIT_FLAG_FLEEING" },
	{ UNIT_FLAG_PLAYER_CONTROLLED_CREATURE, "UNIT_FLAG_PLAYER_CONTROLLED_CREATURE" },
	{ UNIT_FLAG_NOT_SELECTABLE, "UNIT_FLAG_NOT_SELECTABLE" },
	{ UNIT_FLAG_SKINNABLE, "UNIT_FLAG_SKINNABLE" },
	{ UNIT_FLAG_MOUNT, "UNIT_FLAG_MOUNT" },
	{ UNIT_FLAG_UNKNOWN_29, "UNIT_FLAG_UNKNOWN_29" },
	{ UNIT_FLAG_FEIGN_DEATH, "UNIT_FLAG_FEIGN_DEATH" },
	{ UNIT_FLAG_UNKNOWN_31, "UNIT_FLAG_UNKNOWN_31" },
	{ UNIT_FLAG_UNKNOWN_32, "UNIT_FLAG_UNKNOWN_32" }
};

static uint32 numflags = sizeof(UnitFlagToName) / sizeof(UnitFlagNames);

static const UnitDynFlagNames UnitDynFlagToName[] =
{
	{ U_DYN_FLAG_LOOTABLE, "U_DYN_FLAG_LOOTABLE" },
	{ U_DYN_FLAG_UNIT_TRACKABLE, "U_DYN_FLAG_UNIT_TRACKABLE" },
	{ U_DYN_FLAG_TAGGED_BY_OTHER, "U_DYN_FLAG_TAGGED_BY_OTHER" },
	{ U_DYN_FLAG_TAPPED_BY_PLAYER, "U_DYN_FLAG_TAPPED_BY_PLAYER" },
	{ U_DYN_FLAG_PLAYER_INFO, "U_DYN_FLAG_PLAYER_INFO" },
	{ U_DYN_FLAG_DEAD, "U_DYN_FLAG_DEAD" }
};

static uint32 numdynflags = sizeof(UnitDynFlagToName) / sizeof(UnitDynFlagNames);

static const char* GENDER[] =
{
	"male",
	"female",
	"neutral"
};

static const char* CLASS[] =
{
	"invalid 0",
	"warrior",
	"paladin",
	"hunter",
	"rogue",
	"priest",
	"deathknight",
	"shaman",
	"mage",
	"warlock",
	"invalid 10",
	"druid"
};

static const char* SHEATSTATE[] =
{
	"none",
	"melee",
	"Ranged"
};

struct UnitPvPFlagNames
{
	uint32 Flag;
	const char* Name;
};

static const UnitPvPFlagNames UnitPvPFlagToName[] =
{
	{ U_FIELD_BYTES_FLAG_PVP, "U_FIELD_BYTES_FLAG_PVP" },
	{ U_FIELD_BYTES_FLAG_FFA_PVP, "U_FIELD_BYTES_FLAG_FFA_PVP" },
	{ U_FIELD_BYTES_FLAG_SANCTUARY, "U_FIELD_BYTES_FLAG_SANCTUARY" }
};

struct PetFlagNames
{
	uint32 Flag;
	const char* Name;
};

static const PetFlagNames PetFlagToName[] =
{
	{ UNIT_CAN_BE_RENAMED, "UNIT_CAN_BE_RENAMED" },
	{ UNIT_CAN_BE_ABANDONED, "UNIT_CAN_BE_ABANDONED" }
};

static const uint32 numpetflags = sizeof(PetFlagToName) / sizeof(PetFlagNames);

static const uint32 numpvpflags = sizeof(UnitPvPFlagToName) / sizeof(UnitPvPFlagNames);

bool ChatHandler::HandleNpcInfoCommand(const char* args, WorldSession* m_session)
{

	uint32 guid = Arcemu::Util::GUID_LOPART(m_session->GetPlayer()->GetSelection());
	Creature* crt = getSelectedCreature(m_session);
	if(!crt) return false;
	BlueSystemMessage(m_session, "Showing creature info for %s", crt->GetCreatureInfo()->Name);
	SystemMessage(m_session, "GUID: %d", guid);
	SystemMessage(m_session, "Faction: %d", crt->GetFaction());
	SystemMessage(m_session, "Phase: %u", crt->GetPhase());
	{
		string s = "";
		if(crt->isBattleMaster())
			s.append(" (Battlemaster)");
		if(crt->isTrainer())
			s.append(" (Trainer)");
		if(crt->isProf())
			s.append(" (Profession Trainer)");
		if(crt->isQuestGiver())
			s.append(" (Quests)");
		if(crt->isGossip())
			s.append(" (Gossip)");
		if(crt->isTaxi())
			s.append(" (Taxi)");
		if(crt->isCharterGiver())
			s.append(" (Charter)");
		if(crt->isGuildBank())
			s.append(" (Guild Bank)");
		if(crt->isSpiritHealer())
			s.append(" (Spirit Healer)");
		if(crt->isInnkeeper())
			s.append(" (Innkeeper)");
		if(crt->isTabardDesigner())
			s.append(" (Tabard Designer)");
		if(crt->isAuctioner())
			s.append(" (Auctioneer)");
		if(crt->isStableMaster())
			s.append(" (Stablemaster)");
		if(crt->isArmorer())
			s.append(" (Armorer)");

		SystemMessage(m_session, "NPCFlags: %d%s", crt->GetUInt32Value(UNIT_NPC_FLAGS), s.c_str());
	}
	SystemMessage(m_session, "DisplayID: %u", crt->GetDisplayId() );
	SystemMessage(m_session, "VehicleID: %u", crt->GetProto()->vehicleid );

	if(crt->m_faction)
		SystemMessage(m_session, "Combat Support: 0x%.3X", crt->m_faction->FriendlyMask);
	SystemMessage(m_session, "Health (cur/max): %d/%d", crt->GetHealth(), crt->GetMaxHealth());

	uint32 powertype = crt->GetPowerType();
	if(powertype <= 6)
	{
		SystemMessage(m_session, "Powertype: %s", POWERTYPE[ powertype ]);
		SystemMessage(m_session, "Power (cur/max): %d/%d", crt->GetPower(powertype), crt->GetMaxPower(powertype));
	}

	SystemMessage(m_session, "Armor/Holy/Fire/Nature/Frost/Shadow/Arcane");
	SystemMessage(m_session, "%d/%d/%d/%d/%d/%d/%d", crt->GetResistance(SCHOOL_NORMAL), crt->GetResistance(SCHOOL_HOLY), crt->GetResistance(SCHOOL_FIRE), crt->GetResistance(SCHOOL_NATURE), crt->GetResistance(SCHOOL_FROST), crt->GetResistance(SCHOOL_SHADOW), crt->GetResistance(SCHOOL_ARCANE));
	SystemMessage(m_session, "Damage (min/max): %f/%f", crt->GetMinDamage(), crt->GetMaxDamage());

	ColorSystemMessage(m_session, MSG_COLOR_RED, "Entry ID: %d", crt->GetEntry());
	ColorSystemMessage(m_session, MSG_COLOR_RED, "Spawn ID: %d", crt->GetSQL_id());
	// show byte
	std::stringstream sstext;
	uint32 theBytes = crt->GetUInt32Value(UNIT_FIELD_BYTES_0);
	sstext << "UNIT_FIELD_BYTES_0 are " << uint16((uint8)theBytes & 0xFF) << " " << uint16((uint8)(theBytes >> 8) & 0xFF) << " ";
	sstext << uint16((uint8)(theBytes >> 16) & 0xFF) << " " << uint16((uint8)(theBytes >> 24) & 0xFF) << '\n';

	theBytes = crt->GetUInt32Value(UNIT_FIELD_BYTES_1);
	sstext << "UNIT_FIELD_BYTES_1 are " << uint16((uint8)theBytes & 0xFF) << " " << uint16((uint8)(theBytes >> 8) & 0xFF) << " ";
	sstext << uint16((uint8)(theBytes >> 16) & 0xFF) << " " << uint16((uint8)(theBytes >> 24) & 0xFF) << '\n';

	theBytes = crt->GetUInt32Value(UNIT_FIELD_BYTES_2);
	sstext << "UNIT_FIELD_BYTES_2 are " << uint16((uint8)theBytes & 0xFF) << " " << uint16((uint8)(theBytes >> 8) & 0xFF) << " ";
	sstext << uint16((uint8)(theBytes >> 16) & 0xFF) << " " << uint16((uint8)(theBytes >> 24) & 0xFF) << '\0';
	SendMultilineMessage(m_session, sstext.str().c_str());

	uint8 gender = crt->getGender();
	if(gender <= 2)
		SystemMessage(m_session, "Gender: %s", GENDER[ gender ]);
	else
		SystemMessage(m_session, "Gender: invalid %u", gender);

	uint8 crclass = crt->getClass();
	if(crclass <= 11)
		SystemMessage(m_session, "Class: %s", CLASS[ crclass ]);
	else
		SystemMessage(m_session, "Class: invalid %u", crclass);

	SystemMessage(m_session, "Free pet talent points: %u", crt->GetByte(UNIT_FIELD_BYTES_1, 1));

	uint8 sheat = crt->GetByte(UNIT_FIELD_BYTES_2, 0);
	if(sheat <= 2)
		SystemMessage(m_session, "Sheat state: %s", SHEATSTATE[ sheat ]);

	uint8 pvpflags = crt->GetByte(UNIT_FIELD_BYTES_2, 1);

	SystemMessage(m_session, "PvP flags: %u", pvpflags);

	for(uint32 i = 0; i < numpvpflags; i++)
		if((pvpflags & UnitPvPFlagToName[ i ].Flag) != 0)
			SystemMessage(m_session, "%s", UnitPvPFlagToName[ i ].Name);

	uint8 petflags = crt->GetByte(UNIT_FIELD_BYTES_2, 2);

	SystemMessage(m_session, "Pet flags: %u", petflags);

	for(uint32 i = 0; i < numpetflags; i++)
		if((petflags & PetFlagToName[ i ].Flag) != 0)
			SystemMessage(m_session, "%s", PetFlagToName[ i ].Name);

	if(crt->CombatStatus.IsInCombat())
		SystemMessage(m_session, "Creature is in combat");
	else
		SystemMessage(m_session, "Creature is NOT in combat");

	Unit* owner = NULL;
	if(crt->IsSummon())
		owner = TO< Summon* >(crt)->GetOwner();

	if(owner != NULL)
	{
		if(owner->IsPlayer())
			SystemMessage(m_session, "Owner is a %s", "player");
		if(owner->IsPet())
			SystemMessage(m_session, "Owner is a %s", "pet");
		if(owner->IsCreature())
			SystemMessage(m_session, "Owner is a %s", "creature");
	}

	SystemMessage(m_session, "Creator GUID: %u", Arcemu::Util::GUID_LOPART(crt->GetCreatedByGUID()));
	SystemMessage(m_session, "Summoner GUID: %u", Arcemu::Util::GUID_LOPART(crt->GetSummonedByGUID()));
	SystemMessage(m_session, "Charmer GUID: %u", Arcemu::Util::GUID_LOPART(crt->GetCharmedByGUID()));
	SystemMessage(m_session, "Creator Spell: %u", Arcemu::Util::GUID_LOPART(crt->GetCreatedBySpell()));



	uint32 unitflags = crt->GetUInt32Value(UNIT_FIELD_FLAGS);

	SystemMessage(m_session, "Unit flags: %u", unitflags);

	for(uint32 i = 0; i < numflags; i++)
		if((unitflags & UnitFlagToName[ i ].Flag) != 0)
			SystemMessage(m_session, "%s", UnitFlagToName[ i ].Name);

	uint32 dynflags = crt->GetUInt32Value(UNIT_DYNAMIC_FLAGS);
	SystemMessage(m_session, "Unit dynamic flags: %u", dynflags);

	for(uint32 i = 0; i < numdynflags; i++)
		if((dynflags & UnitDynFlagToName[ i ].Flag) != 0)
			SystemMessage(m_session, "%s", UnitDynFlagToName[ i ].Name);

	return true;
}

bool ChatHandler::HandleCreaturePhaseCommand(const char* args, WorldSession* m_session)
{
	char* sPhase = strtok((char*)args, " ");
	if(!sPhase)
		return false;

	uint32 newphase = atoi(sPhase);

	bool Save = false;
	char* pSave = strtok(NULL, " ");
	if(pSave)
		Save = (atoi(pSave) > 0 ? true : false);

	Creature* crt = getSelectedCreature(m_session);
	if(!crt) return false;

	crt->Phase(PHASE_SET, newphase);
	if(crt->m_spawn)
		crt->m_spawn->phase = newphase;
	//VLack: at this point we don't care if it has a spawn or not, as it gets one for sure in SaveToDB, that's why we don't return here from within an else statement.
	//I made this comment in case someone compares this code with the HandleGOPhaseCommand code where we have to have a spawn to be able to save it.

	// Save it to the database.
	if(Save)
	{
		crt->SaveToDB();
		crt->m_loadedFromDB = true;
	}

	sGMLog.writefromsession(m_session, "phased creature with entry %u to %u", crt->GetEntry(), newphase);

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

bool ChatHandler::HandleNpcReturnCommand(const char* args, WorldSession* m_session)
{
	Creature* creature = getSelectedCreature(m_session);
	if(!creature || !creature->m_spawn) return true;

	// return to respawn coords
	float x = creature->m_spawn->x;
	float y = creature->m_spawn->y;
	float z = creature->m_spawn->z;
	float o = creature->m_spawn->o;

	// restart movement
	creature->GetAIInterface()->SetAIState(STATE_IDLE);
	creature->GetAIInterface()->WipeHateList();
	creature->GetAIInterface()->WipeTargetList();
	creature->GetAIInterface()->MoveTo(x, y, z, o);

	sGMLog.writefromsession(m_session, "returned NPC %s, sqlid %u", creature->GetCreatureInfo()->Name, creature->GetSQL_id());

	return true;
}

bool ChatHandler::HandleCreatureSpawnCommand(const char* args, WorldSession* m_session)
{
	uint32 entry = atol(args);
	uint8 gender = 0;

	if(entry == 0)
		return false;

	CreatureProto* proto = CreatureProtoStorage.LookupEntry(entry);
	CreatureInfo* info = CreatureNameStorage.LookupEntry(entry);
	if(proto == NULL || info == NULL)
	{
		RedSystemMessage(m_session, "Invalid entry id.");
		return true;
	}

	CreatureSpawn* sp = new CreatureSpawn;
	//sp->displayid = info->DisplayID;
	gender = info->GenerateModelId(&sp->displayid);
	sp->entry = entry;
	sp->form = 0;
	sp->id = objmgr.GenerateCreatureSpawnID();
	sp->movetype = 0;
	sp->x = m_session->GetPlayer()->GetPositionX();
	sp->y = m_session->GetPlayer()->GetPositionY();
	sp->z = m_session->GetPlayer()->GetPositionZ();
	sp->o = m_session->GetPlayer()->GetOrientation();
	sp->emote_state = 0;
	sp->flags = 0;
	sp->factionid = proto->Faction;
	sp->bytes0 = sp->setbyte(0, 2, gender);
	sp->bytes1 = 0;
	sp->bytes2 = 0;
	//sp->respawnNpcLink = 0;
	sp->stand_state = 0;
	sp->death_state = 0;
	sp->channel_target_creature = sp->channel_target_go = sp->channel_spell = 0;
	sp->MountedDisplayID = 0;
	sp->Item1SlotDisplay = 0;
	sp->Item2SlotDisplay = 0;
	sp->Item3SlotDisplay = 0;
	sp->CanFly = 0;
	sp->phase = m_session->GetPlayer()->GetPhase();


	Creature* p = m_session->GetPlayer()->GetMapMgr()->CreateCreature(entry);
	ARCEMU_ASSERT(p != NULL);
	p->Load(sp, (uint32)NULL, NULL);
	p->m_loadedFromDB = true;
	p->PushToWorld(m_session->GetPlayer()->GetMapMgr());

	uint32 x = m_session->GetPlayer()->GetMapMgr()->GetPosX(m_session->GetPlayer()->GetPositionX());
	uint32 y = m_session->GetPlayer()->GetMapMgr()->GetPosY(m_session->GetPlayer()->GetPositionY());

	// Add spawn to map
	m_session->GetPlayer()->GetMapMgr()->GetBaseMap()->GetSpawnsListAndCreate(
	    x,
	    y)->CreatureSpawns.push_back(sp);

	MapCell* mCell = m_session->GetPlayer()->GetMapMgr()->GetCell(x, y);

	if(mCell != NULL)
		mCell->SetLoaded();

	BlueSystemMessage(m_session, "Spawned a creature `%s` with entry %u at %f %f %f on map %u", info->Name,
	                  entry, sp->x, sp->y, sp->z, m_session->GetPlayer()->GetMapId());

	// Save it to the database.
	p->SaveToDB();

	sGMLog.writefromsession(m_session, "spawned a %s at %u %f %f %f", info->Name, m_session->GetPlayer()->GetMapId(), sp->x, sp->y, sp->z);

	return true;
}

bool ChatHandler::HandleCreatureRespawnCommand(const char* args, WorldSession* m_session)
{
	Creature* cCorpse = getSelectedCreature(m_session, false);

	if(cCorpse != NULL && cCorpse->IsCreature() && cCorpse->getDeathState() == CORPSE && cCorpse->GetSQL_id() != 0)
	{
		sEventMgr.RemoveEvents(cCorpse, EVENT_CREATURE_RESPAWN);

		BlueSystemMessage(m_session, "Respawning a Creature: `%s` with entry: %u on map: %u sqlid: %u", cCorpse->GetCreatureInfo()->Name,
		                  cCorpse->GetEntry(), cCorpse->GetMapMgr()->GetMapId(), cCorpse->GetSQL_id());

		sGMLog.writefromsession(m_session, "Respawned a Creature: `%s` with entry: %u on map: %u sqlid: %u", cCorpse->GetCreatureInfo()->Name,
		                        cCorpse->GetEntry(), cCorpse->GetMapMgr()->GetMapId(), cCorpse->GetSQL_id());

		cCorpse->Despawn(0, 1000);
		return true;
	}

	RedSystemMessage(m_session, "You must select a creature's corpse with a valid CreatureSpawn point.");
	return false;
}

bool ChatHandler::HandleNpcSpawnLinkCommand(const char* args, WorldSession* m_session)
{
	uint32 id;
	char sql[512];
	Creature* target = m_session->GetPlayer()->GetMapMgr()->GetCreature(GET_LOWGUID_PART(m_session->GetPlayer()->GetSelection()));
	if(!target)
		return false;

	int valcount = sscanf(args, "%u", (unsigned int*)&id);
	if(valcount == 1)
	{
		snprintf(sql, 512, "UPDATE creature_spawns SET npc_respawn_link = '%u' WHERE id = '%u'", (unsigned int)id, (unsigned int)target->GetSQL_id());
		WorldDatabase.Execute(sql);
		BlueSystemMessage(m_session, "Spawn linking for this NPC has been updated: %u", id);
	}
	else
	{
		RedSystemMessage(m_session, "Sql entry invalid %u", id);
	}

	return true;
}

bool ChatHandler::HandleNpcPossessCommand(const char* args, WorldSession* m_session)
{
	Unit* pTarget = getSelectedChar(m_session, false);
	if(!pTarget)
	{
		pTarget = getSelectedCreature(m_session, false);
		if(pTarget && (pTarget->IsPet() || pTarget->GetCreatedByGUID() != 0))
			return false;
	}

	if(!pTarget)
	{
		RedSystemMessage(m_session, "You must select a player/creature.");
		return true;
	}

	m_session->GetPlayer()->Possess(pTarget->GetGUID());
	BlueSystemMessage(m_session, "Possessed " I64FMT, pTarget->GetGUID());
	switch(pTarget->GetTypeId())
	{
		case TYPEID_PLAYER:
			sGMLog.writefromsession(m_session, "used possess command on PLAYER %s", TO< Player* >(pTarget)->GetName());
			break;
		case TYPEID_UNIT:
			sGMLog.writefromsession(m_session, "used possess command on CREATURE %s, sqlid %u", TO< Creature* >(pTarget)->GetCreatureInfo()->Name, TO< Creature* >(pTarget)->GetSQL_id());
			break;
	}
	return true;
}

bool ChatHandler::HandleNpcUnPossessCommand(const char* args, WorldSession* m_session)
{
	Creature* creature = getSelectedCreature(m_session);
	m_session->GetPlayer()->UnPossess();

	if(creature)
	{
		// restart movement
		creature->GetAIInterface()->SetAIState(STATE_IDLE);
		creature->GetAIInterface()->WipeHateList();
		creature->GetAIInterface()->WipeTargetList();

		if(creature->m_spawn)
		{
			// return to respawn coords
			float x = creature->m_spawn->x;
			float y = creature->m_spawn->y;
			float z = creature->m_spawn->z;
			float o = creature->m_spawn->o;
			creature->GetAIInterface()->MoveTo(x, y, z, o);
		}
	}
	GreenSystemMessage(m_session, "Removed any possessed targets.");
	sGMLog.writefromsession(m_session, "used unpossess command");
	return true;
}

bool ChatHandler::HandleNpcSelectCommand(const char* args, WorldSession* m_session)
{
	Creature* un = NULL;
	float dist = 999999.0f;
	float dist2;
	Player* plr = m_session->GetPlayer();
	set<Object*>::iterator itr;
	for(itr = plr->GetInRangeSetBegin(); itr != plr->GetInRangeSetEnd(); ++itr)
	{
		if((dist2 = plr->GetDistance2dSq(*itr)) < dist && (*itr)->IsCreature())
		{
			un = TO_CREATURE(*itr);
			dist = dist2;
		}
	}

	if(!un)
	{
		SystemMessage(m_session, "No inrange creatures found.");
		return true;
	}

	plr->SetSelection(un->GetGUID());
	SystemMessage(m_session, "Set selection to " I64FMT " (%s)", un->GetGUID(), un->GetCreatureInfo()->Name);
	return true;
}

bool ChatHandler::HandleNpcFollowCommand(const char* args, WorldSession* m_session)
{
	Creature* creature = getSelectedCreature(m_session, true);
	if(!creature) return true;

	creature->GetAIInterface()->SetUnitToFollow(m_session->GetPlayer());

	sGMLog.writefromsession(m_session, "used npc follow command on %s, sqlid %u", creature->GetCreatureInfo()->Name, creature->GetSQL_id());
	return true;
}

bool ChatHandler::HandleNullFollowCommand(const char* args, WorldSession* m_session)
{
	Creature* c = getSelectedCreature(m_session, true);
	if(!c) return true;

	// restart movement
	c->GetAIInterface()->SetAIState(STATE_IDLE);
	c->GetAIInterface()->ResetUnitToFollow();

	sGMLog.writefromsession(m_session, "cancelled npc follow command on %s, sqlid %u", c->GetCreatureInfo()->Name, c->GetSQL_id());
	return true;
}

bool ChatHandler::HandleFormationLink1Command(const char* args, WorldSession* m_session)
{
	// set formation "master"
	Creature* pCreature = getSelectedCreature(m_session, true);
	if(pCreature == 0) return true;

	m_session->GetPlayer()->linkTarget = pCreature;
	BlueSystemMessage(m_session, "Linkup \"master\" set to %s.", pCreature->GetCreatureInfo()->Name);
	return true;
}

bool ChatHandler::HandleFormationLink2Command(const char* args, WorldSession* m_session)
{
	// set formation "slave" with distance and angle
	float ang, dist;
	if(*args == 0 || sscanf(args, "%f %f", &dist, &ang) != 2)
	{
		RedSystemMessage(m_session, "You must specify a distance and angle.");
		return true;
	}

	if(m_session->GetPlayer()->linkTarget == NULL || m_session->GetPlayer()->linkTarget->IsPet())
	{
		RedSystemMessage(m_session, "Master not selected. select the master, and use formationlink1.");
		return true;
	}

	Creature* slave = getSelectedCreature(m_session, true);
	if(slave == 0) return true;

	slave->GetAIInterface()->m_formationFollowDistance = dist;
	slave->GetAIInterface()->m_formationFollowAngle = ang;
	slave->GetAIInterface()->m_formationLinkTarget = m_session->GetPlayer()->linkTarget->GetGUID();
	slave->GetAIInterface()->m_formationLinkSqlId = m_session->GetPlayer()->linkTarget->GetSQL_id();
	slave->GetAIInterface()->SetUnitToFollowAngle(ang);

	// add to db
	WorldDatabase.Execute("INSERT INTO creature_formations VALUES(%u, %u, '%f', '%f')",
	                      slave->GetSQL_id(), slave->GetAIInterface()->m_formationLinkSqlId, ang, dist);

	BlueSystemMessage(m_session, "%s linked up to %s with a distance of %f at %f radians.", slave->GetCreatureInfo()->Name,
	                  m_session->GetPlayer()->linkTarget->GetCreatureInfo()->Name, dist, ang);

	return true;
}

bool ChatHandler::HandleFormationClearCommand(const char* args, WorldSession* m_session)
{
	Creature* c = getSelectedCreature(m_session, true);
	if(!c) return true;

	c->GetAIInterface()->m_formationLinkSqlId = 0;
	c->GetAIInterface()->m_formationLinkTarget = 0;
	c->GetAIInterface()->m_formationFollowAngle = 0.0f;
	c->GetAIInterface()->m_formationFollowDistance = 0.0f;
	c->GetAIInterface()->ResetUnitToFollow();

	WorldDatabase.Execute("DELETE FROM creature_formations WHERE spawn_id=%u", c->GetSQL_id());
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

bool ChatHandler::HandlePortToCreatureSpawnCommand(const char* args, WorldSession* m_session)
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
	QueryResult* QR = WorldDatabase.Query("SELECT * FROM creature_spawns WHERE id=%u", spawn_id);
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

bool ChatHandler::HandleNPCLootCommand(const char* args, WorldSession* m_session)
{
	Creature* pCreature = getSelectedCreature(m_session, true);
	if(pCreature == NULL)
	{
		return false;
	}

	QueryResult* _result = WorldDatabase.Query("SELECT itemid, normal10percentchance, heroic10percentchance, normal25percentchance, heroic25percentchance, mincount, maxcount FROM loot_creatures WHERE entryid=%u;", pCreature->GetEntry());
	if(_result != NULL)
	{
		Field* _field;
		std::stringstream ss;
		ItemPrototype* proto;
		string color;
		int32 minQuality = 0;
		uint8 numFound = 0;

		if(*args)
		{
			minQuality = atol(args);
		}

		do
		{
			_field = _result->Fetch();
			ss.str("");
			proto = ItemPrototypeStorage.LookupEntry(_field[0].GetUInt32());
			if(proto == NULL || (int32)proto->Quality < minQuality)
			{
				continue;
			}
			++numFound;
			ss << "(N10 " << _field[1].GetFloat() << "%%) (N25 " << _field[3].GetFloat() << "%%) (H10 " << _field[2].GetFloat() << "%%) (H25 " << _field[4].GetFloat() << "%%): ";

			switch(proto->Quality)
			{
				case 0: //Poor,gray
					color = "cff9d9d9d";
					break;
				case 1: //Common,white
					color = "cffffffff";
					break;
				case 2: //Uncommon,green
					color = "cff1eff00";
					break;
				case 3: //Rare,blue
					color = "cff0070dd";
					break;
				case 4: //Epic,purple
					color = "cffa335ee";
					break;
				case 5: //Legendary,orange
					color = "cffff8000";
					break;
				case 6: //Artifact,pale gold
					color = "c00fce080";
					break;
				case 7: //Heirloom,pale gold
					color = "c00fce080";
					break;
				default:
					color = "cff9d9d9d";
					break;
			}

			ss << "|" << color.c_str() << "|Hitem:" << proto->ItemId << ":0:0:0:0:0:0:0|h[" << proto->Name1;
			ss << "]|h|r (" << _field[5].GetUInt32() << "-" << _field[6].GetUInt32() << ")";
			SystemMessage(m_session, ss.str().c_str(), '%', '%');
		}
		while(_result->NextRow() && (numFound <= 25));
		delete _result;
		if(numFound > 25)
		{
			SystemMessage(m_session, "More than 25 results found.");
		}
		else
		{
			SystemMessage(m_session, "%lu results found.", numFound);
		}
	}
	else
	{
		SystemMessage(m_session, "0 results found.");
	}
	return true;
}

bool ChatHandler::HandleNPCCanFlyCommand(const char* args, WorldSession* m_session)
{
	Creature* pCreature = getSelectedCreature(m_session, true);
	if(pCreature == NULL)
		return true;
	if(pCreature->GetAIInterface()->Flying())
		pCreature->GetAIInterface()->StopFlying();
	else
		pCreature->GetAIInterface()->SetFly();

	pCreature->GetAIInterface()->onGameobject = false;
	char* sSave = strtok((char*)args, " ");
	if(sSave)
	{
		bool save = (atoi(sSave) > 0 ? true : false);
		if(save)
		{
			pCreature->SaveToDB();
			pCreature->m_loadedFromDB = true;
		}
	}
	GreenSystemMessage(m_session, "You may have to leave and re-enter this zone for changes to take effect.");
	return true;
}

bool ChatHandler::HandleNPCOnGOCommand(const char* args, WorldSession* m_session)
{
	Creature* pCreature = getSelectedCreature(m_session, true);
	if(pCreature == NULL)
		return true;

	pCreature->GetAIInterface()->StopFlying();

	pCreature->GetAIInterface()->onGameobject = !pCreature->GetAIInterface()->onGameobject;
	char* sSave = strtok((char*)args, " ");
	if(sSave)
	{
		bool save = (atoi(sSave) > 0 ? true : false);
		if(save)
		{
			pCreature->SaveToDB();
			pCreature->m_loadedFromDB = true;
		}
	}
	GreenSystemMessage(m_session, "You may have to leave and re-enter this zone for changes to take effect.");
	return true;
}

bool ChatHandler::HandleNPCCastCommand(const char* args, WorldSession* m_session)
{
	if(*args == '\0')
		return false;

	Creature* c = getSelectedCreature(m_session);
	if(c == NULL)
		return false;

	uint32 spellid = atol(args);
	if(spellid == 0)
		return false;

	SpellEntry* sp = dbcSpell.LookupEntry(spellid);
	if(sp == NULL)
		return false;

	c->CastSpell(reinterpret_cast< Unit* >(NULL), sp, false);

	return true;
}