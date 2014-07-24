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
//  Normal User Chat Commands
//

#include "StdAfx.h"
#include <git_version.h>

bool ChatHandler::ShowHelpForCommand(WorldSession* m_session, ChatCommand* table, const char* cmd)
{
	for(uint32 i = 0; table[i].Name != NULL; i++)
	{
		if(!hasStringAbbr(table[i].Name, cmd))
			continue;

		if(m_session->CanUseCommand(table[i].CommandGroup))
			continue;

		if(table[i].ChildCommands != NULL)
		{
			cmd = strtok(NULL, " ");
			if(cmd && ShowHelpForCommand(m_session, table[i].ChildCommands, cmd))
				return true;
		}

		if(table[i].Help == "")
		{
			SystemMessage(m_session, "There is no help for that command");
			return true;
		}

		SendMultilineMessage(m_session, table[i].Help.c_str());

		return true;
	}

	return false;
}

bool ChatHandler::HandleInfoCommand(const char* args, WorldSession* m_session)
{
	WorldPacket data;


	//uint32 clientsNum = (uint32)sWorld.GetSessionCount();

	int gm = 0;
	int count = 0;
	int avg = 0;
	PlayerStorageMap::const_iterator itr;
	objmgr._playerslock.AcquireReadLock();
	for(itr = objmgr._players.begin(); itr != objmgr._players.end(); ++itr)
	{
		if(itr->second->GetSession())
		{
			count++;
			avg += itr->second->GetSession()->GetLatency();
			if(itr->second->GetSession()->GetPermissionCount())
				gm++;
		}
	}
	objmgr._playerslock.ReleaseReadLock();
	GreenSystemMessage(m_session, "Server Revision: |r%sArcEmu %s/%s-%s-%s %s(www.arcemu.org)", MSG_COLOR_WHITE,
		BUILD_HASH_STR, CONFIG, PLATFORM_TEXT, ARCH, MSG_COLOR_LIGHTBLUE);
	GreenSystemMessage(m_session, "Server Uptime: |r%s", sWorld.GetUptimeString().c_str());
	GreenSystemMessage(m_session, "Current Players: |r%d (%d GMs) (%u Peak)", count, gm, sWorld.PeakSessionCount);
	GreenSystemMessage(m_session, "Active Thread Count: |r%u", ThreadPool.GetActiveThreadCount());
	GreenSystemMessage(m_session, "Free Thread Count: |r%u", ThreadPool.GetFreeThreadCount());
	GreenSystemMessage(m_session, "Average Latency: |r%.3fms", ((float)avg / (float)count));
	GreenSystemMessage(m_session, "CPU Usage: %3.2f %%", sWorld.GetCPUUsage());
	GreenSystemMessage(m_session, "RAM Usage: %4.2f MB", sWorld.GetRAMUsage());
	GreenSystemMessage(m_session, "SQL Query Cache Size (World): |r%u queries delayed", WorldDatabase.GetQueueSize());
	GreenSystemMessage(m_session, "SQL Query Cache Size (Character): |r%u queries delayed", CharacterDatabase.GetQueueSize());

	return true;
}

bool ChatHandler::HandleNetworkStatusCommand(const char* args, WorldSession* m_session)
{
	//sSocketMgr.ShowStatus();
	return true;
}

bool ChatHandler::HandleSaveCommand(const char* args, WorldSession* m_session)
{
	Player* p_target = getSelectedChar(m_session, false);
	if(p_target == NULL)
		return false;

	if(p_target->m_nextSave < 300000)  //5min out of 10 left so 5 min since last save
	{
		p_target->SaveToDB(false);
		GreenSystemMessage(m_session, "Player %s saved to DB", p_target->GetName());
	}
	else
	{
		RedSystemMessage(m_session, "You can only save once every 5 minutes.");
	}
	return true;
}

bool ChatHandler::HandleGMListCommand(const char* args, WorldSession* m_session)
{
	WorldPacket data;
	bool first = true;

	bool isGM = m_session->GetPermissionCount() != 0;

	PlayerStorageMap::const_iterator itr;
	objmgr._playerslock.AcquireReadLock();
	for(itr = objmgr._players.begin(); itr != objmgr._players.end(); ++itr)
	{
		if(itr->second->GetSession()->GetPermissionCount())
		{
			if(isGM || !sWorld.gamemaster_listOnlyActiveGMs || (sWorld.gamemaster_listOnlyActiveGMs && itr->second->HasFlag(PLAYER_FLAGS, PLAYER_FLAG_GM)))
			{
				if(first)
					GreenSystemMessage(m_session, "There are following active GMs on this server:");

				if(sWorld.gamemaster_hidePermissions && !isGM)
					SystemMessage(m_session, " - %s", itr->second->GetName());
				else
				{
					if(sWorld.gamemaster_listOnlyActiveGMs && !itr->second->HasFlag(PLAYER_FLAGS, PLAYER_FLAG_GM))
						SystemMessage(m_session, "|cff888888 - %s [%s]|r", itr->second->GetName(), itr->second->GetSession()->GetPermissions());
					else
						SystemMessage(m_session, " - %s [%s]", itr->second->GetName(), itr->second->GetSession()->GetPermissions());
				}

				first = false;
			}
		}
	}
	objmgr._playerslock.ReleaseReadLock();

	if(first)
		SystemMessage(m_session, "There are no GMs currently logged in on this server.");

	return true;
}

bool ChatHandler::HandleGMStatusCommand(const char* args, WorldSession* m_session)
{
	if(m_session->GetPlayer()->HasFlag(PLAYER_FLAGS, PLAYER_FLAG_GM))
		BlueSystemMessage(m_session, "GM Flag: On");
	else
		BlueSystemMessage(m_session, "GM Flag: Off");

	if(m_session->GetPlayer()->m_isGmInvisible)
		BlueSystemMessage(m_session, "GM Invis: On");
	else
		BlueSystemMessage(m_session, "GM Invis: Off");

	return true;
}