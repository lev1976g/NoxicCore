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

#ifndef __MAP_H
#define __MAP_H

enum MapNames
{
	/// World of Warcraft
	// Eastern Kingdoms
	MAP_BLASTED_LANDS						= 4,
	MAP_SWAMP_OF_SORROWS					= 8,
	MAP_BLACKROCK_MOUNTAIN					= 25,
	MAP_DEADWIND_PASS						= 41,
	MAP_BURNING_STEPPES						= 46,
	MAP_SEARING_GORGE						= 51,
	MAP_BADLANDS							= 3,
	MAP_EASTERN_PLAGUELANDS					= 139,
	MAP_WESTERN_PLAGUELANDS					= 28,
	MAP_STRANGLETHORN_VALE					= 33,
	MAP_THE_HINTERLANDS						= 47,
	MAP_ARATHI_HIGHLANDS					= 45,
	MAP_DUSKWOOD							= 10,
	MAP_WETLANDS							= 11,
	MAP_HILLSBRAD_FOOTHILLS					= 267,
	MAP_REDRIDGE_MOUNTAINS					= 44,
	MAP_LOCH_MODAN							= 38,
	MAP_SILVERPINE_FOREST					= 130,
	MAP_WESTFALL							= 40,
	MAP_ELWYNN_FOREST						= 12,
	MAP_DUN_MOROGH							= 1,
	MAP_TIRISFAL_GLADES						= 85,
	MAP_DEEPRUN_TRAM						= 2257,
	// Kalimdor
	MAP_SILITHUS							= 1377,
	MAP_UNGORO_CRATER						= 490,
	MAP_WINTERSPRING						= 618,
	MAP_FELWOOD								= 361,
	MAP_TANARIS								= 440,
	MAP_THOUSAND_NEEDLES					= 400,
	MAP_DUSTWALLOW_MARSH					= 15,
	MAP_FERALAS								= 357,
	MAP_DESOLACE							= 405,
	MAP_STONETALON_MOUNTAINS				= 406,
	MAP_ASHENVALE							= 331,
	MAP_MOONGLADE							= 493,
	MAP_DARKSHORE							= 148,
	MAP_AZSHARA								= 16,
	MAP_BARRENS								= 17,
	MAP_TELDRASSIL							= 141,
	MAP_DUROTAR								= 14,
	MAP_MULGORE								= 215,
	MAP_GM_ISLAND							= 876,
	MAP_THE_VEILED_SEA						= 457,
};

class MapMgr;
struct MapInfo;
class TerrainMgr;

#include "TerrainMgr.h"

struct Formation;

typedef struct
{
	uint32	id;//spawn ID
	uint32	entry;
	float	x;
	float	y;
	float	z;
	float	o;
	Formation* form;
	uint8 movetype;
	uint32 displayid;
	uint32 factionid;
	uint32 flags;
	uint32 bytes0;
	uint32 bytes1;
	uint32 bytes2;
	uint32 emote_state;
	//uint32 respawnNpcLink;
	uint16 channel_spell;
	uint32 channel_target_go;
	uint32 channel_target_creature;
	uint16 stand_state;
	uint32 death_state;
	uint32 MountedDisplayID;
	uint32 Item1SlotDisplay;
	uint32 Item2SlotDisplay;
	uint32 Item3SlotDisplay;
	uint32 CanFly;
	uint32 phase;

	/* sets one of the bytes of an uint32 */
	uint32 setbyte(uint32 buffer, uint8 index, uint32 byte)
	{

		/* We don't want a segfault, now do we? */
		if(index >= 4)
			return buffer;

		byte = byte << index * 8;
		buffer = buffer | byte;

		return buffer;
	}
} CreatureSpawn;

typedef struct
{
	uint32	id;//spawn ID
	uint32	entry;
	float	x;
	float	y;
	float	z;
	float	o;
	float	o1;
	float	o2;
	float	o3;
	float	facing;
	uint32	flags;
	uint32	state;
	uint32	faction;
	//uint32 level;
	float scale;
	//uint32 stateNpcLink;
	uint32 phase;
	uint32 overrides;
} GOSpawn;

typedef std::vector<CreatureSpawn*> CreatureSpawnList;
typedef std::vector<GOSpawn*> GOSpawnList;

typedef struct
{
	CreatureSpawnList CreatureSpawns;
	GOSpawnList GOSpawns;
} CellSpawns;

class SERVER_DECL Map
{
	public:
		Map(uint32 mapid, MapInfo* inf);
		~Map();

		ARCEMU_INLINE string GetNameString() { return name; }
		ARCEMU_INLINE const char* GetName() { return name.c_str(); }
		ARCEMU_INLINE MapEntry* GetDBCEntry() { return me; }

		ARCEMU_INLINE CellSpawns* GetSpawnsList(uint32 cellx, uint32 celly)
		{
			ARCEMU_ASSERT(cellx < _sizeX);
			ARCEMU_ASSERT(celly < _sizeY);
			if(spawns[cellx] == NULL) return NULL;

			return spawns[cellx][celly];
		}
		ARCEMU_INLINE CellSpawns* GetSpawnsListAndCreate(uint32 cellx, uint32 celly)
		{
			ARCEMU_ASSERT(cellx < _sizeX);
			ARCEMU_ASSERT(celly < _sizeY);
			if(spawns[cellx] == NULL)
			{
				spawns[cellx] = new CellSpawns*[_sizeY];
				memset(spawns[cellx], 0, sizeof(CellSpawns*)*_sizeY);
			}

			if(spawns[cellx][celly] == 0)
				spawns[cellx][celly] = new CellSpawns;
			return spawns[cellx][celly];
		}

		void LoadSpawns(bool reload);//set to true to make clean up
		uint32 CreatureSpawnCount;
		uint32 GameObjectSpawnCount;

		ARCEMU_INLINE void CellGoneActive(uint32 x, uint32 y)
		{
		}

		ARCEMU_INLINE void CellGoneIdle(uint32 x, uint32 y)
		{
		}

	private:
		MapInfo* 	   _mapInfo;
		uint32 _mapId;
		string name;
		MapEntry* me;

		//new stuff
		CellSpawns** spawns[_sizeX];
	public:
		CellSpawns staticSpawns;
};

#endif
