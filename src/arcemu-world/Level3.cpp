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

/////////////////////////////////////////////////
//  Admin Chat Commands
//

#include "StdAfx.h"
#include "ObjectMgr.h"
#include "Master.h"

void ParseBanArgs(char* args, char** BanDuration, char** BanReason)
{
	// Usage: .ban character <char> [duration] [reason]
	//        .ban ip <ipaddr> [duration] [reason]
	//        .ban account <acct> [duration] [reason]
	//        .ban all <char> [duration] [reason]
	// Duration must be a number optionally followed by a character representing the calendar subdivision to use (h>hours, d>days, w>weeks, m>months, y>years, default minutes)
	// Lack of duration results in a permanent ban.
	char* pBanDuration = strchr(args, ' ');
	char* pReason = NULL;
	if(pBanDuration != NULL)
	{
		if(isdigit(*(pBanDuration + 1))) // this is the duration of the ban
		{
			*pBanDuration = 0; // NULL-terminate the first string (character/account/ip)
			++pBanDuration; // point to next arg
			pReason = strchr(pBanDuration + 1, ' ');
			if(pReason != NULL) // BanReason is OPTIONAL
			{
				*pReason = 0; // BanReason was given, so NULL-terminate the duration string
				++pReason; // and point to the ban reason
			}
		}
		else // no duration was given (didn't start with a digit) - so this arg must be ban reason and duration defaults to permanent
		{
			pReason = pBanDuration;
			pBanDuration = NULL;
			*pReason = 0;
			++pReason;
		}
	}
	*BanDuration = pBanDuration;
	*BanReason = pReason;
}

int32 GetSpellIDFromLink(const char* spelllink)
{
	if(spelllink == NULL)
		return 0;

	const char* ptr = strstr(spelllink, "|Hspell:");
	if(ptr == NULL)
	{
		return 0;
	}

	return atol(ptr + 8); // spell id is just past "|Hspell:" (8 bytes)
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

bool ChatHandler::HandleReviveCommand(const char* args, WorldSession* m_session)
{
	Player* SelectedPlayer = getSelectedChar(m_session, true);
	if(!SelectedPlayer)
		return true;

	SelectedPlayer->SetMovement(MOVE_UNROOT, 1);
	SelectedPlayer->ResurrectPlayer();
	SelectedPlayer->SetHealth(SelectedPlayer->GetMaxHealth());
	SelectedPlayer->SetPower(POWER_TYPE_MANA, SelectedPlayer->GetMaxPower(POWER_TYPE_MANA));
	SelectedPlayer->SetPower(POWER_TYPE_ENERGY, SelectedPlayer->GetMaxPower(POWER_TYPE_ENERGY));


	if(SelectedPlayer != m_session->GetPlayer())
		sGMLog.writefromsession(m_session, "revived player %s", SelectedPlayer->GetName());

	return true;
}

bool ChatHandler::HandleAddSkillCommand(const char* args, WorldSession* m_session)
{
	char buf[256];
	Player* target = objmgr.GetPlayer((uint32)m_session->GetPlayer()->GetSelection());

	if(!target)
	{
		SystemMessage(m_session, "Select A Player first.");
		return true;
	}

	uint32 skillline;
	uint16 cur, max;

	char* pSkillline = strtok((char*)args, " ");
	if(!pSkillline)
		return false;

	char* pCurrent = strtok(NULL, " ");
	if(!pCurrent)
		return false;

	char* pMax = strtok(NULL, " ");
	if(!pMax)
		return false;

	skillline = (uint32)atol(pSkillline);
	cur = (uint16)atol(pCurrent);
	max = (uint16)atol(pMax);

	target->_AddSkillLine(skillline, cur, max);

	snprintf(buf, 256, "SkillLine: %u CurrentValue %u Max Value %u Added.", (unsigned int)skillline, (unsigned int)cur, (unsigned int)max);
	sGMLog.writefromsession(m_session, "added skill line %u (%u/%u) to %s", skillline, cur, max, target->GetName());
	SystemMessage(m_session, buf);

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

static const uint32 numpvpflags = sizeof(UnitPvPFlagToName) / sizeof(UnitPvPFlagNames);

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

bool ChatHandler::HandleRemoveAurasCommand(const char* args, WorldSession* m_session)
{
	Player* plr = getSelectedChar(m_session, true);
	if(!plr) return false;

	BlueSystemMessage(m_session, "Removing all auras...");
	for(uint32 i = MAX_REMOVABLE_AURAS_START; i < MAX_REMOVABLE_AURAS_END; ++i)
	{
		if(plr->m_auras[i] != 0) plr->m_auras[i]->Remove();
	}
	if(plr != m_session->GetPlayer())
		sGMLog.writefromsession(m_session, "Removed all of %s's auras.", plr->GetName());
	return true;
}

bool ChatHandler::HandleRemoveRessurectionSickessAuraCommand(const char* args, WorldSession* m_session)
{
	Player* plr = getSelectedChar(m_session, true);
	if(!plr) return false;

	BlueSystemMessage(m_session, "Removing resurrection sickness...");
	plr->RemoveAura(15007);
	if(plr != m_session->GetPlayer())
		sGMLog.writefromsession(m_session, "Removed resurrection sickness from %s", plr->GetName());
	return true;
}

bool ChatHandler::HandleSetMotdCommand(const char* args, WorldSession* m_session)
{
	if(!args || strlen(args) < 2)
	{
		RedSystemMessage(m_session, "You must specify a message.");
		return true;
	}

	GreenSystemMessage(m_session, "Motd has been set to: %s", args);
	World::getSingleton().SetMotd(args);
	sGMLog.writefromsession(m_session, "Set MOTD to %s", args);
	return true;
}

bool ChatHandler::HandleDBReloadCommand(const char* args, WorldSession* m_session)
{

	sWorld.SendWorldText("Support for reloading tables on the fly was disabled in Arcemu revision 3621. You are seeing this message because apparently reading SVN changelog or using forums search is way over the head of some of our users.", 0);
	return true;

	/*

	char str[200];
	int ret = 0;

	if(!*args || strlen(args) < 3)
		return false;


	uint32 mstime = getMSTime();
	snprintf(str, 200, "%s%s initiated server-side reload of table `%s`. The server may experience some lag while this occurs.",
		MSG_COLOR_LIGHTRED, m_session->GetPlayer()->GetName(), args);
	sWorld.SendWorldText(str, 0);

	if (0 == stricmp(args, "spell_disable"))
	{
		objmgr.ReloadDisabledSpells();
		ret = 1;
	} else
	if (0 == stricmp(args, "vendors"))
	{
		objmgr.ReloadVendors();
		ret = 1;
	}
	else
	{
		ret = Storage_ReloadTable(args);
	}

	if (ret == 0)
		snprintf(str, 200, "%sDatabase reload failed.", MSG_COLOR_LIGHTRED);
	else
		snprintf(str, 200, "%sDatabase reload completed in %u ms.", MSG_COLOR_LIGHTBLUE, getMSTime() - mstime);
	sWorld.SendWorldText(str, 0);
	sGMLog.writefromsession(m_session, "reloaded table %s", args);
	return true;

	*/

}

bool ChatHandler::HandleCreatePetCommand(const char* args, WorldSession* m_session)
{
	if((args == NULL) || (strlen(args) < 2))
		return false;

	uint32 entry = atol(args);
	if(entry == 0)
		return false;

	CreatureInfo* ci = CreatureNameStorage.LookupEntry(entry);
	CreatureProto* cp = CreatureProtoStorage.LookupEntry(entry);

	if((ci == NULL) || (cp == NULL))
		return false;

	Player* p = m_session->GetPlayer();

	p->DismissActivePets();
	p->RemoveFieldSummon();

	float followangle = -M_PI_FLOAT * 2;
	LocationVector v(p->GetPosition());

	v.x += (3 * (cosf(followangle + p->GetOrientation())));
	v.y += (3 * (sinf(followangle + p->GetOrientation())));

	Pet* pet = objmgr.CreatePet(entry);

	if(!pet->CreateAsSummon(entry, ci, NULL, p, NULL, 1, 0, &v, true))
	{
		pet->DeleteMe();
		return true;
	}

	pet->GetAIInterface()->SetUnitToFollowAngle(followangle);

	return true;
}


#ifdef USE_SPECIFIC_AIAGENTS
//this is custom stuff !
bool ChatHandler::HandlePetSpawnAIBot(const char* args, WorldSession* m_session)
{
	if(!*args)
		return false;

	if(!m_session->GetPlayer())
		return false; //wtf ?

	uint32 botprice = m_session->GetPlayer()->getLevel() * 10000; //1 gold per level ?

	if(!m_session->GetPlayer()->HasGold(botprice))
	{
		GreenSystemMessage(m_session, "You need a total of %u coins to afford a bot", botprice);
		return false;
	}

	uint8 botType = (uint8)atof((char*)args);

	if(botType != 0)
	{
		RedSystemMessage(m_session, "Incorrect value. Accepting value 0 only = healbot :)");
		return true;
	}

	uint32 Entry;
	char name[50];
	uint8 race = m_session->GetPlayer()->getRace();

	if(race == RACE_HUMAN || race == RACE_DWARF || race == RACE_NIGHTELF || race == RACE_GNOME || race == RACE_DRAENEI)
	{
		Entry = 1826;
		strcpy(name, "|cffff6060A_HealBot");
	}
	else
	{
		Entry = 5473;
		strcpy(name, "|cffff6060H_HealBot");
	}

	CreatureProto* pTemplate = CreatureProtoStorage.LookupEntry(Entry);
	CreatureInfo* pCreatureInfo = CreatureNameStorage.LookupEntry(Entry);
	if(!pTemplate || !pCreatureInfo)
	{
		RedSystemMessage(m_session, "Invalid creature spawn template: %u", Entry);
		return true;
	}
	Player* plr = m_session->GetPlayer();
	Creature* newguard = plr->create_guardian(Entry, 2 * 60 * 1000, float(-M_PI * 2), plr->getLevel());
	AiAgentHealSupport* new_interface = new AiAgentHealSupport;
	new_interface->Init(newguard, AITYPE_PET, MOVEMENTTYPE_NONE, plr);
	newguard->ReplaceAIInterface((AIInterface*) new_interface);

	/*	Pet *old_tame = plr->GetSummon();
		if(old_tame != NULL)
		{
			old_tame->Dismiss(true);
		}

		// create a pet from this creature
		Pet * pPet = objmgr.CreatePet( Entry );
		pPet->SetInstanceID(plr->GetInstanceID());
		pPet->SetMapId(plr->GetMapId());

		pPet->SetFloatValue ( OBJECT_FIELD_SCALE_X, pTemplate->Scale / 2); //we do not wish to block visually other players
		AiAgentHealSupport *new_interface = new AiAgentHealSupport;
		pPet->ReplaceAIInterface( (AIInterface *) new_interface );
	//	new_interface->Init(pPet,AITYPE_PET,MOVEMENTTYPE_NONE,plr); // i think this will get called automatically for pet

		pPet->CreateAsSummon(Entry, pCreatureInfo, pCreature, plr, NULL, 0x2, 0);

		pPet->Rename(name);

		//healer bot should not have any specific actions
		pPet->SetActionBarSlot(0,PET_SPELL_FOLLOW);
		pPet->SetActionBarSlot(1,PET_SPELL_STAY);
		pPet->SetActionBarSlot(2,0);
		pPet->SetActionBarSlot(3,0);
		pPet->SetActionBarSlot(4,0);
		pPet->SetActionBarSlot(5,0);
		pPet->SetActionBarSlot(6,0);
		pPet->SetActionBarSlot(7,0);
		pPet->SetActionBarSlot(8,0);
		pPet->SetActionBarSlot(9,0);
		pPet->SendSpellsToOwner();

		// remove the temp creature
		delete sp;
		delete pCreature;*/

	sGMLog.writefromsession(m_session, "used create an AI bot");
	return true;
}
#endif

bool ChatHandler::HandleAddPetSpellCommand(const char* args, WorldSession* m_session)
{
	Player* plr = getSelectedChar(m_session, false);
	if(!plr)
		return false;

	if(plr->GetSummon() == NULL)
	{
		RedSystemMessage(m_session, "%s has no pet.", plr->GetName());
		return true;
	}

	uint32 SpellId = atol(args);
	SpellEntry* spell = dbcSpell.LookupEntryForced(SpellId);
	if(!SpellId || !spell)
	{
		RedSystemMessage(m_session, "Invalid spell id requested.");
		return true;
	}

	std::list<Pet*> summons = plr->GetSummons();
	for(std::list<Pet*>::iterator itr = summons.begin(); itr != summons.end(); ++itr)
	{
		(*itr)->AddSpell(spell, true);
	}
	GreenSystemMessage(m_session, "Added spell %u to %s's pet.", SpellId, plr->GetName());
	return true;
}

bool ChatHandler::HandleRemovePetSpellCommand(const char* args, WorldSession* m_session)
{
	Player* plr = getSelectedChar(m_session, false);
	if(!plr)
		return false;

	if(plr->GetSummon() == NULL)
	{
		RedSystemMessage(m_session, "%s has no pet.", plr->GetName());
		return true;
	}

	uint32 SpellId = atol(args);
	SpellEntry* spell = dbcSpell.LookupEntryForced(SpellId);
	if(!SpellId || !spell)
	{
		RedSystemMessage(m_session, "Invalid spell id requested.");
		return true;
	}

	std::list<Pet*> summons = plr->GetSummons();
	for(std::list<Pet*>::iterator itr = summons.begin(); itr != summons.end(); ++itr)
	{
		(*itr)->RemoveSpell(SpellId);
	}
	GreenSystemMessage(m_session, "Removed spell %u from %s's pet.", SpellId, plr->GetName());
	return true;
}

bool ChatHandler::HandleRenamePetCommand(const char* args, WorldSession* m_session)
{
	Player* plr = m_session->GetPlayer();
	Pet* pPet = plr->GetSummon();
	if(pPet == NULL)
	{
		RedSystemMessage(m_session, "You have no pet.");
		return true;
	}

	if(strlen(args) < 1)
	{
		RedSystemMessage(m_session, "You must specify a name.");
		return true;
	}

	GreenSystemMessage(m_session, "Renamed your pet to %s.", args);
	pPet->Rename(args);//support for only 1st pet
	return true;
}

bool ChatHandler::HandleDismissPetCommand(const char* args, WorldSession* m_session)
{
	Player* plr = getSelectedChar(m_session, false);
	Pet* pPet = NULL;
	if(plr)
	{
		if(plr->GetSummon() == NULL)
		{
			RedSystemMessage(m_session, "Player has no pet.");
			return true;
		}
		else
		{
			plr->DismissActivePets();
		}
	}
	else
	{
		// no player selected, see if it is a pet
		Creature* pCrt = getSelectedCreature(m_session, false);
		if(!pCrt)
		{
			// show usage string
			return false;
		}
		if(pCrt->IsPet())
		{
			pPet = TO< Pet* >(pCrt);
		}
		if(!pPet)
		{
			RedSystemMessage(m_session, "No player or pet selected.");
			return true;
		}
		plr = pPet->GetPetOwner();
		pPet->Dismiss();
	}

	GreenSystemMessage(m_session, "Dismissed %s's pet.", plr->GetName());
	plr->GetSession()->SystemMessage("%s dismissed your pet.", m_session->GetPlayer()->GetName());
	return true;
}

bool ChatHandler::HandlePetLevelCommand(const char* args, WorldSession* m_session)
{
	if(!args)
	{
		return false;
	}

	int32 newLevel = atol(args);
	if(newLevel < 1)
	{
		return false;
	}

	Player* plr = getSelectedChar(m_session, false);
	Pet* pPet = NULL;
	if(plr)
	{
		pPet = plr->GetSummon();
		if(!pPet)
		{
			RedSystemMessage(m_session, "Player has no pet.");
			return true;
		}
	}
	else
	{
		// no player selected, see if it is a pet
		Creature* pCrt = getSelectedCreature(m_session, false);
		if(!pCrt)
		{
			// show usage string
			return false;
		}
		if(pCrt->IsPet())
		{
			pPet = TO< Pet* >(pCrt);
		}
		if(!pPet)
		{
			RedSystemMessage(m_session, "No player or pet selected.");
			return true;
		}
		plr = pPet->GetPetOwner();
	}

	// Should GMs be allowed to set a pet higher than its owner?  I don't think so
	if((uint32)newLevel > plr->getLevel())
	{
		newLevel = plr->getLevel();
	}

	//support for only 1 pet
	pPet->setLevel(newLevel);
	pPet->SetUInt32Value(UNIT_FIELD_PETEXPERIENCE, 0);
	pPet->SetUInt32Value(UNIT_FIELD_PETNEXTLEVELEXP, pPet->GetNextLevelXP(newLevel));
	pPet->ApplyStatsForLevel();
	pPet->UpdateSpellList();

	GreenSystemMessage(m_session, "Set %s's pet to level %lu.", plr->GetName(), newLevel);
	plr->GetSession()->SystemMessage("%s set your pet to level %lu.", m_session->GetPlayer()->GetName(), newLevel);
	return true;
}

bool ChatHandler::HandleShutdownCommand(const char* args, WorldSession* m_session)
{
	uint32 shutdowntime;
	if(!args)
		shutdowntime = 5;
	else
		shutdowntime = atol(args);

	char msg[500];
	snprintf(msg, 500, "%sServer shutdown initiated by %s, shutting down in %u seconds.", MSG_COLOR_LIGHTBLUE,
	         m_session->GetPlayer()->GetName(), (unsigned int)shutdowntime);

	sWorld.SendWorldText(msg);
	sGMLog.writefromsession(m_session, "initiated server shutdown timer %u sec", shutdowntime);
	shutdowntime *= 1000;
	sMaster.m_ShutdownTimer = shutdowntime;
	sMaster.m_ShutdownEvent = true;
	sMaster.m_restartEvent = false;
	return true;
}

bool ChatHandler::HandleShutdownRestartCommand(const char* args, WorldSession* m_session)
{
	uint32 shutdowntime;
	if(!args)
		shutdowntime = 5;
	else
		shutdowntime = atol(args);

	char msg[500];
	snprintf(msg, 500, "%sServer restart initiated by %s, shutting down in %u seconds.", MSG_COLOR_LIGHTBLUE,
	         m_session->GetPlayer()->GetName(), (unsigned int)shutdowntime);

	sGMLog.writefromsession(m_session, "initiated server restart timer %u sec", shutdowntime);
	sWorld.SendWorldText(msg);
	shutdowntime *= 1000;
	sMaster.m_ShutdownTimer = shutdowntime;
	sMaster.m_ShutdownEvent = true;
	sMaster.m_restartEvent = true;
	return true;
}

bool ChatHandler::HandleCancelShutdownCommand(const char* args, WorldSession* m_session)
{
	if(sMaster.m_ShutdownEvent == false)
		return false;
	char msg[500];
	snprintf(msg, 500, "%sServer %s cancelled by %s.", MSG_COLOR_LIGHTBLUE, (sMaster.m_restartEvent ? "Restart" : "Shutdown"), m_session->GetPlayer()->GetName());
	sWorld.SendWorldText(msg);

	sMaster.m_ShutdownTimer = 5000;
	sMaster.m_ShutdownEvent = false;
	sMaster.m_restartEvent = false;
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

bool ChatHandler::HandleNpcFollowCommand(const char* args, WorldSession* m_session)
{
	Creature* creature = getSelectedCreature(m_session, true);
	if(!creature) return true;

	creature->GetAIInterface()->SetUnitToFollow(m_session->GetPlayer());

	sGMLog.writefromsession(m_session, "used npc follow command on %s, sqlid %u", creature->GetCreatureInfo()->Name, creature->GetSQL_id());
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

bool ChatHandler::HandlePlayerInfo(const char* args, WorldSession* m_session)
{
	Player* plr;
	if(strlen(args) >= 2) // char name can be 2 letters
	{
		plr = objmgr.GetPlayer(args, false);
		if(!plr)
		{
			RedSystemMessage(m_session, "Unable to locate player %s.", args);
			return true;
		}
	}
	else
		plr = getSelectedChar(m_session, true);

	if(!plr) return true;
	if(!plr->GetSession())
	{
		RedSystemMessage(m_session, "ERROR: this player hasn't got any session !");
		return true;
	}
	if(!plr->GetSession()->GetSocket())
	{
		RedSystemMessage(m_session, "ERROR: this player hasn't got any socket !");
		return true;
	}
	WorldSession* sess = plr->GetSession();

//	char* infos = new char[128];
	static const char* classes[12] =
	{"None", "Warrior", "Paladin", "Hunter", "Rogue", "Priest", "Death Knight", "Shaman", "Mage", "Warlock", "None", "Druid"};
	static const char* races[12] =
	{"None", "Human", "Orc", "Dwarf", "Night Elf", "Undead", "Tauren", "Gnome", "Troll", "None", "Blood Elf", "Draenei"};

	char playedLevel[64];
	char playedTotal[64];

	int seconds = (plr->GetPlayedtime())[0];
	int mins = 0;
	int hours = 0;
	int days = 0;
	if(seconds >= 60)
	{
		mins = seconds / 60;
		if(mins)
		{
			seconds -= mins * 60;
			if(mins >= 60)
			{
				hours = mins / 60;
				if(hours)
				{
					mins -= hours * 60;
					if(hours >= 24)
					{
						days = hours / 24;
						if(days)
							hours -= days * 24;
					}
				}
			}
		}
	}
	snprintf(playedLevel, 64, "[%d days, %d hours, %d minutes, %d seconds]", days, hours, mins, seconds);

	seconds = (plr->GetPlayedtime())[1];
	mins = 0;
	hours = 0;
	days = 0;
	if(seconds >= 60)
	{
		mins = seconds / 60;
		if(mins)
		{
			seconds -= mins * 60;
			if(mins >= 60)
			{
				hours = mins / 60;
				if(hours)
				{
					mins -= hours * 60;
					if(hours >= 24)
					{
						days = hours / 24;
						if(days)
							hours -= days * 24;
					}
				}
			}
		}
	}
	snprintf(playedTotal, 64, "[%d days, %d hours, %d minutes, %d seconds]", days, hours, mins, seconds);

	GreenSystemMessage(m_session, "%s is a %s %s %s", plr->GetName(),
	                   (plr->getGender() ? "Female" : "Male"), races[plr->getRace()], classes[plr->getClass()]);

	BlueSystemMessage(m_session, "%s has played %s at this level", (plr->getGender() ? "She" : "He"), playedLevel);
	BlueSystemMessage(m_session, "and %s overall", playedTotal);

	BlueSystemMessage(m_session, "%s is connecting from account '%s'[%u] with permissions '%s'",
	                  (plr->getGender() ? "She" : "He"), sess->GetAccountName().c_str(), sess->GetAccountId(), sess->GetPermissions());

	const char* client;

	// Clean code says you need to work from highest combined bit to lowest. Second, you need to check if both flags exists.
	if(sess->HasFlag(ACCOUNT_FLAG_XPACK_02) && sess->HasFlag(ACCOUNT_FLAG_XPACK_01))
		client = "TBC and WotLK";
	else if(sess->HasFlag(ACCOUNT_FLAG_XPACK_02))
		client = "Wrath of the Lich King";
	else if(sess->HasFlag(ACCOUNT_FLAG_XPACK_01))
		client = "WoW Burning Crusade";
	else
		client = "WoW";

	BlueSystemMessage(m_session, "%s uses %s (build %u)", (plr->getGender() ? "She" : "He"), client, sess->GetClientBuild());
	BlueSystemMessage(m_session, "%s IP is '%s', and has a latency of %ums", (plr->getGender() ? "Her" : "His"), sess->GetSocket()->GetRemoteIP().c_str(), sess->GetLatency());
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

void ChatHandler::SendHighlightedName(WorldSession* m_session, const char* prefix, const char* full_name, string & lowercase_name, string & highlight, uint32 id)
{
	char message[1024];
	char start[50];
	start[0] = message[0] = 0;

	snprintf(start, 50, "%s %u: %s", prefix, (unsigned int)id, MSG_COLOR_WHITE);

	string::size_type hlen = highlight.length();
	string fullname = string(full_name);
	string::size_type offset = lowercase_name.find(highlight);
	string::size_type remaining = fullname.size() - offset - hlen;
	strcat(message, start);
	strncat(message, fullname.c_str(), offset);
	strcat(message, MSG_COLOR_LIGHTRED);
	strncat(message, (fullname.c_str() + offset), hlen);
	strcat(message, MSG_COLOR_WHITE);
	if(remaining > 0) strncat(message, (fullname.c_str() + offset + hlen), remaining);

	SystemMessage(m_session, message);
}

void ChatHandler::SendItemLinkToPlayer(ItemPrototype* iProto, WorldSession* pSession, bool ItemCount, Player* owner, uint32 language)
{
	if(!iProto || !pSession)
		return;
	if(ItemCount && owner == NULL)
		return;

	if(ItemCount)
	{
		int8 count = static_cast<int8>(owner->GetItemInterface()->GetItemCount(iProto->ItemId, true));
		//int8 slot = owner->GetItemInterface()->GetInventorySlotById(iProto->ItemId); //DISABLED due to being a retarded concept
		if(iProto->ContainerSlots > 0)
		{
			SystemMessage(pSession, "Item %u %s Count %u ContainerSlots %u", iProto->ItemId, GetItemLinkByProto(iProto, language).c_str(), count, iProto->ContainerSlots);
		}
		else
		{
			SystemMessage(pSession, "Item %u %s Count %u", iProto->ItemId, GetItemLinkByProto(iProto, language).c_str(), count);
		}
	}
	else
	{
		if(iProto->ContainerSlots > 0)
		{
			SystemMessage(pSession, "Item %u %s ContainerSlots %u", iProto->ItemId, GetItemLinkByProto(iProto, language).c_str(), iProto->ContainerSlots);
		}
		else
		{
			SystemMessage(pSession, "Item %u %s", iProto->ItemId, GetItemLinkByProto(iProto, language).c_str());
		}
	}
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

bool ChatHandler::HandleRehashCommand(const char* args, WorldSession* m_session)
{
	/*
	rehashes
	*/
	char msg[250];
	snprintf(msg, 250, "%s is rehashing config file.", m_session->GetPlayer()->GetName());
	sWorld.SendWorldWideScreenText(msg, 0);
	sWorld.SendWorldText(msg, 0);
	sWorld.Rehash(true);
	return true;
}

struct spell_thingo
{
	uint32 type;
	uint32 target;
};

SpellCastTargets SetTargets(SpellEntry* sp, uint32 type, uint32 targettype, Unit* dst, Creature* src)
{
	SpellCastTargets targets;
	targets.m_unitTarget = 0;
	targets.m_itemTarget = 0;
	targets.m_srcX = 0;
	targets.m_srcY = 0;
	targets.m_srcZ = 0;
	targets.m_destX = 0;
	targets.m_destY = 0;
	targets.m_destZ = 0;

	if(targettype == TTYPE_SINGLETARGET)
	{
		targets.m_targetMask = TARGET_FLAG_UNIT;
		targets.m_unitTarget = dst->GetGUID();
	}
	else if(targettype == TTYPE_SOURCE)
	{
		targets.m_targetMask = TARGET_FLAG_SOURCE_LOCATION;
		targets.m_srcX = src->GetPositionX();
		targets.m_srcY = src->GetPositionY();
		targets.m_srcZ = src->GetPositionZ();
	}
	else if(targettype == TTYPE_DESTINATION)
	{
		targets.m_targetMask = TARGET_FLAG_DEST_LOCATION;
		targets.m_destX = dst->GetPositionX();
		targets.m_destY = dst->GetPositionY();
		targets.m_destZ = dst->GetPositionZ();
	}

	return targets;
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

bool ChatHandler::HandleReloadScripts(const char* args, WorldSession* m_session)
{
	sScriptMgr.ReloadScriptEngines();
	SystemMessage(m_session, "Script engines reloaded.");
	return true;
}
