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

bool ChatHandler::HandleSaveAllCommand(const char* args, WorldSession* m_session)
{
	PlayerStorageMap::const_iterator itr;
	uint32 stime = now();
	uint32 count = 0;
	objmgr._playerslock.AcquireReadLock();
	for(itr = objmgr._players.begin(); itr != objmgr._players.end(); ++itr)
	{
		if(itr->second->GetSession())
		{
			itr->second->SaveToDB(false);
			count++;
		}
	}
	objmgr._playerslock.ReleaseReadLock();
	char msg[100];
	snprintf(msg, 100, "Saved all %u online players in %u msec.", count, now() - stime);
	sWorld.SendWorldText(msg);
	sWorld.SendWorldWideScreenText(msg);
	sGMLog.writefromsession(m_session, "saved all players");
	//sWorld.SendIRCMessage(msg);
	return true;
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

bool ChatHandler::HandleReloadScripts(const char* args, WorldSession* m_session)
{
	sScriptMgr.ReloadScriptEngines();
	SystemMessage(m_session, "Script engines reloaded.");
	return true;
}