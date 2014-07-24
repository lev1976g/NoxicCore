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
//  Admin Chat Commands
//

#include "StdAfx.h"
#include "ObjectMgr.h"
#include "Master.h"

bool ChatHandler::HandleAddSkillCommand(const char* args, WorldSession* m_session)
{
	char buf[256];
	Player* target = objmgr.GetPlayer((uint32)m_session->GetPlayer()->GetSelection());

	if(!target)
	{
		SystemMessage(m_session, "Select A Player first.");
		return true;
	}

	uint32 skillline;
	uint16 cur, max;

	char* pSkillline = strtok((char*)args, " ");
	if(!pSkillline)
		return false;

	char* pCurrent = strtok(NULL, " ");
	if(!pCurrent)
		return false;

	char* pMax = strtok(NULL, " ");
	if(!pMax)
		return false;

	skillline = (uint32)atol(pSkillline);
	cur = (uint16)atol(pCurrent);
	max = (uint16)atol(pMax);

	target->_AddSkillLine(skillline, cur, max);

	snprintf(buf, 256, "SkillLine: %u CurrentValue %u Max Value %u Added.", (unsigned int)skillline, (unsigned int)cur, (unsigned int)max);
	sGMLog.writefromsession(m_session, "added skill line %u (%u/%u) to %s", skillline, cur, max, target->GetName());
	SystemMessage(m_session, buf);

	return true;
}
