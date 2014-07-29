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

bool ChatHandler::HandleLookupItemCommand(const char* args, WorldSession* m_session)
{
	if(!*args)
		return false;

	string x = string(args);
	arcemu_TOLOWER(x);
	if(x.length() < 2)
	{
		RedSystemMessage(m_session, "Your search string must be at least 2 characters long.");
		return false;
	}

	BlueSystemMessage(m_session, "Starting search of item `%s`...", x.c_str());
	uint32 t = getMSTime();
	ItemPrototype* it;
	uint32 count = 0;

	StorageContainerIterator<ItemPrototype> * itr = ItemPrototypeStorage.MakeIterator();

	while(!itr->AtEnd())
	{
		it = itr->Get();
		LocalizedItem* lit	= (m_session->language > 0) ? sLocalizationMgr.GetLocalizedItem(it->ItemId, m_session->language) : NULL;
		std::string litName	= std::string(lit ? lit->Name : "");
		arcemu_TOLOWER(litName);

		bool localizedFound	= false;
		if(FindXinYString(x, litName))
			localizedFound	= true;

		if(FindXinYString(x, it->lowercase_name) || localizedFound)
		{
			// Print out the name in a cool highlighted fashion
			//SendHighlightedName(m_session, it->Name1, it->lowercase_name, x, it->ItemId, true);
			SendItemLinkToPlayer(it, m_session, false, 0, localizedFound ? m_session->language : 0);
			++count;
			if(count == 100)
			{
				RedSystemMessage(m_session, "More than 100 results returned. Aborting.");
				break;
			}
		}

		if(!itr->Inc())
			break;
	}
	itr->Destruct();
	BlueSystemMessage(m_session, "Search completed in %u ms.", getMSTime() - t);
	return true;
}

ARCEMU_INLINE std::string MyConvertIntToString(const int arg)
{
	stringstream out;
	out << arg;
	return out.str();
}

bool ChatHandler::HandleLookupQuestCommand(const char* args, WorldSession* m_session)
{
	if(!*args)
		return false;

	string x = string(args);
	arcemu_TOLOWER(x);
	if(x.length() < 2)
	{
		RedSystemMessage(m_session, "Your search string must be at least 2 characters long.");
		return true;
	}

	BlueSystemMessage(m_session, "Starting search of quests `%s`...", x.c_str());
	uint32 t = getMSTime();

	StorageContainerIterator<Quest> * itr = QuestStorage.MakeIterator();

	Quest* i;
	uint32 count = 0;
	string y;
	string recout;

	while(!itr->AtEnd())
	{
		i = itr->Get();
		y = string(i->title);

		LocalizedQuest* li	= (m_session->language > 0) ? sLocalizationMgr.GetLocalizedQuest(i->id, m_session->language) : NULL;

		std::string liName	= std::string(li ? li->Title : "");

		arcemu_TOLOWER(liName);
		arcemu_TOLOWER(y);

		bool localizedFound	= false;
		if(FindXinYString(x, liName))
			localizedFound	= true;

		if(FindXinYString(x, y) || localizedFound)
		{
			string questid = MyConvertIntToString(i->id);
			const char* questtitle = localizedFound ? (li ? li->Title : "") : i->title;
			// send quest link
			recout = questid.c_str();
			recout += ": |cff00ccff|Hquest:";
			recout += questid.c_str();
			recout += ":";
			recout += MyConvertIntToString(i->min_level);
			recout += "|h[";
			recout += questtitle;
			recout += "]|h|r";
			SendMultilineMessage(m_session, recout.c_str());
			++count;
			if(count == 100)
			{
				RedSystemMessage(m_session, "More than 100 results returned. Aborting.");
				break;
			}
		}
		if(!itr->Inc())
			break;
	}
	itr->Destruct();

	if(!count)
	{
		recout = "|cff00ccffNo matches found.\n\n";
		SendMultilineMessage(m_session, recout.c_str());
	}

	BlueSystemMessage(m_session, "Search completed in %u ms.", getMSTime() - t);
	return true;
}

bool ChatHandler::HandleLookupCreatureCommand(const char* args, WorldSession* m_session)
{
	if(!*args)
		return false;

	string x = string(args);
	arcemu_TOLOWER(x);
	if(x.length() < 2)
	{
		RedSystemMessage(m_session, "Your search string must be at least 2 characters long.");
		return true;
	}

	StorageContainerIterator<CreatureInfo> * itr = CreatureNameStorage.MakeIterator();

	GreenSystemMessage(m_session, "Starting search of creature `%s`...", x.c_str());
	uint32 t = getMSTime();
	CreatureInfo* i;
	uint32 count = 0;
	while(!itr->AtEnd())
	{
		i = itr->Get();
		LocalizedCreatureName* li = (m_session->language > 0) ? sLocalizationMgr.GetLocalizedCreatureName(i->Id, m_session->language) : NULL;
		std::string liName	= std::string(li ? li->Name : "");
		arcemu_TOLOWER(liName);

		bool localizedFound	= false;
		if(FindXinYString(x, liName))
			localizedFound	= true;

		if(FindXinYString(x, i->lowercase_name) || localizedFound)
		{
			// Print out the name in a cool highlighted fashion
			SendHighlightedName(m_session, "Creature", localizedFound ? li->Name : i->Name, localizedFound ? liName : i->lowercase_name, x, i->Id);
			++count;
			if(count == 100)
			{
				RedSystemMessage(m_session, "More than 100 results returned. Aborting.");
				break;
			}
		}
		if(!itr->Inc())
			break;
	}
	itr->Destruct();
	GreenSystemMessage(m_session, "Search completed in %u ms.", getMSTime() - t);
	return true;
}

bool ChatHandler::HandleLookupObjectCommand(const char* args, WorldSession* m_session)
{
	if(!*args)
		return false;

	string x = string(args);
	arcemu_TOLOWER(x);
	StorageContainerIterator<GameObjectInfo> * itr = GameObjectNameStorage.MakeIterator();
	GreenSystemMessage(m_session, "Starting search of object `%s`...", x.c_str());
	uint32 t = getMSTime();
	GameObjectInfo* i;
	uint32 count = 0;
	string y;
	string recout;
	while(!itr->AtEnd())
	{
		i = itr->Get();
		y = string(i->Name);
		arcemu_TOLOWER(y);
		if(FindXinYString(x, y))
		{
			//string objectID=MyConvertIntToString(i->ID);
			string Name;
			std::stringstream strm;
			strm << i->ID;
			strm << ", Display ";
			strm << i->DisplayID;
			//string ObjectID = i.c_str();
			const char* objectName = i->Name;
			recout = "|cfffff000Object ";
			recout += strm.str();
			recout += "|cffFFFFFF: ";
			recout += objectName;
			recout = recout + Name;
			SendMultilineMessage(m_session, recout.c_str());
			++count;
			if(count == 100)
			{
				RedSystemMessage(m_session, "More than 100 results returned. Aborting.");
				break;
			}
		}
		if(!itr->Inc()) break;
	}
	itr->Destruct();
	if(!count)
	{
		recout = "|cff00ccffNo matches found.";
		SendMultilineMessage(m_session, recout.c_str());
	}
	BlueSystemMessage(m_session, "Search completed in %u ms.", getMSTime() - t);
	return true;
}

bool ChatHandler::HandleLookupSpellCommand(const char* args, WorldSession* m_session)
{
	if(!*args)
		return false;

	string x = string(args);
	arcemu_TOLOWER(x);
	if(x.length() < 2)
	{
		RedSystemMessage(m_session, "Your search string must be at least 2 characters long.");
		return true;
	}

	GreenSystemMessage(m_session, "Starting search of spell `%s`...", x.c_str());
	uint32 t = getMSTime();
	uint32 count = 0;
	string recout;
	char itoabuf[12];
	for(uint32 index = 0; index < dbcSpell.GetNumRows(); ++index)
	{
		SpellEntry* spell = dbcSpell.LookupRow(index);
		string y = string(spell->Name);
		arcemu_TOLOWER(y);
		if(FindXinYString(x, y))
		{
			// Print out the name in a cool highlighted fashion
			// SendHighlightedName(m_session, "Spell", spell->Name, y, x, spell->Id);
			// Send spell link instead
			sprintf((char*)itoabuf, "%u", spell->Id);
			recout = (const char*)itoabuf;
			recout += ": |cff71d5ff|Hspell:";
			recout += (const char*)itoabuf;
			recout += "|h[";
			recout += spell->Name;
			recout += "]|h|r";

			std::string::size_type pos = recout.find('%');
			if(pos != std::string::npos)
				recout.insert(pos + 1, "%");

			SendMultilineMessage(m_session, recout.c_str());
			++count;
			if(count == 100)
			{
				RedSystemMessage(m_session, "More than 100 results returned. Aborting.");
				break;
			}
		}
	}

	GreenSystemMessage(m_session, "Search completed in %u ms.", getMSTime() - t);
	return true;
}

bool ChatHandler::HandleLookupSkillCommand(const char* args, WorldSession* m_session)
{
	if(!*args)
		return false;

	string x = string(args);
	arcemu_TOLOWER(x);
	if(x.length() < 2)
	{
		RedSystemMessage(m_session, "Your search string must be at least 2 characters long.");
		return true;
	}

	GreenSystemMessage(m_session, "Starting search of skill `%s`...", x.c_str());
	uint32 t = getMSTime();
	uint32 count = 0;
	for(uint32 index = 0; index < dbcSkillLine.GetNumRows(); ++index)
	{
		skilllineentry* skill = dbcSkillLine.LookupRow(index);
		string y = string(skill->Name);
		arcemu_TOLOWER(y);
		if(FindXinYString(x, y))
		{
			// Print out the name in a cool highlighted fashion
			SendHighlightedName(m_session, "Skill", skill->Name, y, x, skill->id);
			++count;
			if(count == 100)
			{
				RedSystemMessage(m_session, "More than 100 results returned. Aborting.");
				break;
			}
		}
	}

	GreenSystemMessage(m_session, "Search completed in %u ms.", getMSTime() - t);
	return true;
}

bool ChatHandler::HandleLookupFactionCommand(const char* args, WorldSession* m_session)
{
	if(!*args)
		return false;

	string x = string(args);
	arcemu_TOLOWER(x);
	if(x.length() < 2)
	{
		RedSystemMessage(m_session, "Your search string must be at least 2 characters long.");
		return true;
	}

	GreenSystemMessage(m_session, "Starting search of faction `%s`...", x.c_str());
	uint32 t = getMSTime();
	uint32 count = 0;
	for(uint32 index = 0; index < dbcFaction.GetNumRows(); ++index)
	{
		FactionDBC* faction = dbcFaction.LookupRow(index);
		string y = string(faction->Name);
		arcemu_TOLOWER(y);
		if(FindXinYString(x, y))
		{
			// Print out the name in a cool highlighted fashion
			SendHighlightedName(m_session, "Faction", faction->Name, y, x, faction->ID);
			++count;
			if(count == 100)
			{
				RedSystemMessage(m_session, "More than 100 results returned. Aborting.");
				break;
			}
		}
	}

	GreenSystemMessage(m_session, "Search completed in %u ms.", getMSTime() - t);
	return true;
}

#ifdef ENABLE_ACHIEVEMENTS
/**
	Handles .lookup achievement
	GM achievement lookup command usage:
	.lookup achievement string          : searches for "string" in achievement name
	.lookup achievement desc string     : searches for "string" in achievement description
	.lookup achievement reward string   : searches for "string" in achievement reward name
	.lookup achievement criteria string : searches for "string" in achievement criteria name
	.lookup achievement all string      : searches for "string" in achievement name, description, reward, and critiera
*/
bool ChatHandler::HandleLookupAchievementCmd(const char* args, WorldSession* m_session)
{
	if(!*args)
		return false;

	string x;
	bool lookupname = true, lookupdesc = false, lookupcriteria = false, lookupreward = false;
	if(!strnicmp(args, "name ", 5))
		x = string(args + 5);
	else if(!strnicmp(args, "desc ", 5))
	{
		lookupname = false;
		lookupdesc = true;
		x = string(args + 5);
	}
	else if(!strnicmp(args, "criteria ", 9))
	{
		lookupname = false;
		lookupcriteria = true;
		x = string(args + 9);
	}
	else if(!strnicmp(args, "reward ", 7))
	{
		lookupname = false;
		lookupreward = true;
		x = string(args + 7);
	}
	else if(!strnicmp(args, "all ", 4))
	{
		lookupdesc = true;
		lookupcriteria = true;
		lookupreward = true;
		x = string(args + 4);
	}
	else
		x = string(args);

	if(x.length() < 2)
	{
		RedSystemMessage(m_session, "Your search string must be at least 2 characters long.");
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
					continue;

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
					recout += "]|h|r";
				else
				{
					recout += "]|h |cffffffff";
					recout += achievement->rewardName;
					recout += "|r";
				}

				strm.str("");
				SendMultilineMessage(m_session, recout.c_str());
				if(++numFound >= 100)
				{
					RedSystemMessage(m_session, "More than 100 results found. Aborting");
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
					continue;

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
						recout += "]|h|r";
					else
					{
						recout += "]|h |cffffffff";
						recout += achievement->rewardName;
						recout += "|r";
					}
					strm.str("");
				}
				SendMultilineMessage(m_session, recout.c_str());
				if(++numFound >= 100)
				{
					RedSystemMessage(m_session, "More than 100 results found. Aborting");
					break;
				}
			}
		}
	} // lookup criteria

	if(!numFound)
	{
		recout = "|cff00ccffNo matches found.";
		SendMultilineMessage(m_session, recout.c_str());
	}

	BlueSystemMessage(m_session, "Search completed in %u ms.", getMSTime() - t);
	return true;
}

#endif