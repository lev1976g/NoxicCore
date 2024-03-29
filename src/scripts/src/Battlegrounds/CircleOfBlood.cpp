/*
 * ArcEmu MMORPG Server
 * Copyright (C) 2005-2007 Ascent Team <http://www.ascentemu.com/>
 * Copyright (C) 2008-2011 <http://www.ArcEmu.org/>
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

#include "CircleOfBlood.h"

CircleOfBlood::CircleOfBlood(MapMgr* mgr, uint32 id, uint32 lgroup, uint32 t, uint32 players_per_side) : 
Arena(mgr, id, lgroup, t, players_per_side) {}

CircleOfBlood::~CircleOfBlood() {}

void CircleOfBlood::OnCreate()
{
	if(GameObject* obj = SpawnGameObject(183972, 562, 6177.707520f, 227.348145f, 3.604374f, -2.260201f, 32, 1375, 1.0f))
	{
		obj->SetByte(GAMEOBJECT_BYTES_1, 0, 1);
		obj->SetParentRotation(2, 0.90445f);
		obj->SetParentRotation(3, -0.426569f);
		obj->PushToWorld(m_mapMgr);
	}
	else
		Log.Error("Arena", "Failed to spawn 183972 object in Circle of Blood arena (map id 562)");

	if(GameObject* obj = SpawnGameObject(183973, 562, 6189.546387f, 241.709854f, 3.101481f, 0.881392f, 32, 1375, 1.0f))
	{
		obj->SetByte(GAMEOBJECT_BYTES_1, 0, 1);
		obj->SetParentRotation(2, 0.426569f);
		obj->SetParentRotation(3, 0.904455f);
		m_gates.insert(obj);
	}
	else
		Log.Error("Arena", "Failed to spawn 183973 object in Circle of Blood arena (map id 562)");

	if(GameObject* obj = SpawnGameObject(183970, 562, 6299.115723f, 296.549438f, 3.308032f, 0.881392f, 32, 1375, 1.0f))
	{
		obj->SetByte(GAMEOBJECT_BYTES_1, 0, 1);
		obj->SetParentRotation(2, 0.426569f);
		obj->SetParentRotation(3, 0.904455f);
		obj->PushToWorld(m_mapMgr);
	}
	else
		Log.Error("Arena", "Failed to spawn 183970 object in Circle of Blood arena (map id 562)");

	if(GameObject* obj = SpawnGameObject(183971, 562, 6287.276855f, 282.187714f, 3.810925f, -2.260201f, 32, 1375, 1.0f))
	{
		obj->SetByte(GAMEOBJECT_BYTES_1, 0, 1);
		obj->SetParentRotation(2, 0.904455f);
		obj->SetParentRotation(3, -0.426569f);
		m_gates.insert(obj);
	}
	else
		Log.Error("Arena", "Failed to spawn 183971 object in Circle of Blood arena (map id 562)");

	Arena::OnCreate();
}

void CircleOfBlood::HookOnShadowSight()
{
	if(m_buffs[0] = SpawnGameObject(184664, 562, 6249.276855f, 275.187714f, 11.201481f, -2.260201f, 32, 1375, 1.0f))
	{
		m_buffs[0]->SetByte(GAMEOBJECT_BYTES_1, 0, 1);
		m_buffs[0]->SetParentRotation(2, 0.904455f);
		m_buffs[0]->SetParentRotation(3, -0.426569f);
		m_buffs[0]->SetType(GAMEOBJECT_TYPE_TRAP);
		m_buffs[0]->SetByte(GAMEOBJECT_BYTES_1, 3, 100);
		m_buffs[0]->PushToWorld(m_mapMgr);
	}
	else
		Log.Error("Arena", "Failed to spawn 184664 object in Circle of Blood arena (map id 562)");

	if(m_buffs[1] = SpawnGameObject(184664, 562, 6228.546387f, 249.709854f, 11.201481f, 0.881392f, 32, 1375, 1.0f))
	{
		m_buffs[1]->SetByte(GAMEOBJECT_BYTES_1, 0, 1);
		m_buffs[1]->SetParentRotation(2, 0.90445f);
		m_buffs[1]->SetParentRotation(3, -0.426569f);
		m_buffs[1]->SetType(GAMEOBJECT_TYPE_TRAP);
		m_buffs[1]->SetByte(GAMEOBJECT_BYTES_1, 3, 100);
		m_buffs[1]->PushToWorld(m_mapMgr);
	}
	else
		Log.Error("Arena", "Failed to spawn 184664 object in Circle of Blood arena (map id 562)");
}

LocationVector CircleOfBlood::GetStartingCoords(uint32 Team)
{
	return ArenaStartLocation[Team];
}

bool CircleOfBlood::HookHandleRepop(Player* plr )
{
	LocationVector dest;
	dest.ChangeCoords(6241.171875f, 261.067322f, 0.891833f);
	plr->SafeTeleport(m_mapMgr->GetMapId(), m_mapMgr->GetInstanceID(), dest);
	return true;
}
