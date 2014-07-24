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

bool ChatHandler::HandleAccountLevelCommand(const char* args, WorldSession* m_session)
{
	if(!*args)
		return false;

	char account[100];
	char gmlevel[100];
	int argc = sscanf(args, "%s %s", account, gmlevel);
	if(argc != 2)
		return false;

	sLogonCommHandler.Account_SetGM(account, gmlevel);

	GreenSystemMessage(m_session, "Account '%s' level has been updated to '%s'. The change will be effective immediately.", account, gmlevel);
	sGMLog.writefromsession(m_session, "set account %s flags to %s", account, gmlevel);
	return true;
}

bool ChatHandler::HandleAccountMuteCommand(const char* args, WorldSession* m_session)
{
	if(!*args)
		return false;

	char* pAccount = (char*)args;
	char* pDuration = strchr(pAccount, ' ');
	if(!pDuration)
		return false;

	*pDuration = 0;
	++pDuration;

	int32 timeperiod = GetTimePeriodFromString(pDuration);
	if(timeperiod <= 0)
		return false;

	uint32 banned = (uint32)UNIXTIME + timeperiod;

	sLogonCommHandler.Account_SetMute(pAccount, banned);
	string tsstr = ConvertTimeStampToDataTime(timeperiod + (uint32)UNIXTIME);
	GreenSystemMessage(m_session, "Account '%s' has been muted until %s. The change will be effective immediately.", pAccount, tsstr.c_str());
	sGMLog.writefromsession(m_session, "mutex account %s until %s", pAccount, ConvertTimeStampToDataTime(timeperiod + (uint32)UNIXTIME).c_str());

	WorldSession* pSession = sWorld.FindSessionByName(pAccount);
	if(pSession)
	{
		pSession->m_muted = banned;
		pSession->SystemMessage("Your voice has been muted until %s by a GM. Until this time, you will not be able to speak in any form.", tsstr.c_str());
	}

	return true;
}

bool ChatHandler::HandleAccountUnmuteCommand(const char* args, WorldSession* m_session)
{
	sLogonCommHandler.Account_SetMute(args, 0);

	GreenSystemMessage(m_session, "Account '%s' has been unmuted.", args);
	sGMLog.writefromsession(m_session, "unmuted account %s", args);
	WorldSession* pSession = sWorld.FindSessionByName(args);
	if(pSession)
	{
		pSession->m_muted = 0;
		pSession->SystemMessage("Your voice has been restored. You may speak again.");
	}

	return true;
}