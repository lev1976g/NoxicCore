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

bool ChatHandler::HandleCreatePetCommand(const char* args, WorldSession* m_session)
{
	if((args == NULL) || (strlen(args) < 2))
		return false;

	uint32 entry = atol(args);
	if(entry == 0)
		return false;

	CreatureInfo* ci = CreatureNameStorage.LookupEntry(entry);
	CreatureProto* cp = CreatureProtoStorage.LookupEntry(entry);

	if((ci == NULL) || (cp == NULL))
		return false;

	Player* p = m_session->GetPlayer();

	p->DismissActivePets();
	p->RemoveFieldSummon();

	float followangle = -M_PI_FLOAT * 2;
	LocationVector v(p->GetPosition());

	v.x += (3 * (cosf(followangle + p->GetOrientation())));
	v.y += (3 * (sinf(followangle + p->GetOrientation())));

	Pet* pet = objmgr.CreatePet(entry);

	if(!pet->CreateAsSummon(entry, ci, NULL, p, NULL, 1, 0, &v, true))
	{
		pet->DeleteMe();
		return true;
	}

	pet->GetAIInterface()->SetUnitToFollowAngle(followangle);

	return true;
}

bool ChatHandler::HandleDismissPetCommand(const char* args, WorldSession* m_session)
{
	Player* plr = getSelectedChar(m_session, false);
	Pet* pPet = NULL;
	if(plr)
	{
		if(plr->GetSummon() == NULL)
		{
			RedSystemMessage(m_session, "Player has no pet.");
			return true;
		}
		else
		{
			plr->DismissActivePets();
		}
	}
	else
	{
		// no player selected, see if it is a pet
		Creature* pCrt = getSelectedCreature(m_session, false);
		if(!pCrt)
		{
			// show usage string
			return false;
		}
		if(pCrt->IsPet())
		{
			pPet = TO< Pet* >(pCrt);
		}
		if(!pPet)
		{
			RedSystemMessage(m_session, "No player or pet selected.");
			return true;
		}
		plr = pPet->GetPetOwner();
		pPet->Dismiss();
	}

	GreenSystemMessage(m_session, "Dismissed %s's pet.", plr->GetName());
	plr->GetSession()->SystemMessage("%s dismissed your pet.", m_session->GetPlayer()->GetName());
	return true;
}

bool ChatHandler::HandleRenamePetCommand(const char* args, WorldSession* m_session)
{
	Player* plr = m_session->GetPlayer();
	Pet* pPet = plr->GetSummon();
	if(pPet == NULL)
	{
		RedSystemMessage(m_session, "You have no pet.");
		return true;
	}

	if(strlen(args) < 1)
	{
		RedSystemMessage(m_session, "You must specify a name.");
		return true;
	}

	GreenSystemMessage(m_session, "Renamed your pet to %s.", args);
	pPet->Rename(args);//support for only 1st pet
	return true;
}

bool ChatHandler::HandleAddPetSpellCommand(const char* args, WorldSession* m_session)
{
	Player* plr = getSelectedChar(m_session, false);
	if(!plr)
		return false;

	if(plr->GetSummon() == NULL)
	{
		RedSystemMessage(m_session, "%s has no pet.", plr->GetName());
		return true;
	}

	uint32 SpellId = atol(args);
	SpellEntry* spell = dbcSpell.LookupEntryForced(SpellId);
	if(!SpellId || !spell)
	{
		RedSystemMessage(m_session, "Invalid spell id requested.");
		return true;
	}

	std::list<Pet*> summons = plr->GetSummons();
	for(std::list<Pet*>::iterator itr = summons.begin(); itr != summons.end(); ++itr)
	{
		(*itr)->AddSpell(spell, true);
	}
	GreenSystemMessage(m_session, "Added spell %u to %s's pet.", SpellId, plr->GetName());
	return true;
}

bool ChatHandler::HandleRemovePetSpellCommand(const char* args, WorldSession* m_session)
{
	Player* plr = getSelectedChar(m_session, false);
	if(!plr)
		return false;

	if(plr->GetSummon() == NULL)
	{
		RedSystemMessage(m_session, "%s has no pet.", plr->GetName());
		return true;
	}

	uint32 SpellId = atol(args);
	SpellEntry* spell = dbcSpell.LookupEntryForced(SpellId);
	if(!SpellId || !spell)
	{
		RedSystemMessage(m_session, "Invalid spell id requested.");
		return true;
	}

	std::list<Pet*> summons = plr->GetSummons();
	for(std::list<Pet*>::iterator itr = summons.begin(); itr != summons.end(); ++itr)
	{
		(*itr)->RemoveSpell(SpellId);
	}
	GreenSystemMessage(m_session, "Removed spell %u from %s's pet.", SpellId, plr->GetName());
	return true;
}

bool ChatHandler::HandlePetLevelCommand(const char* args, WorldSession* m_session)
{
	if(!args)
	{
		return false;
	}

	int32 newLevel = atol(args);
	if(newLevel < 1)
	{
		return false;
	}

	Player* plr = getSelectedChar(m_session, false);
	Pet* pPet = NULL;
	if(plr)
	{
		pPet = plr->GetSummon();
		if(!pPet)
		{
			RedSystemMessage(m_session, "Player has no pet.");
			return true;
		}
	}
	else
	{
		// no player selected, see if it is a pet
		Creature* pCrt = getSelectedCreature(m_session, false);
		if(!pCrt)
		{
			// show usage string
			return false;
		}
		if(pCrt->IsPet())
		{
			pPet = TO< Pet* >(pCrt);
		}
		if(!pPet)
		{
			RedSystemMessage(m_session, "No player or pet selected.");
			return true;
		}
		plr = pPet->GetPetOwner();
	}

	// Should GMs be allowed to set a pet higher than its owner?  I don't think so
	if((uint32)newLevel > plr->getLevel())
	{
		newLevel = plr->getLevel();
	}

	//support for only 1 pet
	pPet->setLevel(newLevel);
	pPet->SetUInt32Value(UNIT_FIELD_PETEXPERIENCE, 0);
	pPet->SetUInt32Value(UNIT_FIELD_PETNEXTLEVELEXP, pPet->GetNextLevelXP(newLevel));
	pPet->ApplyStatsForLevel();
	pPet->UpdateSpellList();

	GreenSystemMessage(m_session, "Set %s's pet to level %lu.", plr->GetName(), newLevel);
	plr->GetSession()->SystemMessage("%s set your pet to level %lu.", m_session->GetPlayer()->GetName(), newLevel);
	return true;
}

#ifdef USE_SPECIFIC_AIAGENTS
//this is custom stuff !
bool ChatHandler::HandlePetSpawnAIBot(const char* args, WorldSession* m_session)
{
	if(!*args)
		return false;

	if(!m_session->GetPlayer())
		return false; //wtf ?

	uint32 botprice = m_session->GetPlayer()->getLevel() * 10000; //1 gold per level ?

	if(!m_session->GetPlayer()->HasGold(botprice))
	{
		GreenSystemMessage(m_session, "You need a total of %u coins to afford a bot", botprice);
		return false;
	}

	uint8 botType = (uint8)atof((char*)args);

	if(botType != 0)
	{
		RedSystemMessage(m_session, "Incorrect value. Accepting value 0 only = healbot :)");
		return true;
	}

	uint32 Entry;
	char name[50];
	uint8 race = m_session->GetPlayer()->getRace();

	if(race == RACE_HUMAN || race == RACE_DWARF || race == RACE_NIGHTELF || race == RACE_GNOME || race == RACE_DRAENEI)
	{
		Entry = 1826;
		strcpy(name, "|cffff6060A_HealBot");
	}
	else
	{
		Entry = 5473;
		strcpy(name, "|cffff6060H_HealBot");
	}

	CreatureProto* pTemplate = CreatureProtoStorage.LookupEntry(Entry);
	CreatureInfo* pCreatureInfo = CreatureNameStorage.LookupEntry(Entry);
	if(!pTemplate || !pCreatureInfo)
	{
		RedSystemMessage(m_session, "Invalid creature spawn template: %u", Entry);
		return true;
	}
	Player* plr = m_session->GetPlayer();
	Creature* newguard = plr->create_guardian(Entry, 2 * 60 * 1000, float(-M_PI * 2), plr->getLevel());
	AiAgentHealSupport* new_interface = new AiAgentHealSupport;
	new_interface->Init(newguard, AITYPE_PET, MOVEMENTTYPE_NONE, plr);
	newguard->ReplaceAIInterface((AIInterface*) new_interface);

	/*	Pet *old_tame = plr->GetSummon();
		if(old_tame != NULL)
		{
			old_tame->Dismiss(true);
		}

		// create a pet from this creature
		Pet * pPet = objmgr.CreatePet( Entry );
		pPet->SetInstanceID(plr->GetInstanceID());
		pPet->SetMapId(plr->GetMapId());

		pPet->SetFloatValue ( OBJECT_FIELD_SCALE_X, pTemplate->Scale / 2); //we do not wish to block visually other players
		AiAgentHealSupport *new_interface = new AiAgentHealSupport;
		pPet->ReplaceAIInterface( (AIInterface *) new_interface );
	//	new_interface->Init(pPet,AITYPE_PET,MOVEMENTTYPE_NONE,plr); // i think this will get called automatically for pet

		pPet->CreateAsSummon(Entry, pCreatureInfo, pCreature, plr, NULL, 0x2, 0);

		pPet->Rename(name);

		//healer bot should not have any specific actions
		pPet->SetActionBarSlot(0,PET_SPELL_FOLLOW);
		pPet->SetActionBarSlot(1,PET_SPELL_STAY);
		pPet->SetActionBarSlot(2,0);
		pPet->SetActionBarSlot(3,0);
		pPet->SetActionBarSlot(4,0);
		pPet->SetActionBarSlot(5,0);
		pPet->SetActionBarSlot(6,0);
		pPet->SetActionBarSlot(7,0);
		pPet->SetActionBarSlot(8,0);
		pPet->SetActionBarSlot(9,0);
		pPet->SendSpellsToOwner();

		// remove the temp creature
		delete sp;
		delete pCreature;*/

	sGMLog.writefromsession(m_session, "used create an AI bot");
	return true;
}
#endif