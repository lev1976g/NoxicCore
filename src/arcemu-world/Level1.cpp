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
