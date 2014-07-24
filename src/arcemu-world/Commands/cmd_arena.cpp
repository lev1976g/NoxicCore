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

bool ChatHandler::HandleArenaCreateTeamCommand(const char* args, WorldSession* m_session)
{
	uint32 arena_team_type;
	char name[1000];
	uint32 real_type;
	Player* plr = getSelectedChar(m_session, true);
	if(sscanf(args, "%u %[^\n]", &arena_team_type, name) != 2)
	{
		SystemMessage(m_session, "Invalid syntax.");
		return true;
	}

	switch(arena_team_type)
	{
		case 2:
			real_type = 0;
		break;
		case 3:
			real_type = 1;
		break;
		case 5:
			real_type = 2;
		break;
		default:
		{
			SystemMessage(m_session, "Invalid arena team type specified.");
			return true;
		}break;
	}

	if(!plr)
		return true;

	if(plr->m_arenaTeams[real_type])
	{
		SystemMessage(m_session, "Already in an arena team of that type.");
		return true;
	}

	ArenaTeam* t = new ArenaTeam(real_type, objmgr.GenerateArenaTeamId());
	t->m_emblemStyle = 22;
	t->m_emblemColour = 4292133532UL;
	t->m_borderColour = 4294931722UL;
	t->m_borderStyle = 1;
	t->m_backgroundColour = 4284906803UL;
	t->m_leader = plr->GetLowGUID();
	t->m_name = string(name);
	t->AddMember(plr->getPlayerInfo());
	objmgr.AddArenaTeam(t);
	SystemMessage(m_session, "created arena team.");
	return true;
}

bool ChatHandler::HandleArenaSetTeamLeaderCommand(const char* args, WorldSession* m_session)
{
	uint32 arena_team_type;
	uint32 real_type;
	Player* plr = getSelectedChar(m_session, true);
	if(sscanf(args, "%u", &arena_team_type) != 1)
	{
		SystemMessage(m_session, "Invalid syntax.");
		return true;
	}

	switch(arena_team_type)
	{
		case 2:
			real_type = 0;
		break;
		case 3:
			real_type = 1;
		break;
		case 5:
			real_type = 2;
		break;
		default:
		{
			SystemMessage(m_session, "Invalid arena team type specified.");
			return true;
		}break;
	}

	if(!plr)
		return true;

	if(!plr->m_arenaTeams[real_type])
	{
		SystemMessage(m_session, "Not in an arena team of that type.");
		return true;
	}

	ArenaTeam* t = plr->m_arenaTeams[real_type];
	t->SetLeader(plr->getPlayerInfo());
	SystemMessage(m_session, "player is now arena team leader.");
	return true;
}

bool ChatHandler::HandleArenaResetAllRatingsCommand(const char* args, WorldSession* m_session)
{
	objmgr.ResetArenaTeamRatings();
	SystemMessage(m_session, "All ratings reset for Arena Teams.");
	return true;
}