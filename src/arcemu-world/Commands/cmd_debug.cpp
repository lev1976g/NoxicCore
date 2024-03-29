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

//#define _ONLY_FOOLS_TRY_THIS_

struct spell_thingo
{
	uint32 type;
	uint32 target;
};

SpellCastTargets SetTargets(SpellEntry* sp, uint32 type, uint32 targettype, Unit* dst, Creature* src)
{
	SpellCastTargets targets;
	targets.m_unitTarget = 0;
	targets.m_itemTarget = 0;
	targets.m_srcX = 0;
	targets.m_srcY = 0;
	targets.m_srcZ = 0;
	targets.m_destX = 0;
	targets.m_destY = 0;
	targets.m_destZ = 0;

	if(targettype == TTYPE_SINGLETARGET)
	{
		targets.m_targetMask = TARGET_FLAG_UNIT;
		targets.m_unitTarget = dst->GetGUID();
	}
	else if(targettype == TTYPE_SOURCE)
	{
		targets.m_targetMask = TARGET_FLAG_SOURCE_LOCATION;
		targets.m_srcX = src->GetPositionX();
		targets.m_srcY = src->GetPositionY();
		targets.m_srcZ = src->GetPositionZ();
	}
	else if(targettype == TTYPE_DESTINATION)
	{
		targets.m_targetMask = TARGET_FLAG_DEST_LOCATION;
		targets.m_destX = dst->GetPositionX();
		targets.m_destY = dst->GetPositionY();
		targets.m_destZ = dst->GetPositionZ();
	}

	return targets;
}

bool ChatHandler::HandleDebugInFrontCommand(const char* args, WorldSession* m_session)
{
	Object* obj = NULL;
	uint64 guid = m_session->GetPlayer()->GetSelection();
	if(guid)
	{
		if((obj = m_session->GetPlayer()->GetMapMgr()->GetUnit(guid)) == 0)
		{
			SystemMessage(m_session, "You should select a character or a creature.");
			return true;
		}
	}
	else
		obj = m_session->GetPlayer();

	char buf[256];
	snprintf((char*)buf, 256, "%d", m_session->GetPlayer()->isInFront((Unit*)obj));
	SystemMessage(m_session, buf);
	return true;
}

bool ChatHandler::HandleShowReactionCommand(const char* args, WorldSession* m_session)
{
	Object* obj = NULL;
	uint64 guid = m_session->GetPlayer()->GetSelection();
	if(guid)
		obj = m_session->GetPlayer()->GetMapMgr()->GetCreature(GET_LOWGUID_PART(guid));

	if(!obj)
	{
		SystemMessage(m_session, "You should select a creature.");
		return false;
	}

	char* pReaction = strtok((char*)args, " ");
	if(!pReaction)
		return false;

	uint32 Reaction = atoi(pReaction);
	obj->SendAIReaction(Reaction);
	std::stringstream sstext;
	sstext << "Sent Reaction of " << Reaction << " to " << obj->GetUIdFromGUID() << '\0';
	SystemMessage(m_session,  sstext.str().c_str());
	return true;
}

bool ChatHandler::HandleAIMoveCommand(const char* args, WorldSession* m_session)
{
	Object* obj = NULL;
	uint64 guid = m_session->GetPlayer()->GetSelection();
	if(guid)
		obj = m_session->GetPlayer()->GetMapMgr()->GetCreature(GET_LOWGUID_PART(guid));

	if(!obj)
	{
		SystemMessage(m_session, "You should select a creature.");
		return false;
	}

	//m_session->GetPlayer()->GetOrientation();

	uint32 Move = 1;
	uint32 Run = 0;
	uint32 Time = 0;
	uint32 Meth = 0;

	char* pMove = strtok((char*)args, " ");
	if(pMove)
		Move = atoi(pMove);

	char* pRun = strtok(NULL, " ");
	if(pRun)
		Run = atoi(pRun);

	char* pTime = strtok(NULL, " ");
	if(pTime)
		Time = atoi(pTime);

	char* pMeth = strtok(NULL, " ");
	if(pMeth)
		Meth = atoi(pMeth);

	float x = m_session->GetPlayer()->GetPositionX();
	float y = m_session->GetPlayer()->GetPositionY();
	float z = m_session->GetPlayer()->GetPositionZ();
	float o = m_session->GetPlayer()->GetOrientation();
	if(Run)
		TO<Creature*>(obj)->GetAIInterface()->SetRun();
	else
		TO<Creature*>(obj)->GetAIInterface()->SetWalk();

	float distance = TO<Creature*>(obj)->CalcDistance(x, y, z);
	if(Move == 1)
	{
		if(Meth == 1)
		{
			float q = distance - 0.5f;
			x = (TO<Creature*>(obj)->GetPositionX() + x * q) / (1 + q);
			y = (TO<Creature*>(obj)->GetPositionY() + y * q) / (1 + q);
			z = (TO<Creature*>(obj)->GetPositionZ() + z * q) / (1 + q);
		}
		else if(Meth == 2)
		{
			float q = distance - 1;
			x = (TO<Creature*>(obj)->GetPositionX() + x * q) / (1 + q);
			y = (TO<Creature*>(obj)->GetPositionY() + y * q) / (1 + q);
			z = (TO<Creature*>(obj)->GetPositionZ() + z * q) / (1 + q);
		}
		else if(Meth == 3)
		{
			float q = distance - 2;
			x = (TO<Creature*>(obj)->GetPositionX() + x * q) / (1 + q);
			y = (TO<Creature*>(obj)->GetPositionY() + y * q) / (1 + q);
			z = (TO<Creature*>(obj)->GetPositionZ() + z * q) / (1 + q);
		}
		else if(Meth == 4)
		{
			float q = distance - 2.5f;
			x = (TO<Creature*>(obj)->GetPositionX() + x * q) / (1 + q);
			y = (TO<Creature*>(obj)->GetPositionY() + y * q) / (1 + q);
			z = (TO<Creature*>(obj)->GetPositionZ() + z * q) / (1 + q);
		}
		else if(Meth == 5)
		{
			float q = distance - 3;
			x = (TO<Creature*>(obj)->GetPositionX() + x * q) / (1 + q);
			y = (TO<Creature*>(obj)->GetPositionY() + y * q) / (1 + q);
			z = (TO<Creature*>(obj)->GetPositionZ() + z * q) / (1 + q);
		}
		else if(Meth == 6)
		{
			float q = distance - 3.5f;
			x = (TO<Creature*>(obj)->GetPositionX() + x * q) / (1 + q);
			y = (TO<Creature*>(obj)->GetPositionY() + y * q) / (1 + q);
			z = (TO<Creature*>(obj)->GetPositionZ() + z * q) / (1 + q);
		}
		else
		{
			float q = distance - 4;
			x = (TO<Creature*>(obj)->GetPositionX() + x * q) / (1 + q);
			y = (TO<Creature*>(obj)->GetPositionY() + y * q) / (1 + q);
			z = (TO<Creature*>(obj)->GetPositionZ() + z * q) / (1 + q);
		}
		TO<Creature*>(obj)->GetAIInterface()->MoveTo(x, y, z, 0);
	}
	else
		TO_CREATURE(obj)->GetAIInterface()->MoveTo(x, y, z, o);

	return true;
}

bool ChatHandler::HandleDistanceCommand(const char* args, WorldSession* m_session)
{
	Object* obj = NULL;
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

	float dist = m_session->GetPlayer()->CalcDistance(obj);
	std::stringstream sstext;
	sstext << "Distance is: " << dist << '\0';

	SystemMessage(m_session, sstext.str().c_str());
	return true;
}

bool ChatHandler::HandleFaceCommand(const char* args, WorldSession* m_session)
{
	Object* obj = NULL;
	uint64 guid = m_session->GetPlayer()->GetSelection();
	if(guid)
		obj = m_session->GetPlayer()->GetMapMgr()->GetCreature(GET_LOWGUID_PART(guid));

	if(!obj)
	{
		SystemMessage(m_session,  "You should select a creature.");
		return false;
	}

	uint32 Orentation = 0;
	char* pOrentation = strtok((char*)args, " ");
	if(pOrentation)
		Orentation = atoi(pOrentation);

	/* Convert to Blizzards Format */
	float theOrientation = Orentation / (180.0f / M_PI_FLOAT);
	obj->SetPosition(obj->GetPositionX(), obj->GetPositionY(), obj->GetPositionZ(), theOrientation, false);
	/*data.Initialize(SMSG_MONSTER_MOVE);
	data << obj->GetGUID();
	data << obj->GetPositionX() << obj->GetPositionY() << obj->GetPositionZ() << obj->GetOrientation();
	data << uint8(1);

	data << uint32(0x100); //run
	data << uint32(0); //time
	data << uint32(2);
	data << obj->GetPositionX() << obj->GetPositionY() << obj->GetPositionZ() << theOrientation;
	UpdateData upd;

	// update movment for others
	obj->BuildMovementUpdateBlock(&upd, 0);
	upd.BuildPacket(&data);
	GetSession()->SendPacket(&packet);
	obj->BuildMovementUpdateBlock(data, 0)
	obj->SendMessageToSet(&data, false);*/
	LOG_DEBUG("Facing sent.");
	return true;
	//((Creature*)obj)->AI_MoveTo(obj->GetPositionX() + 0.1, obj->GetPositionY() + 0.1, obj->GetPositionZ() + 0.1, theOrientation);
}

bool ChatHandler::HandleMoveInfoCommand(const char* args, WorldSession* m_session)
{
	Object* obj = NULL;
	uint64 guid = m_session->GetPlayer()->GetSelection();
	if((obj = m_session->GetPlayer()->GetMapMgr()->GetCreature(GET_LOWGUID_PART(guid))) == 0)
	{
		SystemMessage(m_session, "You should select a character or a creature.");
		return false;
	}

	float dist = m_session->GetPlayer()->CalcDistance(obj);
	bool minfront = obj->isInFront(m_session->GetPlayer());
	bool pinfront = m_session->GetPlayer()->isInFront(obj);
	uint32 movetype = TO<Creature*>(obj)->GetAIInterface()->getMoveType();
	bool run = TO<Creature*>(obj)->GetAIInterface()->HasWalkMode(WALKMODE_RUN) || TO< Creature* >(obj)->GetAIInterface()->HasWalkMode(WALKMODE_SPRINT);
	uint32 attackerscount = (uint32)TO<Creature*>(obj)->GetAIInterface()->getAITargetsCount();
	uint32 creatureState = TO<Creature*>(obj)->GetAIInterface()->m_creatureState;
	uint32 curwp = TO<Creature*>(obj)->GetAIInterface()->getCurrentWaypoint();
	//Unit* unitToFollow = ((Creature*)obj)->GetAIInterface()->getUnitToFollow();
	uint32 aistate = TO<Creature*>(obj)->GetAIInterface()->getAIState();
	uint32 aitype = TO<Creature*>(obj)->GetAIInterface()->getAIType();
	uint32 aiagent = TO<Creature*>(obj)->GetAIInterface()->getCurrentAgent();
	uint32 lowfollow = 0;
	uint32 highfollow = 0;
	/*if(!unitToFollow)
	{
		lowfollow = 0;
		highfollow = 0;
	}
	else
	{
		lowfollow = unitToFollow->GetGUIDLow();
		highfollow = unitToFollow->GetGUIDHigh();;
	}*/

	std::stringstream sstext;
	sstext << "Following Unit: Low: " << lowfollow << " High: " << highfollow << "\n";
	sstext << "Distance is: " << dist << "\n";
	sstext << "Mob Facing Player: " << minfront << " Player Facing Mob: " << pinfront << "\n";
	sstext << "Attackers Count: " << attackerscount << "\n";
	sstext << "Creature State: " << creatureState << " Run: " << run << "\n";
	sstext << "AIState: " << aistate << " AIType: " << aitype << " AIAgent: " << aiagent << "\n";
	sstext << "MoveType: " << movetype << " Current Waypoint: " << curwp << "\n";

	SendMultilineMessage(m_session, sstext.str().c_str());
	//FillSystemMessageData(&data, sstext.str().c_str());
	//m_session->SendPacket(&data);

	return true;
}

bool ChatHandler::HandleSetBytesCommand(const char* args, WorldSession* m_session)
{
	Object* obj = NULL;
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

	char* pBytesIndex = strtok((char*)args, " ");
	if(!pBytesIndex)
		return false;

	uint32 BytesIndex  = atoi(pBytesIndex);

	char* pValue1 = strtok(NULL, " ");
	if(!pValue1)
		return false;

	uint8 Value1  = static_cast<uint8>(atol(pValue1));

	char* pValue2 = strtok(NULL, " ");
	if(!pValue2)
		return false;

	uint8 Value2  = static_cast<uint8>(atol(pValue2));

	char* pValue3 = strtok(NULL, " ");
	if(!pValue3)
		return false;

	uint8 Value3  = static_cast<uint8>(atol(pValue3));

	char* pValue4 = strtok(NULL, " ");
	if(!pValue4)
		return false;

	uint8 Value4 = static_cast<uint8>(atol(pValue4));

	std::stringstream sstext;
	sstext << "Set Field " << BytesIndex << " bytes to " << uint16((uint8)Value1) << " " << uint16((uint8)Value2) << " " << uint16((uint8)Value3) << " " << uint16((uint8)Value4) << '\0';
	obj->SetUInt32Value(BytesIndex, ((Value1) | (Value2 << 8) | (Value3 << 16) | (Value4 << 24)));
	SystemMessage(m_session, sstext.str().c_str());
	return true;
}

bool ChatHandler::HandleGetBytesCommand(const char* args, WorldSession* m_session)
{
	Object* obj = NULL;
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

	char* pBytesIndex = strtok((char*)args, " ");
	if(!pBytesIndex)
		return false;

	uint32 BytesIndex = atoi(pBytesIndex);
	uint32 theBytes = obj->GetUInt32Value(BytesIndex);
	/*switch(Bytes)
	{
		case 0:
			theBytes = obj->GetUInt32Value(UNIT_FIELD_BYTES_0);
		break;
		case 1:
			theBytes = obj->GetUInt32Value(UNIT_FIELD_BYTES_1);
		break;
		case 2:
			theBytes = obj->GetUInt32Value(UNIT_FIELD_BYTES_2);
		break;
	}*/

	std::stringstream sstext;
	sstext << "bytes for Field " << BytesIndex << " are " << uint16((uint8)theBytes & 0xFF) << " " << uint16((uint8)(theBytes >> 8) & 0xFF) << " ";
	sstext << uint16((uint8)(theBytes >> 16) & 0xFF) << " " << uint16((uint8)(theBytes >> 24) & 0xFF) << '\0';

	SystemMessage(m_session, sstext.str().c_str());
	return true;
}

bool ChatHandler::HandleDebugRoot(const char* args, WorldSession* m_session)
{
	if(!m_session->GetPlayer()->GetTargetGUID())
	{
		RedSystemMessage(m_session, "You need to select a unit!");
		return true;
	}

	Unit* u = m_session->GetPlayer()->GetMapMgr()->GetUnit( m_session->GetPlayer()->GetTargetGUID() );
	if(!u)
	{
		RedSystemMessage(m_session, "You need to select a unit!");
		return true;
	}

	if(!stricmp(args, "on"))
	{
		u->Root();
		SystemMessage(m_session, "Unit rooted!");
	}
	else if(!stricmp(args, "off"))
	{
		u->Unroot();
		SystemMessage(m_session, "Unit unrooted!");
	}
	else
		return false;

	return true;
}

bool ChatHandler::HandleDebugLandWalk(const char* args, WorldSession* m_session)
{
	Player* chr = getSelectedChar(m_session);
	char buf[256];
	if(!chr) // Ignatich: what should NOT happen but just in case...
	{
		SystemMessage(m_session, "You need to select a character.");
		return false;
	}
	chr->SetMovement(MOVE_LAND_WALK, 8);
	snprintf((char*)buf, 256, "Land Walk Test Ran.");
	SystemMessage(m_session, buf);
	return true;
}

bool ChatHandler::HandleDebugWaterWalk(const char* args, WorldSession* m_session)
{
	Player* chr = getSelectedChar(m_session);
	char buf[256];
	if(!chr) // Ignatich: what should NOT happen but just in case...
	{
		SystemMessage(m_session, "You need to select a character.");
		return false;
	}
	chr->SetMovement(MOVE_WATER_WALK, 4);
	snprintf((char*)buf, 256, "Water Walk Test Ran.");
	SystemMessage(m_session,  buf);
	return true;
}

bool ChatHandler::HandleCastSpellCommand(const char* args, WorldSession* m_session)
{
	Unit* caster = m_session->GetPlayer();
	Unit* target = getSelectedChar(m_session, false);
	if(!target)
		target = getSelectedCreature(m_session, false);

	if(!target)
	{
		RedSystemMessage(m_session, "You need to select a character or creature.");
		return false;
	}

	uint32 spellid = atol(args);
	SpellEntry* spellentry = dbcSpell.LookupEntryForced(spellid);
	if(!spellentry)
	{
		RedSystemMessage(m_session, "Invalid spell id!");
		return false;
	}

	Spell* sp = sSpellFactoryMgr.NewSpell(caster, spellentry, false, NULL);

	BlueSystemMessage(m_session, "Casting spell %d on target.", spellid);
	SpellCastTargets targets;
	targets.m_unitTarget = target->GetGUID();
	sp->prepare(&targets);

	switch(target->GetTypeId())
	{
		case TYPEID_PLAYER:
		{
			if(caster != target)
				sGMLog.writefromsession(m_session, "cast spell %d on PLAYER %s", spellid, TO< Player* >(target)->GetName());
		}break;
		case TYPEID_UNIT:
			sGMLog.writefromsession(m_session, "cast spell %d on CREATURE %u [%s], sqlid %u", spellid, TO< Creature* >(target)->GetEntry(), TO< Creature* >(target)->GetCreatureInfo()->Name, TO< Creature* >(target)->GetSQL_id());
		break;
	}

	return true;
}

bool ChatHandler::HandleCastSelfCommand(const char* args, WorldSession* m_session)
{
	Unit* target = getSelectedChar(m_session, false);
	if(!target)
		target = getSelectedCreature(m_session, false);

	if(!target)
	{
		RedSystemMessage(m_session, "You need to select a character or creature.");
		return false;
	}

	uint32 spellid = atol(args);
	SpellEntry* spellentry = dbcSpell.LookupEntryForced(spellid);
	if(!spellentry)
	{
		RedSystemMessage(m_session, "Invalid spell id!");
		return false;
	}

	Spell* sp = sSpellFactoryMgr.NewSpell(target, spellentry, false, NULL);
	BlueSystemMessage(m_session, "Target is casting spell %d on himself.", spellid);
	SpellCastTargets targets;
	targets.m_unitTarget = target->GetGUID();
	sp->prepare(&targets);

	switch(target->GetTypeId())
	{
		case TYPEID_PLAYER:
		{
			if(m_session->GetPlayer() != target)
				sGMLog.writefromsession(m_session, "used castself with spell %d on PLAYER %s", spellid, TO< Player* >(target)->GetName());
		}break;
		case TYPEID_UNIT:
			sGMLog.writefromsession(m_session, "used castself with spell %d on CREATURE %u [%s], sqlid %u", spellid, TO< Creature* >(target)->GetEntry(), TO< Creature* >(target)->GetCreatureInfo()->Name, TO< Creature* >(target)->GetSQL_id());
		break;
	}

	return true;
}

bool ChatHandler::HandleCastSpellNECommand(const char* args, WorldSession* m_session)
{
	Unit* caster = m_session->GetPlayer();
	Unit* target = getSelectedChar(m_session, false);
	if(!target)
		target = getSelectedCreature(m_session, false);

	if(!target)
	{
		RedSystemMessage(m_session, "You need to select a character or creature.");
		return false;
	}

	uint32 spellId = atol(args);
	SpellEntry* spellentry = dbcSpell.LookupEntryForced(spellId);
	if(!spellentry)
	{
		RedSystemMessage(m_session, "Invalid spell id!");
		return false;
	}

	BlueSystemMessage(m_session, "Casting spell %d on target.", spellId);
	WorldPacket data;
	data.Initialize(SMSG_SPELL_START);
	data << caster->GetNewGUID();
	data << caster->GetNewGUID();
	data << spellId;
	data << uint8(0);
	data << uint16(0);
	data << uint32(0);
	data << uint16(2);
	data << target->GetGUID();
	//WPARCEMU_ASSERT(data.size() == 36);
	m_session->SendPacket(&data);

	data.Initialize(SMSG_SPELL_GO);
	data << caster->GetNewGUID();
	data << caster->GetNewGUID();
	data << spellId;
	data << uint8(0) << uint8(1) << uint8(1);
	data << target->GetGUID();
	data << uint8(0);
	data << uint16(2);
	data << target->GetGUID();
	//WPARCEMU_ASSERT(data.size() == 42);
	m_session->SendPacket(&data);

	switch(target->GetTypeId())
	{
		case TYPEID_PLAYER:
		{
			if(caster != target)
				sGMLog.writefromsession(m_session, "cast spell %d on PLAYER %s", spellId, TO< Player* >(target)->GetName());
		}break;
		case TYPEID_UNIT:
			sGMLog.writefromsession(m_session, "cast spell %d on CREATURE %u [%s], sqlid %u", spellId, TO< Creature* >(target)->GetEntry(), TO< Creature* >(target)->GetCreatureInfo()->Name, TO< Creature* >(target)->GetSQL_id());
		break;
	}

	return true;
}

bool ChatHandler::HandleAggroRangeCommand(const char* args, WorldSession* m_session)
{
	Unit* obj = NULL;
	uint64 guid = m_session->GetPlayer()->GetSelection();
	if(guid)
	{
		if((obj = m_session->GetPlayer()->GetMapMgr()->GetUnit(guid)) == 0)
		{
			SystemMessage(m_session,  "You should select a character or creature.");
			return false;
		}
	}
	else
	{
		SystemMessage(m_session, "You should select a character or creature.");
		return false;
	}

	float aggroRange = obj->GetAIInterface()->_CalcAggroRange(m_session->GetPlayer());
	std::stringstream sstext;
	sstext << "Aggrorange is: " << sqrtf(aggroRange) << '\0';

	SystemMessage(m_session, sstext.str().c_str());

	return true;
}

bool ChatHandler::HandleKnockBackCommand(const char* args, WorldSession* m_session)
{
	float f = args ? (float)atof(args) : 0.0f;
	if(f == 0.0f)
		f = 5.0f;

	float dx = sinf(m_session->GetPlayer()->GetOrientation());
	float dy = cosf(m_session->GetPlayer()->GetOrientation());

	float z = f * 0.66f;

	WorldPacket data(SMSG_MOVE_KNOCK_BACK, 50);
	data << m_session->GetPlayer()->GetNewGUID();
	data << getMSTime();
	data << dy;
	data << dx;
	data << f;
	data << z;
	m_session->SendPacket(&data);
	return true;
}

bool ChatHandler::HandleFadeCommand(const char* args, WorldSession* m_session)
{
	Unit* target = m_session->GetPlayer()->GetMapMgr()->GetUnit(m_session->GetPlayer()->GetSelection());
	if(!target)
		target = m_session->GetPlayer();

	char* v = strtok((char*)args, " ");
	if(!v)
		return false;

	target->ModThreatModifyer(atoi(v));

	std::stringstream sstext;
	sstext << "threat is now reduced by: " << target->GetThreatModifyer() << '\0';

	SystemMessage(m_session, sstext.str().c_str());
	return true;
}

bool ChatHandler::HandleThreatModCommand(const char* args, WorldSession* m_session)
{
	Unit* target = m_session->GetPlayer()->GetMapMgr()->GetUnit(m_session->GetPlayer()->GetSelection());
	if(!target)
		target = m_session->GetPlayer();

	char* v = strtok((char*)args, " ");
	if(!v)
		return false;

	target->ModGeneratedThreatModifyer(0, atoi(v));

	std::stringstream sstext;
	sstext << "new threat caused is now reduced by: " << target->GetGeneratedThreatModifyer(0) << "%" << '\0';

	SystemMessage(m_session, sstext.str().c_str());
	return true;
}

bool ChatHandler::HandleCalcThreatCommand(const char* args, WorldSession* m_session)
{
	Unit* target = m_session->GetPlayer()->GetMapMgr()->GetUnit(m_session->GetPlayer()->GetSelection());
	if(!target)
	{
		SystemMessage(m_session, "You must select a creature.");
		return true;
	}

	char* dmg = strtok((char*)args, " ");
	if(!dmg)
		return false;

	char* spellId = strtok(NULL, " ");
	if(!spellId)
		return false;

	uint32 threat = target->GetAIInterface()->_CalcThreat(atol(dmg), dbcSpell.LookupEntry(atoi(spellId)), m_session->GetPlayer());

	std::stringstream sstext;
	sstext << "generated threat is: " << threat << '\0';

	SystemMessage(m_session, sstext.str().c_str());
	return true;
}

bool ChatHandler::HandleThreatListCommand(const char* args, WorldSession* m_session)
{
	Unit* target = m_session->GetPlayer()->GetMapMgr()->GetUnit(m_session->GetPlayer()->GetSelection());
	if(!target)
	{
		SystemMessage(m_session, "You should select a creature.");
		return false;
	}

	std::stringstream sstext;
	sstext << "threatlist of creature: " << Arcemu::Util::GUID_LOPART(m_session->GetPlayer()->GetSelection()) << " " << Arcemu::Util::GUID_HIPART(m_session->GetPlayer()->GetSelection()) << '\n';
	TargetMap::iterator itr;
	for(itr = target->GetAIInterface()->GetAITargets()->begin(); itr != target->GetAIInterface()->GetAITargets()->end(); ++itr)
	{
		Unit* ai_t = target->GetMapMgr()->GetUnit(itr->first);
		if(!ai_t || !itr->second)
		{
			++itr;
			continue;
		}
		sstext << "guid: " << itr->first << " | threat: " << itr->second << "| threat after mod: " << (itr->second + ai_t->GetThreatModifyer()) << "\n";
	}

	SendMultilineMessage(m_session, sstext.str().c_str());
	return true;
}

bool ChatHandler::HandleGetTransporterTime(const char* args, WorldSession* m_session)
{
	//Player *plyr = m_session->GetPlayer();
	Creature* crt = getSelectedCreature(m_session, false);
	if(!crt)
		return false;

	WorldPacket data(SMSG_ATTACKERSTATEUPDATE, 1000);
	data << uint32(0x00000102);
	data << crt->GetNewGUID();
	data << m_session->GetPlayer()->GetNewGUID();

	data << uint32(6);
	data << uint8(1);
	data << uint32(1);
	data << uint32(0x40c00000);
	data << uint32(6);
	data << uint32(0);
	data << uint32(0);
	data << uint32(1);
	data << uint32(0x000003e8);
	data << uint32(0);
	data << uint32(0);
	m_session->SendPacket(&data);
	return true;
}

bool ChatHandler::HandleSendItemPushResult(const char* args, WorldSession* m_session)
{
	uint32 uint_args[7];
	char* arg = const_cast<char*>(args);
	char* token = strtok(arg, " ");

	uint8 i = 0;
	while(token != NULL && i < 7)
	{
		uint_args[i] = atol(token);
		token = strtok(NULL, " ");
		i++;
	}
	for(; i < 7; i++)
		uint_args[i] = 0;

	if(!uint_args[0]) // null itemid
		return false;

	// lookup item
	//ItemPrototype* proto = ItemPrototypeStorage.LookupEntry(itemid);

	WorldPacket data;
	data.SetOpcode(SMSG_ITEM_PUSH_RESULT);
	data << m_session->GetPlayer()->GetGUID(); // recivee_guid
	data << uint_args[2]; // type
	data << uint32(1); // unk
	data << uint_args[1]; // count
	data << uint8(0xFF); // uint8 unk const 0xFF
	data << uint_args[3]; // unk1
	data << uint_args[0]; // item id
	data << uint_args[4]; // unk2
	data << uint_args[5]; // random prop
	data << uint_args[6]; // unk3
	m_session->SendPacket(&data);
	return true;
	//data << ((proto != NULL) ? proto->Quality : uint32(0)); // quality
}

bool ChatHandler::HandleModifyBitCommand(const char* args, WorldSession* m_session)
{
	Object* obj;
	uint64 guid = m_session->GetPlayer()->GetSelection();
	if(guid)
	{
		if((obj = m_session->GetPlayer()->GetMapMgr()->GetUnit(guid)) == 0)
		{
			SystemMessage(m_session, "You should select a character or creature.");
			return false;
		}
	}
	else
		obj = m_session->GetPlayer();

	char* pField = strtok((char*)args, " ");
	if(!pField)
		return false;

	char* pBit = strtok(NULL, " ");
	if(!pBit)
		return false;

	uint16 field = static_cast<uint16>(atoi(pField));
	uint32 bit = atoi(pBit);

	if(field < 1 || field >= PLAYER_END)
	{
		SystemMessage(m_session, "Incorrect values.");
		return false;
	}

	if(bit < 1 || bit > 32)
	{
		SystemMessage(m_session, "Incorrect values.");
		return false;
	}

	char buf[256];

	if(obj->HasFlag(field, (1 << (bit - 1))))
	{
		obj->RemoveFlag(field, (1 << (bit - 1)));
		snprintf((char*)buf, 256, "Removed bit %i in field %i.", (unsigned int)bit, (unsigned int)field);
	}
	else
	{
		obj->SetFlag(field, (1 << (bit - 1)));
		snprintf((char*)buf, 256, "Set bit %i in field %i.", (unsigned int)bit, (unsigned int)field);
	}

	SystemMessage(m_session, buf);
	return true;
}

bool ChatHandler::HandleModifyValueCommand(const char* args,  WorldSession* m_session)
{
	Object* obj;
	uint64 guid = m_session->GetPlayer()->GetSelection();
	if(guid)
	{
		if((obj = m_session->GetPlayer()->GetMapMgr()->GetUnit(guid)) == 0)
		{
			SystemMessage(m_session, "You should select a character or creature.");
			return false;
		}
	}
	else
		obj = m_session->GetPlayer();

	char* pField = strtok((char*)args, " ");
	if(!pField)
		return false;

	char* pValue = strtok(NULL, " ");
	if(!pValue)
		return false;

	uint16 field = static_cast<uint16>(atoi(pField));
	uint32 value = atoi(pValue);

	if(field < 1 || field >= PLAYER_END)
	{
		SystemMessage(m_session, "Incorrect Field.");
		return false;
	}

	/*if(value > sizeof(uint32))
	{
		FillSystemMessageData(&data, m_session, "Incorrect value.");
		m_session->SendPacket(&data);
		return false;
	}*/

	char buf[256];
	uint32 oldValue = obj->GetUInt32Value(field);
	obj->SetUInt32Value(field, value);

	snprintf((char*)buf, 256, "Set Field %i from %i to %i.", (unsigned int)field, (unsigned int)oldValue, (unsigned int)value);

	if(obj->IsPlayer())
		TO<Player*>(obj)->UpdateChances();

	SystemMessage(m_session, buf);
	return true;
}

list<SpellEntry*> aiagent_spells;
map<uint32, spell_thingo> aiagent_extra;

bool ChatHandler::HandleAIAgentDebugBegin(const char* args, WorldSession* m_session)
{
	QueryResult* result = WorldDatabase.Query("SELECT DISTINCT spell FROM ai_agents");
	if(!result)
		return false;

	do
	{
		SpellEntry* se = dbcSpell.LookupEntryForced(result->Fetch()[0].GetUInt32());
		if(se)
			aiagent_spells.push_back(se);
	}
	while(result->NextRow());
	delete result;

	for(list<SpellEntry*>::iterator itr = aiagent_spells.begin(); itr != aiagent_spells.end(); ++itr)
	{
		result = WorldDatabase.Query("SELECT * FROM ai_agents WHERE spell = %u", (*itr)->Id);
		ARCEMU_ASSERT(result != NULL);
		spell_thingo t;
		t.type = result->Fetch()[6].GetUInt32();
		t.target = result->Fetch()[7].GetUInt32();
		delete result;
		aiagent_extra[(*itr)->Id] = t;
	}

	GreenSystemMessage(m_session, "Loaded %u spells for testing.", aiagent_spells.size());
	return true;
}

bool ChatHandler::HandleAIAgentDebugContinue(const char* args, WorldSession* m_session)
{
	uint32 count = atoi(args);
	if(!count)
		return false;

	Creature* pCreature = getSelectedCreature(m_session, true);
	if(!pCreature)
	{
		SystemMessage(m_session, "You should select a creature.");
		return false;
	}

	Player* pPlayer = m_session->GetPlayer();

	for(uint32 i = 0; i < count; ++i)
	{
		if(!aiagent_spells.size())
			break;

		SpellEntry* sp = *aiagent_spells.begin();
		aiagent_spells.erase(aiagent_spells.begin());
		BlueSystemMessage(m_session, "Casting %u, " MSG_COLOR_SUBWHITE "%u remaining.", sp->Id, aiagent_spells.size());

		map<uint32, spell_thingo>::iterator it = aiagent_extra.find(sp->Id);
		ARCEMU_ASSERT(it != aiagent_extra.end());

		SpellCastTargets targets;
		if(it->second.type == STYPE_BUFF)
			targets = SetTargets(sp, it->second.type, it->second.type, pCreature, pCreature);
		else
			targets = SetTargets(sp, it->second.type, it->second.type, pPlayer, pCreature);

		pCreature->GetAIInterface()->CastSpell(pCreature, sp, targets);
	}

	if(!aiagent_spells.size())
		RedSystemMessage(m_session, "Finished.");
	/*else
		BlueSystemMessage(m_session, "Got %u remaining.", aiagent_spells.size());*/
	return true;
}

bool ChatHandler::HandleAIAgentDebugSkip(const char* args, WorldSession* m_session)
{
	uint32 count = atoi(args);
	if(!count)
		return false;

	for(uint32 i = 0; i < count; ++i)
	{
		if(!aiagent_spells.size())
			break;

		aiagent_spells.erase(aiagent_spells.begin());
	}
	BlueSystemMessage(m_session, "Erased %u spells.", count);
	return true;
}

bool ChatHandler::HandleDebugDumpCoordsCommmand(const char* args, WorldSession* m_session)
{
	Player* p = m_session->GetPlayer();
	//char buffer[200] = {0};
	FILE* f = fopen("C:\\script_dump.txt", "a");
	if(!f)
		return false;

	fprintf(f, "mob.CreateWaypoint(%f, %f, %f, %f, 0, 0, 0);\n", p->GetPositionX(), p->GetPositionY(), p->GetPositionZ(), p->GetOrientation());
	fclose(f);
	return true;
}

bool ChatHandler::HandleSendpacket(const char* args, WorldSession* m_session)
{
#ifdef _ONLY_FOOLS_TRY_THIS_
	uint32 arg_len = strlen(args);
	char* xstring = new char [arg_len];
	memcpy(xstring, args, arg_len);

	for(uint32 i = 0; i < arg_len; i++)
	{
		if(xstring[i] == ' ')
			xstring[i] = '\0';
	}

	// we receive our packet as hex, that means we get it like ff ff ff ff
	// the opcode consists of 2 bytes

	if(!xstring)
	{
		LOG_DEBUG("[Debug][Sendpacket] Packet is invalid");
		return false;
	}

	WorldPacket data(arg_len);

	uint32 loop = 0;
	uint16 opcodex = 0;
	uint16 opcodez = 0;

	// get the opcode
	sscanf(xstring , "%x", &opcodex);

	// should be around here
	sscanf(&xstring[3] , "%x", &opcodez);

	opcodex = opcodex << 8;
	opcodex |= opcodez;
	data.Initialize(opcodex);


	int j = 3;
	int x = 0;
	do
	{
		if(xstring[j] == '\0')
		{
			uint32 HexValue;
			sscanf(&xstring[j + 1], "%x", &HexValue);
			if(HexValue > 0xFF)
			{
				LOG_DEBUG("[Debug][Sendpacket] Packet is invalid");
				return false;
			}
			data << uint8(HexValue);
			//j++;
		}
		j++;
	}
	while(j < arg_len);
	data.hexlike();

	m_session->SendPacket(&data);
	LOG_DEBUG("[Debug][Sendpacket] Packet was send.");
#endif
	return true;
}

//As requested by WaRxHeAd for database development.
//This should really only be available in special cases and NEVER on real servers... -DGM

bool ChatHandler::HandleSQLQueryCommand(const char* args, WorldSession* m_session)
{
#ifdef _ONLY_FOOLS_TRY_THIS_
	if(!*args)
	{
		RedSystemMessage(m_session, "No query given.");
		return false;
	}

	bool isok = false;
	//SQL query lenght is seems to be limited to 16384 characters, thus the check
	if(strlen(args) > 16384)
	{
		RedSystemMessage(m_session, "Query is longer than 16384 chars!");
		//Now let the user now what are we talking about
		GreenSystemMessage(m_session, args);
	}
	else
	{
		//First send back what we got. Feedback to the user of the command.
		GreenSystemMessage(m_session, args);
		//Sending the query, but this time getting the result back
		QueryResult* qResult = WorldDatabase.Query(args);
		if(qResult)
		{
			Field* field;
			//Creating the line (instancing)
			std::string line = "";
			do
			{
				field = qResult->Fetch();
				for(uint32 i = 0; i < (qResult->GetFieldCount()); i++)
				{
					//Constructing the line
					line += field[i].GetString();
				}
				//Sending the line as ingame message
				BlueSystemMessage(m_session, line.data());
				//Clear the line
				line.clear();
			}
			while(qResult->NextRow());
			delete field;
		}
		else
		{
			RedSystemMessage(m_session, "Invalid query, or the database may be busy.");
			isok = false;
		}
		//delete qResult anyway, not to cause some leak!
		delete qResult;
		isok = true;
	}

	if(isok)
		GreenSystemMessage(m_session, "Query was executed successfully.");
	else
		RedSystemMessage(m_session, "Query failed to execute.");
#endif
	return true;
}

bool ChatHandler::HandleRangeCheckCommand(const char* args , WorldSession* m_session)
{
	WorldPacket data;
	uint64 guid = m_session->GetPlayer()->GetSelection();
	m_session->SystemMessage("=== RANGE CHECK ===");
	if(!guid)
	{
		m_session->SystemMessage("You must select a character or creature.");
		return false;
	}

	Unit* unit = m_session->GetPlayer()->GetMapMgr()->GetUnit(guid);
	if(!unit)
	{
		m_session->SystemMessage("Invalid selection.");
		return false;
	}

	float DistSq = unit->GetDistanceSq(m_session->GetPlayer());
	m_session->SystemMessage("GetDistanceSq : %u" , float2int32(DistSq));
	LocationVector locvec(m_session->GetPlayer()->GetPositionX() , m_session->GetPlayer()->GetPositionY() , m_session->GetPlayer()->GetPositionZ());
	float DistReal = unit->CalcDistance(locvec);
	m_session->SystemMessage("CalcDistance : %u" , float2int32(DistReal));
	float Dist2DSq = unit->GetDistance2dSq(m_session->GetPlayer());
	m_session->SystemMessage("GetDistance2dSq: %u" , float2int32(Dist2DSq));
	return true;
}

bool ChatHandler::HandleRatingsCommand(const char* args , WorldSession* m_session)
{
	m_session->SystemMessage("Ratings:");
	Player* m_plyr = getSelectedChar(m_session, false);
	if(!m_plyr)
	{
		SystemMessage(m_session, "You must select a character.");
		return false;
	}

	for(uint32 i = 0; i < 24; i++)
	{
		m_plyr->ModUnsigned32Value(PLAYER_FIELD_COMBAT_RATING_1 + i, i);
	}
	m_plyr->UpdateStats();
	return true;
}

bool ChatHandler::HandleCollisionTestLOS(const char* args, WorldSession* m_session)
{
	if(sWorld.Collision)
	{
		Object* pObj = NULL;
		Creature* pCreature = getSelectedCreature(m_session, false);
		Player* pPlayer = getSelectedChar(m_session, false);
		if(pCreature)
			pObj = pCreature;
		else if(pPlayer)
			pObj = pPlayer;

		if(!pObj)
		{
			SystemMessage(m_session, "You must select a character or creature.");
			return false;
		}

		const LocationVector & loc2 = pObj->GetPosition();
		const LocationVector & loc1 = m_session->GetPlayer()->GetPosition();
		bool res = CollideInterface.CheckLOS(pObj->GetMapId(), loc1.x, loc1.y, loc1.z, loc2.x, loc2.y, loc2.z);
		bool res2 = CollideInterface.CheckLOS(pObj->GetMapId(), loc1.x, loc1.y, loc1.z + 2.0f, loc2.x, loc2.y, loc2.z + 2.0f);
		bool res3 = CollideInterface.CheckLOS(pObj->GetMapId(), loc1.x, loc1.y, loc1.z + 5.0f, loc2.x, loc2.y, loc2.z + 5.0f);
		SystemMessage(m_session, "Result was: %s %s %s.", res ? "in LOS" : "not in LOS", res2 ? "in LOS" : "not in LOS", res3 ? "in LOS" : "not in LOS");
		return true;
	}
	else
	{
		SystemMessage(m_session, "Collision is not enabled.");
		return false;
	}
}

bool ChatHandler::HandleCollisionTestIndoor(const char* args, WorldSession* m_session)
{
	if(sWorld.Collision)
	{
		Player* plr = m_session->GetPlayer();
		const LocationVector & loc = plr->GetPosition();
		bool res = CollideInterface.IsIndoor(plr->GetMapId(), loc.x, loc.y, loc.z + 2.0f);
		SystemMessage(m_session, "Result was: %s.", res ? "indoors" : "outside");
		return true;
	}
	else
	{
		SystemMessage(m_session, "Collision is not enabled.");
		return false;
	}
}

bool ChatHandler::HandleCollisionGetHeight(const char* args, WorldSession* m_session)
{
	if(sWorld.Collision)
	{
		Player* plr = m_session->GetPlayer();
		float radius = 5.0f;

		float posX = plr->GetPositionX();
		float posY = plr->GetPositionY();
		float posZ = plr->GetPositionZ();
		float ori  = plr->GetOrientation();

		LocationVector src(posX, posY, posZ);

		LocationVector dest(posX + (radius * (cosf(ori))), posY + (radius * (sinf(ori))), posZ);
		//LocationVector destest(posX + (radius * (cosf(ori))), posY + (radius * (sinf(ori))), posZ);

		float z = CollideInterface.GetHeight(plr->GetMapId(), posX, posY, posZ + 2.0f);
		float z2 = CollideInterface.GetHeight(plr->GetMapId(), posX, posY, posZ + 5.0f);
		float z3 = CollideInterface.GetHeight(plr->GetMapId(), posX, posY, posZ);
		float z4 = plr->GetMapMgr()->GetADTLandHeight(plr->GetPositionX(), plr->GetPositionY());
		bool fp = CollideInterface.GetFirstPoint(plr->GetMapId(), src, dest, dest, -1.5f);

		SystemMessage(m_session, "Results were: %f(offset2.0f) | %f(offset5.0f) | %f(org) | landheight:%f | target radius5 FP:%d", z, z2, z3, z4, fp);
		return true;
	}
	else
	{
		SystemMessage(m_session, "Collision is not enabled.");
		return false;
	}
}

bool ChatHandler::HandleGetDeathState(const char* args, WorldSession* m_session)
{
	Player* SelectedPlayer = getSelectedChar(m_session, true);
	if(!SelectedPlayer)
	{
		SystemMessage(m_session, "You must select a character.");
		return false;
	}

	SystemMessage(m_session, "Death State: %d", SelectedPlayer->getDeathState());
	return true;
}

bool ChatHandler::HandleGetPosCommand(const char* args, WorldSession* m_session)
{
	if(!args || !m_session)
		return false;
	/*if(!m_session->GetPlayer()->GetSelection())
		return false;

	Creature* creature = objmgr.GetCreature(m_session->GetPlayer()->GetSelection());
	if(!creature)
		return false;

	BlueSystemMessage(m_session, "Creature Position: \nX: %f\nY: %f\nZ: %f\n", creature->GetPositionX(), creature->GetPositionY(), creature->GetPositionZ());
	return true;*/

	uint32 spell = atol(args);
	SpellEntry* se = dbcSpell.LookupEntryForced(spell);
	if(se)
		BlueSystemMessage(m_session, "SpellIcon for %d is %d", se->Id, se->field114);

	return true;
}

bool ChatHandler::HandleSendFailed(const char* args , WorldSession* m_session)
{
	Player* plr = getSelectedChar(m_session, true);
	if(!plr)
	{
		SystemMessage(m_session, "You must select a character.");
		return false;
	}

	uint32 fail = atol(args);
	if(SPELL_CANCAST_OK < fail)
	{
		RedSystemMessage(m_session, "Argument %u is out of range!", fail);
		return false;
	}
	plr->SendCastResult(1, (uint8)fail, 0, 0);
	return true;
}

bool ChatHandler::HandlePlayMovie(const char* args, WorldSession* m_session)
{
	Player* plr = getSelectedChar(m_session, true);
	if(!plr)
	{
		SystemMessage(m_session, "You must select a character.");
		return false;
	}

	uint32 movie = atol(args);

	plr->SendTriggerMovie(movie);

	SystemMessage(m_session, "Movie started.");
	return true;
}

bool ChatHandler::HandleAuraUpdateAdd(const char* args, WorldSession* m_session)
{
	if(!args)
		return false;

	uint32 SpellID = 0;
	int Flags = 0;
	int StackCount = 0;
	if(sscanf(args, "%u 0x%X %i", &SpellID, &Flags, &StackCount) != 3 && sscanf(args, "%u %u %i", &SpellID, &Flags, &StackCount) != 3)
		return false;

	Player* Pl = m_session->GetPlayer();
	if(Aura* AuraPtr = Pl->FindAura(SpellID))
	{
		uint8 VisualSlot = AuraPtr->m_visualSlot;
		Pl->SendAuraUpdate(AuraPtr->m_auraSlot, false);
		SystemMessage(m_session, "SMSG_AURA_UPDATE (update): VisualSlot %u - SpellID %u - Flags %i (0x%04X) - StackCount %i", VisualSlot, SpellID, Flags, Flags, StackCount);
	}
	else
	{
		SpellEntry* Sp = dbcSpell.LookupEntryForced(SpellID);
		if(!Sp)
		{
			SystemMessage(m_session, "SpellID %u is invalid.", SpellID);
			return false;
		}
		Spell* SpellPtr = sSpellFactoryMgr.NewSpell(Pl, Sp, false, NULL);
		AuraPtr = sSpellFactoryMgr.NewAura(Sp, SpellPtr->GetDuration(), Pl, Pl);
		Pl->AddAura(AuraPtr); // Serves purpose to just add the aura to our auraslots
		SystemMessage(m_session, "SMSG_AURA_UPDATE (add): VisualSlot %u - SpellID %u - Flags %i (0x%04X) - StackCount %i", AuraPtr->m_visualSlot, SpellID, Flags, Flags, StackCount);
		delete SpellPtr;
	}
	return true;
}

bool ChatHandler::HandleAuraUpdateRemove(const char* args, WorldSession* m_session)
{
	if(!args)
		return false;

	char* pArgs = strtok((char*)args, " ");
	if(!pArgs)
		return false;

	uint8 VisualSlot = (uint8)atoi(pArgs);
	Player* Pl = m_session->GetPlayer();
	Aura* AuraPtr = Pl->FindAura(Pl->m_auravisuals[VisualSlot]);
	if(!AuraPtr)
	{
		SystemMessage(m_session, "No aura ID found in slot %u", VisualSlot);
		return false;
	}
	SystemMessage(m_session, "SMSG_AURA_UPDATE (remove): VisualSlot %u - SpellID 0", VisualSlot);
	AuraPtr->Remove();
	return true;
}

bool ChatHandler::HandleDebugSpawnWarCommand(const char* args, WorldSession* m_session)
{
	uint32 count, npcid;
	uint32 health = 0;

	// takes 2 or 3 arguments: npcid, count, (health)
	if(sscanf(args, "%u %u %u", &npcid, &count, &health) != 3)
		if(sscanf(args, "%u %u", &count, &npcid) != 2)
			return false;

	if(!count || !npcid)
		return false;

	CreatureProto* cp = CreatureProtoStorage.LookupEntry(npcid);
	CreatureInfo* ci = CreatureNameStorage.LookupEntry(npcid);
	if(!cp || !ci)
		return false;

	MapMgr* m = m_session->GetPlayer()->GetMapMgr();

	// if we have selected unit, use its position
	Unit* unit = m->GetUnit(m_session->GetPlayer()->GetSelection());
	if(!unit)
		unit = m_session->GetPlayer(); // otherwise ours

	float bx = unit->GetPositionX();
	float by = unit->GetPositionY();
	float x, y, z;

	float angle = 1;
	float r = 3; // starting radius
	for(; count > 0; --count)
	{
		// spawn in spiral
		x = r * sinf(angle);
		y = r * cosf(angle);
		z = m->GetLandHeight(bx + x, by + y, unit->GetPositionZ() + 2);

		Creature* c = m->CreateCreature(npcid);
		c->Load(cp, bx + x, by + y, z, 0.0f);
		if(health != 0)
		{
			c->SetUInt32Value(UNIT_FIELD_MAXHEALTH, health);
			c->SetUInt32Value(UNIT_FIELD_HEALTH, health);
		}
		c->SetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE, (count % 2) ? 1 : 2);
		c->_setFaction();
		c->PushToWorld(m);

		r += 0.5;
		angle += 8 / r;
	}
	return true;
}

bool ChatHandler::HandleUpdateWorldStateCommand(const char* args, WorldSession* session)
{
	if(*args == '\0')
	{
		RedSystemMessage(session, "You need to specify the worldstate field and the new value.");
		return false;
	}

	uint32 field = 0;
	uint32 state = 0;

	std::stringstream ss(args);

	ss >> field;
	if(ss.fail())
	{
		RedSystemMessage(session, "You need to specify the worldstate field and the new value.");
		return false;
	}

	ss >> state;
	if(ss.fail())
	{
		RedSystemMessage(session, "You need to specify the worldstate field and the new value.");
		return false;
	}

	session->GetPlayer()->SendWorldStateUpdate(field, state);

	return true;
}

bool ChatHandler::HandleInitWorldStatesCommand(const char* args, WorldSession* session)
{
	Player* p = session->GetPlayer();
	uint32 zone = p->GetZoneId();
	if(!zone)
		zone = p->GetAreaID();

	BlueSystemMessage(session, "Sending initial worldstates for zone %u.", zone);
	p->SendInitialWorldstates();
	return true;
}

bool ChatHandler::HandleClearWorldStatesCommand(const char* args, WorldSession* session)
{
	Player* p = session->GetPlayer();
	uint32 zone = p->GetZoneId();
	if(!zone)
		zone = p->GetAreaID();

	BlueSystemMessage(session, "Clearing worldstates for zone %u", zone);

	WorldPacket data(SMSG_INIT_WORLD_STATES, 16);
	data << uint32(p->GetMapId());
	data << uint32(p->GetZoneId());
	data << uint32(p->GetAreaID());
	data << uint16(0 );
	p->SendPacket(&data);
	return true;
}