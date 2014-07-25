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

bool ChatHandler::HandleWPAddCommand(const char* args, WorldSession* m_session)
{
	uint64 guid = m_session->GetPlayer()->GetSelection();
	if(!guid)
	{
		SystemMessage(m_session, "You have no selected creature.");
		return false;
	}
	AIInterface* ai = NULL;
	Creature* pCreature = NULL;
	Player* p = m_session->GetPlayer();
	if(p->waypointunit)
	{
		SystemMessage(m_session, "Using previous unit.");
		ai = p->waypointunit;
		if(!ai)
		{
			SystemMessage(m_session, "Invalid creature.");
			return false;
		}

		pCreature = TO_CREATURE(ai->GetUnit());
		if(!pCreature || pCreature->IsPet())
		{
			SystemMessage(m_session, "Invalid creature.");
			return false;
		}
	}
	else
	{
		pCreature = m_session->GetPlayer()->GetMapMgr()->GetCreature(GET_LOWGUID_PART(guid));
		if(!pCreature || pCreature->IsPet())
		{
			SystemMessage(m_session, "You should select a creature.");
			return false;
		}
		ai = pCreature->GetAIInterface();
	}

	char* pWaitTime = strtok((char*)args, " ");
	uint32 WaitTime = (pWaitTime) ? atoi(pWaitTime) : 10000;
	char* pFlags = strtok(NULL, " ");
	uint32 Flags = (pFlags) ? atoi(pFlags) : 0 ;
	char* pForwardEmoteId = strtok(NULL, " ");
	uint32 ForwardEmoteId = (pForwardEmoteId) ? atoi(pForwardEmoteId) : 0;
	char* pBackwardEmoteId = strtok(NULL, " ");
	uint32 BackwardEmoteId = (pBackwardEmoteId) ? atoi(pBackwardEmoteId) : 0;
	char* pForwardSkinId = strtok(NULL, " ");
	uint32 ForwardSkinId = (pForwardSkinId) ? atoi(pForwardSkinId) : pCreature->GetNativeDisplayId();
	char* pBackwardSkinId = strtok(NULL, " ");
	uint32 BackwardSkinId = (pBackwardSkinId) ? atoi(pBackwardSkinId) : pCreature->GetNativeDisplayId();
	char* pForwardEmoteOneShot = strtok(NULL, " ");
	uint32 ForwardEmoteOneShot = (pForwardEmoteOneShot) ? atoi(pForwardEmoteOneShot) : 1;
	char* pBackwardEmoteOneShot = strtok(NULL, " ");
	uint32 BackwardEmoteOneShot = (pBackwardEmoteOneShot) ? atoi(pBackwardEmoteOneShot) : 1;

	WayPoint* wp = new WayPoint;
	bool showing = ai->m_WayPointsShowing;
	wp->id = (uint32)ai->GetWayPointsCount() + 1;
	wp->x = p->GetPositionX();
	wp->y = p->GetPositionY();
	wp->z = p->GetPositionZ();
	//wp->o = p->GetOrientation();
	wp->waittime = WaitTime;
	wp->flags = Flags;
	wp->forwardemoteoneshot = (ForwardEmoteOneShot > 0) ? true : false;
	wp->forwardemoteid = ForwardEmoteId;
	wp->backwardemoteoneshot = (BackwardEmoteOneShot > 0) ? true : false;
	wp->backwardemoteid = BackwardEmoteId;
	wp->forwardskinid = ForwardSkinId;
	wp->backwardskinid = BackwardSkinId;

	if(showing)
		ai->hideWayPoints(p);

	if(ai->addWayPointUnsafe(wp))
	{
		ai->saveWayPoints();

		SystemMessage(m_session, "WayPoint %u has been added.", wp->id);
	}
	else
	{
		SystemMessage(m_session, "An error has occurred while adding the WayPoint.");
		delete wp;
	}

	if(showing)
		ai->showWayPoints(p, ai->m_WayPointsShowBackwards);

	return true;
}

bool ChatHandler::HandleWPShowCommand(const char* args, WorldSession* m_session)
{
	uint64 guid = m_session->GetPlayer()->GetSelection();
	if(!guid)
	{
		SystemMessage(m_session, "You have no selected creature.");
		return false;
	}

	Creature* pCreature = m_session->GetPlayer()->GetMapMgr()->GetCreature(GET_LOWGUID_PART(guid));
	if(!pCreature)
	{
		SystemMessage(m_session, "You should select a creature.");
		return false;
	}

	char* pBackwards = strtok((char*)args, " ");
	bool Backwards = (pBackwards) ? ((atoi(pBackwards) > 0) ? true : false) : false;

	AIInterface* ai = pCreature->GetAIInterface();
	Player* pPlayer = m_session->GetPlayer();

	if(pPlayer->waypointunit != ai)
	{
		if(ai->m_WayPointsShowing == true)
		{
			SystemMessage(m_session, "Some one else is also viewing this creatures WayPoints.");
			SystemMessage(m_session, "Viewing WayPoints at the same time as some one else can cause undesirable results.");
			return false;
		}

		if(pPlayer->waypointunit)
			pPlayer->waypointunit->hideWayPoints(pPlayer);

		pPlayer->waypointunit = ai;
		ai->showWayPoints(pPlayer, Backwards);
		ai->m_WayPointsShowBackwards = Backwards;
	}
	else
	{
		if(ai->m_WayPointsShowing == true)
			SystemMessage(m_session, "These WayPoints are already showing.");
		else
			ai->showWayPoints(m_session->GetPlayer(), Backwards);
	}

	SystemMessage(m_session, "Showing WayPoints for creature %u", pCreature->GetSQL_id());
	return true;
}

bool ChatHandler::HandleWPHideCommand(const char* args, WorldSession* m_session)
{
	uint64 guid = m_session->GetPlayer()->GetSelection();
	if(!guid)
	{
		SystemMessage(m_session, "You have no selected creature.");
		return false;
	}

	Creature* pCreature = m_session->GetPlayer()->GetMapMgr()->GetCreature(GET_LOWGUID_PART(guid));
	if(!pCreature)
	{
		SystemMessage(m_session, "You should select a creature.");
		return false;
	}

	AIInterface* ai = pCreature->GetAIInterface();
	Player* pPlayer = m_session->GetPlayer();


	if(pPlayer->waypointunit == ai)
	{
		if(ai->m_WayPointsShowing == true)
			pPlayer->waypointunit->hideWayPoints(pPlayer);

		pPlayer->waypointunit = NULL;
	}
	else
	{
		SystemMessage(m_session, "WayPoints for that unit are not visible.");
		return false;
	}

	std::stringstream ss;
	ss << "Hiding WayPoints for " << pCreature->GetSQL_id();
	SystemMessage(m_session, ss.str().c_str());

	return true;
}

bool ChatHandler::HandleWPDeleteCommand(const char* args, WorldSession* m_session)
{
	uint64 guid = m_session->GetPlayer()->GetSelection();
	if(!guid)
	{
		SystemMessage(m_session, "You have no selected WayPoint.");
		return false;
	}

	if(GET_TYPE_FROM_GUID(guid) != HIGHGUID_TYPE_WAYPOINT)
	{
		SystemMessage(m_session, "You should select a WayPoint.");
		return false;
	}

	Player* pPlayer = m_session->GetPlayer();
	AIInterface* ai = pPlayer->waypointunit;
	if(!ai || !ai->GetUnit())
	{
		SystemMessage(m_session, "Invalid WayPoint.");
		return false;
	}
	std::stringstream ss;

	uint32 wpid = Arcemu::Util::GUID_LOPART(guid);
	if(wpid)
	{
		//Refresh client
		//Hide all
		bool show = ai->m_WayPointsShowing;
		if(show == true)
			ai->hideWayPoints(pPlayer);

		ai->deleteWayPoint(wpid);
		//Show All again after delete
		if(show == true)
			ai->showWayPoints(pPlayer, ai->m_WayPointsShowBackwards);

		SystemMessage(m_session, "WayPoint %u has been deleted.", wpid);
	}
	else
	{
		SystemMessage(m_session, "Invalid WayPoint.");
		return false;
	}
	return true;
}

bool ChatHandler::HandleWPMoveHereCommand(const char* args, WorldSession* m_session)
{
	uint64 guid = m_session->GetPlayer()->GetSelection();
	if(!guid)
	{
		SystemMessage(m_session, "You have no selected WayPoint.");
		return false;
	}

	if(GET_TYPE_FROM_GUID(guid) != HIGHGUID_TYPE_WAYPOINT)
	{
		SystemMessage(m_session, "You should select a WayPoint.");
		return false;
	}

	Player* pPlayer = m_session->GetPlayer();
	AIInterface* ai = pPlayer->waypointunit;
	if(!ai || !ai->GetUnit())
	{
		SystemMessage(m_session, "Invalid WayPoint.");
		return false;
	}
	std::stringstream ss;

	uint32 wpid = Arcemu::Util::GUID_LOPART(guid);
	if(wpid)
	{
		WayPoint* wp = ai->getWayPoint(wpid);
		if(wp)
		{
			wp->x = pPlayer->GetPositionX();
			wp->y = pPlayer->GetPositionY();
			wp->z = pPlayer->GetPositionZ();

			//save wp
			ai->saveWayPoints();
		}
		//Refresh client
		if(ai->m_WayPointsShowing == true)
		{
			ai->hideWayPoints(pPlayer);
			ai->showWayPoints(pPlayer, ai->m_WayPointsShowBackwards);
		}

		ss << "WayPoint " << wpid << " has been moved.";
		SystemMessage(m_session, ss.str().c_str());
	}
	else
	{
		SystemMessage(m_session, "Invalid WayPoint.");
		return true;
	}
	return true;
}

bool ChatHandler::HandleWPFlagsCommand(const char* args, WorldSession* m_session)
{
	uint64 guid = m_session->GetPlayer()->GetSelection();
	if(!guid)
	{
		SystemMessage(m_session, "You have no selected WayPoint.");
		return false;
	}

	if(GET_TYPE_FROM_GUID(guid) != HIGHGUID_TYPE_WAYPOINT)
	{
		SystemMessage(m_session, "You should select a WayPoint.");
		return false;
	}

	Player* pPlayer = m_session->GetPlayer();
	AIInterface* ai = pPlayer->waypointunit;
	if(!ai || !ai->GetUnit())
	{
		SystemMessage(m_session, "Invalid WayPoint.");
		return false;
	}
	std::stringstream ss;

	uint32 wpid = Arcemu::Util::GUID_LOPART(guid);
	if(wpid)
	{
		WayPoint* wp = ai->getWayPoint(wpid);
		if(!wp)
		{
			SystemMessage(m_session, "Invalid WayPoint.");
			return false;
		}
		uint32 flags = wp->flags;

		char* pNewFlags = strtok((char*)args, " ");
		uint32 NewFlags = (pNewFlags) ? atoi(pNewFlags) : 0;

		wp->flags = NewFlags;

		//save wp
		ai->saveWayPoints();

		ss << "WayPoint " << wpid << " flags changed from " << flags << " to " << NewFlags;
		SystemMessage(m_session, ss.str().c_str());
	}
	else
	{
		SystemMessage(m_session, "Invalid WayPoint.");
		return false;
	}
	return true;
}

bool ChatHandler::HandleWPWaitCommand(const char* args, WorldSession* m_session)
{
	uint64 guid = m_session->GetPlayer()->GetSelection();
	if(!guid)
	{
		SystemMessage(m_session, "You have no selected WayPoint.");
		return false;
	}

	if(GET_TYPE_FROM_GUID(guid) != HIGHGUID_TYPE_WAYPOINT)
	{
		SystemMessage(m_session, "You should select a WayPoint.");
		return false;
	}

	Player* pPlayer = m_session->GetPlayer();
	AIInterface* ai = pPlayer->waypointunit;
	if(!ai || !ai->GetUnit())
	{
		SystemMessage(m_session, "Invalid WayPoint.");
		return false;
	}

	uint32 Wait = 10000;
	std::stringstream ss;

	uint32 wpid = Arcemu::Util::GUID_LOPART(guid);
	if(wpid)
	{
		WayPoint* wp = ai->getWayPoint(wpid);
		if(wp)
		{
			char* pWait = strtok((char*)args, " ");
			Wait = (pWait) ? atoi(pWait) : 10000;

			if(Wait < 5000)
				SystemMessage(m_session, "A wait time of less then 5000ms can cause lag, consider extending it.");

			wp->waittime = Wait;

			//save wp
			ai->saveWayPoints();
		}

		ss << "Wait time for WayPoint [" << wpid << "] is now " << Wait << "ms.";
		SystemMessage(m_session, ss.str().c_str());
	}
	else
	{
		SystemMessage(m_session, "Invalid WayPoint.");
		return false;
	}
	return true;
}

bool ChatHandler::HandleWPEmoteCommand(const char* args, WorldSession* m_session)
{
	uint64 guid = m_session->GetPlayer()->GetSelection();
	if(!guid)
	{
		SystemMessage(m_session, "You have no selected WayPoint.");
		return false;
	}

	if(GET_TYPE_FROM_GUID(guid) != HIGHGUID_TYPE_WAYPOINT)
	{
		SystemMessage(m_session, "You should select a WayPoint.");
		return false;
	}

	Player* pPlayer = m_session->GetPlayer();
	AIInterface* ai = pPlayer->waypointunit;
	if(!ai || !ai->GetUnit())
	{
		SystemMessage(m_session, "Invalid WayPoint.");
		return false;
	}
	uint32 EmoteId = 0;
	bool OneShot = true;
	std::stringstream ss;

	uint32 wpid = Arcemu::Util::GUID_LOPART(guid);
	if(wpid)
	{
		WayPoint* wp = ai->getWayPoint(wpid);
		if(wp)
		{
			char* pBackwards = strtok((char*)args, " ");
			uint32 Backwards = (pBackwards) ? atoi(pBackwards) : 0;
			char* pEmoteId = strtok(NULL, " ");
			EmoteId = (pEmoteId) ? atoi(pEmoteId) : 0;
			char* pOneShot = strtok(NULL, " ");
			OneShot = (pOneShot) ? ((atoi(pOneShot) > 0) ? true : false) : 1;
			if(Backwards)
			{
				wp->backwardemoteid = EmoteId;
				wp->backwardemoteoneshot = OneShot;
			}
			else
			{
				wp->forwardemoteid = EmoteId;
				wp->forwardemoteoneshot = OneShot;
			}

			//save wp
			ai->saveWayPoints();
		}

		ss << "EmoteID for WayPoint [" << wpid << "] is now " << EmoteId << " and oneshot is " << ((OneShot == true) ? "Enabled." : "Disabled.");
		SystemMessage(m_session,  ss.str().c_str());
	}
	else
	{
		SystemMessage(m_session, "Invalid WayPoint.");
		return false;
	}
	return true;
}

bool ChatHandler::HandleWPSkinCommand(const char* args, WorldSession* m_session)
{
	uint64 guid = m_session->GetPlayer()->GetSelection();
	if(!guid)
	{
		SystemMessage(m_session,  "You have no selected WayPoint.");
		return false;
	}

	if(GET_TYPE_FROM_GUID(guid) != HIGHGUID_TYPE_WAYPOINT)
	{
		SystemMessage(m_session,  "You should select a WayPoint.");
		return false;
	}

	Player* pPlayer = m_session->GetPlayer();
	AIInterface* ai = pPlayer->waypointunit;
	if(!ai || !ai->GetUnit())
	{
		SystemMessage(m_session, "Invalid WayPoint.");
		return false;
	}
	uint32 SkinId = 0;
	std::stringstream ss;

	uint32 wpid = Arcemu::Util::GUID_LOPART(guid);
	if(wpid)
	{
		WayPoint* wp = ai->getWayPoint(wpid);
		if(wp)
		{
			char* pBackwards = strtok((char*)args, " ");
			uint32 Backwards = (pBackwards) ? atoi(pBackwards) : 0;
			char* pSkinId = strtok(NULL, " ");
			SkinId = (pSkinId) ? atoi(pSkinId) : 0;
			if(Backwards)
				wp->backwardskinid = SkinId;
			else
				wp->forwardskinid = SkinId;

			//save wp
			ai->saveWayPoints();
		}

		ss << "SkinID for WayPoint [" << wpid << "] is now " << SkinId;
		SystemMessage(m_session, ss.str().c_str());
	}
	else
	{
		SystemMessage(m_session, "Invalid WayPoint.");
		return false;
	}
	return true;
}

bool ChatHandler::HandleWPChangeNoCommand(const char* args, WorldSession* m_session)
{
	uint64 guid = m_session->GetPlayer()->GetSelection();
	if(!guid)
	{
		SystemMessage(m_session, "You have no selected WayPoint.");
		return false;
	}

	if(GET_TYPE_FROM_GUID(guid) != HIGHGUID_TYPE_WAYPOINT)
	{
		SystemMessage(m_session, "You should select a WayPoint.");
		return false;
	}

	Player* pPlayer = m_session->GetPlayer();
	AIInterface* ai = pPlayer->waypointunit;
	if(!ai || !ai->GetUnit())
	{
		SystemMessage(m_session, "Invalid WayPoint.");
		return false;
	}

	std::stringstream ss;
	//get newid
	char* pNewID = strtok((char*)args, " ");
	uint32 NewID = (pNewID) ? atoi(pNewID) : 0;

	uint32 wpid = Arcemu::Util::GUID_LOPART(guid);
	if(NewID == wpid)
		return false;

	if(wpid)
	{
		//Refresh client
		//Hide all

		bool show = ai->m_WayPointsShowing;
		if(show == true)
			ai->hideWayPoints(pPlayer);

		//update to new id
		ai->changeWayPointID(wpid, NewID);

		//Show All again after update
		if(show == true)
			ai->showWayPoints(pPlayer, ai->m_WayPointsShowBackwards);

		ss << "WayPoint [" << wpid << "] changed to WayPoint [" << NewID << "].";
		SystemMessage(m_session, ss.str().c_str());
	}
	else
	{
		SystemMessage(m_session, "Invalid WayPoint.");
		return false;
	}
	return true;
}

bool ChatHandler::HandleWPInfoCommand(const char* args, WorldSession* m_session)
{
	uint64 guid = m_session->GetPlayer()->GetSelection();
	if(!guid)
	{
		SystemMessage(m_session, "You have no selected WayPoint.");
		return false;
	}

	if(GET_TYPE_FROM_GUID(guid) != HIGHGUID_TYPE_WAYPOINT)
	{
		SystemMessage(m_session, "You should select a WayPoint.");
		return false;
	}

	Player* pPlayer = m_session->GetPlayer();
	AIInterface* ai = pPlayer->waypointunit;
	if(!ai || !ai->GetUnit())
	{
		SystemMessage(m_session, "Invalid WayPoint.");
		return false;
	}
	std::stringstream ss;

	uint32 wpid = Arcemu::Util::GUID_LOPART(guid);
	if((wpid > 0) && (wpid <= ai->GetWayPointsCount()))
	{
		WayPoint* wp = ai->getWayPoint(wpid);
		if(wp)
		{
			ss << "WayPoint ID: " << wp->id << ":\n";
			ss << "Wait Time: " << wp->waittime << "\n";
			ss << "Flags: " << wp->flags;
			if(wp->flags == 768)
				ss << " (Fly)\n";
			else if(wp->flags == 256)
				ss << " (Run)\n";
			else
				ss << " (Walk)\n";
			ss << "Backwards\n";
			ss << "   Emote ID: " << wp->backwardemoteid << "\n";
			ss << "   Oneshot: " << ((wp->backwardemoteoneshot == 1) ? "Yes" : "No") << "\n";
			ss << "   Skin ID: " << wp->backwardskinid << "\n";
			ss << "Forwards\n";
			ss << "   Emote ID: " << wp->forwardemoteid << "\n";
			ss << "	  Oneshot: " << ((wp->forwardemoteoneshot == 1) ? "Yes" : "No") << "\n";
			ss << "   Skin ID: " << wp->forwardskinid << "\n";
			SendMultilineMessage(m_session, ss.str().c_str());
		}
	}
	else
	{
		SystemMessage(m_session,  "Invalid WayPoint.");
		return false;
	}
	return true;
}

bool ChatHandler::HandleWPMoveTypeCommand(const char* args, WorldSession* m_session)
{
	if(!*args)
		return false;

	uint32 option = atoi((char*)args);

	if(option != 0 && option != 1 && option != 2)
	{
		std::stringstream ss;
		ss << "Incorrect value." << endl;
		ss << "   0 is Move from WP 1 -> 10 then 10 -> 1." << endl;
		ss << "   1 is Move from WP to a random WP." << endl;
		ss << "   2 is Move from WP 1 -> 10 then 1 -> 10." << endl;
		SendMultilineMessage(m_session, ss.str().c_str());
		return false;
	}

	uint64 guid = m_session->GetPlayer()->GetSelection();
	if(!guid)
	{
		SystemMessage(m_session, "You have no selected creature.");
		return false;
	}

	Creature* pCreature = m_session->GetPlayer()->GetMapMgr()->GetCreature(GET_LOWGUID_PART(guid));
	if(!pCreature)
	{
		SystemMessage(m_session, "You should select a creature.");
		return false;
	}

	char sql[512];
	snprintf(sql, 512, "UPDATE creature_spawns SET movetype = '%u' WHERE id = '%u'", (unsigned int)option, (unsigned int)Arcemu::Util::GUID_LOPART(guid));
	WorldDatabase.Execute(sql);

	pCreature->GetAIInterface()->setMoveType(option);

	SystemMessage(m_session, "The creature's [%u] movetype has been changed to %u and saved to database.", (unsigned int)Arcemu::Util::GUID_LOPART(guid), (unsigned int)option);
	return true;
}

bool ChatHandler::HandleGenerateWaypoints(const char* args, WorldSession* m_session)
{
	Creature* cr = m_session->GetPlayer()->GetMapMgr()->GetCreature(GET_LOWGUID_PART(m_session->GetPlayer()->GetSelection()));
	if(!cr)
	{
		SystemMessage(m_session, "You should select a creature.");
		return false;
	}
	if(cr->GetAIInterface()->GetWayPointsCount()) //ALREADY HAVE WAYPOINTS
	{
		SystemMessage(m_session, "The creature already has WayPoints.");
		return false;
	}
	if(m_session->GetPlayer()->waypointunit)
	{
		SystemMessage(m_session, "You are already showing WayPoints, hide them first.");
		return false;
	}

	if(!cr->GetSQL_id())
		return false;

	char* pR = strtok((char*)args, " ");
	if(!pR)
	{
		SystemMessage(m_session, "Randomly generate wps params: range count");
		return false;
	}

	int r = atoi(pR);
	char* pC = strtok(NULL, " ");
	if(!pC)
	{
		SystemMessage(m_session, "Randomly generate wps params: range count");
		return false;
	}

	int n = atol(pC);
	for(int i = 0; i < n; i++)
	{
		float ang = rand() / 100.0f;
		float ran = (rand() % (r * 10)) / 10.0f;
		while(ran < 1)
		{
			ran = (rand() % (r * 10)) / 10.0f;
		}

		float x = cr->GetPositionX() + ran * sin(ang);
		float y = cr->GetPositionY() + ran * cos(ang);
		float z = cr->GetMapMgr()->GetLandHeight(x, y, cr->GetPositionZ() + 3);
		//float o = cr->GetOrentation();

		WayPoint* wp = new WayPoint;
		wp->id = (uint32)cr->GetAIInterface()->GetWayPointsCount() + 1;
		wp->x = x;
		wp->y = y;
		wp->z = z;
		//wp->o = o;
		wp->waittime = 5000;
		wp->flags = 0;
		wp->forwardemoteoneshot = false;
		wp->forwardemoteid = 0;
		wp->backwardemoteoneshot = false;
		wp->backwardemoteid = 0;
		wp->forwardskinid = 0;
		wp->backwardskinid = 0;

		cr->GetAIInterface()->addWayPoint(wp);
	}

	if(cr->m_spawn && cr->m_spawn->movetype != 1) /* move random */
	{
		cr->m_spawn->movetype = 1;
		cr->GetAIInterface()->m_moveType = 1;
		WorldDatabase.Execute("UPDATE creature_spawns SET movetype = 1 WHERE id = %u", cr->GetSQL_id());
	}

	m_session->GetPlayer()->waypointunit = cr->GetAIInterface();
	cr->GetAIInterface()->showWayPoints(m_session->GetPlayer(), cr->GetAIInterface()->m_WayPointsShowBackwards);
	return true;
}

bool ChatHandler::HandleSaveWaypoints(const char* args, WorldSession* m_session)
{
	Creature* cr = m_session->GetPlayer()->GetMapMgr()->GetCreature(GET_LOWGUID_PART(m_session->GetPlayer()->GetSelection()));
	if(!cr || !cr->GetSQL_id())
		return false;

	Player* pPlayer = m_session->GetPlayer();

	if(pPlayer->waypointunit == cr->GetAIInterface())
	{
		if(cr->GetAIInterface()->m_WayPointsShowing)
			pPlayer->waypointunit->hideWayPoints(pPlayer);

		pPlayer->waypointunit = NULL;
	}

	cr->GetAIInterface()->saveWayPoints();
	return true;
}

bool ChatHandler::HandleDeleteWaypoints(const char* args, WorldSession* m_session)
{
	Creature* cr = m_session->GetPlayer()->GetMapMgr()->GetCreature(GET_LOWGUID_PART(m_session->GetPlayer()->GetSelection()));
	if(!cr)
		return false;

	if(!cr->GetSQL_id())
		return false;

	if(cr->GetAIInterface()->m_WayPointsShowing)
	{
		SystemMessage(m_session, "WayPoints are showing, hide them first.");
		return false;
	}

	WorldDatabase.Execute("DELETE FROM creature_waypoints WHERE spawnid=%u", cr->GetSQL_id());

	cr->GetAIInterface()->deleteWaypoints();
	SystemMessage(m_session, "Deleted WayPoints for %u.", cr->GetSQL_id());
	return true;
}

bool ChatHandler::HandleWaypointAddFlyCommand(const char* args, WorldSession* m_session)
{
	uint64 guid = m_session->GetPlayer()->GetSelection();
	if(!guid)
	{
		SystemMessage(m_session, "You have no selected creature.");
		return false;
	}
	AIInterface* ai = NULL;
	Creature* pCreature = NULL;
	Player* p = m_session->GetPlayer();
	if(p->waypointunit)
	{
		SystemMessage(m_session, "Using previous unit.");
		ai = p->waypointunit;
		if(!ai)
		{
			SystemMessage(m_session, "Invalid creature.");
			return false;
		}

		pCreature = TO_CREATURE(ai->GetUnit());
		if(!pCreature || pCreature->IsPet())
		{
			SystemMessage(m_session, "Invalid creature.");
		return false;
		}
	}
	else
	{
		pCreature = m_session->GetPlayer()->GetMapMgr()->GetCreature(GET_LOWGUID_PART(guid));
		if(!pCreature || pCreature->IsPet())
		{
			SystemMessage(m_session, "You should select a creature.");
			return false;
		}
		ai = pCreature->GetAIInterface();
	}

	char* pWaitTime = strtok((char*)args, " ");
	uint32 WaitTime = (pWaitTime) ? atoi(pWaitTime) : 10000;
	char* pForwardEmoteId = strtok(NULL, " ");
	uint32 ForwardEmoteId = (pForwardEmoteId) ? atoi(pForwardEmoteId) : 0;
	char* pBackwardEmoteId = strtok(NULL, " ");
	uint32 BackwardEmoteId = (pBackwardEmoteId) ? atoi(pBackwardEmoteId) : 0;
	char* pForwardSkinId = strtok(NULL, " ");
	uint32 ForwardSkinId = (pForwardSkinId) ? atoi(pForwardSkinId) : pCreature->GetNativeDisplayId();
	char* pBackwardSkinId = strtok(NULL, " ");
	uint32 BackwardSkinId = (pBackwardSkinId) ? atoi(pBackwardSkinId) : pCreature->GetNativeDisplayId();
	char* pForwardEmoteOneShot = strtok(NULL, " ");
	uint32 ForwardEmoteOneShot = (pForwardEmoteOneShot) ? atoi(pForwardEmoteOneShot) : 1;
	char* pBackwardEmoteOneShot = strtok(NULL, " ");
	uint32 BackwardEmoteOneShot = (pBackwardEmoteOneShot) ? atoi(pBackwardEmoteOneShot) : 1;

	WayPoint* wp = new WayPoint;
	bool showing = ai->m_WayPointsShowing;
	wp->id = (uint32)ai->GetWayPointsCount() + 1;
	wp->x = p->GetPositionX();
	wp->y = p->GetPositionY();
	wp->z = p->GetPositionZ();
	wp->waittime = WaitTime;
	wp->flags = 768;
	wp->forwardemoteoneshot = (ForwardEmoteOneShot > 0) ? true : false;
	wp->forwardemoteid = ForwardEmoteId;
	wp->backwardemoteoneshot = (BackwardEmoteOneShot > 0) ? true : false;
	wp->backwardemoteid = BackwardEmoteId;
	wp->forwardskinid = ForwardSkinId;
	wp->backwardskinid = BackwardSkinId;

	if(showing)
		ai->hideWayPoints(p);

	if(ai->addWayPointUnsafe(wp))
	{
		ai->saveWayPoints();
		SystemMessage(m_session, "WayPoint %u has beem added.", wp->id);
	}
	else
	{
		SystemMessage(m_session, "An error occurred while adding the WayPoint.");
		delete wp;
	}

	if(showing)
		ai->showWayPoints(p, ai->m_WayPointsShowBackwards);

	return true;
}