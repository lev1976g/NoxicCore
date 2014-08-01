/*
 * ArcScripts for ArcEmu MMORPG Server
 * Copyright (C) 2009 ArcEmu Team <http://www.arcemu.org/>
 * Copyright (C) 2008-2009 Sun++ Team <http://www.sunscripting.com/>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Setup.h"
#include "../Common/EasyFunctions.h"

/////////////////////////////////
// CHAPTER ONE
/////////////////////////////////

class InServiceOfLichKing : public QuestScript
{
	public:
		void OnQuestStart(Player* mTarget, QuestLogEntry* /*qLogEntry*/)
		{
			mTarget->PlaySound(14734);
            sEventMgr.AddEvent(mTarget, &Player::PlaySound, (uint32)14735, EVENT_UNK, 22500, 1, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
            sEventMgr.AddEvent(mTarget, &Player::PlaySound, (uint32)14736, EVENT_UNK, 48500, 1, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
		}
};

static uint32 IntiateEntries[5]= {29519, 29565, 29567, 29520};
class AcherusSoulPrison : GameObjectAIScript
{
	public:
		AcherusSoulPrison(GameObject* goinstance) : GameObjectAIScript(goinstance) {}
		static GameObjectAIScript* Create(GameObject* GO) { return new AcherusSoulPrison(GO); }

		void OnActivate(Player* pPlayer)
		{
			if(!pPlayer->HasQuest(12848))
				return;

			Creature* pCreature = NULL;
			for(uint8 i = 0; i < 5; i++)
			{
				if(pCreature = pPlayer->GetMapMgr()->GetInterface()->GetCreatureNearestCoords(pPlayer->GetPositionX(), pPlayer->GetPositionY(), pPlayer->GetPositionZ(), IntiateEntries[i]))
					break;
			}

			if(!pCreature || !pCreature->isAlive())
				return;

			pPlayer->SendChatMessage(CHAT_MSG_SAY, LANG_UNIVERSAL, "I give you the key to your salvation");
			pCreature->SetUInt64Value(UNIT_FIELD_FLAGS, 0);
			pCreature->GetAIInterface()->setNextTarget(pPlayer);
			pCreature->GetAIInterface()->AttackReaction(pPlayer, 1, 0);
			pCreature->SendChatMessage(CHAT_MSG_MONSTER_SAY, LANG_UNIVERSAL, "You have committed a big mistake, demon");
		}
};

class ScourgeGryphons : public Arcemu::Gossip::Script
{
	public:
		void OnHello(Object* pObject, Player* plr)
		{
			if(!plr->HasQuest(12670) || !plr->HasFinishedQuest(12670))
				return;

			if(pObject->GetEntry() == 29488)
				plr->TaxiStart(sTaxiMgr.GetTaxiPath(1053), 26308, 0);
			else if (pObject->GetEntry() == 29501)
				plr->TaxiStart(sTaxiMgr.GetTaxiPath(1054), 26308, 0);
		}
};

// SPELL EFFECTS
bool PreperationForBattle_sEffect(uint32 i, Spell* pSpell)
{
	if(pSpell->p_caster->HasQuest(12842) && !pSpell->p_caster->HasFinishedQuest(12842))
		pSpell->p_caster->CastSpell(pSpell->p_caster, 54586, true);

	return true;
}

void SetupDeathKnight(ScriptMgr* mgr)
{
	mgr->register_quest_script(12687, new InServiceOfLichKing());
	mgr->register_gameobject_script(191588, &AcherusSoulPrison::Create);
	mgr->register_gameobject_script(191577, &AcherusSoulPrison::Create);
	mgr->register_gameobject_script(191580, &AcherusSoulPrison::Create);
	mgr->register_gameobject_script(191581, &AcherusSoulPrison::Create);
	mgr->register_gameobject_script(191582, &AcherusSoulPrison::Create);
	mgr->register_gameobject_script(191583, &AcherusSoulPrison::Create);
	mgr->register_gameobject_script(191584, &AcherusSoulPrison::Create);
	mgr->register_gameobject_script(191585, &AcherusSoulPrison::Create);
	mgr->register_gameobject_script(191586, &AcherusSoulPrison::Create);
	mgr->register_gameobject_script(191587, &AcherusSoulPrison::Create);
	mgr->register_gameobject_script(191589, &AcherusSoulPrison::Create);
	mgr->register_gameobject_script(191590, &AcherusSoulPrison::Create);
    mgr->register_creature_gossip(29488, new ScourgeGryphons);
    mgr->register_creature_gossip(29501, new ScourgeGryphons);

	// Spell Effects
	uint32 preperationForBattle_spells[]= {53341, 53343, 53331, 54447, 53342, 53323, 53344, 70164, 62158, 0};
	mgr->register_dummy_spell(preperationForBattle_spells, &PreperationForBattle_sEffect);
}