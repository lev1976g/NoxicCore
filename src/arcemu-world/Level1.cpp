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
//  GM Chat Commands
//

#include "StdAfx.h"

uint16 GetItemIDFromLink(const char* itemlink, uint32* itemid)
{
	if(itemlink == NULL)
	{
		*itemid = 0;
		return 0;
	}
	uint16 slen = (uint16)strlen(itemlink);

	const char* ptr = strstr(itemlink, "|Hitem:");
	if(ptr == NULL)
	{
		*itemid = 0;
		return slen;
	}

	ptr += 7; // item id is just past "|Hitem:" (7 bytes)
	*itemid = atoi(ptr);

	ptr = strstr(itemlink, "|r"); // the end of the item link
	if(ptr == NULL) // item link was invalid
	{
		*itemid = 0;
		return slen;
	}

	ptr += 2;
	return (ptr - itemlink) & 0x0000ffff;
}

bool ChatHandler::HandleGMOnCommand(const char* args, WorldSession* m_session)
{
	GreenSystemMessage(m_session, "Setting GM Flag on yourself.");

	Player* _player = m_session->GetPlayer();
	if(_player->HasFlag(PLAYER_FLAGS, PLAYER_FLAG_GM))
		RedSystemMessage(m_session, "GM Flag is already set on. Use .gm off to disable it.");
	else
	{
		_player->SetFlag(PLAYER_FLAGS, PLAYER_FLAG_GM);	// <GM>
		_player->TriggerpassCheat = true;
		_player->SetFaction(35);
		_player->RemovePvPFlag();

		BlueSystemMessage(m_session, "GM flag set. It will now appear above your name and in chat messages until you use .gm off.");

		_player->UpdateVisibility();
	}

	return true;
}

bool ChatHandler::HandleGMOffCommand(const char* args, WorldSession* m_session)
{
	Player* _player = m_session->GetPlayer();
	if(_player->HasFlag(PLAYER_FLAGS, PLAYER_FLAG_GM))
	{
		_player->RemoveFlag(PLAYER_FLAGS, PLAYER_FLAG_GM); // <GM>
		_player->TriggerpassCheat = false;
		_player->SetFaction(_player->GetInitialFactionId());
		_player->UpdatePvPArea();

		BlueSystemMessage(m_session, "GM Flag Removed. <GM> Will no longer show in chat messages or above your name.");

		_player->UpdateVisibility();
	}

	return true;
}

bool ChatHandler::HandleAddInvItemCommand(const char* args, WorldSession* m_session)
{
	uint32 itemid, count = 1;
	int32 randomprop = 0;
	int32 numadded = 0;

	if(strlen(args) < 1)
	{
		return false;
	}

	if(sscanf(args, "%u %u %d", &itemid, &count, &randomprop) < 1)
	{
		// check for item link
		uint16 ofs = GetItemIDFromLink(args, &itemid);
		if(itemid == 0)
			return false;
		sscanf(args + ofs, "%u %d", &count, &randomprop); // these may be empty
	}

	Player* chr = getSelectedChar(m_session, false);
	if(chr == NULL)
		chr = m_session->GetPlayer();

	ItemPrototype* it = ItemPrototypeStorage.LookupEntry(itemid);
	if(it)
	{
		numadded -= chr->GetItemInterface()->GetItemCount(itemid);
		bool result = false;
		result = chr->GetItemInterface()->AddItemById(itemid, count, randomprop);
		numadded += chr->GetItemInterface()->GetItemCount(itemid);
		if(result == true)
		{
			if(count == 0)
			{
				sGMLog.writefromsession(m_session, "used add item command, item id %u [%s], quantity %u, to %s", it->ItemId, it->Name1, numadded, chr->GetName());
			}
			else
			{
				sGMLog.writefromsession(m_session, "used add item command, item id %u [%s], quantity %u (only %lu added due to full inventory), to %s", it->ItemId, it->Name1, numadded, numadded, chr->GetName());
			}

			char messagetext[512];

			snprintf(messagetext, 512, "Added item %s (id: %d), quantity %u, to %s's inventory.", GetItemLinkByProto(it, m_session->language).c_str(), (unsigned int)it->ItemId, numadded, chr->GetName());
			SystemMessage(m_session, messagetext);
			//snprintf(messagetext, 128, "%s added item %d (%s) to your inventory.", m_session->GetPlayer()->GetName(), (unsigned int)itemid, it->Name1);
			snprintf(messagetext, 512, "%s added item %s, quantity %u, to your inventory.", m_session->GetPlayer()->GetName(), GetItemLinkByProto(it, chr->GetSession()->language).c_str(), numadded);

			SystemMessageToPlr(chr,  messagetext);
		}
		else
		{
			SystemMessageToPlr(chr, "Failed to add item.");
		}
		return true;

	}
	else
	{
		RedSystemMessage(m_session, "Item %d is not a valid item!", itemid);
		return true;
	}
}

bool ChatHandler::HandleSummonCommand(const char* args, WorldSession* m_session)
{
	if(!*args)
		return false;

	// Summon Blocking
	if(!stricmp(args, "on"))
	{
		if(m_session->GetPlayer()->IsSummonDisabled())
		{
			BlueSystemMessage(m_session, "Summon blocking is already enabled");
		}
		else
		{
			m_session->GetPlayer()->DisableSummon(true);
			GreenSystemMessage(m_session, "Summon blocking is now enabled");
		}
		return true;
	}
	else if(!stricmp(args, "off"))
	{
		if(m_session->GetPlayer()->IsSummonDisabled())
		{
			m_session->GetPlayer()->DisableSummon(false);
			GreenSystemMessage(m_session, "Summon blocking is now disabled");
		}
		else
		{
			BlueSystemMessage(m_session, "Summon blocking is already disabled");
		}
		return true;
	}

	Player* chr = objmgr.GetPlayer(args, false);
	if(chr)
	{
		// send message to user
		char buf[256];
		char buf0[256];

		if(!m_session->CanUseCommand('z') && chr->IsSummonDisabled())
		{
			snprintf((char*)buf, 256, "%s has blocked other GMs from summoning them.", chr->GetName());
			SystemMessage(m_session, buf);
			return true;
		}

		if(chr->GetMapMgr() == NULL)
		{
			snprintf((char*)buf, 256, "%s is already being teleported.", chr->GetName());
			SystemMessage(m_session, buf);
			return true;
		}
		snprintf((char*)buf, 256, "You are summoning %s.", chr->GetName());
		SystemMessage(m_session, buf);

		// Don't summon the dead, lol, I see dead people. :P
		// If you do, we better bring them back to life
		if(chr->getDeathState() == 1)  // Just died
			chr->RemoteRevive();
		if(chr->getDeathState() != 0)  // Not alive
			chr->ResurrectPlayer();

		if(!m_session->GetPlayer()->m_isGmInvisible)
		{
			// send message to player
			snprintf((char*)buf0, 256, "You are being summoned by %s.", m_session->GetPlayer()->GetName());
			SystemMessageToPlr(chr, buf0);
		}

		Player* plr = m_session->GetPlayer();

		if(plr->GetMapMgr() == chr->GetMapMgr())
			chr->_Relocate(plr->GetMapId(), plr->GetPosition(), false, false, plr->GetInstanceID());
		else
		{
			sEventMgr.AddEvent(chr, &Player::EventPortToGM, plr, 0, 1, 1, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
		}
	}
	else
	{
		PlayerInfo* pinfo = objmgr.GetPlayerInfoByName(args);
		if(!pinfo)
		{
			char buf[256];
			snprintf((char*)buf, 256, "Player (%s) does not exist.", args);
			SystemMessage(m_session, buf);
			return true;
		}
		else
		{
			Player* pPlayer = m_session->GetPlayer();
			char query[512];
			snprintf((char*) &query, 512, "UPDATE characters SET mapId = %u, positionX = %f, positionY = %f, positionZ = %f, zoneId = %u WHERE guid = %u;",	pPlayer->GetMapId(), pPlayer->GetPositionX(), pPlayer->GetPositionY(), pPlayer->GetPositionZ(), pPlayer->GetZoneId(), pinfo->guid);
			CharacterDatabase.Execute(query);
			char buf[256];
			snprintf((char*)buf, 256, "(Offline) %s has been summoned.", pinfo->name);
			SystemMessage(m_session, buf);
			return true;
		}
	}

	sGMLog.writefromsession(m_session, "summoned %s on map %u, %f %f %f", args, m_session->GetPlayer()->GetMapId(), m_session->GetPlayer()->GetPositionX(), m_session->GetPlayer()->GetPositionY(), m_session->GetPlayer()->GetPositionZ());
	return true;
}

bool ChatHandler::HandleModifySpeedCommand(const char* args, WorldSession* m_session)
{
	WorldPacket data;

	if(!*args)
		return false;

	float Speed = (float)atof((char*)args);

	if(Speed > 255 || Speed < 1)
	{
		RedSystemMessage(m_session, "Incorrect value. Range is 1..255");
		return true;
	}

	Player* chr = getSelectedChar(m_session);
	if(chr == NULL)
		return true;

	if(chr != m_session->GetPlayer())
		sGMLog.writefromsession(m_session, "modified speed of %s to %2.2f.", chr->GetName(), Speed);


	char buf[256];

	// send message to user
	BlueSystemMessage(m_session, "You set the speed of %s to %2.2f.", chr->GetName(), Speed);

	// send message to player
	snprintf((char*)buf, 256, "%s set your speed to %2.2f.", m_session->GetPlayer()->GetName(), Speed);
	SystemMessage(chr->GetSession(), buf);

	chr->SetSpeeds(RUN, Speed);
	chr->SetSpeeds(SWIM, Speed);
	chr->SetSpeeds(RUNBACK, Speed / 2); // Backwards slower, it's more natural :P
	chr->SetSpeeds(FLY, Speed * 2); // Flying is faster :P

	return true;
}

bool ChatHandler::HandleLearnSkillCommand(const char* args, WorldSession* m_session)
{
	uint32 skill, min, max;
	min = max = 1;
	char* pSkill = strtok((char*)args, " ");
	if(!pSkill)
		return false;
	else
		skill = atol(pSkill);

	BlueSystemMessage(m_session, "Adding skill line %d", skill);

	char* pMin = strtok(NULL, " ");
	if(pMin)
	{
		min = atol(pMin);
		char* pMax = strtok(NULL, "\n");
		if(pMax)
			max = atol(pMax);
	}
	else
	{
		return false;
	}

	Player* plr = getSelectedChar(m_session, true);
	if(!plr) return false;
	if(!plr->IsPlayer()) return false;
	sGMLog.writefromsession(m_session, "used add skill of %u %u %u on %s", skill, min, max, plr->GetName());

	plr->_AddSkillLine(skill, min, max);

	return true;
}

/// DGM: Get skill level command for getting information about a skill
bool ChatHandler::HandleGetSkillLevelCommand(const char* args, WorldSession* m_session)
{
	uint32 skill = 0;
	char* pSkill = strtok((char*)args, " ");
	if(!pSkill)
		return false;
	else
		skill = atol(pSkill);

	Player* plr = getSelectedChar(m_session, true);
	if(!plr) return false;

	if(skill > SkillNameManager->maxskill)
	{
		BlueSystemMessage(m_session, "Skill: %u does not exists", skill);
		return false;
	}

	char* SkillName = SkillNameManager->SkillNames[skill];

	if(SkillName == 0)
	{
		BlueSystemMessage(m_session, "Skill: %u does not exists", skill);
		return false;
	}

	if(!plr->_HasSkillLine(skill))
	{
		BlueSystemMessage(m_session, "Player does not have %s skill.", SkillName);
		return false;
	}

	uint32 nobonus = plr->_GetSkillLineCurrent(skill, false);
	uint32 bonus = plr->_GetSkillLineCurrent(skill, true) - nobonus;
	uint32 max = plr->_GetSkillLineMax(skill);

	BlueSystemMessage(m_session, "Player's %s skill has level: %u maxlevel: %u. (+ %u bonus)", SkillName, nobonus, max, bonus);
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

bool ChatHandler::HandleModifyGoldCommand(const char* args, WorldSession* m_session)
{
//	WorldPacket data;

	if(*args == 0)
		return false;

	Player* chr = getSelectedChar(m_session, true);
	if(chr == NULL) return true;

	int32 total   = atoi((char*)args);

	// gold = total / 10000;
	// silver = (total / 100) % 100;
	// copper = total % 100;
	uint32 gold   = (uint32) std::floor((float)int32abs(total) / 10000.0f);
	uint32 silver = (uint32) std::floor(((float)int32abs(total) / 100.0f)) % 100;
	uint32 copper = int32abs2uint32(total) % 100;

	sGMLog.writefromsession(m_session, "used modify gold on %s, gold: %d", chr->GetName(), total);

	int32 newgold = chr->GetGold() + total;

	if(newgold < 0)
	{
		BlueSystemMessage(m_session, "Taking all gold from %s's backpack...", chr->GetName());
		GreenSystemMessageToPlr(chr, "%s took the all gold from your backpack.", m_session->GetPlayer()->GetName());
		newgold = 0;
	}
	else
	{
		if(total >= 0)
		{
			BlueSystemMessage(m_session,
			                  "Adding %u gold, %u silver, %u copper to %s's backpack...",
			                  gold, silver, copper,
			                  chr->GetName());

			GreenSystemMessageToPlr(chr, "%s added %u gold, %u silver, %u copper to your backpack.",
			                        m_session->GetPlayer()->GetName(),
			                        gold, silver, copper);
		}
		else
		{
			BlueSystemMessage(m_session,
			                  "Taking %u gold, %u silver, %u copper from %s's backpack...",
			                  gold, silver, copper,
			                  chr->GetName());

			GreenSystemMessageToPlr(chr, "%s took %u gold, %u silver, %u copper from your backpack.",
			                        m_session->GetPlayer()->GetName(),
			                        gold, silver, copper);
		}
	}

	// Check they don't have more than the max gold
	if(sWorld.GoldCapEnabled)
	{
		if((chr->GetGold() + newgold) > sWorld.GoldLimit)
		{
			RedSystemMessage(m_session, "Maximum amount of gold is %u and %s already has %u", (sWorld.GoldLimit / 10000), chr->GetName(), (chr->GetGold() / 10000));
			return true;
		}
	}

	chr->SetGold(newgold);

	return true;
}

bool ChatHandler::HandleUnlearnCommand(const char* args, WorldSession* m_session)
{
	Player* plr = getSelectedChar(m_session, true);
	if(plr == 0)
		return true;

	uint32 SpellId = atol(args);
	if(SpellId == 0)
	{
		SpellId = GetSpellIDFromLink(args);
		if(SpellId == 0)
		{
			RedSystemMessage(m_session, "You must specify a spell id.");
			return true;
		}
	}

	sGMLog.writefromsession(m_session, "removed spell %u from %s", SpellId, plr->GetName());

	if(plr->HasSpell(SpellId))
	{
		GreenSystemMessageToPlr(plr, "Removed spell %u.", SpellId);
		GreenSystemMessage(m_session, "Removed spell %u from %s.", SpellId, plr->GetName());
		plr->removeSpell(SpellId, false, false, 0);
	}
	else
	{
		RedSystemMessage(m_session, "That player does not have spell %u learnt.", SpellId);
	}

	return true;
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

bool ChatHandler::HandleModifyTPsCommand(const char* args, WorldSession* m_session)
{
	if(!args)
		return false;

	Player* Pl = getSelectedChar(m_session, false);
	if(!Pl)
	{
		SystemMessage(m_session, "Invalid or no target provided, please target a player to modify its talentpoints.");
		return true;
	}

	uint32 TP1 = 0;
	uint32 TP2 = 0;
	std::stringstream ss( args );

	ss >> TP1;
	ss >> TP2;

	Pl->m_specs[SPEC_PRIMARY].SetTP( TP1 );
	Pl->m_specs[SPEC_SECONDARY].SetTP( TP2 );
	Pl->smsg_TalentsInfo(false);
	return true;
}

#ifdef ENABLE_ACHIEVEMENTS
bool ChatHandler::HandleLookupAchievementCmd(const char* args, WorldSession* m_session)
{
	if(!*args)
		return false;

	string x;
	bool lookupname = true, lookupdesc = false, lookupcriteria = false, lookupreward = false;
	if(strnicmp(args, "name ", 5) == 0)
	{
		x = string(args + 5);
	}
	else if(strnicmp(args, "desc ", 5) == 0)
	{
		lookupname = false;
		lookupdesc = true;
		x = string(args + 5);
	}
	else if(strnicmp(args, "criteria ", 9) == 0)
	{
		lookupname = false;
		lookupcriteria = true;
		x = string(args + 9);
	}
	else if(strnicmp(args, "reward ", 7) == 0)
	{
		lookupname = false;
		lookupreward = true;
		x = string(args + 7);
	}
	else if(strnicmp(args, "all ", 4) == 0)
	{
		lookupdesc = true;
		lookupcriteria = true;
		lookupreward = true;
		x = string(args + 4);
	}
	else
	{
		x = string(args);
	}
	if(x.length() < 4)
	{
		RedSystemMessage(m_session, "Your search string must be at least 4 characters long.");
		return true;
	}
	arcemu_TOLOWER(x);
	GreenSystemMessage(m_session, "Starting search of achievement `%s`...", x.c_str());
	uint32 t = getMSTime();
	uint32 i, j, numFound = 0;
	string y, recout;
	char playerGUID[17];
	snprintf(playerGUID, 17, I64FMT, m_session->GetPlayer()->GetGUID());

	if(lookupname || lookupdesc || lookupreward)
	{
		std::set<uint32> foundList;
		j = dbcAchievementStore.GetNumRows();
		bool foundmatch;
		for(i = 0; i < j && numFound < 25; ++i)
		{
			AchievementEntry const* achievement = dbcAchievementStore.LookupRowForced(i);
			if(achievement)
			{
				if(foundList.find(achievement->ID) != foundList.end())
				{
					// already listed this achievement (some achievements have multiple entries in dbc)
					continue;
				}
				foundmatch = false;
				if(lookupname)
				{
					y = string(achievement->name);
					arcemu_TOLOWER(y);
					foundmatch = FindXinYString(x, y);
				}
				if(!foundmatch && lookupdesc)
				{
					y = string(achievement->description);
					arcemu_TOLOWER(y);
					foundmatch = FindXinYString(x, y);
				}
				if(!foundmatch && lookupreward)
				{
					y = string(achievement->rewardName);
					arcemu_TOLOWER(y);
					foundmatch = FindXinYString(x, y);
				}
				if(!foundmatch)
				{
					continue;
				}
				foundList.insert(achievement->ID);
				std::stringstream strm;
				strm << achievement->ID;
				// create achievement link
				recout = "|cffffffffAchievement ";
				recout += strm.str();
				recout += ": |cfffff000|Hachievement:";
				recout += strm.str();
				recout += ":";
				recout += (const char*)playerGUID;
				time_t completetime = m_session->GetPlayer()->GetAchievementMgr().GetCompletedTime(achievement);
				if(completetime)
				{
					// achievement is completed
					struct tm* ct;
					ct = localtime(&completetime);
					strm.str("");
					strm << ":1:" << ct->tm_mon + 1 << ":" << ct->tm_mday << ":" << ct->tm_year - 100 << ":-1:-1:-1:-1|h[";
					recout += strm.str();
				}
				else
				{
					// achievement is not completed
					recout += ":0:0:0:-1:0:0:0:0|h[";
				}
				recout += achievement->name;
				if(!lookupreward)
				{
					recout += "]|h|r";
				}
				else
				{
					recout += "]|h |cffffffff";
					recout += achievement->rewardName;
					recout += "|r";
				}
				strm.str("");
				SendMultilineMessage(m_session, recout.c_str());
				if(++numFound >= 25)
				{
					RedSystemMessage(m_session, "More than 25 results found.");
					break;
				}
			}
		} // for loop (number of rows, up to 25)
	} // lookup name or description

	if(lookupcriteria && numFound < 25)
	{
		std::set<uint32> foundList;
		j = dbcAchievementCriteriaStore.GetNumRows();
		for(i = 0; i < j && numFound < 25; ++i)
		{
			AchievementCriteriaEntry const* criteria = dbcAchievementCriteriaStore.LookupRowForced(i);
			if(criteria)
			{
				if(foundList.find(criteria->ID) != foundList.end())
				{
					// already listed this achievement (some achievements have multiple entries in dbc)
					continue;
				}
				y = string(criteria->name);
				arcemu_TOLOWER(y);
				if(!FindXinYString(x, y))
				{
					continue;
				}
				foundList.insert(criteria->ID);
				std::stringstream strm;
				strm << criteria->ID;
				recout = "|cffffffffCriteria ";
				recout += strm.str();
				recout += ": |cfffff000";
				recout += criteria->name;
				strm.str("");
				AchievementEntry const* achievement = dbcAchievementStore.LookupEntryForced(criteria->referredAchievement);
				if(achievement)
				{
					// create achievement link
					recout += " |cffffffffAchievement ";
					strm << achievement->ID;
					recout +=  strm.str();
					recout += ": |cfffff000|Hachievement:";
					recout += strm.str();
					recout += ":";
					recout += (const char*)playerGUID;
					time_t completetime = m_session->GetPlayer()->GetAchievementMgr().GetCompletedTime(achievement);
					if(completetime)
					{
						// achievement is completed
						struct tm* ct;
						ct = localtime(&completetime);
						strm.str("");
						strm << ":1:" << ct->tm_mon + 1 << ":" << ct->tm_mday << ":" << ct->tm_year - 100 << ":-1:-1:-1:-1|h[";
						recout += strm.str();
					}
					else
					{
						// achievement is not completed
						recout += ":0:0:0:-1:0:0:0:0|h[";
					}
					recout += achievement->name;
					if(!lookupreward)
					{
						recout += "]|h|r";
					}
					else
					{
						recout += "]|h |cffffffff";
						recout += achievement->rewardName;
						recout += "|r";
					}
					strm.str("");
				}
				SendMultilineMessage(m_session, recout.c_str());
				if(++numFound >= 25)
				{
					RedSystemMessage(m_session, "More than 25 results found.");
					break;
				}
			}
		} // for loop (number of rows, up to 25)
	} // lookup criteria

	if(numFound == 0)
	{
		recout = "|cff00ccffNo matches found.";
		SendMultilineMessage(m_session, recout.c_str());
	}

	BlueSystemMessage(m_session, "Search completed in %u ms.", getMSTime() - t);

	return true;
}

#endif
