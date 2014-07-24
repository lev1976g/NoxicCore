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

bool ChatHandler::HandleGenderChanger(const char* args, WorldSession* m_session)
{
	uint8 gender;
	Player* target = objmgr.GetPlayer((uint32)m_session->GetPlayer()->GetSelection());
	if(target == NULL)
	{
		SystemMessage(m_session, "Select A Player first.");
		return true;
	}
	uint32 displayId = target->GetNativeDisplayId();
	if(*args == 0)
		gender = target->getGender() == 1 ? 0 : 1;
	else
	{
		gender = (uint8)atoi((char*)args);
		if(gender > 1)
			gender = 1;
	}

	if(gender == target->getGender())
	{
		SystemMessage(m_session, "%s's gender is already set to %s(%u).", target->GetName(), gender ? "Female" : "Male", gender);
		return true;
	}

	target->setGender(gender);

	if(target->getGender() == 0)
	{
		target->SetDisplayId((target->getRace() == RACE_BLOODELF) ? ++displayId : --displayId);
		target->SetNativeDisplayId(displayId);
	}
	else
	{
		target->SetDisplayId((target->getRace() == RACE_BLOODELF) ? --displayId : ++displayId);
		target->SetNativeDisplayId(displayId);
	}
	target->EventModelChange();

	SystemMessage(m_session, "Set %s's gender to %s(%u).", target->GetName(), gender ? "Female" : "Male", gender);
	return true;
}

bool ChatHandler::HandleModifyLevelCommand(const char* args, WorldSession* m_session)
{
	Player* plr = getSelectedChar(m_session, true);
	if(plr == 0) return true;

	uint32 Level = args ? atol(args) : 0;
	if(Level == 0 || Level > sWorld.m_levelCap)
	{
		RedSystemMessage(m_session, "A level (numeric) is required to be specified after this command.");
		return true;
	}

	// Set level message
	BlueSystemMessage(m_session, "Setting the level of %s to %u.", plr->GetName(), Level);
	GreenSystemMessageToPlr(plr, "%s set your level to %u.", m_session->GetPlayer()->GetName(), Level);

	sGMLog.writefromsession(m_session, "used modify level on %s, level %u", plr->GetName(), Level);

	// lookup level information
	LevelInfo* Info = objmgr.GetLevelInfo(plr->getRace(), plr->getClass(), Level);
	if(Info == NULL)
	{
		RedSystemMessage(m_session, "Levelup information not found.");
		return true;
	}

	plr->ApplyLevelInfo(Info, Level);
	if(plr->getClass() == WARLOCK)
	{
		std::list<Pet*> summons = plr->GetSummons();
		for(std::list<Pet*>::iterator itr = summons.begin(); itr != summons.end(); ++itr)
		{
			Pet* summon = *itr;
			if(summon->IsInWorld() && summon->isAlive())
			{
				summon->setLevel(Level);
				summon->ApplyStatsForLevel();
				summon->UpdateSpellList();
			}
		}
	}

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