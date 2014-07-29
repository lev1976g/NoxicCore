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

bool ChatHandler::HandleSetTitle(const char* args, WorldSession* m_session)
{
	Player* plr = getSelectedChar(m_session, true);
	if(!plr)
		return false;

	int32 title = atol(args);
	if(title > int32(PVPTITLE_END) || title < - int32(PVPTITLE_END))
	{
		RedSystemMessage(m_session, "Argument %i is out of range!", title);
		return false;
	}
	if(!title)
	{
		plr->SetUInt64Value(PLAYER__FIELD_KNOWN_TITLES, 0);
		plr->SetUInt64Value(PLAYER__FIELD_KNOWN_TITLES1, 0);
		plr->SetUInt64Value(PLAYER__FIELD_KNOWN_TITLES2, 0);
	}
	else if(title > 0)
		plr->SetKnownTitle(static_cast<RankTitles>(title), true);
	else
		plr->SetKnownTitle(static_cast<RankTitles>(-title), false);

	plr->SetChosenTitle(0); // better remove chosen one
	SystemMessage(m_session, "Title has been %s.", title > 0 ? "set" : "reset");

	std::stringstream logtext;
	logtext << "has ";
	if(title > 0)
		logtext << "set the title field of " << plr->GetName() << "to " << title << ".";
	else
		logtext << "reset the title field of " << plr->GetName();

	sGMLog.writefromsession(m_session, logtext.str().c_str());
	return true;
}