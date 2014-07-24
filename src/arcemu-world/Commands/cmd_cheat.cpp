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

bool ChatHandler::HandleShowCheatsCommand(const char* args, WorldSession* m_session)
{
	Player* plyr = getSelectedChar(m_session, true);
	if(!plyr)
	{
		SystemMessage(m_session, "You don't have a selected player.");
		return false;
	}

	uint32 active = 0, inactive = 0;
#define print_cheat_status(CheatName, CheatVariable) SystemMessage(m_session, "%s%s: %s%s", MSG_COLOR_LIGHTBLUE, CheatName, \
		CheatVariable ? MSG_COLOR_LIGHTRED : MSG_COLOR_GREEN, CheatVariable ? "Active" : "Inactive");  \
		if(CheatVariable) \
		active++; \
		else \
		inactive++;

	GreenSystemMessage(m_session, "Showing cheat status for: %s", plyr->GetName());
	print_cheat_status("Cooldown: ", plyr->CooldownCheat);
	print_cheat_status("CastTime: ", plyr->CastTimeCheat);
	print_cheat_status("GodMode: ", plyr->GodModeCheat);
	print_cheat_status("Power: ", plyr->PowerCheat);
	print_cheat_status("Fly: ", plyr->FlyCheat);
	print_cheat_status("AuraStack: ", plyr->AuraStackCheat);
	print_cheat_status("ItemStack: ", plyr->ItemStackCheat);
	print_cheat_status("TriggerPass: ", plyr->TriggerpassCheat);
	if(plyr->GetSession() && plyr->GetSession()->CanUseCommand('a'))
	{
		print_cheat_status("GM Invisibility: ", plyr->m_isGmInvisible);
		print_cheat_status("GM Invincibility: ", plyr->bInvincible);
	}
	SystemMessage(m_session, "There are %u cheats active, and %u inactive.", active, inactive);

#undef print_cheat_status

	return true;
}

bool ChatHandler::HandleTaxiCheatCommand(const char* args, WorldSession* m_session)
{
	if(!*args)
		return false;

	Player* chr = getSelectedChar(m_session);
	if(!chr)
	{
		SystemMessage(m_session, "You don't have a selected player.");
		return false;
	}

	if(!stricmp(args, "on"))
	{
		GreenSystemMessage(m_session, "%s has all taxi nodes now.", chr->GetName());
		SystemMessage(m_session, "%s has given you all taxi nodes.", m_session->GetPlayer()->GetName());
	}
	else if(!stricmp(args, "off"))
	{
		GreenSystemMessage(m_session, "%s has no more taxi nodes now.", chr->GetName());
		SystemMessage(chr->GetSession(), "%s has deleted all your taxi nodes.", m_session->GetPlayer()->GetName());
	}
	else
		return false;

	for(uint8 i = 0; i < 12; i++)
	{
		if(stricmp(args, "on") == 0)
			chr->SetTaximask(i, 0xFFFFFFFF);
		else if(stricmp(args, "off") == 0)
			chr->SetTaximask(i, 0);
	}
	return true;
}

bool ChatHandler::HandleCooldownCheatCommand(const char* args, WorldSession* m_session)
{
	Player* plyr = getSelectedChar(m_session, true);
	if(!plyr)
	{
		SystemMessage(m_session, "You don't have a selected player.");
		return false;
	}

	if(!*args)
	{
		if(plyr->CooldownCheat)
			args = "off";
		else
			args = "on";
	}

	if(!stricmp(args, "on"))
	{
		plyr->CooldownCheat = true;

		//best case we could simply iterate through cooldowns or create a special function ...
		SpellSet::const_iterator itr = plyr->mSpells.begin();
		for(; itr != plyr->mSpells.end(); ++itr)
			plyr->ClearCooldownForSpell((*itr));

		BlueSystemMessage(m_session, "activated the cooldown cheat on %s.", plyr->GetName());
		GreenSystemMessageToPlr(plyr, "activated the cooldown cheat on you.", m_session->GetPlayer()->GetName());
	}
	else if(!stricmp(args, "off"))
	{
		plyr->CooldownCheat = false;

		BlueSystemMessage(m_session, "deactivated the cooldown cheat on %s.", plyr->GetName());
		GreenSystemMessageToPlr(plyr, "deactivated the cooldown cheat on you.", m_session->GetPlayer()->GetName());

		if(plyr != m_session->GetPlayer())
			sGMLog.writefromsession(m_session, "cooldown cheat on %s set to %s", plyr->GetName(), args);
	}
	else
		return false;

	return true;
}

bool ChatHandler::HandleCastTimeCheatCommand(const char* args, WorldSession* m_session)
{
	Player* plyr = getSelectedChar(m_session, true);
	if(!plyr)
	{
		SystemMessage(m_session, "You don't have a selected player.");
		return false;
	}

	if(!*args)
	{
		if(plyr->CastTimeCheat)
			args = "off";
		else
			args = "on";
	}

	if(!stricmp(args, "on"))
	{
		plyr->CastTimeCheat = true;
		BlueSystemMessage(m_session, "activated the cast time cheat on %s.", plyr->GetName());
		GreenSystemMessageToPlr(plyr, "activated the cast time cheat on you.", m_session->GetPlayer()->GetName());
	}
	else if(!stricmp(args, "off"))
	{
		plyr->CastTimeCheat = false;
		BlueSystemMessage(m_session, "deactivated the cast time cheat on %s.", plyr->GetName());
		GreenSystemMessageToPlr(plyr, "deactivated the cast time cheat on you.", m_session->GetPlayer()->GetName());

		if(plyr != m_session->GetPlayer())
			sGMLog.writefromsession(m_session, "god cast time on %s set to %s", plyr->GetName(), args);
	}
	else
		return false;

	return true;
}

bool ChatHandler::HandlePowerCheatCommand(const char* args, WorldSession* m_session)
{
	Player* plyr = getSelectedChar(m_session, true);
	if(!plyr)
	{
		SystemMessage(m_session, "You don't have a selected player.");
		return false;
	}

	if(!*args)
	{
		if(plyr->PowerCheat)
			args = "off";
		else
			args = "on";
	}

	if(!stricmp(args, "on"))
	{
		plyr->PowerCheat = true;
		BlueSystemMessage(m_session, "activated the power cheat on %s.", plyr->GetName());
		GreenSystemMessageToPlr(plyr, "activated the power cheat on you.", m_session->GetPlayer()->GetName());
	}
	else if(!stricmp(args, "off"))
	{
		plyr->PowerCheat = false;
		BlueSystemMessage(m_session, "deactivated the power cheat on %s.", plyr->GetName());
		GreenSystemMessageToPlr(plyr, "deactivated the power cheat on you.", m_session->GetPlayer()->GetName());

		if(plyr != m_session->GetPlayer())
			sGMLog.writefromsession(m_session, "power cheat on %s set to %s", plyr->GetName(), args);
	}
	else
		return false;

	return true;
}

bool ChatHandler::HandleGodModeCommand(const char* args, WorldSession* m_session)
{
	Player* plyr = getSelectedChar(m_session, true);
	if(!plyr)
	{
		SystemMessage(m_session, "You don't have a selected player.");
		return false;
	}

	if(!*args)
	{
		if(plyr->GodModeCheat)
			args = "off";
		else
			args = "on";
	}

	if(!stricmp(args, "on"))
	{
		plyr->GodModeCheat = true;
		BlueSystemMessage(m_session, "Activating the god mode cheat on %s.", plyr->GetName());
		GreenSystemMessageToPlr(plyr, "%s activated the god mode cheat on you.", m_session->GetPlayer()->GetName());
	}
	else if(!stricmp(args, "off"))
	{
		plyr->GodModeCheat = false;
		BlueSystemMessage(m_session, "Deactivating the god mode cheat on %s.", plyr->GetName());
		GreenSystemMessageToPlr(plyr, "%s deactivated the god mode cheat on you.", m_session->GetPlayer()->GetName());

		if(plyr != m_session->GetPlayer())
			sGMLog.writefromsession(m_session, "god mode cheat on %s set to %s", plyr->GetName(), args);
	}
	else
		return false;

	return true;
}

bool ChatHandler::HandleExploreCheatCommand(const char* args, WorldSession* m_session)
{
	if(!*args)
		return false;

	Player* chr = getSelectedChar(m_session, true);
	if(!chr)
	{
		SystemMessage(m_session, "You don't have a selected player.");
		return false;
	}

	if(!stricmp(args, "on"))
	{
		SystemMessage(m_session, "%s has explored all zones now.", chr->GetName());
		SystemMessage(m_session, "%s has explored all zones for you.", m_session->GetPlayer()->GetName());
		sGMLog.writefromsession(m_session, "explored all zones for player %s", chr->GetName());
	}
	else if(!stricmp(args, "off"))
	{
		SystemMessage(m_session, "%s has no more explored zones.", chr->GetName());
		SystemMessage(m_session, "%s has hidden all zones from you.", m_session->GetPlayer()->GetName());
		sGMLog.writefromsession(m_session, "hid all zones for player %s", chr->GetName());
	}
	else
		return false;

	for(uint8 i = 0; i < PLAYER_EXPLORED_ZONES_LENGTH; ++i)
	{
		if(stricmp(args, "on") == 0)
			chr->SetFlag(PLAYER_EXPLORED_ZONES_1 + i, 0xFFFFFFFF);
		else if(stricmp(args, "off") == 0)
			chr->RemoveFlag(PLAYER_EXPLORED_ZONES_1 + i, 0xFFFFFFFF);
	}
#ifdef ENABLE_ACHIEVEMENTS
	chr->GetAchievementMgr().UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_EXPLORE_AREA); // update
#endif
	return true;
}

bool ChatHandler::HandleAuraStackCheatCommand(const char* args, WorldSession* m_session)
{
	Player* plyr = getSelectedChar(m_session, true);
	if(!plyr)
	{
		SystemMessage(m_session, "You don't have a selected player.");
		return false;
	}

	if(!*args)
	{
		if(plyr->AuraStackCheat)
			args = "off";
		else
			args = "on";
	}

	if(!stricmp(args, "on"))
	{
		plyr->AuraStackCheat = true;
		BlueSystemMessage(m_session, "activated the aura stack cheat on %s.", plyr->GetName());
		GreenSystemMessageToPlr(plyr, "activated the aura stack cheat on you.", m_session->GetPlayer()->GetName());
	}
	else if(!stricmp(args, "off"))
	{
		plyr->AuraStackCheat = false;
		BlueSystemMessage(m_session, "deactivated the aura stack cheat on %s.", plyr->GetName());
		GreenSystemMessageToPlr(plyr, "deactivated the aura stack cheat on you.", m_session->GetPlayer()->GetName());

		if(plyr != m_session->GetPlayer())
			sGMLog.writefromsession(m_session, "aura stack cheat on %s set to %s", plyr->GetName(), args);
	}
	else
		return false;

	return true;
}

bool ChatHandler::HandleItemStackCheatCommand(const char* args, WorldSession* m_session)
{
	Player* p = getSelectedChar(m_session, true);
	if(!p)
	{
		SystemMessage(m_session, "You don't have a selected player.");
		return false;
	}

	bool turnCheatOn;
	if(!*args)
		turnCheatOn = (p->ItemStackCheat) ? false : true;
	else if(!stricmp(args, "on"))
		turnCheatOn = true;
	else if(!stricmp(args, "off"))
		turnCheatOn = false;
	else
		return false;

	p->ItemStackCheat = turnCheatOn;
	BlueSystemMessage(m_session, "%s the item stack cheat on %s.", (turnCheatOn) ? "activated" : "deactivated", p->GetName());
	GreenSystemMessageToPlr(p, "%s %s the item stack cheat on you.%s", m_session->GetPlayer()->GetName(), (turnCheatOn) ? "activated" : "deactivated", (turnCheatOn) ? "" : "  WARNING!!! Make sure all your item stacks are normal (if possible) before logging off, or else you may lose some items!");
	if(p != m_session->GetPlayer())
		sGMLog.writefromsession(m_session, "item stack cheat on %s set to %s", p->GetName(), (turnCheatOn) ? "on" : "off");

	return true;
}

bool ChatHandler::HandleTriggerpassCheatCommand(const char* args, WorldSession* m_session)
{
	Player* plyr = getSelectedChar(m_session, true);
	if(!plyr)
	{
		SystemMessage(m_session, "You don't have a selected player.");
		return false;
	}

	if(!*args)
	{
		if(plyr->TriggerpassCheat)
			args = "off";
		else
			args = "on";
	}

	if(!stricmp(args, "on"))
	{
		plyr->TriggerpassCheat = true;
		BlueSystemMessage(m_session, "activated the triggerpass cheat on %s.", plyr->GetName());
		GreenSystemMessageToPlr(plyr, "activated the triggerpass cheat on you.", m_session->GetPlayer()->GetName());
	}
	else if(!stricmp(args, "off"))
	{
		plyr->TriggerpassCheat = false;
		BlueSystemMessage(m_session, "deactivated the triggerpass cheat on %s.", plyr->GetName());
		GreenSystemMessageToPlr(plyr, "deactivated the triggerpass cheat on you.", m_session->GetPlayer()->GetName());

		if(plyr != m_session->GetPlayer())
			sGMLog.writefromsession(m_session, "triggerpass cheat on %s set to %s", plyr->GetName(), args);
	}
	else
		return false;

	return true;
}