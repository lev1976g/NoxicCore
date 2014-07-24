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

bool ChatHandler::HandleGuildJoinCommand(const char* args, WorldSession* m_session)
{
	if(!*args)
		return false;

	Player* ptarget = getSelectedChar(m_session);
	if(!ptarget) return false;

	if(ptarget->IsInGuild())
	{
		RedSystemMessage(m_session, "%s is already in a guild.", ptarget->GetName());
		return true;
	}

	Guild* pGuild = NULL;
	pGuild = objmgr.GetGuildByGuildName(string(args));

	if(pGuild)
	{
		pGuild->getLock().Acquire();
		uint32 memberCount = pGuild->GetNumMembers();
		pGuild->getLock().Release();
		
		if( memberCount >= MAX_GUILD_MEMBERS ){
			m_session->SystemMessage( "That guild is full." );
			return true;
		}

		pGuild->AddGuildMember(ptarget->getPlayerInfo(), m_session, -2);
		GreenSystemMessage(m_session, "You have joined the guild '%s'", pGuild->GetGuildName());
		sGMLog.writefromsession(m_session, "Force joined guild '%s'", pGuild->GetGuildName());
		return true;
	}

	return false;
}

bool ChatHandler::HandleCreateGuildCommand(const char* args, WorldSession* m_session)
{
	if(!*args)
		return false;

	Player* ptarget = getSelectedChar(m_session);
	if(!ptarget) return false;

	if(ptarget->IsInGuild())
	{
		RedSystemMessage(m_session, "%s is already in a guild.", ptarget->GetName());
		return true;
	}

	if(strlen((char*)args) > 75)
	{
		// send message to user
		char buf[256];
		snprintf((char*)buf, 256, "The name was too long by %u", (uint32)strlen(args) - 75);
		SystemMessage(m_session, buf);
		return true;
	}

	for(uint32 i = 0; i < strlen(args); i++)
	{
		if(!isalpha(args[i]) && args[i] != ' ')
		{
			SystemMessage(m_session, "Error, name can only contain chars A-Z and a-z.");
			return true;
		}
	}

	Guild* pGuild = NULL;
	pGuild = objmgr.GetGuildByGuildName(string(args));

	if(pGuild)
	{
		RedSystemMessage(m_session, "Guild name is already taken.");
		return true;
	}

	Charter tempCharter(0, ptarget->GetLowGUID(), CHARTER_TYPE_GUILD);
	tempCharter.SignatureCount = 0;
	tempCharter.GuildName = string(args);

	pGuild = Guild::Create();
	pGuild->CreateFromCharter(&tempCharter, ptarget->GetSession());
	GreenSystemMessage(m_session, "Guild created");
	sGMLog.writefromsession(m_session, "Created guild '%s'", args);
	return true;
}

bool ChatHandler::HandleRenameGuildCommand(const char* args, WorldSession* m_session)
{
	Player* plr = getSelectedChar(m_session);
	if(!plr || !plr->GetGuildId() || !plr->GetGuild() || !args || !strlen(args))
		return false;

	Guild* pGuild = objmgr.GetGuildByGuildName(string(args));

	if(pGuild)
	{
		RedSystemMessage(m_session, "Guild name %s is already taken.", args);
		return false;
	}

	GreenSystemMessage(m_session, "Changed guild name of %s to %s. This will take effect next restart.", plr->GetGuild()->GetGuildName(), args);
	CharacterDatabase.Execute("UPDATE guilds SET `guildName` = \'%s\' WHERE `guildId` = '%u'", CharacterDatabase.EscapeString(string(args)).c_str(), plr->GetGuild()->GetGuildId());
	sGMLog.writefromsession(m_session, "Changed guild name of '%s' to '%s'", plr->GetGuild()->GetGuildName(), args);
	return true;
}

bool ChatHandler::HandleGuildMembersCommand(const char* args, WorldSession* m_session)
{
	Player* plr = getSelectedChar(m_session);
	if(!plr || !plr->GetGuildId() || !plr->GetGuild())
		return false;
	GreenSystemMessage(m_session, "Now showing guild members for %s", plr->GetGuild()->GetGuildName());
	plr->GetGuild()->Lock();
	for(GuildMemberMap::iterator itr = plr->GetGuild()->GetGuildMembersBegin(); itr != plr->GetGuild()->GetGuildMembersEnd(); ++itr)
	{
		GuildMember* member = itr->second;
		if(!member || !member->pPlayer)
			continue;

		BlueSystemMessage(m_session, "%s (Rank: %s)", member->pPlayer->name, member->pRank->szRankName);
	}
	plr->GetGuild()->Unlock();
	return true;
}

bool ChatHandler::HandleGuildRemovePlayerCommand(const char* args, WorldSession* m_session)
{
	Player* plr = getSelectedChar(m_session);
	if(!plr || !plr->GetGuildId() || !plr->GetGuild() || plr->GetGuild()->GetGuildLeader() == plr->GetLowGUID())
		return false;

	GreenSystemMessage(m_session, "Kicked %s from Guild: %s", plr->GetName(), plr->GetGuild()->GetGuildName());

	if(plr->GetLowGUID() != m_session->GetPlayer()->GetLowGUID())
		sGMLog.writefromsession(m_session, "Kicked %s from Guild %s", plr->GetName(), plr->GetGuild()->GetGuildName());

	plr->GetGuild()->RemoveGuildMember(plr->getPlayerInfo(), plr->GetSession());
	return true;
}

bool ChatHandler::HandleGuildDisbandCommand(const char* args, WorldSession* m_session)
{
	Player* plr = getSelectedChar(m_session);
	if(!plr || !plr->GetGuildId() || !plr->GetGuild())
		return false;

	GreenSystemMessage(m_session, "Disbanded Guild: %s", plr->GetGuild()->GetGuildName());
	sGMLog.writefromsession(m_session, "Disbanded Guild %s", plr->GetGuild()->GetGuildName());
	plr->GetGuild()->Disband();
	return true;
}