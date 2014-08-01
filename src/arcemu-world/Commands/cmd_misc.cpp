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

bool ChatHandler::HandleNYICommand(const char* args, WorldSession* m_session)
{
	RedSystemMessage(m_session, "Command not yet implemented.");
	return true;
}

bool ChatHandler::HandleAddTrainerSpellCommand(const char* args, WorldSession* m_session)
{
	Creature* pCreature = getSelectedCreature(m_session, true);
	if(!pCreature)
		return false;

	uint32 spellid, cost, reqspell, reqlevel, delspell;
	if(sscanf(args, "%u %u %u %u %u", &spellid, &cost, &reqspell, &reqlevel, &delspell) != 5)
		return false;

	Trainer* pTrainer = pCreature->GetTrainer();
	if(!pTrainer)
	{
		RedSystemMessage(m_session, "Target %u is not a trainer.", pCreature->GetEntry());
		return false;
	}

	SpellEntry* pSpell = dbcSpell.LookupEntryForced(spellid);
	if(!pSpell)
	{
		RedSystemMessage(m_session, "Spell entry '%u' is an invalid spell.", spellid);
		return false;
	}

	if(pSpell->Effect[0] == SPELL_EFFECT_INSTANT_KILL || pSpell->Effect[1] == SPELL_EFFECT_INSTANT_KILL || pSpell->Effect[2] == SPELL_EFFECT_INSTANT_KILL)
	{
		RedSystemMessage(m_session, "Terminated. Console denied.");
		return false;
	}

	TrainerSpell sp;
	sp.Cost = cost;
	sp.IsProfession = false;
	sp.pLearnSpell = pSpell;
	sp.pCastRealSpell = NULL;
	sp.pCastSpell = NULL;
	sp.RequiredLevel = reqlevel;
	sp.RequiredSpell = reqspell;
	sp.DeleteSpell = delspell;

	pTrainer->Spells.push_back(sp);
	pTrainer->SpellCount++;

	SystemMessage(m_session, "Added spell %u (%s) to trainer.", pSpell->Id, pSpell->Name);
	sGMLog.writefromsession(m_session, "added spell %u to trainer %u", spellid, pCreature->GetEntry());
	WorldDatabase.Execute("INSERT INTO trainer_spells VALUES(%u, %u, %u, %u, %u, %u, %u, %u, %u, %u)", pCreature->GetEntry(), (int)0, pSpell->Id, cost, reqspell, (int)0, (int)0, reqlevel, delspell, (int)0);

	return true;
}

bool ChatHandler::HandleAnnounceCommand(const char* args, WorldSession* m_session)
{
	if(!*args || strlen(args) < 3 || strchr(args, '%'))
	{
		m_session->SystemMessage("Announces cannot contain the %% character and must be at least 3 characters.");
		return false;
	}

	char msg[1024];
	string input2;
	input2 = sWorld.ann_tagcolor;
	input2 += "[";
	input2 += sWorld.announce_tag;
	input2 += "]";
	input2 += sWorld.ann_gmtagcolor;
	if(sWorld.GMAdminTag)
	{
		if(m_session->CanUseCommand('3'))
			input2 += "<Admin>";
		else if(m_session->GetPermissionCount())
			input2 += "<GM>";
	}
	if(sWorld.NameinAnnounce)
	{
		input2 += "|r" + sWorld.ann_namecolor + "|Hplayer:";
		input2 += m_session->GetPlayer()->GetName();
		input2 += "|h[";
		input2 += m_session->GetPlayer()->GetName();
		input2 += "]|h:|r " + sWorld.ann_msgcolor;
	}
	else if(!sWorld.NameinAnnounce)
	{
		input2 += ": ";
		input2 += sWorld.ann_msgcolor;
	}

	snprintf((char*)msg, 1024, "%s%s", input2.c_str(), args);
	sWorld.SendWorldText(msg); // send message
	sGMLog.writefromsession(m_session, "used announce command, [%s]", args);
	return true;
}

bool ChatHandler::HandleAppearCommand(const char* args, WorldSession* m_session)
{
	if(!*args)
		return false;

	// Appear Blocking
	if(!stricmp(args, "on"))
	{
		if(m_session->GetPlayer()->IsAppearDisabled())
			BlueSystemMessage(m_session, "Appear blocking is already enabled.");
		else
		{
			m_session->GetPlayer()->DisableAppear(true);
			GreenSystemMessage(m_session, "Appear blocking is now enabled.");
		}
		return true;
	}
	else if(!stricmp(args, "off"))
	{
		if(m_session->GetPlayer()->IsAppearDisabled())
		{
			m_session->GetPlayer()->DisableAppear(false);
			GreenSystemMessage(m_session, "Appear blocking is now disabled.");
		}
		else
			BlueSystemMessage(m_session, "Appear blocking is already disabled.");

		return true;
	}

	Player* chr = objmgr.GetPlayer(args, false);
	if(chr)
	{
		char buf[256];
		if(!m_session->CanUseCommand('z') && chr->IsAppearDisabled())
		{
			snprintf((char*)buf, 256, "Player %s has blocked other GMs from appearing to them.", chr->GetName());
			SystemMessage(m_session, buf);
			return true;
		}

		if(!chr->GetMapMgr())
		{
			snprintf((char*)buf, 256, "Player %s is already being teleported.", chr->GetName());
			SystemMessage(m_session, buf);
			return false;
		}
		snprintf((char*)buf, 256, "You are appearing at %s's location.", chr->GetName()); // -- europa
		SystemMessage(m_session, buf);

		if(!m_session->GetPlayer()->m_isGmInvisible)
		{
			char buf0[256];
			snprintf((char*)buf0, 256, "Player %s is appearing at your location.", m_session->GetPlayer()->GetName());
			SystemMessageToPlr(chr, buf0);
		}

		//m_session->GetPlayer()->SafeTeleport(chr->GetMapId(), chr->GetInstanceID(), chr->GetPosition());
		//If the GM is on the same map as the player, use the normal safeteleport method
		if(m_session->GetPlayer()->GetMapId() == chr->GetMapId() && m_session->GetPlayer()->GetInstanceID() == chr->GetInstanceID())
			m_session->GetPlayer()->SafeTeleport(chr->GetMapId(), chr->GetInstanceID(), chr->GetPosition());
		else
			m_session->GetPlayer()->SafeTeleport(chr->GetMapMgr(), chr->GetPosition());
		//The player and GM are not on the same map. We use this method so we can port to BG's (Above method doesn't support them)
	}
	else
	{
		char buf[256];
		snprintf((char*)buf, 256, "Player %s does not exist or is not logged in.", args);
		SystemMessage(m_session, buf);
	}

	return true;
}

float CalculateDistance(float x1, float y1, float z1, float x2, float y2, float z2)
{
	float dx = x1 - x2;
	float dy = y1 - y2;
	float dz = z1 - z2;
	return sqrt(dx * dx + dy * dy + dz * dz);
}

bool ChatHandler::HandleSimpleDistanceCommand(const char* args , WorldSession* m_session)
{
	float toX, toY, toZ;
	if(sscanf(args, "%f %f %f", &toX, &toY, &toZ) != 3)
		return false;

	if(toX >= _maxX || toX <= _minX || toY <= _minY || toY >= _maxY)
		return false;

	float distance = CalculateDistance(m_session->GetPlayer()->GetPositionX(), m_session->GetPlayer()->GetPositionY(), m_session->GetPlayer()->GetPositionZ(), toX, toY, toZ);
	m_session->SystemMessage("Your distance to location (%f, %f, %f) is %0.2f meters.", toX, toY, toZ, distance);

	return true;
}

bool ChatHandler::HandleClearCooldownsCommand(const char* args, WorldSession* m_session)
{
	uint32 guid = (uint32)m_session->GetPlayer()->GetSelection();
	Player* plr = getSelectedChar(m_session, true);
	if(!plr)
	{
		plr = m_session->GetPlayer();
		SystemMessage(m_session, "Auto-targeting self.");
	}

	if(!plr)
		return false;

	if(plr != m_session->GetPlayer())
		sGMLog.writefromsession(m_session, "Cleared all cooldowns for player %s", plr->GetName());

	if(plr->getClass() == WARRIOR)
	{
		plr->ClearCooldownsOnLine(26, guid);
		plr->ClearCooldownsOnLine(256, guid);
		plr->ClearCooldownsOnLine(257 , guid);
		BlueSystemMessage(m_session, "Cleared all Warrior cooldowns.");
		return true;
	}
	else if(plr->getClass() == PALADIN)
	{
		plr->ClearCooldownsOnLine(594, guid);
		plr->ClearCooldownsOnLine(267, guid);
		plr->ClearCooldownsOnLine(184, guid);
		BlueSystemMessage(m_session, "Cleared all Paladin cooldowns.");
		return true;
	}
	else if(plr->getClass() == HUNTER)
	{
		plr->ClearCooldownsOnLine(50, guid);
		plr->ClearCooldownsOnLine(51, guid);
		plr->ClearCooldownsOnLine(163, guid);
		BlueSystemMessage(m_session, "Cleared all Hunter cooldowns.");
		return true;
	}
	else if(plr->getClass() == ROGUE)
	{
		plr->ClearCooldownsOnLine(253, guid);
		plr->ClearCooldownsOnLine(38, guid);
		plr->ClearCooldownsOnLine(39, guid);
		BlueSystemMessage(m_session, "Cleared all Rogue cooldowns.");
		return true;
	}
	else if(plr->getClass() == PRIEST)
	{
		plr->ClearCooldownsOnLine(56, guid);
		plr->ClearCooldownsOnLine(78, guid);
		plr->ClearCooldownsOnLine(613, guid);
		BlueSystemMessage(m_session, "Cleared all Priest cooldowns.");
		return true;
	}
	else if(plr->getClass() == DEATHKNIGHT)
	{
		plr->ClearCooldownsOnLine(770, guid);
		plr->ClearCooldownsOnLine(771, guid);
		plr->ClearCooldownsOnLine(772, guid);
		BlueSystemMessage(m_session, "Cleared all Death Knight cooldowns.");
		return true;
	}
	else if(plr->getClass() == SHAMAN)
	{
		plr->ClearCooldownsOnLine(373, guid);
		plr->ClearCooldownsOnLine(374, guid);
		plr->ClearCooldownsOnLine(375, guid);
		BlueSystemMessage(m_session, "Cleared all Shaman cooldowns.");
		return true;
	}
	else if(plr->getClass() == MAGE)
	{
		plr->ClearCooldownsOnLine(6, guid);
		plr->ClearCooldownsOnLine(8, guid);
		plr->ClearCooldownsOnLine(237, guid);
		BlueSystemMessage(m_session, "Cleared all Mage cooldowns.");
		return true;
	}
	else if(plr->getClass() == WARLOCK)
	{
		plr->ClearCooldownsOnLine(355, guid);
		plr->ClearCooldownsOnLine(354, guid);
		plr->ClearCooldownsOnLine(593, guid);
		BlueSystemMessage(m_session, "Cleared all Warlock cooldowns.");
		return true;
	}
	else if(plr->getClass() == DRUID)
	{
		plr->ClearCooldownsOnLine(573, guid);
		plr->ClearCooldownsOnLine(574, guid);
		plr->ClearCooldownsOnLine(134, guid);
		BlueSystemMessage(m_session, "Cleared all Druid cooldowns.");
		return true;
	}
	return true;
}

bool ChatHandler::HandleCommandsCommand(const char* args, WorldSession* m_session)
{
	ChatCommand* table = CommandTableStorage::getSingleton().Get();
	WorldPacket data;

	std::string output;
	uint32 count = 0;

	output = "Current available commands: \n\n";
	for(uint32 i = 0; table[i].Name != NULL; i++)
	{
		if(*args && !hasStringAbbr(table[i].Name, (char*)args))
			continue;

		if(table[i].CommandGroup != '0' && !m_session->CanUseCommand(table[i].CommandGroup))
			continue;

		switch(table[i].CommandGroup)
		{
			case '3':
			{
				output += "|cffff6060";
				output += table[i].Name;
				output += "|r, ";
			}break;
			case '2':
			{
				output += "|cff00ffff";
				output += table[i].Name;
				output += "|r, ";
			}break;
			case '1':
			{
				output += "|cff00ff00";
				output += table[i].Name;
				output += "|r, ";
			}break;
			default:
			{
				output += "|cff00ccff";
				output += table[i].Name;
				output += "|r, ";
			}break;
		}

		count++;
		if(count == 5) // 5 per line
		{
			output += "\n";
			count = 0;
		}
	}
	if(count)
		output += "\n";

	SendMultilineMessage(m_session, output.c_str());
	return true;
}

bool ChatHandler::HandleCooldownCommand(const char* args, WorldSession* m_session)
{
	Player* plr = getSelectedChar(m_session, true);

	if(!plr)
	{
		plr = m_session->GetPlayer();
		SystemMessage(m_session, "Auto-targeting self.");
	}

	if(!plr)
		return false;

	if(plr != m_session->GetPlayer())
		sGMLog.writefromsession(m_session, "Cleared all cooldowns for player %s", plr->GetName());

	plr->ResetAllCooldowns();
	return true;
}

bool ChatHandler::HandleDeMorphCommand(const char* args, WorldSession* m_session)
{
	Player* target = getSelectedChar(m_session);
	if(!target)
		target = m_session->GetPlayer();

	target->DeMorph();
	return true;
}

bool ChatHandler::HandleDeveloperCommand(const char* args, WorldSession* m_session)
{
	HandleGMOffCommand(args, m_session);
	Player* _player = m_session->GetPlayer();

	if(!stricmp(args, "on"))
	{
		if(!_player->HasFlag(PLAYER_FLAGS, PLAYER_FLAG_DEVELOPER))
			_player->SetFlag(PLAYER_FLAGS, PLAYER_FLAG_DEVELOPER);
		else
			RedSystemMessage(m_session, "You already have the developer flag off.");
	}
	else if(!stricmp(args, "off"))
	{
		if(_player->HasFlag(PLAYER_FLAGS, PLAYER_FLAG_DEVELOPER))
			_player->RemoveFlag(PLAYER_FLAGS, PLAYER_FLAG_DEVELOPER);
		else
			RedSystemMessage(m_session, "You already have the developer flag on.");
	}

	return true;
}

bool ChatHandler::HandleKillCommand(const char* args, WorldSession* m_session)
{
	Unit* target = m_session->GetPlayer()->GetMapMgr()->GetUnit(m_session->GetPlayer()->GetSelection());
	if(!target)
	{
		RedSystemMessage(m_session, "A valid selection is required.");
		return false;
	}

	switch(target->GetTypeId())
	{
		case TYPEID_PLAYER:
			sGMLog.writefromsession(m_session, "used kill command on PLAYER %s", TO<Player*>(target)->GetName());
		break;
		case TYPEID_UNIT:
			sGMLog.writefromsession(m_session, "used kill command on CREATURE %u [%s], sqlid %u%s", TO<Creature*>(target)->GetEntry(), TO<Creature*>(target)->GetCreatureInfo()->Name, TO<Creature*>(target)->GetSQL_id(), m_session->GetPlayer()->InGroup() ? ", in group" : "");
		break;
	}

	// If we're killing a player, send a message indicating a gm killed them.
	if(target->IsPlayer())
	{
		Player* plr = TO<Player*>(target);
		plr->SetHealth(0);
		plr->KillPlayer();
		BlueSystemMessageToPlr(plr, "The player %s killed you with a GM command.", m_session->GetPlayer()->GetName());
	}
	else
	{
		// Cast insta-kill.
		SpellEntry* se = dbcSpell.LookupEntryForced(5);
		if(!se)
			return false;

		SpellCastTargets targets(target->GetGUID());
		Spell* sp = sSpellFactoryMgr.NewSpell(m_session->GetPlayer(), se, true, 0);
		sp->prepare(&targets);

		/*SpellEntry* se = dbcSpell.LookupEntry(20479);
		if(!se)
			return false;

		SpellCastTargets targets(target->GetGUID());
		Spell* sp = sSpellFactoryMgr.NewSpell(target, se, true, 0);
		sp->prepare(&targets);*/
	}

	return true;
}

bool ChatHandler::HandleDismountCommand(const char* args, WorldSession* m_session)
{
	Unit* m_target = NULL;
	Player* p_target = getSelectedChar(m_session, false);
	Creature* m_crt = getSelectedCreature(m_session, false);

	if(p_target)
		m_target = p_target;
	else
	{
		if(m_crt)
			m_target = m_crt;
	}

	if(!m_target)
	{
		RedSystemMessage(m_session, "No selected target was found.");
		return false;
	}

	if(!m_target->GetMount())
	{
		RedSystemMessage(m_session, "The selected target is not mounted.");
		return false;
	}

	if(p_target)
		p_target->Dismount();
	/*else if(m_crt)
	{*/
		m_target->SetUInt32Value(UNIT_FIELD_MOUNTDISPLAYID, 0);
		/*m_target->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_MOUNTED_TAXI);
	}*/

	BlueSystemMessage(m_session, "Target now unmounted.");
	return true;
}

bool ChatHandler::HandleFixScaleCommand(const char* args, WorldSession* m_session)
{
	Creature* pCreature = getSelectedCreature(m_session, true);
	if(!pCreature)
		return false;

	float sc = (float)atof(args);
	if(sc < 0.1f)
		return false;

	pCreature->SetScale(sc);
	pCreature->GetProto()->Scale = sc;
	WorldDatabase.Execute("UPDATE creature_proto SET scale = '%f' WHERE entry = %u", sc, pCreature->GetEntry());
	return true;
}

bool ChatHandler::HandleGMAnnounceCommand(const char* args, WorldSession* m_session)
{
	if(!*args)
	{
		Log.Error("cmd_misc.cpp", "HandleGMAnnounceCommand !args = failed");
		return false;
	}

	char GMAnnounce[1024];
	snprintf(GMAnnounce, 1024, MSG_COLOR_RED "[Team]" MSG_COLOR_GREEN " |Hplayer:%s|h[%s]|h:" MSG_COLOR_YELLOW " %s", m_session->GetPlayer()->GetName(), m_session->GetPlayer()->GetName(), args);
	sWorld.SendGMWorldText(GMAnnounce);
	sGMLog.writefromsession(m_session, "used team announce command, [%s]", args);
	return true;
}

bool ChatHandler::HandleTriggerCommand(const char* args, WorldSession* m_session)
{
	if(!args)
	{
		RedSystemMessage(m_session, "No information was provided.");
		return true;
	}
	int32 instance_id;
	uint32 trigger_id;
	int valcount = sscanf(args, "%u %d", (unsigned int*)&trigger_id, (int*)&instance_id);
	if(valcount < 1)
		return false;

	if(valcount == 1)
		instance_id = 0;

	AreaTriggerEntry* entry = dbcAreaTrigger.LookupEntryForced(trigger_id);
	if(!trigger_id || !entry)
	{
		RedSystemMessage(m_session, "Could not find area trigger %s.", args);
		return true;
	}

	m_session->GetPlayer()->SafeTeleport(entry->mapid, instance_id, LocationVector(entry->x, entry->y, entry->z, entry->o));
	BlueSystemMessage(m_session, "Teleported to area trigger %u on [%u][%.2f][%.2f][%.2f].", entry->id, entry->mapid, entry->x, entry->y, entry->z);
	return true;
}

bool ChatHandler::HandleGPSCommand(const char* args, WorldSession* m_session)
{
	Object* obj;
	uint64 guid = m_session->GetPlayer()->GetSelection();
	if(guid)
	{
		if((obj = m_session->GetPlayer()->GetMapMgr()->GetUnit(guid)) == 0)
		{
			SystemMessage(m_session, "You should select a character or a creature.");
			return false;
		}
	}
	else
		obj = m_session->GetPlayer();

	char buf[328];
	AreaTable* at = dbcArea.LookupEntryForced(obj->GetMapMgr()->GetAreaID(obj->GetPositionX(), obj->GetPositionY()));
	if(!at)
	{
		snprintf((char*)buf, 328, "|cff00ff00Current Position: |cffffffffMap: |cff00ff00%d |cffffffffX: |cff00ff00%f |cffffffffY: |cff00ff00%f |cffffffffZ: |cff00ff00%f |cffffffffOrientation: |cff00ff00%f|r", (unsigned int)obj->GetMapId(), obj->GetPositionX(), obj->GetPositionY(), obj->GetPositionZ(), obj->GetOrientation());
		SystemMessage(m_session, buf);
		return true;
	}

	snprintf((char*)buf, 328, "|cff00ff00Current Position: |cffffffffMap: |cff00ff00%d |cffffffffZone: |cff00ff00%u |cffffffffArea: |cff00ff00%u  |cffffffffX: |cff00ff00%f |cffffffffY: |cff00ff00%f |cffffffffZ: |cff00ff00%f |cffffffffOrientation: |cff00ff00%f |cffffffffArea Name: |cff00ff00%s |r", (unsigned int)obj->GetMapId(), at->ZoneId, at->AreaId, obj->GetPositionX(), obj->GetPositionY(), obj->GetPositionZ(), obj->GetOrientation(), at->name);
	SystemMessage(m_session, buf);
	// ".gps 1" will save gps info to file logs/gps.log - This probably isn't very multithread safe so don't have many gms spamming it!
	if(args && *args == '1')
	{
		FILE* gpslog = fopen(FormatOutputString("logs", "gps", false).c_str(), "at");
		if(gpslog)
		{
			fprintf(gpslog, "%d, %u, %u, %f, %f, %f, %f, \'%s\'", (unsigned int)obj->GetMapId(), at->ZoneId, at->AreaId, obj->GetPositionX(), obj->GetPositionY(), obj->GetPositionZ(), obj->GetOrientation(), at->name);
			// ".gps 1 comment" will save comment after the gps data
			if(*(args + 1) == ' ')
				fprintf(gpslog, ",%s\n", args + 2);
			else
				fprintf(gpslog, "\n");

			fclose(gpslog);
		}
	}
	return true;
}

bool ChatHandler::HandleHelpCommand(const char* args, WorldSession* m_session)
{
	WorldPacket data;

	if(!*args)
		return false;

	char* cmd = strtok((char*)args, " ");
	if(!cmd)
		return false;

	if(!ShowHelpForCommand(m_session, CommandTableStorage::getSingleton().Get(), cmd))
		RedSystemMessage(m_session, "Sorry, no help was found for this command, or that command does not exist.");

	return true;
}

bool ChatHandler::HandleInvincibleCommand(const char* args, WorldSession* m_session)
{
	Player* chr = getSelectedChar(m_session);
	char msg[100];
	if(chr)
	{
		chr->bInvincible = !chr->bInvincible;
		snprintf(msg, 100, "Invincibility is now %s", chr->bInvincible ? "ON." : "OFF.");
	}
	else
		snprintf(msg, 100, "Select a player or yourself first.");

	if(chr != m_session->GetPlayer() && chr)
		sGMLog.writefromsession(m_session, "toggled invincibility on %s", chr->GetName());

	snprintf(msg, 256, "%s You may have to leave and re-enter this zone for changes to take effect.", msg);
	GreenSystemMessage(m_session, (const char*)msg);
	return true;
}

bool ChatHandler::HandleInvisibleCommand(const char* args, WorldSession* m_session)
{
	char msg[256];
	Player* pChar = m_session->GetPlayer();

	snprintf(msg, 256, "Invisibility is now ");
	if(pChar->m_isGmInvisible)
	{
		pChar->m_isGmInvisible = false;
		pChar->m_invisible = false;
		pChar->Social_TellFriendsOnline();
		if(pChar->m_bg)
			pChar->m_bg->RemoveInvisGM();

		snprintf(msg, 256, "%s OFF.", msg);
	}
	else
	{
		pChar->m_isGmInvisible = true;
		pChar->m_invisible = true;
		pChar->Social_TellFriendsOffline();
		if(pChar->m_bg)
			pChar->m_bg->AddInvisGM();

		snprintf(msg, 256, "%s ON.", msg);
	}

	pChar->UpdateVisibility();
	snprintf(msg, 256, "%s You may have to leave and re-enter this zone for changes to take effect.", msg);
	GreenSystemMessage(m_session, (const char*)msg);
	return true;
}

bool ChatHandler::HandleKillByPlrCommand(const char* args , WorldSession* m_session)
{
	Player* plr = objmgr.GetPlayer(args, false);
	if(!plr)
	{
		RedSystemMessage(m_session, "Player %s is not online or does not exist.", args);
		return true;
	}

	if(plr->IsDead())
		RedSystemMessage(m_session, "Player %s is already dead.", args);
	else
	{
		plr->SetHealth(0); // Die, insect
		plr->KillPlayer();
		BlueSystemMessageToPlr(plr, "You were killed by %s with a GM command.", m_session->GetPlayer()->GetName());
		GreenSystemMessage(m_session, "Killed player %s.", args);
		sGMLog.writefromsession(m_session, "remote killed " I64FMT " (Name: %s)", plr->GetGUID(), plr->GetNameString());
	}
	return true;
}

bool ChatHandler::HandleKickCommand(const char* args, WorldSession* m_session)
{
	if(!*args)
		return false;

	char* pname = strtok((char*)args, " ");
	if(!pname)
	{
		RedSystemMessage(m_session, "No name specified.");
		return false;
	}

	Player* chr = objmgr.GetPlayer((const char*)pname, false);
	if(chr)
	{
		char* reason = strtok(NULL, "\n");
		std::string kickreason = "No reason";
		if(reason)
			kickreason = reason;

		BlueSystemMessage(m_session, "Attempting to kick %s from the server for \'%s\'.", chr->GetName(), kickreason.c_str());
		sGMLog.writefromsession(m_session, "Kicked player %s from the server for %s", chr->GetName(), kickreason.c_str());
		if(!m_session->CanUseCommand('3') && chr->GetSession()->CanUseCommand('3') || !m_session->CanUseCommand('2') && chr->GetSession()->CanUseCommand('2'))
		{
			RedSystemMessage(m_session, "You cannot kick %s.", chr->GetName());
			return false;
		}

		char msg[200];
		snprintf(msg, 200, "%sGM: %s was kicked from the server by %s. Reason: %s", MSG_COLOR_RED, chr->GetName(), m_session->GetPlayer()->GetName(), kickreason.c_str());
		sWorld.SendWorldText(msg, NULL);
		//sWorld.SendIRCMessage(msg);
		SystemMessageToPlr(chr, "You are being kicked from the server by %s. Reason: %s", m_session->GetPlayer()->GetName(), kickreason.c_str());

		WorldPacket data;
		data.Initialize(SMSG_FORCE_MOVE_ROOT);
		data << chr->GetNewGUID();
		data << uint32(1);
		chr->SendMessageToSet(&data, true);
		chr->Kick(6000);

		return true;
	}
	else
	{
		RedSystemMessage(m_session, "Player is not online at the moment.");
		return false;
	}
}

bool ChatHandler::HandleLevelUpCommand(const char* args, WorldSession* m_session)
{
	int levels = 0;

	if(!*args)
		levels = 1;
	else
		levels = atoi(args);

	if(levels <= 0)
		return false;

	Player* plr = getSelectedChar(m_session, true);

	if(!plr)
		plr = m_session->GetPlayer();

	if(!plr)
		return false;

	sGMLog.writefromsession(m_session, "used level up command on %s, with %u levels", plr->GetName(), levels);

	levels += plr->getLevel();
	if(levels > PLAYER_LEVEL_CAP)
		levels = PLAYER_LEVEL_CAP;

	LevelInfo* inf = objmgr.GetLevelInfo(plr->getRace(), plr->getClass(), levels);
	if(!inf)
		return false;

	plr->ApplyLevelInfo(inf, levels);
	if(plr->getClass() == WARLOCK)
	{
		std::list<Pet*> summons = plr->GetSummons();
		for(std::list<Pet*>::iterator itr = summons.begin(); itr != summons.end(); ++itr)
		{
			if((*itr)->IsInWorld() && (*itr)->isAlive())
			{
				(*itr)->setLevel(levels);
				(*itr)->ApplyStatsForLevel();
				(*itr)->UpdateSpellList();
			}
		}
	}

	WorldPacket data;
	std::stringstream sstext;
	sstext << "You have been leveled up to level " << levels << '\0';
	SystemMessage(plr->GetSession(), sstext.str().c_str());

	plr->Social_TellFriendsOnline();
	return true;
}

bool ChatHandler::HandleGmLogCommentCommand(const char* args , WorldSession* m_session)
{
	if(!args || !strlen(args))
		return false;

	BlueSystemMessage(m_session, "Added Logcomment: %s", args);
	sGMLog.writefromsession(m_session, " - Logcomment: %s", args);
	return true;
}

bool ChatHandler::HandleModPeriodCommand(const char* args, WorldSession* m_session)
{
	Transporter* trans = m_session->GetPlayer()->m_CurrentTransporter;
	if(!trans)
	{
		RedSystemMessage(m_session, "You must be on a transporter.");
		return false;
	}

	uint32 np = args ? atol(args) : 0;
	if(!np)
	{
		RedSystemMessage(m_session, "A time in ms must be specified.");
		return false;
	}

	trans->SetPeriod(np);
	BlueSystemMessage(m_session, "Period of %s set to %u.", trans->GetInfo()->Name, np);
	return true;
}

bool ChatHandler::HandleMountCommand(const char* args, WorldSession* m_session)
{
	if(!args)
	{
		RedSystemMessage(m_session, "No model specified.");
		return false;
	}

	uint32 modelid = atol(args);
	if(!modelid)
	{
		RedSystemMessage(m_session, "No model specified.");
		return false;
	}

	Unit* m_target = NULL;
	Player* m_plyr = getSelectedChar(m_session, false);
	if(m_plyr)
		m_target = m_plyr;
	else
	{
		Creature* m_crt = getSelectedCreature(m_session, false);
		if(m_crt)
			m_target = m_crt;
	}

	if(!m_target)
	{
		RedSystemMessage(m_session, "No target found.");
		return false;
	}

	if(m_target->GetMount())
	{
		RedSystemMessage(m_session, "Target is already mounted.");
		return false;
	}

	m_target->SetUInt32Value(UNIT_FIELD_MOUNTDISPLAYID, modelid);
	//m_target->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_MOUNTED_TAXI);

	BlueSystemMessage(m_session, "Now mounted with model %d.", modelid);
	sGMLog.writefromsession(m_session, "used mount command to model %u", modelid);
	return true;
}

bool ChatHandler::HandleParalyzeCommand(const char* args, WorldSession* m_session)
{
	Unit* plr = m_session->GetPlayer()->GetMapMgr()->GetUnit(m_session->GetPlayer()->GetSelection());
	if(!plr || !plr->IsPlayer())
	{
		RedSystemMessage(m_session, "Invalid target.");
		return false;
	}

	if(!stricmp(args, "on"))
	{
		BlueSystemMessage(m_session, "Rooting target.");
		BlueSystemMessageToPlr(TO<Player*>(plr), "You have been rooted by %s.", m_session->GetPlayer()->GetName());
		sGMLog.writefromsession(m_session, "Rooted player %s.", TO<Player*>(plr)->GetName());
		WorldPacket data;
		data.Initialize(SMSG_FORCE_MOVE_ROOT);
		data << plr->GetNewGUID();
		data << uint32(1);
		plr->SendMessageToSet(&data, true);
	}
	else if(!stricmp(args, "off"))
	{
		BlueSystemMessage(m_session, "Unrooting target.");
		BlueSystemMessageToPlr(TO<Player*>(plr), "You have been unrooted by %s.", m_session->GetPlayer()->GetName());
		sGMLog.writefromsession(m_session, "Unrooted player %s.", TO<Player*>(plr)->GetName());
		WorldPacket data;
		data.Initialize(SMSG_FORCE_MOVE_UNROOT);
		data << plr->GetNewGUID();
		data << uint32(5);
		plr->SendMessageToSet(&data, true);
	}
	return true;
}

bool ChatHandler::HandlePlayerInfo(const char* args, WorldSession* m_session)
{
	Player* plr;
	if(strlen(args) >= 2) // char name can be 2 letters
	{
		plr = objmgr.GetPlayer(args, false);
		if(!plr)
		{
			RedSystemMessage(m_session, "Unable to locate player %s.", args);
			return false;
		}
	}
	else
		plr = getSelectedChar(m_session, true);

	if(!plr)
		return false;

	if(!plr->GetSession())
	{
		RedSystemMessage(m_session, "ERROR: this player hasn't got any session !");
		return false;
	}
	if(!plr->GetSession()->GetSocket())
	{
		RedSystemMessage(m_session, "ERROR: this player hasn't got any socket !");
		return false;
	}
	WorldSession* sess = plr->GetSession();

	//char* infos = new char[128];
	static const char* classes[12] =
	{"None", "Warrior", "Paladin", "Hunter", "Rogue", "Priest", "Death Knight", "Shaman", "Mage", "Warlock", "None", "Druid"};
	static const char* races[12] =
	{"None", "Human", "Orc", "Dwarf", "Night Elf", "Undead", "Tauren", "Gnome", "Troll", "None", "Blood Elf", "Draenei"};

	char playedLevel[64];
	char playedTotal[64];

	int seconds = (plr->GetPlayedtime())[0];
	int mins = 0;
	int hours = 0;
	int days = 0;
	if(seconds >= 60)
	{
		mins = seconds / 60;
		if(mins)
		{
			seconds -= mins * 60;
			if(mins >= 60)
			{
				hours = mins / 60;
				if(hours)
				{
					mins -= hours * 60;
					if(hours >= 24)
					{
						days = hours / 24;
						if(days)
							hours -= days * 24;
					}
				}
			}
		}
	}
	snprintf(playedLevel, 64, "[%d days, %d hours, %d minutes, %d seconds]", days, hours, mins, seconds);

	seconds = (plr->GetPlayedtime())[1];
	mins = 0;
	hours = 0;
	days = 0;
	if(seconds >= 60)
	{
		mins = seconds / 60;
		if(mins)
		{
			seconds -= mins * 60;
			if(mins >= 60)
			{
				hours = mins / 60;
				if(hours)
				{
					mins -= hours * 60;
					if(hours >= 24)
					{
						days = hours / 24;
						if(days)
							hours -= days * 24;
					}
				}
			}
		}
	}
	snprintf(playedTotal, 64, "[%d days, %d hours, %d minutes, %d seconds]", days, hours, mins, seconds);
	GreenSystemMessage(m_session, "%s is a %s %s %s", plr->GetName(), (plr->getGender() ? "Female" : "Male"), races[plr->getRace()], classes[plr->getClass()]);
	BlueSystemMessage(m_session, "%s has played %s at this level", (plr->getGender() ? "She" : "He"), playedLevel);
	BlueSystemMessage(m_session, "and %s overall", playedTotal);
	BlueSystemMessage(m_session, "%s is connecting from account '%s'[%u] with permissions '%s'", (plr->getGender() ? "She" : "He"), sess->GetAccountName().c_str(), sess->GetAccountId(), sess->GetPermissions());
	const char* client;
	// Clean code says you need to work from highest combined bit to lowest. Second, you need to check if both flags exists.
	if(sess->HasFlag(ACCOUNT_FLAG_XPACK_02) && sess->HasFlag(ACCOUNT_FLAG_XPACK_01))
		client = "TBC and WotLK";
	else if(sess->HasFlag(ACCOUNT_FLAG_XPACK_02))
		client = "Wrath of the Lich King";
	else if(sess->HasFlag(ACCOUNT_FLAG_XPACK_01))
		client = "WoW Burning Crusade";
	else
		client = "WoW";

	BlueSystemMessage(m_session, "%s uses %s (build %u)", (plr->getGender() ? "She" : "He"), client, sess->GetClientBuild());
	BlueSystemMessage(m_session, "%s IP is '%s', and has a latency of %ums", (plr->getGender() ? "Her" : "His"), sess->GetSocket()->GetRemoteIP().c_str(), sess->GetLatency());
	return true;
}

bool ChatHandler::HandleStartCommand(const char* args, WorldSession* m_session)
{
	std::string race;
	Player* m_plyr = getSelectedChar(m_session, false);
	if(!m_plyr)
		return false;

	uint8 raceid = 0;
	uint8 classid = 0;
	if(strlen(args) > 0)
	{
		race = args;
		if(race == "human")
			raceid = 1;
		else if(race == "orc")
			raceid = 2;
		else if(race == "dwarf")
			raceid = 3;
		else if(race == "nightelf" || race == "ne")
			raceid = 4;
		else if(race == "undead")
			raceid = 5;
		else if(race == "tauren")
			raceid = 6;
		else if(race == "gnome")
			raceid = 7;
		else if(race == "troll")
			raceid = 8;
		else if(race == "bloodelf" || race == "be")
			raceid = 10;
		else if(race == "draenei")
			raceid = 11;
		else if(race == "deathknight" || race == "dk")
			classid = 6;
		else
		{
			RedSystemMessage(m_session, "Invalid start location! Valid locations are: human, dwarf, gnome, nightelf, draenei, orc, troll, tauren, undead, bloodelf, deathknight");
		return false;
		}
	}
	else
	{
		raceid = m_plyr->getRace();
		classid = m_plyr->getClass();
		race = "his";
	}
	// find the first matching one
	PlayerCreateInfo* info = NULL;
	for(uint8 i = 1; i <= 11; i++)
	{
		info = objmgr.GetPlayerCreateInfo((raceid ? raceid : i), (classid ? classid : i));
		if(info)
			break;
	}

	if(!info)
	{
		RedSystemMessage(m_session, "Internal error: Could not find create info for race %u and class %u.", raceid, classid);
		return false;
	}

	GreenSystemMessage(m_session, "Teleporting %s to %s starting location.", m_plyr->GetName(), race.c_str());
	m_plyr->SafeTeleport(info->mapId, 0, LocationVector(info->positionX, info->positionY, info->positionZ));
	return true;
}

bool ChatHandler::HandleSummonCommand(const char* args, WorldSession* m_session)
{
	if(!*args)
		return false;

	// Summon Blocking
	if(!stricmp(args, "on"))
	{
		if(m_session->GetPlayer()->IsSummonDisabled())
			BlueSystemMessage(m_session, "Summon blocking is already enabled");
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
			BlueSystemMessage(m_session, "Summon blocking is already disabled");

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

		if(!chr->GetMapMgr())
		{
			snprintf((char*)buf, 256, "%s is already being teleported.", chr->GetName());
			SystemMessage(m_session, buf);
			return false;
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
			sEventMgr.AddEvent(chr, &Player::EventPortToGM, plr, 0, 1, 1, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
	}
	else
	{
		PlayerInfo* pinfo = objmgr.GetPlayerInfoByName(args);
		if(!pinfo)
		{
			char buf[256];
			snprintf((char*)buf, 256, "Player (%s) does not exist.", args);
			SystemMessage(m_session, buf);
			return false;
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

bool ChatHandler::HandleUnlearnCommand(const char* args, WorldSession* m_session)
{
	Player* plr = getSelectedChar(m_session, true);
	if(!plr)
		return true;

	uint32 SpellId = atol(args);
	if(!SpellId)
	{
		SpellId = GetSpellIDFromLink(args);
		if(!SpellId)
		{
			RedSystemMessage(m_session, "You must specify a spell id.");
			return false;
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
		RedSystemMessage(m_session, "That player does not have spell %u learnt.", SpellId);

	return true;
}

/*bool ChatHandler::HandleUnParalyzeCommand(const char* args, WorldSession* m_session)
{
	Unit* plr = m_session->GetPlayer()->GetMapMgr()->GetUnit(m_session->GetPlayer()->GetSelection());
	if(!plr || !plr->IsPlayer())
	{
		RedSystemMessage(m_session, "Invalid target.");
		return false;
	}

	BlueSystemMessage(m_session, "Unrooting target.");
	BlueSystemMessageToPlr(TO< Player* >(plr), "You have been unrooted by %s.", m_session->GetPlayer()->GetName());
	sGMLog.writefromsession(m_session, "unrooted player %s", TO< Player* >(plr)->GetName());
	WorldPacket data;
	data.Initialize(SMSG_FORCE_MOVE_UNROOT);
	data << plr->GetNewGUID();
	data << uint32(5);

	plr->SendMessageToSet(&data, true);
	return true;
}*/

bool ChatHandler::HandleWorldPortCommand(const char* args, WorldSession* m_session)
{
	float x, y, z, o = 0;
	uint32 mapid;
	if(sscanf(args, "%u %f %f %f %f", (unsigned int*)&mapid, &x, &y, &z, &o) < 4)
		return false;

	if(x >= _maxX || x <= _minX || y <= _minY || y >= _maxY)
		return false;

	LocationVector vec(x, y, z, o);
	m_session->GetPlayer()->SafeTeleport(mapid, 0, vec);
	return true;
}

bool ChatHandler::HandleWAnnounceCommand(const char* args, WorldSession* m_session)
{
	if(!*args)
		return false;

	char pAnnounce[1024];
	string input3;
	input3 = sWorld.ann_tagcolor;
	input3 += "[";
	input3 += sWorld.announce_tag;
	input3 += "]";
	input3 += sWorld.ann_gmtagcolor;
	if(sWorld.GMAdminTag)
	{
		if(m_session->CanUseCommand('3'))
			input3 += "<Admin>";
		else if(m_session->GetPermissionCount())
			input3 += "<GM>";
	}
	if(sWorld.NameinWAnnounce)
	{
		input3 += "|r" + sWorld.ann_namecolor + "[";
		input3 += m_session->GetPlayer()->GetName();
		input3 += "]:|r " + sWorld.ann_msgcolor;
	}
	else if(!sWorld.NameinWAnnounce)
	{
		input3 += ": ";
		input3 += sWorld.ann_msgcolor;
	}

	snprintf((char*)pAnnounce, 1024, "%s%s", input3.c_str(), args);
	sWorld.SendWorldWideScreenText(pAnnounce); // send message
	sGMLog.writefromsession(m_session, "used wannounce command [%s]", args);
	return true;
}

bool ChatHandler::HandleRemoveAurasCommand(const char* args, WorldSession* m_session)
{
	Player* plr = getSelectedChar(m_session, true);
	if(!plr)
		return false;

	BlueSystemMessage(m_session, "Removing all auras...");
	for(uint32 i = MAX_REMOVABLE_AURAS_START; i < MAX_REMOVABLE_AURAS_END; ++i)
	{
		if(plr->m_auras[i])
			plr->m_auras[i]->Remove();
	}
	if(plr != m_session->GetPlayer())
		sGMLog.writefromsession(m_session, "Removed all of %s's auras.", plr->GetName());

	return true;
}

bool ChatHandler::HandleRemoveRessurectionSickessAuraCommand(const char* args, WorldSession* m_session)
{
	Player* plr = getSelectedChar(m_session, true);
	if(!plr)
		return false;

	BlueSystemMessage(m_session, "Removing resurrection sickness...");
	plr->RemoveAura(15007);
	if(plr != m_session->GetPlayer())
		sGMLog.writefromsession(m_session, "Removed resurrection sickness from %s", plr->GetName());

	return true;
}

bool ChatHandler::HandleReviveCommand(const char* args, WorldSession* m_session)
{
	Player* SelectedPlayer = getSelectedChar(m_session, true);
	if(!SelectedPlayer)
		return false;

	SelectedPlayer->SetMovement(MOVE_UNROOT, 1);
	SelectedPlayer->ResurrectPlayer();
	SelectedPlayer->SetHealth(SelectedPlayer->GetMaxHealth());
	SelectedPlayer->SetPower(POWER_TYPE_MANA, SelectedPlayer->GetMaxPower(POWER_TYPE_MANA));
	SelectedPlayer->SetPower(POWER_TYPE_ENERGY, SelectedPlayer->GetMaxPower(POWER_TYPE_ENERGY));

	if(SelectedPlayer != m_session->GetPlayer())
		sGMLog.writefromsession(m_session, "revived player %s", SelectedPlayer->GetName());

	return true;
}

bool ChatHandler::HandleReviveStringCommand(const char* args, WorldSession* m_session)
{
	Player* plr = objmgr.GetPlayer(args, false);
	if(!plr)
	{
		RedSystemMessage(m_session, "Could not find player %s.", args);
		return false;
	}

	if(plr->IsDead())
	{
		if(plr->GetInstanceID() == m_session->GetPlayer()->GetInstanceID())
			plr->RemoteRevive();
		else
			sEventMgr.AddEvent(plr, &Player::RemoteRevive, EVENT_PLAYER_REST, 1, 1, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);

		GreenSystemMessage(m_session, "Revived player %s.", args);
		sGMLog.writefromsession(m_session, "revived player %s", args);
	}
	else
		GreenSystemMessage(m_session, "Player %s is not dead.", args);

	return true;
}