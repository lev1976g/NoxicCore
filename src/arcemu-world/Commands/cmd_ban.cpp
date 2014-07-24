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

bool ChatHandler::HandleIPBanCommand(const char* args, WorldSession* m_session)
{
	char* pIp = (char*)args;
	char* pReason;
	char* pDuration;
	ParseBanArgs(pIp, &pDuration, &pReason);
	int32 timeperiod = 0;
	if(pDuration)
	{
		timeperiod = GetTimePeriodFromString(pDuration);
		if(timeperiod < 0)
			return false;
	}

	uint32 o1, o2, o3, o4;
	if(sscanf(pIp, "%3u.%3u.%3u.%3u", (unsigned int*)&o1, (unsigned int*)&o2, (unsigned int*)&o3, (unsigned int*)&o4) != 4 || o1 > 255 || o2 > 255 || o3 > 255 || o4 > 255)
	{
		RedSystemMessage(m_session, "Invalid IPv4 address [%s]", pIp);
		return true; // error in syntax, but we wont remind client of command usage
	}

	time_t expire_time;
	if(timeperiod == 0) // permanent ban
		expire_time = 0;
	else
		expire_time = UNIXTIME + (time_t)timeperiod;

	string IP = pIp;
	string::size_type i = IP.find("/");
	if(i == string::npos)
	{
		RedSystemMessage(m_session, "Lack of CIDR address assumes a 32bit match (if you don't understand, don't worry, it worked)");
		IP.append("/32");
	}

	//temporal IP or real pro flooder who will change it tomorrow ?
	char emptystring = 0;
	if(!pReason)
		pReason = &emptystring;

	SystemMessage(m_session, "Adding [%s] to IP ban table, expires %s.Reason is :%s", pIp, (expire_time == 0) ? "Never" : ctime(&expire_time), pReason);
	sLogonCommHandler.IPBan_Add(IP.c_str(), (uint32)expire_time, pReason);
	sWorld.DisconnectUsersWithIP(IP.substr(0, IP.find("/")).c_str(), m_session);
	sGMLog.writefromsession(m_session, "banned ip address %s, expires %s", pIp, (expire_time == 0) ? "Never" : ctime(&expire_time));
	return true;
}

bool ChatHandler::HandleBanCharacterCommand(const char* args, WorldSession* m_session)
{
	if(!*args)
		return false;

	char* pCharacter = (char*)args;
	PlayerInfo* pInfo = NULL;
	char* pReason;
	char* pDuration;
	int32 BanTime = 0;
	ParseBanArgs(pCharacter, &pDuration, &pReason);
	if(pDuration)
	{
		BanTime = GetTimePeriodFromString(pDuration);
		if(BanTime < 0) // if time is 0, ban is permanent
			return false;
	}

	Player* pPlayer = objmgr.GetPlayer(pCharacter, false);
	if(!pPlayer)
	{
		pInfo = objmgr.GetPlayerInfoByName(pCharacter);
		if(!pInfo)
		{
			SystemMessage(m_session, "Player not found.");
			return true;
		}
		SystemMessage(m_session, "Banning player '%s' in database for '%s'.", pCharacter, (!pReason) ? "No reason." : pReason);
		string escaped_reason = (!pReason) ? "No reason." : CharacterDatabase.EscapeString(string(pReason));
		CharacterDatabase.Execute("UPDATE characters SET banned = %u, banReason = '%s' WHERE guid = %u", BanTime ? BanTime + (uint32)UNIXTIME : 1, escaped_reason.c_str(), pInfo->guid);
	}
	else
	{
		SystemMessage(m_session, "Banning player '%s' ingame for '%s'.", pCharacter, (!pReason) ? "No reason." : pReason);
		string sReason = (!pReason) ? "No Reason." : string(pReason);
		uint32 uBanTime = BanTime ? BanTime + (uint32)UNIXTIME : 1;
		pPlayer->SetBanned(uBanTime, sReason);
		pInfo = pPlayer->getPlayerInfo();
	}
	SystemMessage(m_session, "This ban is due to expire %s%s.", BanTime ? "on " : "", BanTime ? ConvertTimeStampToDataTime(BanTime + (uint32)UNIXTIME).c_str() : "Never");

	sGMLog.writefromsession(m_session, "banned %s, reason %s, for %s", pCharacter, (!pReason) ? "No reason" : pReason, BanTime ? ConvertTimeStampToString(BanTime).c_str() : "ever");
	char msg[200];
	snprintf(msg, 200, "%sGM: %s has been banned by %s for %s. Reason: %s", MSG_COLOR_RED, pCharacter, m_session->GetPlayer()->GetName(), BanTime ? ConvertTimeStampToString(BanTime).c_str() : "ever", (!pReason) ? "No reason." : pReason);
	sWorld.SendWorldText(msg, NULL);
	if(sWorld.m_banTable && pInfo)
		CharacterDatabase.Execute("INSERT INTO %s VALUES('%s', '%s', %u, %u, '%s')", sWorld.m_banTable, m_session->GetPlayer()->GetName(), pInfo->name, (uint32)UNIXTIME, (uint32)UNIXTIME + BanTime, (!pReason) ? "No reason." : CharacterDatabase.EscapeString(string(pReason)).c_str());

	if(pPlayer)
	{
		SystemMessage(m_session, "Kicking %s.", pPlayer->GetName());
		pPlayer->Kick();
	}
	return true;
}

bool ChatHandler::HandleAccountBannedCommand(const char* args, WorldSession* m_session)
{
	if(!*args)
		return false;

	char* pAccount = (char*)args;
	char* pReason;
	char* pDuration;
	ParseBanArgs(pAccount, &pDuration, &pReason);
	int32 timeperiod = 0;
	if(pDuration)
	{
		timeperiod = GetTimePeriodFromString(pDuration);
		if(timeperiod < 0)
			return false;
	}
	uint32 banned = (timeperiod ? (uint32)UNIXTIME + timeperiod : 1);

	char emptystring = 0;
	if(!pReason)
		pReason = &emptystring;

	/*stringstream my_sql;
	my_sql << "UPDATE accounts SET banned = " << banned << " WHERE login = '" << CharacterDatabase.EscapeString(string(pAccount)) << "'";

	sLogonCommHandler.LogonDatabaseSQLExecute(my_sql.str().c_str());
	sLogonCommHandler.LogonDatabaseReloadAccounts();*/
	sLogonCommHandler.Account_SetBanned(pAccount, banned, pReason);

	GreenSystemMessage(m_session, "Account '%s' has been banned %s%s for reason : %s. The change will be effective immediately.", pAccount, timeperiod ? "until " : "forever", timeperiod ? ConvertTimeStampToDataTime(timeperiod + (uint32)UNIXTIME).c_str() : "", pReason);

	sWorld.DisconnectUsersWithAccount(pAccount, m_session);
	sGMLog.writefromsession(m_session, "banned account %s until %s", pAccount, timeperiod ? ConvertTimeStampToDataTime(timeperiod + (uint32)UNIXTIME).c_str() : "permanent");
	return true;
}

bool ChatHandler::HandleBanAllCommand(const char* args, WorldSession* m_session)
{
	if(!*args)
		return false;

	//our vars

	Player* pBanned;
	string pAcc;
	string pIP;
	string pArgs = args;
	char* pCharacter = (char*)args;
	char* pReason;
	char* pDuration;
	ParseBanArgs(pCharacter, &pDuration, &pReason);
	int32 BanTime = 0;
	if(pDuration)
	{
		BanTime = GetTimePeriodFromString(pDuration);
		if(BanTime < 0)
			return false;
	}
	pBanned = objmgr.GetPlayer(pCharacter, false);
	if(!pBanned || !pBanned->IsInWorld())
	{
		RedSystemMessage(m_session, "Player \'%s\' is not online or does not exists!", pCharacter);
		return true;
	}

	if(pBanned == m_session->GetPlayer())
	{
		RedSystemMessage(m_session, "You cannot ban yourself!");
		return true;
	}

	if(!pBanned->GetSession())
	{
		RedSystemMessage(m_session, "Player does not have a session!");
		return true;
	}
	if(!pBanned->GetSession()->GetSocket())
	{
		RedSystemMessage(m_session, "Player does not have a socket!");
		return true;
	}

	pAcc = pBanned->GetSession()->GetAccountName();
	pIP = pBanned->GetSession()->GetSocket()->GetRemoteIP();
	//This check is there incase a gm tries to ban someone on their LAN etc.
	if(pIP == m_session->GetSocket()->GetRemoteIP())
	{
		RedSystemMessage(m_session, "That player has the same IP as you - ban failed");
		return true;
	}

	//Checks complete. time to fire it up?
	/*char Msg[512];
	snprintf(Msg, 510, "%s [BAN] %s Player %s %s%s has been banned by %s %s %s- this is an account and ip ban. Reason: %s %s", MSG_COLOR_RED, MSG_COLOR_WHITE, MSG_COLOR_CYAN, pCharacter, MSG_COLOR_WHITE, MSG_COLOR_GREEN, m_session->GetPlayer()->GetName(), MSG_COLOR_WHITE, MSG_COLOR_RED, pReason);
	sWorld.SendWorldText(Msg, NULL);*/
	HandleBanCharacterCommand(pArgs.c_str(), m_session);
	char pIPCmd[256];
	snprintf(pIPCmd, 254, "%s %s %s", pIP.c_str(), pDuration, pReason);
	HandleIPBanCommand(pIPCmd, m_session);
	char pAccCmd[256];
	snprintf(pAccCmd, 254, "%s %s %s", pAcc.c_str(), pDuration, pReason);
	HandleAccountBannedCommand((const char*)pAccCmd, m_session);
	//GreenSystemMessage(m_session, "Successfully banned player %s with ip %s and account %s", pCharacter, pIP.c_str(), pAcc.c_str());
	return true;
}

bool ChatHandler::HandleIPUnBanCommand(const char* args, WorldSession* m_session)
{
	string pIp = args;
	if(!pIp.length())
		return false;

	if(pIp.find("/") == string::npos)
	{
		RedSystemMessage(m_session, "Lack of CIDR address assumes a 32bit match (if you don't understand, don't worry, it worked)");
		pIp.append("/32");
	}
	/**
	* We can afford to be less fussy with the validity of the IP address given since
	 * we are only attempting to remove it.
	 * Sadly, we can only blindly execute SQL statements on the logonserver so we have
	 * no idea if the address existed and so the account/IPBanner cache requires reloading.
	 */

	SystemMessage(m_session, "Deleting [%s] from ip ban table if it exists", pIp.c_str());
	sLogonCommHandler.IPBan_Remove(pIp.c_str());
	sGMLog.writefromsession(m_session, "unbanned ip address %s", pIp.c_str());
	return true;
}

bool ChatHandler::HandleUnBanCharacterCommand(const char* args, WorldSession* m_session)
{
	if(!*args)
		return false;

	char Character[255];
	if(sscanf(args, "%s", Character) != 1)
	{
		RedSystemMessage(m_session, "A character name and a reason are required.");
		return true;
	}

	// Check if player is in world.
	Player* pPlayer = ObjectMgr::getSingleton().GetPlayer(Character, false);
	if(pPlayer)
	{
		GreenSystemMessage(m_session, "Unbanned player %s ingame.", pPlayer->GetName());
		pPlayer->UnSetBanned();
	}
	else
		GreenSystemMessage(m_session, "Player %s not found ingame.", Character);

	// Ban in database
	CharacterDatabase.Execute("UPDATE characters SET banned = 0 WHERE name = '%s'", CharacterDatabase.EscapeString(string(Character)).c_str());

	SystemMessage(m_session, "Unbanned character %s in database.", Character);
	sGMLog.writefromsession(m_session, "unbanned %s", Character);
	return true;
}

bool ChatHandler::HandleAccountUnbanCommand(const char* args, WorldSession* m_session)
{
	if(!*args)
		return false;

	char* pAccount = (char*)args;
	sLogonCommHandler.Account_SetBanned(pAccount, 0, "");
	GreenSystemMessage(m_session, "Account '%s' has been unbanned. This change will be effective immediately.", pAccount);
	sGMLog.writefromsession(m_session, "unbanned account %s", pAccount);
	return true;
}