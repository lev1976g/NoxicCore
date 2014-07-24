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

/////////////////////////////////////////////////
//  Normal User Chat Commands
//

#include "StdAfx.h"
#include <git_version.h>

bool ChatHandler::ShowHelpForCommand(WorldSession* m_session, ChatCommand* table, const char* cmd)
{
	for(uint32 i = 0; table[i].Name != NULL; i++)
	{
		if(!hasStringAbbr(table[i].Name, cmd))
			continue;

		if(m_session->CanUseCommand(table[i].CommandGroup))
			continue;

		if(table[i].ChildCommands != NULL)
		{
			cmd = strtok(NULL, " ");
			if(cmd && ShowHelpForCommand(m_session, table[i].ChildCommands, cmd))
				return true;
		}

		if(table[i].Help == "")
		{
			SystemMessage(m_session, "There is no help for that command");
			return true;
		}

		SendMultilineMessage(m_session, table[i].Help.c_str());

		return true;
	}

	return false;
}