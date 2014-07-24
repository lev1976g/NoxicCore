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

bool ChatHandler::HandleAdministratorCommand(const char* args, WorldSession* m_session)
{
	HandleGMOffCommand(args, m_session);
	Player* _player = m_session->GetPlayer();

	if(!stricmp(args, "on"))
	{
		if(!_player->HasFlag(PLAYER_FLAGS, PLAYER_FLAG_ADMIN))
			_player->SetFlag(PLAYER_FLAGS, PLAYER_FLAG_ADMIN);
		else
			RedSystemMessage(m_session, "You already have the admin flag off.");
	}
	else if(!stricmp(args, "off"))
	{
		if(_player->HasFlag(PLAYER_FLAGS, PLAYER_FLAG_ADMIN))
			_player->RemoveFlag(PLAYER_FLAGS, PLAYER_FLAG_ADMIN);
		else
			RedSystemMessage(m_session, "You already have the admin flag on.");
	}

	return true;
}

bool ChatHandler::HandleCastAllCommand(const char* args, WorldSession* m_session)
{
	if(!args || strlen(args) < 2)
	{
		RedSystemMessage(m_session, "No spellid specified.");
		return true;
	}

	uint32 spellid = atol(args);
	SpellEntry* info = dbcSpell.LookupEntryForced(spellid);
	if(!info)
	{
		RedSystemMessage(m_session, "Invalid spell specified.");
		return true;
	}

	// this makes sure no moron casts a learn spell on everybody and wrecks the server
	for(int i = 0; i < 3; i++)
	{
		if(info->Effect[i] == SPELL_EFFECT_LEARN_SPELL) //SPELL_EFFECT_LEARN_SPELL - 36
		{
			sGMLog.writefromsession(m_session, "used wrong / learnall castall command, spellid %u", spellid);
			RedSystemMessage(m_session, "Learn spell specified.");
			return true;
		}
	}

	sGMLog.writefromsession(m_session, "used castall command, spellid %u", spellid);

	objmgr._playerslock.AcquireReadLock();
	for(PlayerStorageMap::const_iterator itr = objmgr._players.begin(); itr != objmgr._players.end(); ++itr)
	{
		Player* plr = itr->second;
		if(plr->GetSession() && plr->IsInWorld())
		{
			if(plr->GetMapMgr() != m_session->GetPlayer()->GetMapMgr())
				sEventMgr.AddEvent(TO<Unit*>(plr), &Unit::EventCastSpell, TO<Unit*>(plr), info, EVENT_PLAYER_CHECKFORCHEATS, 100, 1, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
			else
			{
				Spell* sp = sSpellFactoryMgr.NewSpell(plr, info, true, 0);
				SpellCastTargets targets(plr->GetGUID());
				sp->prepare(&targets);
			}
		}
	}
	objmgr._playerslock.ReleaseReadLock();

	BlueSystemMessage(m_session, "Casted spell %u on all players!", spellid);
	return true;
}

bool ChatHandler::HandleDispelAllCommand(const char* args, WorldSession* m_session)
{
	uint32 pos = 0;
	if(*args)
		pos = atoi(args);

	sGMLog.writefromsession(m_session, "used dispelall command, pos %u", pos);

	objmgr._playerslock.AcquireReadLock();
	for(PlayerStorageMap::const_iterator itr = objmgr._players.begin(); itr != objmgr._players.end(); ++itr)
	{
		Player* plr = itr->second;
		if(plr->GetSession() && plr->IsInWorld())
		{
			if(plr->GetMapMgr() != m_session->GetPlayer()->GetMapMgr())
				sEventMgr.AddEvent(TO<Unit*>(plr), &Unit::DispelAll, pos ? true : false, EVENT_PLAYER_CHECKFORCHEATS, 100, 1, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
			else
				plr->DispelAll(pos ? true : false);
		}
	}
	sGMLog.writefromsession(m_session, "used mass dispel");
	objmgr._playerslock.ReleaseReadLock();

	BlueSystemMessage(m_session, "Dispel action done.");
	return true;
}

bool ChatHandler::HandleRenameAllCharacter(const char* args, WorldSession* m_session)
{
	uint32 uCount = 0;
	uint32 ts = getMSTime();
	QueryResult* result = CharacterDatabase.Query("SELECT guid, name FROM characters");
	if(result)
	{
		do
		{
			uint32 uGuid = result->Fetch()[0].GetUInt32();
			const char* pName = result->Fetch()[1].GetString();
			size_t szLen = strlen(pName);

			if(VerifyName(pName, szLen) != E_CHAR_NAME_SUCCESS)
			{
				LOG_DEBUG("Renaming character %s, %u", pName, uGuid);
				Player* pPlayer = objmgr.GetPlayer(uGuid);
				if(pPlayer)
				{
					pPlayer->rename_pending = true;
					pPlayer->GetSession()->SystemMessage("Your character has had a force rename set, you will be prompted to rename your character at next login in conformance with server rules.");
				}

				CharacterDatabase.WaitExecute("UPDATE characters SET forced_rename_pending = 1 WHERE guid = %u", uGuid);
				++uCount;
			}

		}
		while(result->NextRow());
		delete result;
	}

	SystemMessage(m_session, "Procedure completed in %u ms. %u character(s) forced to rename.", getMSTime() - ts, uCount);
	return true;
}

bool ChatHandler::HandleMassSummonCommand(const char* args, WorldSession* m_session)
{
	objmgr._playerslock.AcquireReadLock();
	Player* summoner = m_session->GetPlayer();
	int faction = -1;
	char Buffer[170];
	if(*args == 'a' || *args == 'A')
	{
		faction = TEAM_ALLIANCE;
		snprintf(Buffer, 170, "%s%s Has requested a mass summon of all Alliance players. Do not feel obliged to accept the summon, as it is most likely for an event or a test of sorts", MSG_COLOR_GOLD, m_session->GetPlayer()->GetName());
	}
	else if(*args == 'h' || *args == 'H')
	{
		faction = TEAM_HORDE;
		snprintf(Buffer, 170, "%s%s Has requested a mass summon of all Horde players. Do not feel obliged to accept the summon, as it is most likely for an event or a test of sorts", MSG_COLOR_GOLD, m_session->GetPlayer()->GetName());
	}
	else
		snprintf(Buffer, 170, "%s%s Has requested a mass summon of all players. Do not feel obliged to accept the summon, as it is most likely for an event or a test of sorts", MSG_COLOR_GOLD, m_session->GetPlayer()->GetName());

	uint32 c = 0;

	for(PlayerStorageMap::const_iterator itr = objmgr._players.begin(); itr != objmgr._players.end(); ++itr)
	{
		Player* plr = itr->second;
		if(plr->GetSession() && plr->IsInWorld())
		{
			//plr->SafeTeleport(summoner->GetMapId(), summoner->GetInstanceID(), summoner->GetPosition());
			/* let's do this the blizz way */
			if(faction > -1 && plr->GetTeam() == static_cast<uint32>(faction))
			{
				plr->SummonRequest(summoner->GetLowGUID(), summoner->GetZoneId(), summoner->GetMapId(), summoner->GetInstanceID(), summoner->GetPosition());
				++c;
			}
			else if(faction == -1)
			{
				plr->SummonRequest(summoner->GetLowGUID(), summoner->GetZoneId(), summoner->GetMapId(), summoner->GetInstanceID(), summoner->GetPosition());
				++c;
			}
		}
	}
	sGMLog.writefromsession(m_session, "requested a mass summon of %u players.", c);
	objmgr._playerslock.ReleaseReadLock();
	return true;
}

bool ChatHandler::HandleGlobalPlaySoundCommand(const char* args, WorldSession* m_session)
{
	if(*args == '\0')
		return false;

	uint32 sound = atoi(args);
	if(!sound)
		return false;

	sWorld.PlaySoundToAll(sound);
	BlueSystemMessage(m_session, "Broadcasted sound %u to server.", sound);
	sGMLog.writefromsession(m_session, "used play all command with soundid [%u]", sound);

	return true;
}