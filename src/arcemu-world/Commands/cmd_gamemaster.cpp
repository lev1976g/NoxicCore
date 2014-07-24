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

bool ChatHandler::HandleFlyCommand(const char* args, WorldSession* m_session)
{
	Player* chr = getSelectedChar(m_session);

	if(!chr)
		chr = m_session->GetPlayer();

	if(!*args)
	{
		if(chr->FlyCheat)
			args = "off";
		else
			args = "on";
	}

	if(stricmp(args, "on") == 0)
	{
		WorldPacket fly(835, 13);
		chr->m_setflycheat = true;
		fly << chr->GetNewGUID();
		fly << uint32(2);
		chr->SendMessageToSet(&fly, true);
		BlueSystemMessage(chr->GetSession(), "Flying mode enabled.");
		if(chr != m_session->GetPlayer())
			sGMLog.writefromsession(m_session, "enabled flying mode for %s", chr->GetName());
	}
	else if(stricmp(args, "off") == 0)
	{
		WorldPacket fly(836, 13);
		chr->m_setflycheat = false;
		fly << chr->GetNewGUID();
		fly << uint32(5);
		chr->SendMessageToSet(&fly, true);
		BlueSystemMessage(chr->GetSession(), "Flying mode disabled.");
		if(chr != m_session->GetPlayer())
			sGMLog.writefromsession(m_session, "disabled flying mode for %s", chr->GetName());
	}
	else
		return false;
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

bool ChatHandler::HandleGMOffCommand(const char* args, WorldSession* m_session)
{
	Player* _player = m_session->GetPlayer();
	if(_player->HasFlag(PLAYER_FLAGS, PLAYER_FLAG_GM))
	{
		_player->RemoveFlag(PLAYER_FLAGS, PLAYER_FLAG_GM); // <GM>
		_player->TriggerpassCheat = false;
		_player->SetFaction(_player->GetInitialFactionId());
		_player->UpdatePvPArea();

		BlueSystemMessage(m_session, "GM Flag Removed. <GM> Will no longer show in chat messages or above your name.");

		_player->UpdateVisibility();
	}

	return true;
}

bool ChatHandler::HandleGMOnCommand(const char* args, WorldSession* m_session)
{
	GreenSystemMessage(m_session, "Setting GM Flag on yourself.");

	Player* _player = m_session->GetPlayer();
	if(_player->HasFlag(PLAYER_FLAGS, PLAYER_FLAG_GM))
		RedSystemMessage(m_session, "GM Flag is already set on. Use .gm off to disable it.");
	else
	{
		_player->SetFlag(PLAYER_FLAGS, PLAYER_FLAG_GM);	// <GM>
		_player->TriggerpassCheat = true;
		_player->SetFaction(35);
		_player->RemovePvPFlag();

		BlueSystemMessage(m_session, "GM flag set. It will now appear above your name and in chat messages until you use .gm off.");

		_player->UpdateVisibility();
	}

	return true;
}

bool ChatHandler::HandleWhisperBlockCommand(const char* args, WorldSession* m_session)
{
	if(m_session->GetPlayer()->HasFlag(PLAYER_FLAGS, PLAYER_FLAG_GM))
		return false;

	m_session->GetPlayer()->SetFlag(PLAYER_FLAGS, PLAYER_FLAG_GM);
	return true;
}

bool ChatHandler::HandleAllowWhispersCommand(const char* args, WorldSession* m_session)
{
	if(args == 0 || strlen(args) < 2) return false;
	PlayerCache* playercache = objmgr.GetPlayerCache(args, false);
	if(playercache == NULL)
	{
		RedSystemMessage(m_session, "Player not found.");
		return true;
	}

	m_session->GetPlayer()->m_cache->InsertValue64(CACHE_GM_TARGETS, playercache->GetUInt32Value(CACHE_PLAYER_LOWGUID));
	std::string name;
	playercache->GetStringValue(CACHE_PLAYER_NAME, name);
	BlueSystemMessage(m_session, "Now accepting whispers from %s.", name.c_str());
	playercache->DecRef();
	return true;
}

bool ChatHandler::HandleBlockWhispersCommand(const char* args, WorldSession* m_session)
{
	if(args == 0 || strlen(args) < 2) return false;
	PlayerCache* playercache = objmgr.GetPlayerCache(args, false);
	if(playercache == NULL)
	{
		RedSystemMessage(m_session, "Player not found.");
		return true;
	}

	m_session->GetPlayer()->m_cache->RemoveValue64(CACHE_GM_TARGETS, playercache->GetUInt32Value(CACHE_PLAYER_LOWGUID));
	std::string name;
	playercache->GetStringValue(CACHE_PLAYER_NAME, name);
	BlueSystemMessage(m_session, "Now blocking whispers from %s.", name.c_str());
	playercache->DecRef();
	return true;
}

bool ChatHandler::HandleKillByPlayerCommand(const char* args, WorldSession* m_session)
{
	if(!args || strlen(args) < 2)
	{
		RedSystemMessage(m_session, "A player's name is required.");
		return true;
	}

	sWorld.DisconnectUsersWithPlayerName(args, m_session);
	sGMLog.writefromsession(m_session, "disconnected player %s", args);
	return true;
}

bool ChatHandler::HandleKillBySessionCommand(const char* args, WorldSession* m_session)
{
	if(!args || strlen(args) < 2)
	{
		RedSystemMessage(m_session, "A player's name is required.");
		return true;
	}

	sWorld.DisconnectUsersWithAccount(args, m_session);
	sGMLog.writefromsession(m_session, "disconnected player with account %s", args);
	return true;
}
bool ChatHandler::HandleKillByIPCommand(const char* args, WorldSession* m_session)
{
	if(!args || strlen(args) < 2)
	{
		RedSystemMessage(m_session, "An IP is required.");
		return true;
	}

	sWorld.DisconnectUsersWithIP(args, m_session);
	sGMLog.writefromsession(m_session, "disconnected players with IP %s", args);
	return true;
}