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

bool ChatHandler::HandleVehicleEjectPassengerCommand(const char* args, WorldSession* session)
{
	uint32 seat = 0;
	std::stringstream ss(args);
	ss >> seat;
	if(ss.fail())
	{
		RedSystemMessage(session, "You need to specify a seat number.");
		return false;
	}

	Player* pPlayer = session->GetPlayer();
	if(!pPlayer->GetTargetGUID())
	{
		RedSystemMessage(session, "You need to select a vehicle.");
		return false;
	}

	Unit* pUnit = pPlayer->GetMapMgr()->GetUnit(pPlayer->GetTargetGUID());
	if(!pUnit)
	{
		RedSystemMessage(session, "You need to select a vehicle.");
		return false;
	}

	if(!pUnit->GetVehicleComponent())
	{
		RedSystemMessage(session, "You need to select a vehicle.");
		return false;
	}

	pUnit->GetVehicleComponent()->EjectPassengerFromSeat(seat);
	return true;
}

bool ChatHandler::HandleVehicleEjectAllPassengersCommand(const char* args, WorldSession* session)
{
	Player* pPlayer = session->GetPlayer();
	if(!pPlayer->GetTargetGUID())
	{
		RedSystemMessage(session, "You need to select a vehicle.");
		return false;
	}

	Unit* pUnit = pPlayer->GetMapMgr()->GetUnit(pPlayer->GetTargetGUID());
	if(!pUnit)
	{
		RedSystemMessage(session, "You need to select a vehicle.");
		return false;
	}

	if(!pUnit->GetVehicleComponent())
	{
		RedSystemMessage(session, "You need to select a vehicle.");
		return false;
	}

	pUnit->GetVehicleComponent()->EjectAllPassengers();
	return true;
}

bool ChatHandler::HandleVehicleInstallAccessoriesCommand(const char* args, WorldSession* session)
{
	Player* pPlayer = session->GetPlayer();
	if(!pPlayer->GetTargetGUID())
	{
		RedSystemMessage(session, "You need to select a vehicle.");
		return false;
	}

	Unit* pUnit = pPlayer->GetMapMgr()->GetUnit(pPlayer->GetTargetGUID());
	if(!pUnit)
	{
		RedSystemMessage(session, "You need to select a vehicle.");
		return false;
	}

	if(!pUnit->GetVehicleComponent())
	{
		RedSystemMessage(session, "You need to select a vehicle.");
		return false;
	}

	pUnit->GetVehicleComponent()->InstallAccessories();
	return true;
}

bool ChatHandler::HandleVehicleRemoveAccessoriesCommand(const char* args, WorldSession* session)
{
	Player* pPlayer = session->GetPlayer();
	if(!pPlayer->GetTargetGUID())
	{
		RedSystemMessage(session, "You need to select a vehicle.");
		return false;
	}

	Unit* pUnit = pPlayer->GetMapMgr()->GetUnit(pPlayer->GetTargetGUID());
	if(!pUnit)
	{
		RedSystemMessage(session, "You need to select a vehicle.");
		return false;
	}

	if(!pUnit->GetVehicleComponent())
	{
		RedSystemMessage(session, "You need to select a vehicle.");
		return false;
	}

	pUnit->GetVehicleComponent()->RemoveAccessories();
	return true;
}

bool ChatHandler::HandleVehicleAddPassengerCommand(const char* args, WorldSession* session)
{
	std::stringstream ss(args);
	uint32 creature_entry;
	ss >> creature_entry;
	if(ss.fail())
	{
		RedSystemMessage(session, "You need to specify a creature ID.");
		return false;
	}

	if(!session->GetPlayer()->GetTargetGUID())
	{
		RedSystemMessage(session, "You need to select a vehicle.");
		return false;
	}

	Unit* pUnit = session->GetPlayer()->GetMapMgr()->GetUnit(session->GetPlayer()->GetTargetGUID());
	if(!pUnit)
	{
		RedSystemMessage(session, "You need to select a vehicle.");
		return false;
	}

	if(!pUnit->GetVehicleComponent())
	{
		RedSystemMessage( session, "You need to select a vehicle." );
		return false;
	}

	if(!pUnit->GetVehicleComponent()->HasEmptySeat())
	{
		RedSystemMessage( session, "That vehicle has no more empty seats." );
		return false;
	}

	CreatureInfo* pCreatureInfo = CreatureNameStorage.LookupEntry(creature_entry);
	CreatureProto* pCreatureProto = CreatureProtoStorage.LookupEntry(creature_entry);

	if(!pCreatureInfo || !pCreatureProto)
	{
		RedSystemMessage(session, "Creature %u does not exist in the database.", creature_entry);
		return false;
	}

	Creature* pCreature = pUnit->GetMapMgr()->CreateCreature(creature_entry);
	pCreature->Load(pCreatureProto, pUnit->GetPositionX(), pUnit->GetPositionY(), pUnit->GetPositionZ(), pUnit->GetOrientation());
	pCreature->PushToWorld(pUnit->GetMapMgr());	
	pCreature->EnterVehicle(pUnit->GetGUID(), 1);
	return true;
}