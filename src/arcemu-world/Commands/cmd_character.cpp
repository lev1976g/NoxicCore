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

bool ChatHandler::HandleGetSkillsInfoCommand(const char* args, WorldSession* m_session)
{
	Player* plr = getSelectedChar(m_session, true);
	if(!plr)
	{
		SystemMessage(m_session, "You don't have a selected player.");
		return false;
	}

	uint32 nobonus = 0;
	int32 bonus = 0;
	uint32 max = 0;

	BlueSystemMessage(m_session, "Player: %s has skills.", plr->GetName());

	for(uint32 SkillId = 0; SkillId <= SkillNameManager->maxskill; SkillId++)
	{
		if(plr->_HasSkillLine(SkillId))
		{
			char* SkillName = SkillNameManager->SkillNames[SkillId];
			if(!SkillName)
			{
				RedSystemMessage(m_session, "Invalid skill: %u", SkillId);
				continue;
			}

			nobonus = plr->_GetSkillLineCurrent(SkillId, false);
			bonus = plr->_GetSkillLineCurrent(SkillId, true) - nobonus;
			max = plr->_GetSkillLineMax(SkillId);

			BlueSystemMessage(m_session, "  %s: Value: %u \n  Max Value: %u. (+ %d bonus)", SkillName, nobonus, max, bonus);
		}
	}

	return true;
}

bool ChatHandler::HandleModifySkillCommand(const char* args, WorldSession* m_session)
{
	uint32 skill, min, max;
	min = max = 1;
	char* pSkill = strtok((char*)args, " ");
	if(!pSkill)
		return false;
	else
		skill = atol(pSkill);

	char* pMin = strtok(NULL, " ");
	uint32 cnt = 0;
	if(!pMin)
		cnt = 1;
	else
		cnt = atol(pMin);

	skill = atol(pSkill);

	BlueSystemMessage(m_session, "Modifying skill line %d. Advancing %d times.", skill, cnt);

	Player* plr = getSelectedChar(m_session, true);
	if(!plr)
		plr = m_session->GetPlayer();

	if(!plr)
	{
		SystemMessage(m_session, "You must select a player.");
		return false;
	}

	sGMLog.writefromsession(m_session, "used modify skill of %u %u on %s.", skill, cnt, plr->GetName());

	if(!plr->_HasSkillLine(skill))
	{
		SystemMessage(m_session, "Does not have skill line, adding.");
		plr->_AddSkillLine(skill, 1, 300);
	}
	else
		plr->_AdvanceSkillLine(skill, cnt);

	return true;
}

bool ChatHandler::HandleRemoveSkillCommand(const char* args, WorldSession* m_session)
{
	uint32 skill = 0;
	char* pSkill = strtok((char*)args, " ");
	if(!pSkill)
		return false;
	else
		skill = atol(pSkill);

	BlueSystemMessage(m_session, "Removing skill line %d", skill);

	Player* plr = getSelectedChar(m_session, true);
	if(!plr)
	{
		SystemMessage(m_session, "You don't have a selected player.");
		return false;
	}
	
	if(/*plr && */plr->_HasSkillLine(skill)) //fix bug; removing skill twice will mess up skills
	{
		plr->_RemoveSkillLine(skill);
		sGMLog.writefromsession(m_session, "used remove skill of %u on %s", skill, plr->GetName());
		SystemMessageToPlr(plr, "%s removed skill line %d from you. ", m_session->GetPlayer()->GetName(), skill);
	}
	else
		BlueSystemMessage(m_session, "Player doesn't have skill line %d.", skill);

	return true;
}

bool ChatHandler::HandleIncreaseWeaponSkill(const char* args, WorldSession* m_session)
{
	char* pMin = strtok((char*)args, " ");
	uint32 cnt = 0;
	if(!pMin)
		cnt = 1;
	else
		cnt = atol(pMin);

	Player* pr = getSelectedChar(m_session, true);

	uint32 SubClassSkill = 0;
	if(!pr)
		pr = m_session->GetPlayer();

	if(!pr)
	{
		SystemMessage(m_session, "You must select a player.");
		return false;
	}

	Item* it = pr->GetItemInterface()->GetInventoryItem(EQUIPMENT_SLOT_MAINHAND);
	ItemPrototype* proto = NULL;
	if(!it)
		it = pr->GetItemInterface()->GetInventoryItem(EQUIPMENT_SLOT_RANGED);

	if(it)
		proto = it->GetProto();

	if(proto)
	{
		switch(proto->SubClass)
		{
			// Weapons
			case 0:	// 1 handed axes
				SubClassSkill = SKILL_AXES;
			break;
			case 1:	// 2 handed axes
				SubClassSkill = SKILL_2H_AXES;
			break;
			case 2:	// bows
				SubClassSkill = SKILL_BOWS;
			break;
			case 3:	// guns
				SubClassSkill = SKILL_GUNS;
			break;
			case 4:	// 1 handed mace
				SubClassSkill = SKILL_MACES;
			break;
			case 5:	// 2 handed mace
				SubClassSkill = SKILL_2H_MACES;
			break;
			case 6:	// polearms
				SubClassSkill = SKILL_POLEARMS;
			break;
			case 7: // 1 handed sword
				SubClassSkill = SKILL_SWORDS;
			break;
			case 8: // 2 handed sword
				SubClassSkill = SKILL_2H_SWORDS;
			break;
			case 9: // obsolete
				SubClassSkill = 136;
			break;
			case 10: //1 handed exotic
				SubClassSkill = 136;
			break;
			case 11: // 2 handed exotic
				SubClassSkill = 0;
			break;
			case 12: // fist
				SubClassSkill = SKILL_FIST_WEAPONS;
			break;
			case 13: // misc
				SubClassSkill = 0;
			break;
			case 15: // daggers
				SubClassSkill = SKILL_DAGGERS;
			break;
			case 16: // thrown
				SubClassSkill = SKILL_THROWN;
			break;
			case 18: // crossbows
				SubClassSkill = SKILL_CROSSBOWS;
			break;
			case 19: // wands
				SubClassSkill = SKILL_WANDS;
			break;
			case 20: // fishing
				SubClassSkill = SKILL_FISHING;
			break;
		}
	}
	else
		SubClassSkill = 162;

	if(!SubClassSkill)
	{
		RedSystemMessage(m_session, "Invalid skill.");
		return false;
	}

	uint32 skill = SubClassSkill;

	BlueSystemMessage(m_session, "Modifying skill line %d. Advancing %d times.", skill, cnt);
	sGMLog.writefromsession(m_session, "increased weapon skill (%u) of %s by %u", skill, pr->GetName(), cnt);

	if(!pr->_HasSkillLine(skill))
	{
		SystemMessage(m_session, "Does not have skill line, adding.");
		pr->_AddSkillLine(skill, 1, 450);
	}
	else
		pr->_AdvanceSkillLine(skill, cnt);

	return true;
}

bool ChatHandler::HandleResetReputationCommand(const char* args, WorldSession* m_session)
{
	Player* plr = getSelectedChar(m_session);
	if(!plr)
	{
		SystemMessage(m_session, "You don't have a selected player.");
		return false;
	}

	plr->_InitialReputation();

	SystemMessage(m_session, "Done. You must re-login for changes to take effect.");
	sGMLog.writefromsession(m_session, "used reset reputation for %s", plr->GetName());
	return true;
}

bool ChatHandler::HandleResetSpellsCommand(const char* args, WorldSession* m_session)
{
	Player* plr = getSelectedChar(m_session);
	if(!plr)
	{
		SystemMessage(m_session, "You don't have a selected player.");
		return false;
	}

	plr->Reset_Spells();

	SystemMessage(m_session, "Reset spells of %s to level 1.", plr->GetName());
	BlueSystemMessageToPlr(plr, "%s reset all your spells to starting values.", m_session->GetPlayer()->GetName());
	sGMLog.writefromsession(m_session, "reset spells of %s", plr->GetName());
	return true;
}

bool ChatHandler::HandleResetTalentsCommand(const char* args, WorldSession* m_session)
{
	Player* plr = getSelectedChar(m_session);
	if(!plr)
	{
		SystemMessage(m_session, "You don't have a selected player.");
		return false;
	}

	plr->Reset_Talents();

	SystemMessage(m_session, "Reset talents of %s.", plr->GetName());
	BlueSystemMessageToPlr(plr, "%s reset all your talents.", m_session->GetPlayer()->GetName());
	sGMLog.writefromsession(m_session, "reset talents of %s", plr->GetName());
	return true;
}

bool ChatHandler::HandleResetSkillsCommand(const char* args, WorldSession* m_session)
{
	Player* plr = getSelectedChar(m_session, true);
	if(!plr)
	{
		SystemMessage(m_session, "You don't have a selected player.");
		return false;
	}

	plr->_RemoveAllSkills();

	// Load skills from create info.
	PlayerCreateInfo* info = objmgr.GetPlayerCreateInfo(plr->getRace(), plr->getClass());
	if(!info)
	{
		SystemMessage(m_session, "This player does not contain PlayerCreateInfo.");
		return false;
	}

	for(std::list<CreateInfo_SkillStruct>::iterator ss = info->skills.begin(); ss != info->skills.end(); ++ss)
	{
		skilllineentry* se = dbcSkillLine.LookupEntry(ss->skillid);
		if(se->type != SKILL_TYPE_LANGUAGE && ss->skillid && ss->currentval && ss->maxval)
			plr->_AddSkillLine(ss->skillid, ss->currentval, ss->maxval);
	}
	//Chances depend on stats must be in this order!
	plr->UpdateStats();
	plr->UpdateChances();
	plr->_UpdateMaxSkillCounts();
	plr->_AddLanguages(false);
	BlueSystemMessage(m_session, "Reset skills to default.");
	sGMLog.writefromsession(m_session, "reset skills of %s to default", plr->GetName());
	return true;
}

bool ChatHandler::HandleAdvanceAllSkillsCommand(const char* args, WorldSession* m_session)
{
	uint32 amt = args ? atol(args) : 0;
	if(!amt)
	{
		RedSystemMessage(m_session, "An amount to increment is required.");
		return true;
	}

	Player* plr = getSelectedChar(m_session);
	if(!plr)
	{
		SystemMessage(m_session, "You don't have a selected player.");
		return false;
	}

	plr->_AdvanceAllSkills(amt);

	GreenSystemMessageToPlr(plr, "Advanced all your skill lines by %u points.", amt);
	sGMLog.writefromsession(m_session, "advanced all skills by %u on %s", amt, plr->GetName());
	return true;
}

bool ChatHandler::HandleGetStandingCommand(const char* args, WorldSession* m_session)
{
	uint32 faction = atoi(args);
	Player* plr = getSelectedChar(m_session, true);
	if(!plr)
	{
		SystemMessage(m_session, "You don't have a selected player.");
		return false;
	}

	int32 standing = plr->GetStanding(faction);
	int32 bstanding = plr->GetBaseStanding(faction);

	GreenSystemMessage(m_session, "Reputation for faction %u:", faction);
	SystemMessage(m_session, "Base Standing: %d", bstanding);
	BlueSystemMessage(m_session, "Standing: %d", standing);
	return true;
}

bool ChatHandler::HandleSetStandingCommand(const char* args, WorldSession* m_session)
{
	uint32 faction;
	int32 standing;
	if(sscanf(args, "%u %d", (unsigned int*)&faction, (unsigned int*)&standing) != 2)
		return false;

	Player* plr = getSelectedChar(m_session, true);
	if(!plr)
	{
		SystemMessage(m_session, "You don't have a selected player.");
		return false;
	}

	BlueSystemMessage(m_session, "Setting standing of %u to %d on %s.", faction, standing, plr->GetName());
	plr->SetStanding(faction, standing);
	GreenSystemMessageToPlr(plr, "%s set your standing of faction %u to %d.", m_session->GetPlayer()->GetName(), faction, standing);
	sGMLog.writefromsession(m_session, "set standing of faction %u to %u for %s", faction, standing, plr->GetName());
	return true;
}

bool ChatHandler::HandleShowSkills(const char* args, WorldSession* m_session)
{
	Player* plr = getSelectedChar(m_session, true);
	if(!plr)
	{
		SystemMessage(m_session, "You don't have a selected player.");
		return false;
	}

	BlueSystemMessage(m_session, "Listing items for player %s:", plr->GetName());
	int itemcount = 0;
	SkillIterator itr2(plr);
	itr2.BeginSearch();
	for(; !itr2.End(); itr2.Increment())
	{
		itemcount++;
		SystemMessage(m_session, "Skill %u %s %u/%u", itr2->Skill->id, itr2->Skill->Name , itr2->CurrentValue, itr2->MaximumValue);
	}
	itr2.EndSearch();
	BlueSystemMessage(m_session, "Listed %d skills for player %s.", itemcount, plr->GetName());
	sGMLog.writefromsession(m_session, "used show skills command on %s,", plr->GetName());

	return true;
}

bool ChatHandler::HandleRenameCommand(const char* args, WorldSession* m_session)
{
	if(strlen(args) > 100) // prevent buffer overflow
		return false;

	char name1[100];
	char name2[100];

	if(sscanf(args, "%s %s", name1, name2) != 2)
		return false;

	if(VerifyName(name2, strlen(name2)) != E_CHAR_NAME_SUCCESS)
	{
		RedSystemMessage(m_session, "That name is invalid or contains invalid characters.");
		return true;
	}

	string new_name = name2;
	PlayerInfo* pi = objmgr.GetPlayerInfoByName(name1);
	if(!pi)
	{
		RedSystemMessage(m_session, "A player with that name was not found.");
		return false;
	}

	if(objmgr.GetPlayerInfoByName(new_name.c_str()))
	{
		RedSystemMessage(m_session, "Player found with this name is already in use.");
		return false;
	}

	objmgr.RenamePlayerInfo(pi, pi->name, new_name.c_str());

	free(pi->name);
	pi->name = strdup(new_name.c_str());

	// look in world for him
	Player* plr = objmgr.GetPlayer(pi->guid);
	if(plr)
	{
		plr->SetName(new_name);
		BlueSystemMessageToPlr(plr, "%s changed your name to '%s'.", m_session->GetPlayer()->GetName(), new_name.c_str());
		plr->SaveToDB(false);
	}
	else
		CharacterDatabase.WaitExecute("UPDATE characters SET name = '%s' WHERE guid = %u", CharacterDatabase.EscapeString(new_name).c_str(), (uint32)pi->guid);

	GreenSystemMessage(m_session, "Changed name from '%s' to '%s'.", name1, name2);
	sGMLog.writefromsession(m_session, "renamed character %s (GUID: %u) to %s", name1, pi->guid, name2);
	sPlrLog.writefromsession(m_session, "GM renamed character %s (GUID: %u) to %s", name1, pi->guid, name2);
	return true;
}

bool ChatHandler::HandleForceRenameCommand(const char* args, WorldSession* m_session)
{
	if(strlen(args) > 100) // prevent buffer overflow
		return false;

	string tmp = string(args);
	PlayerInfo* pi = objmgr.GetPlayerInfoByName(tmp.c_str());
	if(!pi)
	{
		RedSystemMessage(m_session, "A player with that name was not found.");
		return true;
	}

	Player* plr = objmgr.GetPlayer((uint32)pi->guid);
	if(!plr)
		CharacterDatabase.Execute("UPDATE characters SET forced_rename_pending = 1 WHERE guid = %u", (uint32)pi->guid);
	else
	{
		plr->rename_pending = true;
		plr->SaveToDB(false);
		BlueSystemMessageToPlr(plr, "%s forced your character to be renamed next logon.", m_session->GetPlayer()->GetName());
	}

	CharacterDatabase.Execute("INSERT INTO banned_names VALUES('%s')", CharacterDatabase.EscapeString(string(pi->name)).c_str());
	GreenSystemMessage(m_session, "Forcing %s to rename his character next logon.", args);
	sGMLog.writefromsession(m_session, "forced %s to rename his charater (%u)", pi->name, pi->guid);
	return true;
}

bool ChatHandler::HandlePhaseCommand(const char* args , WorldSession* m_session)
{
	Player* p_target = getSelectedChar(m_session, false);
	if(!p_target)
	{
		SystemMessage(m_session, "You don't have a selected player.");
		return false;
	}

	if(strlen(args) < 1)
	{
		SystemMessage(m_session, "%s phase:%s%u", MSG_COLOR_GREEN, MSG_COLOR_LIGHTBLUE, p_target->GetPhase());
		return true;
	}

	uint32 i = atoi(args);
	p_target->Phase(PHASE_SET, i);

	if(p_target->GetSession())
	{
		WorldPacket data(SMSG_SET_PHASE_SHIFT, 4);
		data << i;
		p_target->GetSession()->SendPacket(&data);
	}

	return true;
}