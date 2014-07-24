/*
 * ArcEmu MMORPG Server
 * Copyright (C) 2005-2007 Ascent Team <http://www.ascentemu.com/>
 * Copyright (C) 2008-2012 <http://www.ArcEmu.org/>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

///////////////////////////////////////////////
//  Waypoint Commands
//

#include "StdAfx.h"

bool ChatHandler::HandleNpcSelectCommand(const char* args, WorldSession* m_session)
{
	Creature* un = NULL;
	float dist = 999999.0f;
	float dist2;
	Player* plr = m_session->GetPlayer();
	set<Object*>::iterator itr;
	for(itr = plr->GetInRangeSetBegin(); itr != plr->GetInRangeSetEnd(); ++itr)
	{
		if((dist2 = plr->GetDistance2dSq(*itr)) < dist && (*itr)->IsCreature())
		{
			un = TO_CREATURE(*itr);
			dist = dist2;
		}
	}

	if(!un)
	{
		SystemMessage(m_session, "No inrange creatures found.");
		return true;
	}

	plr->SetSelection(un->GetGUID());
	SystemMessage(m_session, "Set selection to " I64FMT " (%s)", un->GetGUID(), un->GetCreatureInfo()->Name);
	return true;
}
