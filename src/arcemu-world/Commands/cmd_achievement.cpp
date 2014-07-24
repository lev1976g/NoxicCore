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

#ifdef ENABLE_ACHIEVEMENTS
/**
	Handles .achieve complete
	.achieve complete id : completes achievement "id" (can be an achievement link) for the selected player
*/
bool ChatHandler::HandleAchievementCompleteCommand(const char* args, WorldSession* m_session)
{
	if(!*args)
		return false;

	Player* plr = getSelectedChar(m_session, true);
	if(!plr)
	{
		plr = m_session->GetPlayer();
		SystemMessage(m_session, "Auto-targeting self.");
	}

	uint32 achievement_id = atol(args);
	if(!achievement_id)
	{
		achievement_id = GetAchievementIDFromLink(args);
		if(!achievement_id)
		{
			if(!stricmp(args, "all"))
			{
				plr->GetAchievementMgr().GMCompleteAchievement(m_session, -1);
				SystemMessage(m_session, "All achievements have now been completed for that player.");
				sGMLog.writefromsession(m_session, "completed all achievements for player %s", plr->GetName());
				return true;
			}
			return false;
		}
	}

	if(plr->GetAchievementMgr().GMCompleteAchievement(m_session, achievement_id))
	{
		SystemMessage(m_session, "The achievement has now been completed for that player.");
		sGMLog.writefromsession(m_session, "completed achievement %u for player %s", achievement_id, plr->GetName());
	}
	return true;
}

/**
	Handles .achieve criteria
	.achieve criteria id : completes achievement criteria "id" for the selected player
*/
bool ChatHandler::HandleAchievementCriteriaCommand(const char* args, WorldSession* m_session)
{
	if(!*args)
		return false;

	Player* plr = getSelectedChar(m_session, true);
	if(!plr)
	{
		plr = m_session->GetPlayer();
		SystemMessage(m_session, "Auto-targeting self.");
	}

	uint32 criteria_id = atol(args);
	if(!criteria_id)
	{
		if(!stricmp(args, "all"))
		{
			plr->GetAchievementMgr().GMCompleteCriteria(m_session, -1);
			SystemMessage(m_session, "All achievement criteria have now been completed for that player.");
			sGMLog.writefromsession(m_session, "completed all achievement criteria for player %s", plr->GetName());
			return true;
		}
		return false;
	}

	if(plr->GetAchievementMgr().GMCompleteCriteria(m_session, criteria_id))
	{
		SystemMessage(m_session, "The achievement criteria has now been completed for that player.");
		sGMLog.writefromsession(m_session, "completed achievement criteria %u for player %s", criteria_id, plr->GetName());
	}
	return true;
}

/**
	Handles .achieve reset
	.achieve reset id			: removes achievement "id" (can be an achievement link) from the selected player
	.achieve reset criteria id	: removes achievement criteria "id" from the selected player
	.achieve reset all			: removes all achievement and criteria data from the selected player
*/
bool ChatHandler::HandleAchievementResetCommand(const char* args, WorldSession* m_session)
{
	if(!*args)
		return false;

	Player* plr = getSelectedChar(m_session, true);
	if(!plr)
	{
		plr = m_session->GetPlayer();
		SystemMessage(m_session, "Auto-targeting self.");
	}

	bool resetAch = true, resetCri = false;
	int32 achievement_id;
	if(!strnicmp(args, "criteria ", 9))
	{
		achievement_id = atol(args + 9);
		if(!achievement_id)
		{
			if(stricmp(args + 9, "all"))
				return false;

			achievement_id = -1;
		}
		resetCri = true;
		resetAch = false;
	}
	else if(!stricmp(args, "all"))
	{
		achievement_id = -1;
		resetCri = true;
	}
	else
	{
		achievement_id = atol(args);
		if(!achievement_id)
		{
			achievement_id = GetAchievementIDFromLink(args);
			if(!achievement_id)
				return false;
		}
	}

	if(resetAch)
		plr->GetAchievementMgr().GMResetAchievement(achievement_id);
	if(resetCri)
		plr->GetAchievementMgr().GMResetCriteria(achievement_id);

	return true;
}

/**
	Handles .lookup achievement
	GM achievement lookup command usage:
	.lookup achievement string          : searches for "string" in achievement name
	.lookup achievement desc string     : searches for "string" in achievement description
	.lookup achievement reward string   : searches for "string" in achievement reward name
	.lookup achievement criteria string : searches for "string" in achievement criteria name
	.lookup achievement all string      : searches for "string" in achievement name, description, reward, and critiera
*/
#endif